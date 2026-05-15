#ifndef LADSteppingAction_h
#define LADSteppingAction_h 1

#include "G4UserSteppingAction.hh"

#include "G4TrackStatus.hh"

#include "G4Run.hh"

#include "LADDetectorConstruction.hh"
#include "LADEventAction.hh"

#include "HodoAnalysis.hh"
class HodoAnalysis;

class G4Track;

//If we wanted more general we can get the mother and move from there
class LADDetectorConstruction;
class LADDetectorConstructionHodoCreator;
class LADEventAction;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class LADSteppingAction : public G4UserSteppingAction
{
public:
  // LADSteppingAction();

  LADSteppingAction(const LADDetectorConstruction* detConstruction, LADEventAction* eventAction, HodoAnalysis* );

  //  LADSteppingAction(HodoAnalysis *);
  
  virtual ~LADSteppingAction();

  void UserSteppingAction(const G4Step*);
  void RunStartAction(const G4Run *aRun);
  void RunStopAction(const G4Run* aRun);

  G4bool Entry(G4String Volumen);
  G4bool Exit(G4String Volumen);
  
  // G4int GetEventID()    {return EventID;}
  // G4double GetEDepTot() {return EDepTot;}  
  // G4int Getpaddle()     {return paddle;}


private:
  const LADDetectorConstruction*  fDetConstruction = nullptr;
  LADDetectorConstructionHodoCreator* detector;
  LADEventAction*         fEventAction = nullptr;  

  G4Track *actualTrack; // pointer to the actual track
  
  G4String ActualVolume, NextVolume; // At start of step, the names of the actual volume
  // G4int EventID;
  // G4double EDepTot;
  // G4int paddle;
  HodoAnalysis* AnalysisHodo;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
