#ifndef LADEventAction_h
#define LADEventAction_h 1

#include "G4UserEventAction.hh"
#include "LADRunAction.hh"
//#include "LADHodoHit.hh"
#include <vector>
#include "LADVariables.hh"
//#include "LADLUNDReader.hh"

#include "globals.hh"

#include "HistoManager.hh"

#include "HodoAnalysis.hh"
/// Event action class
///
/// In EndOfEventAction(), it prints the accumulated quantities of the energy 
/// deposit and track lengths of charged particles in Absober and Gap layers 
/// stored in the hits collections.


// This is what I don't like of the SD code I am using. I need to force
// the number of GEMs chambers (hard coded). 
const G4int kDim = 2;


class HodoAnalysis;

class LADRunAction;
class HistoManager;

class LADEventAction : public G4UserEventAction
{
public:
  LADEventAction(LADRunAction*, HistoManager*, HodoAnalysis*);
  virtual ~LADEventAction();

  virtual void  BeginOfEventAction(const G4Event* event);
  virtual void    EndOfEventAction(const G4Event* event);

  G4int    GetEvent()    {return EventID;}
  G4double GetEDepTot() {return EDepTot;}  
  G4int    Getpaddle()     {return paddle;}

  //  void FillEvent(G4int copy, G4double ene);
  
  // void SetEventID(G4int v){EventID = v;}
  // void SetEDepTot(G4double v){EDepTot = v;}
  // void Setpaddle(G4int v){paddle = v;}

  // LADSteppingAction *stepa;
  LADRunAction *RunAction;
  
private:
  // methods
  // LADHodoHitsCollection* GetHitsCollection(G4int hcID,
  //                                           const G4Event* event) const;
  void PrintEventStatistics(G4double absoEdep) const;
  // , G4double absoTrackLength,
  //                         G4double gapEdep, G4double gapTrackLength) const;
  
  // data members                   
  G4int  fAbsHCID;
  G4int  fGapHCID;
  

  G4int EventID;
  G4double EDepTot;
  G4int paddle;

  HodoAnalysis* AnalysisHodo;
  // HistoManager* histo;
  LADRunAction* fRunAct = nullptr;
  HistoManager* fHistoManager  = nullptr;

  // vector<G4double> vEnergyDep;
  // vector<G4int>    vPadNumber;

  // I guess the use of array is similar to vector, but more elegant?
  //  fGEMHCID -> GEM Hits Collection ID 
  std::array<G4int, kDim> fGEMHCID = { -1, -1 };
};
                     
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

//extern LADEventAction *VariablesHandler;

#endif

    
