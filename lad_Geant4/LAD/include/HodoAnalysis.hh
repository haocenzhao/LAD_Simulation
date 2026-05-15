#ifndef HodoAnalysis_h
#define HodoAnalysis_h 1


#include <string>
#include <iostream>
//#include <sstream>
#include <time.h>


#include <chrono>
#include <iomanip> // put_time
#include <fstream>
#include <sstream> // stringstream



//#include "LADEventAction.hh"

//#include "SystemOfUnits.h"
#include "G4ios.hh"

#include "G4UnitsTable.hh"

#include "G4VPhysicalVolume.hh"
#include "G4Event.hh"
#include "G4Run.hh"
#include "G4Track.hh"
#include "G4ClassificationOfNewTrack.hh"
#include "G4TrackStatus.hh"
#include "G4Step.hh"
#include "G4Types.hh"
#include "G4UImanager.hh"
#include "G4MaterialCutsCouple.hh"
#include "G4Material.hh"

#include "G4SystemOfUnits.hh"


// New function for me... 
#include <tuple>



#include "CLHEP/Vector/ThreeVector.h"
using namespace CLHEP;


#include <TFile.h>
// #include <string>
// #include <iostream>
// #include <sstream>

#include "HistoManager.hh"
#include "LADVariables.hh"

#define NoMaxModules 50212 //too big, change to vector


//#include "HodoStruct.hh" //not sure if I will need this

// These are the Geant4 classes we work with
class G4VPhysicalVolume;
class G4Event;
class G4Run;
class G4Track;
class G4Step;

#include "G4ClassificationOfNewTrack.hh"
#include "G4TrackStatus.hh"

#include <time.h>

// We are using ROOT histogramms to store some data. And trees to store event data in files.

#define G4ANALYSIS_USE_ROOT 1

// Including the ROOT classes (look to include the path in the system)
//#include "TROOT.h"
// #include "TSystem.h"
//#include "TFile.h"
//#include "TTree.h"
// #include "TParameter.h"
// #include "TRandom2.h"

//tuple<G4int, G4int, G4int> paddle(G4int);

using namespace std;

class HodoAnalysis {
  
public:
  HodoAnalysis(HistoManager*);
  ~HodoAnalysis();
   
  // Here we clear some counters at the begin of a run.  
  void BeginOfRunAction(const G4Run *);

  // Here we do the final analysis after a run.  
  void EndOfRunAction(const G4Run *);

  
  // G4UserEventAction
  void BeginOfEventAction(const G4Event*);
  // Here we clear some counters at the begin of an event.
  void EndOfEventAction(const G4Event*);
  // Here we analyse the total event.
  
  void TrackStartAction();
  void TrackStopAction();

  // G4UserStackingAction
  //  void ClassifyNewTrack(const G4Track*, G4ClassificationOfNewTrack*);
  //  void NewStage();
  //  void PrepareNewEvent();
  
  // G4UserSteppingAction
  // We have different SteppingActions for different modes of operation.
  void StepAnalysis(const G4Step *aStep);
  
  void dosave(); // Save the ROOT file

  // Counter method
  void Counter();

  //    void InitilizationOfEventAction();
  // MY MODIFICATIONS

  G4int NumberOfEvents(const G4Run*);

  //  G4int GetNumberOfEvents();


  // GeneratorDaten *GetGDatenStruct() {return &gdaten;} ;  
  // G4UserRunAction
  //  void BeginOfRunAction(const G4Run*);

  G4int TotalNnbEvent;//How many events have been set (public variable)   
  // G4double edep;
  //  G4double stepl;
  // G4double  EnergyAbs = 20;
  // G4double  EnergyAbsMod[NoMaxModules];

  vector<G4double> GetEneDep() {return vEneDep;}  
  vector<G4int>    GetPad()    {return vPadNum;}
  vector<G4int>    GetPDG()    {return vPDG;}
  vector<G4int>    GetLevel()  {return vLevel;}



  vector<G4int>    GetPadPosition()  {return vPaddle;}
  vector<G4double> GetXpos() {return vXbar;}
  vector<G4double> GetYpos() {return vYbar;}
  vector<G4double> GetZpos() {return vZbar;}  
  
    
private:
  void fillPerEvent(G4double, G4double, G4double, G4double);        

  
// It is the first time I use this template.
// I don't know if I am using it correctly, but it works
  tuple<G4int, G4int, G4int> paddle(G4int copynumber)
 {
   G4int aawall = copynumber / 10000 ;
   G4int bbsubwall  = ( copynumber - ( aawall *10000 ) ) / 100 ;
   G4int ccpaddle = ( ( copynumber - ( aawall * 10000 ) ) - ( bbsubwall * 100 ) ) ;
   return { aawall,bbsubwall,ccpaddle};
}
  
  
  G4double *psumEAbs;       

  G4double sumEAbs, sum2EAbs;
  G4double sumEGap, sum2EGap;

  G4double sumLAbs, sum2LAbs;
  G4double sumLGap, sum2LGap;    
  
  //  G4double  EnergyAbs;
  G4double  EnergyGap;
  G4double  EnergyAbs ;


  
  G4double  EnergyAbsBar[3][2][11] ;
  G4double  CopyNumberBar[3][2][11] ;
  
  vector<G4double> vEneDep;
  vector<G4int>    vPadNum;
  vector<G4int>    vPDG;
  vector<G4int>    vLevel;//Level 1 -> primary particle, >1 secundaries


  // These variables only for position at the bars
  vector<G4int>    vPaddle;
  vector<G4double> vXbar;
  vector<G4double> vYbar;
  vector<G4double> vZbar;
  vector<G4double> vTbar; //time
  vector<G4int>    vTrackID;// similar to vLevel but just for pos-time purposes

  G4double EnergyTemp;

  //variables to be stored
  //   G4double  EnergyAbsMod[NoMaxModules];
  G4double  EnergyGapMod[NoMaxModules];
  G4double  EnergyGapModLay[NoMaxModules][20];

  //I Keep this variables because I'm not sure, its probable to use later 
  // and its well implemented
  G4double  TrackLAbs;
  G4double  TrackLGap;
  
  G4int     testvalue;       

  G4int     printModulo;

  G4double edep;
  G4double stepl;
    
  G4ThreeVector Position;
 
    
  G4int nbEventInRun; //How many events have been set
  G4int NbOfEventsC;//Number of present event (Counter)
  G4int NbOfEvents;//Number of present event (RunAction). It is neccesary at the end of the run, calculate
  // it in other part is not clear

  G4double RestEventFactor;//Is a factor based in events ocurred to estimate time to finish
  G4double time_elapsed;
  time_t resttime;
  time_t start, end;
  G4double length;
  
  G4int Even25;//25% of events done (module), aproximate value since is not simple to implement
  // exact value

  void StepStart(const G4Step *aStep); 
    
  // Called by KAOSSteppingAction.
  G4bool Entry(G4Track*, G4String Volumen); // True if particle enters volume with name "Volumen"
  G4bool Exit(G4Track*, G4String Volumen); // True if particle leaves volume with name "Volumen"
  void CheckSM(G4String Volumename, G4int ID, const G4Step *aStep);
  void CheckAkzeptiert(); // Check if particle enters the front of KAOS dipolefield

  TFile *CreateSaveFile(G4String Name); // Creating a ROOT file for output with the filename "Name"

 
  G4String ActualVolume, NextVolume; // At start of step, the names of the actual volume
  //an the next volume are stored here
  G4Track *actualTrack; // pointer to the actual track
    
  TFile *rootfile; // the ROOT file for output

  HistoManager* fHistoManager  = nullptr;
  // G4Run *Run; // pointer to the current Run

  //  G4String  FileNameSuffix;	


  // TTree *eventtree;
  // TTree *hittree;
  // TTree *settree;
  // UTEvent event; 
  // UThit hit; 
  // UTSet set; 

  // string to store a commando to draw a track in LISP for AutoCAD
};

#endif

