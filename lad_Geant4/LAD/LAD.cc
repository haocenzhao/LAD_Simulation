//....oooOO0OOooo........ooaoOO0OOooo........oooOO0OOooo........oooOO0OOooo.....// Larga Angle Detector (LAD) simulation (at least the Hodoscope)
// C. Ayerbe - gayoso@jlab.org
//
// v0.1  December 2023 - Just Hodo geometry
// v0.11 February 2024 - Added GEM geometry
// v0.12 April    2024 - Independent analysis for Hodo to skip SD
//
//....oooOO0OOooo........ooaoOO0OOooo........oooOO0OOooo........oooOO0OOooo.....
#include "LADConstants.hh"
#include "LADVariables.hh"


#include "LADDetectorConstruction.hh"
#include "LADActionInitialization.hh"
#include "LADPrimaryGeneratorAction.hh"
#include "LADRunAction.hh"
#include "LADSteppingAction.hh"


//#include "G4AnalysisManager.hh"
#include "G4RunManagerFactory.hh"
#include "G4SteppingVerbose.hh"
#include "G4TScoreNtupleWriter.hh"
#include "G4UIcommand.hh"
#include "G4UImanager.hh"
#include "G4UIExecutive.hh"
#include "G4VisExecutive.hh"

#include "G4UIterminal.hh"
#include <G4UItcsh.hh>

#include "FTFP_BERT.hh"
#include "Randomize.hh"
 


#include "G4ScoringManager.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

namespace {
  void PrintUsage() {
    G4cerr << " Usage: " << G4endl;
    G4cerr << " exampleB4d [-m macro ] [-u UIsession] [-t nThreads] [-vDefault]"
           << G4endl;
    G4cerr << "   note: -t option is available only for multi-threaded mode."
           << G4endl;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

int main(int argc,char** argv)
{
  // Evaluate arguments
  //
  if ( argc > 7 ) {
    PrintUsage();
    return 1;
  }

  G4cout << "Compiled on: " __DATE__ " " __TIME__ "." << G4endl;
  
  Variables = new LADVariables(); // This object stores all parameters which are read from LAD.ini
  Constants = new LADConstants(); // This object stores all parameters which are fix.

    
  G4String dataname;
  dataname = "LAD.ini";
  Variables->LoadFromFile(dataname); // Read the parameters
  G4cout<<"Generator: "<<   Variables->GeneratorCase<<G4endl;
 
  
  G4String macro;
  G4String session;
  G4bool verboseBestUnits = true;


#ifdef G4MULTITHREADED
  G4int nThreads = Variables->MultiTh; //set to 0 for multithread
  G4cout<<"nThreads: "<<nThreads<<G4endl;
#endif
  
  for ( G4int i=1; i<argc; i=i+2 ) {
    if      ( G4String(argv[i]) == "-m" ) macro = argv[i+1];
    else if ( G4String(argv[i]) == "-u" ) session = argv[i+1];
#ifdef G4MULTITHREADED
    else if ( G4String(argv[i]) == "-t" ) {
      nThreads = G4UIcommand::ConvertToInt(argv[i+1]);
    }
#endif
    else if ( G4String(argv[i]) == "-vDefault" ) {
      verboseBestUnits = false;
      --i;  // this option is not followed with a parameter
    }
    else {
      PrintUsage();
      return 1;
    }
  }

  // Detect interactive mode (if no macro provided) and define UI session
  //
  G4UIExecutive* ui = nullptr;
  if ( ! macro.size() ) {
    ui = new G4UIExecutive(argc, argv, session);
  }

  // Optionally: choose a different Random engine...
  // G4Random::setTheEngine(new CLHEP::MTwistEngine);

  // Use G4SteppingVerboseWithUnits
  if ( verboseBestUnits ) {
    G4int precision = 4;
    G4SteppingVerbose::UseBestUnit(precision);
  }

  // Construct the default run manager
  //
  auto runManager =
    G4RunManagerFactory::CreateRunManager(G4RunManagerType::Default);
#ifdef G4MULTITHREADED
  if ( nThreads > 0 ) {
    runManager->SetNumberOfThreads(nThreads);
   
  }
#endif

 
  // I wanted to do the analysis, run, step in this class, but I have a hard time to have the variables declared in the header available in the methods/function
  //  HodoAnalysis *HodoHandle = new HodoAnalysis();// This object does the analysis of the hodoscope until I figure it out how the Sensitive Detector works

  LADDetectorConstruction *detConstruction = new LADDetectorConstruction();
  runManager->SetUserInitialization(detConstruction);
  
  // G4ScoringManager* scoringManager = G4ScoringManager::GetScoringManager();

  
  // Set mandatory initialization classes
  //
  // LADSteppingAction *mySteppingAction = new LADSteppingAction(HodoHandle);

  // SetUserAction(new PrimaryGeneratorAction);
  // SetUserAction(mySteppingAction);
  // G4cout << "LAD main: LADDetectorConstruction " << G4endl;

  
  // G4cout << "LAD main: physicsList" << G4endl;
  auto physicsList = new FTFP_BERT(0); //The 0 gives the verbosity level 
  runManager->SetUserInitialization(physicsList);


  
  // PrimaryGeneratorAction *myPrimaryGeneratorAction =new PrimaryGeneratorAction();
  // runManager-> SetUserAction(myPrimaryGeneratorAction);

  // runManager-> SetUserAction(new LADRunAction(HodoHandle));
  
  // runManager-> SetUserAction(new LADEventAction(HodoHandle));
  
  // runManager-> SetUserAction(new LADSteppingAction(HodoHandle));

   
  G4cout << "LAD main: LADActionInitialization" << G4endl;
  LADActionInitialization *actionInitialization = new LADActionInitialization(detConstruction);
  runManager->SetUserInitialization(actionInitialization);



  // Initialize G4 kernel
  //
  G4cout << "LAD main: RunManager Starting!" << G4endl;
  runManager->Initialize();
  G4cout << "LAD main: RunManager Running!" << G4endl;


  
  // Initialize visualization
  auto visManager = new G4VisExecutive("Quiet"); // remove the option quiet and it will show ALL the visualization modules
  // G4VisExecutive can take a verbosity argument - see /vis/verbose guidance.
  // auto visManager = new G4VisExecutive("Quiet");
 
  visManager->Initialize();
  
  // Get the pointer to the User Interface manager
  auto UImanager = G4UImanager::GetUIpointer();


    
  // Activate score ntuple writer
  // The verbose level can be also set via UI commands
  // /score/ntuple/writerVerbose level
  // The default file type ("root") can be changed in xml, csv, hdf5
  // scoreNtupleWriter.SetDefaultFileType("xml");
  // G4TScoreNtupleWriter<G4AnalysisManager> scoreNtupleWriter;
  // scoreNtupleWriter.SetVerboseLevel(0);
  // scoreNtupleWriter.SetNtupleMerging(true);
    // Note: merging ntuples is available only with Root output
    // (the default in G4TScoreNtupleWriter)


  // BE CAREFUL!! THIS WORKS, BUT IS NOT VERY COMMON TO USE IT
  // This case is for read LUND format file.
  // The number of events, should be read from the INI file
  if(Variables->GeneratorCase == 2)
    {
      G4cout<<">>>>>>>>>>>>>LUND generator<<<<<<<<<<<"<<G4endl;
      //     runManager -> BeamOn(2);
    }

  // Process macro or start UI session
  //

  if ( macro.size() )
    {
      // batch mode
      G4String command = "/control/execute ";
      UImanager->ApplyCommand(command+macro);
    }
  else
    {
      if(Variables->G4GUI)
	{
	  //	  G4cout << "LAD main: 4" << G4endl;
	  // G4UIterminal is a (dumb) terminal.
	  G4UIsession * session = 0;
	  // I use the ini file to select the terminal or GUI
	  // no need of the pre-compiler definitions
//#ifdef G4UI_USE
	  session = new G4UIterminal(new G4UItcsh);
// #else
// 	  session = new G4UIterminal();
// #endif
	  session->SessionStart();
	  delete session;
    	}
      else
	{
	  // interactive mode : define UI session
	  UImanager->ApplyCommand("/control/execute init_vis.mac");
	  if (ui->IsGUI())
	    {
	      UImanager->ApplyCommand("/control/execute gui.mac");
	    }
	  ui->SessionStart();
	  delete ui;
	}
    }
     

  

  // Job termination
  // Free the store: user actions, physics_list and detector_description are
  // owned and deleted by the run manager, so they should not be deleted
  // in the main() program !
  //  delete HodoHandle;
  delete Variables;
  delete Constants;
  
  delete visManager;
  delete runManager;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....
