//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef HistoManager_h
#define HistoManager_h 1

#include "globals.hh"
#include <vector>

#include <array>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

using namespace std;

class TFile;
class TTree;
class TH1D;

const G4int kMaxHisto = 4;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class HistoManager
{
public:
  HistoManager();// = default;
  ~HistoManager();

  void Book();
  void Save();

  void FillHisto(G4int id, G4double bin, G4double weight = 1.0);
  void Normalize(G4int id, G4double fac);

  void FillNtuple1(G4double energyAbs, G4double energyGap,
		  G4double trackLAbs, G4double trackLGap);

  void FillHodoEnergy(G4int ID);
  void FillHodoPosition(G4int ID);
  void FillGEM(G4int ID);

  void PrintStatistic();


  TFile *CreateSaveFile(G4String Name); // Creating a ROOT file for output with the filename "Name"

 

  void SetEDepTot(vector<G4double> v){vEneDep = v;}

  void SetPDG(vector<G4int> v){vPDG = v;}

  void SetLevel(vector<G4int> v){vLevel = v;}


  void SetPadID(vector<G4int> v){vPadCopy = v;}  
  //Need to modify how to store the data
  void SetPadPosition(vector<G4int> v){vPaddle = v;}
  void SetXbar(vector<G4double> v){vXbar = v;}
  void SetYbar(vector<G4double> v){vYbar = v;}
  void SetZbar(vector<G4double> v){vZbar = v;}
  void SetTbar(vector<G4double> v){vTbar = v;}//time
  void SetTrackID(vector<G4int> v){vTrackID = v;}



  // GEM Leaves
  void AddXloc(G4double v){vXloc.push_back(v);}
  void AddYloc(G4double v){vYloc.push_back(v);}
  void AddZloc(G4double v){vZloc.push_back(v);}

  void AddXglo(G4double v){vXglo.push_back(v);}
  void AddYglo(G4double v){vYglo.push_back(v);}
  void AddZglo(G4double v){vZglo.push_back(v);}
  
  void AddTchamber(G4double v){vTchamber.push_back(v);}//time
  void AddgLevel(G4int v){vgLevel.push_back(v);}
  void AddChamber(G4int v){vChamber.push_back(v);}
  void AddgPDG(G4int v){vgPDG.push_back(v);}


  
  
private:

  TFile *rootfile; // the ROOT file for output
  TTree *hodopos;
  TTree *hodoene;
  TTree *gemana;
  G4String  FileNameSuffix;	
  
  G4int fEventID = 0;
  G4double fEDepTot = 0 ;
  G4int fpaddle = 0;

  
  TFile* fRootFile = nullptr;
  std::array<TH1D*, kMaxHisto> fHisto = {nullptr, nullptr, nullptr, nullptr};
  TTree* fNtuple1 = nullptr;
  TTree* fNtuple2 = nullptr;

  G4double fEabs = 0.;
  G4double fEgap = 0.;
  G4double fLabs = 0.;
  G4double fLgap = 0.;


  // As I have the code now, I have three vectors in memory, with the same information
  vector<G4double> vEneDep;
  vector<G4int>    vPadCopy;
  vector<G4int>    vPDG;
  vector<G4int>    vLevel;
  vector<G4int>    vPaddle;


  vector<G4double> vXbar;
  vector<G4double> vYbar;
  vector<G4double> vZbar;
  vector<G4double> vTbar;
  vector<G4int> vTrackID;



  
  // These variables only for position at the bars
  vector<G4double> vXloc;
  vector<G4double> vYloc;
  vector<G4double> vZloc;

  vector<G4double> vXglo;
  vector<G4double> vYglo;
  vector<G4double> vZglo;
  
  vector<G4double> vTchamber;//time
  vector<G4int>    vgPDG;
  vector<G4int>    vgLevel;// same as vLevel but for pos-time purposes
  vector<G4int>    vChamber;

  // GEMs


  
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

