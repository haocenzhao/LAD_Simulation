#include "LADActionInitialization.hh"

#include "LADDetectorConstruction.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

LADActionInitialization:: LADActionInitialization(LADDetectorConstruction* detConstruction)
{
  G4cout << "<LADActionInitialization:: LADActionInitialization> Running " <<G4endl;
   
}

LADActionInitialization:: ~LADActionInitialization()
{
  G4cout << "<LADActionInitialization:: ~LADActionInitialization> closing " <<G4endl;

}


void LADActionInitialization::BuildForMaster() const
{
  // I believe this is for multithreading...
  G4cout<<" LADActionInitialization::BuildForMaster()"<<G4endl;

  /*
  HodoAnalysis *
    HodoHandler = new HodoAnalysis();
   
  HistoManager *
    histo = new HistoManager();
  
  SetUserAction(new LADRunAction(histo, HodoHandler) );
  */
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void LADActionInitialization::Build() const
{
  G4cout<<" LADActionInitialization::Build()"<<G4endl;

 
  HistoManager *
    histo = new HistoManager();

  HodoAnalysis *
    HodoHandler = new HodoAnalysis(histo);
 


  if(Variables->GeneratorCase == 2)
    {
      G4cout<<">>>>>>>>>>>>>LUND generator<<<<<<<<<<<  "<<Variables->GeneratorCase<<G4endl;
      LundRead = new LADLUNDReader();
      
    } 
  
  // Actions
  // I am using old way to declare the actions instead of using 'auto'
  LADPrimaryGeneratorAction * GeneratorAction = new LADPrimaryGeneratorAction();
  SetUserAction(GeneratorAction);

  LADRunAction *  runAction = new LADRunAction(histo, HodoHandler);
  SetUserAction(runAction);

  LADEventAction *  eventAction = new LADEventAction(runAction, histo, HodoHandler);
  SetUserAction(eventAction);

  LADSteppingAction *  steppingAction = new LADSteppingAction(detConstruction, eventAction, HodoHandler);
  SetUserAction(steppingAction);

  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
