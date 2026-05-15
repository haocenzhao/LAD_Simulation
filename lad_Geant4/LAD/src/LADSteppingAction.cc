//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "LADSteppingAction.hh"



#include "LADDetectorConstructionHodoCreator.hh"
#include "G4Step.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4Track.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

LADSteppingAction::LADSteppingAction(const LADDetectorConstruction* detConstruction,
				     LADEventAction* eventAction,
				     HodoAnalysis* HodoHandle)
: fDetConstruction(detConstruction)
//  fEventAction(eventAction)

//LADSteppingAction::LADSteppingAction()
  
//LADSteppingAction::LADSteppingAction( HodoAnalysis *HodoHandle)
// : fDetConstruction(detConstruction),
//   fEventAction(eventAction)	
    
{

  // fDetConstruction=new   LADDetectorConstruction;
  // fEventaction= new   LADEventAction;
  //It was modified in order to obtain the information directly from ECal
  //but should be a way to extract the information from the mother class

  //  G4cout<<"<LADSteppingAction::LADSteppingAction>"<<G4endl;
   
  // detector = (LADDetectorConstructionHodoCreator*)
  // 	G4RunManager::GetRunManager()->GetUserDetectorConstruction();
    
 
  //Here is created the object which will send the information to the
  //method in the EventAction class

  // eventaction = (LADEventAction*)
  // 	G4RunManager::GetRunManager()->GetUserEventAction();	     


  // I know that the next could be simplify initializing in the constructor
  //if (detConstruction!=NULL) fDetConstruction = detConstruction;
  if (eventAction!=NULL) fEventAction = eventAction;
  
  if (HodoHandle!=NULL) AnalysisHodo=HodoHandle;
  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

LADSteppingAction::~LADSteppingAction()
{
 
  // if(Analysis)
  G4cout<<"LADSteppingAction::~LADSteppingAction()->finishing"<<G4endl;
  // delete Analysis;//was commented 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......




void LADSteppingAction::UserSteppingAction(const G4Step* aStep)
{
  
  //  G4cout<<"<LADSteppingAction::UserSteppingAction>"<<G4endl;

  AnalysisHodo->StepAnalysis(aStep);

  G4int EventID = fEventAction->GetEvent();

  //  G4cout<<"EventId "<<EventID<<G4endl;

  //  G4cout<<"end of a STEP (from Stepping)"<<G4endl; 

}


void 
LADSteppingAction::RunStartAction(const G4Run *aRun)
{

  //  Analysis->BeginOfRunAction(aRun);
  //  OnceAWhileDoIt(true);
}
void 
LADSteppingAction::RunStopAction(const G4Run* aRun)
{

  //  G4cout<<"<LADSteppingAction::RunStopAction>"<<G4endl;

  //  Analysis->EndOfRunAction(aRun);
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
// These methods just check if particles enter a volume different to the present (Entry)
// or leaves the volume (Exit)

G4bool 
LADSteppingAction::Entry(G4String Volumen)
{
  if ((ActualVolume != Volumen) 
      && (NextVolume == Volumen))
    return true;
  else 
    return false;
}

G4bool
LADSteppingAction::Exit(G4String Volumen)
{
  if ((ActualVolume == Volumen) 
      && (NextVolume != Volumen))
    return true;
  else 
    return false; 
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
