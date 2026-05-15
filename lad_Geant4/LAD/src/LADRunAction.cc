#include "LADRunAction.hh"

#include <time.h>

//#include "G4AnalysisManager.hh"
#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

//LADRunAction::LADRunAction(LADSteppingAction *AStepAction)

LADRunAction::LADRunAction(HistoManager* histo, HodoAnalysis *HodoHandle): fHistoManager(histo)
{

  // MAYBE THIS COULD BE USEFUL
  //  if (AStepAction!=NULL) StepAction=AStepAction;
  //  VariablesHandler = new LADEventAction;
  
  // EventAction = new LADEventAction;
  //  stepa = new LADSteppingAction;
  //   if (HodoHandle!=NULL) Analysis=HodoHandle;
  G4cout<<"<LADRunAction::LADRunAction()>: begin"<<G4endl;
  
  // set printing event number per each event
  //  G4RunManager::GetRunManager()->SetPrintProgress(1);

  
  if (HodoHandle!=NULL) AnalysisHodo=HodoHandle;


}


LADRunAction::~LADRunAction()
{
  G4cout<<"<LADRunAction::~LADRunAction()>: closing"<<G4endl;
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void LADRunAction::BeginOfRunAction(const G4Run* run) // The pointer run was commented, why? It is causing me a lot of problems
{

  G4cout<<"<LADRunAction::BeginOfRunAction()>: begin"<<G4endl;

  time_t START = time(0);
  char bufferST[100];
  strftime((char*)&bufferST,100,"%Y-%m-%d - %H-%M-%S",localtime(&START));
  G4cout<<"Time Stamp <START>: "<<bufferST<<G4endl;


  G4cout<<"BoRA: Going to Histo"<<G4endl;
  fHistoManager->Book();
  
  //inform the runManager to save random number seed
  //G4RunManager::GetRunManager()->SetRandomNumberStore(true);

  /*
  // Get analysis manager
  auto analysisManager = G4AnalysisManager::Instance();

 


  */
  G4cout<<"run: "<<run->GetRunID()<<G4endl;
  aRun = const_cast<G4Run *>(run);//a POINTER to the present run to use in the rest of the code


  if(Variables->GeneratorCase == 2)
    {
      G4cout<<">>>>>>>>>>>>>LUND generator (open file)<<<<<<<<<<<  "<<Variables->GeneratorCase<<G4endl;
      LundRead -> OpenFile();
      
    }

  
  // Going to HodoAnalysis
  //  if (StepAction!=NULL) StepAction->RunStartAction(run);

  G4cout<<"Going to HodoAnalysis"<<G4endl;
  AnalysisHodo->BeginOfRunAction(run);



  
  // event.EventID = 0;
  // event.EDepTot = 0;
  // event.paddle = 0;
  // fEventID = 0;
  // fEDepTot = 0;
  // fpaddle = 0;


  //
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void LADRunAction::EndOfRunAction(const G4Run* run)
{
  
  G4cout<<"<LADRunAction::EndOfRunAction()>: begin"<<G4endl;
  
  // if(run)
  //   {
  //   // Going to HodoAnalysis
  //     G4cout<<"Going to HodoAnalysis"<<G4endl;
  //     if (StepAction!=NULL) StepAction->RunStopAction(aRun);    
  //   }
  
  G4int NbOfEvents = run->GetNumberOfEvent(); 
  G4cout<<" NbOfEvents: "<< NbOfEvents<<G4endl;


  if(Variables->GeneratorCase == 2)
    {
      G4cout<<">>>>>>>>>>>>>LUND generator (closing file)<<<<<<<<<<<  "<<G4endl;
      LundRead -> CloseFile();
      
    }
  
  if (NbOfEvents == 0) {
    // close open files
    fHistoManager->Save();
    return;
  }

  
  // print histogram statistics
  //
  // auto analysisManager = G4AnalysisManager::Instance();
  // if ( analysisManager->GetH1(1) ) {
  //   G4cout << G4endl << " ----> print histograms statistic ";
  //   if(isMaster) {
  //     G4cout << "for the entire run " << G4endl << G4endl;
  //   }
  //   else {
  //     G4cout << "for the local thread " << G4endl << G4endl;
  //   }

  //   G4cout << " EAbs : mean = "
  //      << G4BestUnit(analysisManager->GetH1(0)->mean(), "Energy")
  //      << " rms = "
  //      << G4BestUnit(analysisManager->GetH1(0)->rms(),  "Energy") << G4endl;

  //   G4cout << " EGap : mean = "
  //      << G4BestUnit(analysisManager->GetH1(1)->mean(), "Energy")
  //      << " rms = "
  //      << G4BestUnit(analysisManager->GetH1(1)->rms(),  "Energy") << G4endl;

  //   G4cout << " LAbs : mean = "
  //     << G4BestUnit(analysisManager->GetH1(2)->mean(), "Length")
  //     << " rms = "
  //     << G4BestUnit(analysisManager->GetH1(2)->rms(),  "Length") << G4endl;

  //   G4cout << " LGap : mean = "
  //     << G4BestUnit(analysisManager->GetH1(3)->mean(), "Length")
  //     << " rms = "
  //     << G4BestUnit(analysisManager->GetH1(3)->rms(),  "Length") << G4endl;
  //}

  // save histograms & ntuple
  //
  // analysisManager->Write();
  // analysisManager->CloseFile();

  fHistoManager->Save();

  G4cout<<"Going to HodoAnalysis"<<G4endl;
  AnalysisHodo->EndOfRunAction(run);

  
  time_t END = time(0);
  char bufferEN[100];
  strftime((char*)&bufferEN,100,"%Y-%m-%d - %H-%M-%S",localtime(&END));
  G4cout<<"Time Stamp <END>: "<<bufferEN<<G4endl;


  
}





//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

