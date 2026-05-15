#ifndef LADRunAction_h
#define LADRunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"
#include "LADSteppingAction.hh"

#include "LADVariables.hh"
#include "LADLUNDReader.hh"

#include "HodoAnalysis.hh"
#include "LADEventAction.hh"
#include "TTree.h"
#include <TFile.h>

class G4Run;


class HodoAnalysis;
class LADEventAction;
class HistoManager;

// I am using a struct but maybe is not necesary. Also, in old codes the struct was separate
// typedef struct {
//   G4int EventID;
//   G4double EDepTot;
//   G4int paddle;
// } UTEvent;




/// Run action class
///
/// It accumulates statistic and computes dispersion of the energy deposit
/// and track lengths of charged particles with use of analysis tools:
/// H1D histograms are created in BeginOfLADRunAction() for the following
/// physics quantities:
/// - Edep in absorber
/// - Edep in gap
/// - Track length in absorber
/// - Track length in gap
/// The same values are also saved in the ntuple.
/// The histograms and ntuple are saved in the output file in a format
/// according to a specified file extension.
///
/// In EndOfLADRunAction(), the accumulated statistic and computed
/// dispersion is printed.
///



class LADRunAction : public G4UserRunAction
{
public:
  LADRunAction(HistoManager*,  HodoAnalysis *);
  ~LADRunAction();
  
  void BeginOfRunAction(const G4Run*);
  void   EndOfRunAction(const G4Run*);
  

  // G4int GetEventID(){return event.EventID;}
  // G4double GetEDepTot(){return event.EDepTot;}  
  // G4int Getpaddle(){return event.paddle;}

  // void SetEventID(G4int v){event.EventID = v;}
  // void SetEDepTot(G4double v){event.EDepTot = v;}
  // void Setpaddle(G4int v){event.paddle = v;}

  //LADEventAction  *VariablesHandler;
  // LADSteppingAction *stepa;
  

  // void SetEventID(G4int v){fEventID = v;}
  // void SetEDepTot(G4double v){fEDepTot = v;}
  // void Setpaddle(G4int v){fpaddle = v;}


  G4Run *GetaRun(){return aRun;}
  
private: 
  
  G4Run *aRun; // pointer to the current Run
 

 
  HistoManager* fHistoManager = nullptr;
  
  HodoAnalysis* AnalysisHodo;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
