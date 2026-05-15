//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include <TH1D.h>
#include <TFile.h>
#include <TTree.h>
#include <CLHEP/Units/SystemOfUnits.h>

#include "HistoManager.hh"
#include "G4UnitsTable.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

HistoManager::HistoManager()
{
  G4cout<<"HistoManager::HistoManager() STARTED"<<G4endl;
}


HistoManager::~HistoManager()
{
  delete fRootFile;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HistoManager::Book()
{
  // Creating a tree container to handle histograms and ntuples.
  // This tree is associated to an output file.
  //

  rootfile = CreateSaveFile("HodoFile");

  if (rootfile == nullptr)
    {
      G4cout << " HistoManager::Book :"
	     << " problem creating the ROOT TFile "
	     << G4endl;
      return;
    }
  
  
  
  hodopos = new TTree("hodoposition","Hodo Position");
  hodoene = new TTree("hodoenergy","Hodo Energy");

  gemana = new TTree("gemana","GEM analysis");
  
  // This method works, do we need structure? 
  // hodotree ->Branch("EventID",event.EventID,"EventID/I");
  // hodotree ->Branch("EDepTot",&event.EDepTot,"EDepTot/D");
  // hodotree ->Branch("paddle",&event.paddle,"paddle/I");



  
  hodopos -> Branch("vXbar", &vXbar );
  hodopos -> Branch("vYbar", &vYbar );
  hodopos -> Branch("vZbar", &vZbar );
  hodopos -> Branch("vTbar", &vTbar );
  hodopos -> Branch("vPaddle", &vPaddle );
  hodopos -> Branch("vTrackID",  &vTrackID );


  hodoene -> Branch("EventID",   &fEventID,"EventID/I");
  hodoene -> Branch("vPadCopy", &vPadCopy );
  hodoene -> Branch("vEneDep",  &vEneDep );
  hodoene -> Branch("vPDG",     &vPDG );
  hodoene -> Branch("vLevel",   &vLevel );



  // GEM 
  //local coordinates
  gemana -> Branch("vXloc", &vXloc );
  gemana -> Branch("vYloc", &vYloc );
  gemana -> Branch("vZloc", &vZloc );
  //global coordinates
  gemana -> Branch("vXglo", &vXglo );
  gemana -> Branch("vYglo", &vYglo );
  gemana -> Branch("vZglo", &vZglo );
  
  gemana -> Branch("vTchamber", &vTchamber );
  gemana -> Branch("vChamber", &vChamber );
  gemana -> Branch("vgPDG",  &vgPDG );
  gemana -> Branch("vgLevel",  &vgLevel );



  
  
  // create 1st ntuple
  // fNtuple1 = new TTree("Ntuple1", "Edep");
  // fNtuple1->Branch("Eabs", &fEabs, "Eabs/D");
  // fNtuple1->Branch("Egap", &fEgap, "Egap/D");

  // // create 2nd ntuple
  // fNtuple2 = new TTree("Ntuple2", "TrackL");
  // fNtuple2->Branch("Labs", &fLabs, "Labs/D");
  // fNtuple2->Branch("Lgap", &fLgap, "Lgap/D");

 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// This method is called by EndOfRunAction
void HistoManager::Save()
{
  if (rootfile == nullptr) { return;
  }
  
  hodopos->Write();
  hodoene->Write();
 
  rootfile->Write();        // Writing the histograms to the file
  rootfile->Close();  // and closing the tree (and the file)

 

  G4cout << "\n----> Histograms and ntuples are saved\n" << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HistoManager::FillHisto(G4int ih, G4double xbin, G4double weight)
{
  if (ih >= kMaxHisto) {
    G4cerr << "---> warning from HistoManager::FillHisto() : histo " << ih
           << " does not exist. (xbin=" << xbin << " weight=" << weight << ")"
           << G4endl;
    return;
  }
  if  (fHisto[ih] != nullptr) { fHisto[ih]->Fill(xbin, weight); }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HistoManager::Normalize(G4int ih, G4double fac)
{
  if (ih >= kMaxHisto) {
    G4cout << "---> warning from HistoManager::Normalize() : histo " << ih
           << " does not exist. (fac=" << fac << ")" << G4endl;
    return;
  }
  if (fHisto[ih] != nullptr) {
    fHisto[ih]->Scale(fac);
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HistoManager::FillNtuple1(G4double energyAbs, G4double energyGap,
                              G4double trackLAbs , G4double trackLGap )
{
  fEabs = energyAbs;
  fEgap = energyGap;
  fLabs = trackLAbs;
  fLgap = trackLGap;

  G4cout<<"***EVentID: "<<fEabs <<" EDepTot: "<<fEgap <<" paddle: "<< fLabs <<G4endl;
  
  // if (fNtuple1 != nullptr) { fNtuple1->Fill(); }
  // if (fNtuple2 != nullptr) { fNtuple2->Fill(); }
}








void HistoManager::FillHodoEnergy(G4int ID)
{
  G4cout<<"Filling NTuples"<<G4endl;
  //  G4cout<<"EVentID: "<< fEventID<<" EDepTot: "<< fEDepTot <<" paddle: "<< fpaddle<<G4endl;

  fEventID = ID;
  
  G4cout<<"***EVentID (finished): "<<ID<<G4endl;
    

  // The variables are filled in the corresponding detector classes.
  hodoene->Fill();
    
  G4cout<<"Filled Energy NTuples"<<G4endl;

  vPadCopy.clear();
  vEneDep.clear();
  vPDG.clear();
  vLevel.clear();


}



void HistoManager::FillHodoPosition(G4int ID)
{
  G4cout<<"Filling NTuples"<<G4endl;
  //  G4cout<<"EVentID: "<< fEventID<<" EDepTot: "<< fEDepTot <<" paddle: "<< fpaddle<<G4endl;

  fEventID = ID;
  
  G4cout<<"***EVentID: "<<ID<<G4endl;
    
  // The variables are filled in the corresponding detector classes.
  hodopos->Fill();
    
  G4cout<<"Filled Position NTuples"<<G4endl;

  vXbar.clear();
  vYbar.clear();
  vZbar.clear();
  vTbar.clear();
  vPaddle.clear();
  vTrackID.clear();

}






void HistoManager::FillGEM(G4int ID)
{
  G4cout<<"Filling GEM"<<G4endl;
  //  G4cout<<"EVentID: "<< fEventID<<" EDepTot: "<< fEDepTot <<" paddle: "<< fpaddle<<G4endl;

  fEventID = ID;
  
  G4cout<<"***EVentID: "<<ID<<G4endl;
    
  // The variables are filled in the corresponding detector classes.
  gemana ->Fill();
    
  G4cout<<"Filled GEM leaves"<<G4endl;

  
  vXloc.clear();
  vYloc.clear();
  vZloc.clear();

  vXglo.clear();
  vYglo.clear();
  vZglo.clear();

  vTchamber.clear();
  vgLevel.clear();
  vChamber.clear();
  vgPDG.clear();
  
}



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HistoManager::PrintStatistic()
{
  G4cout << "\n ----> print histograms statistic \n" << G4endl;
  for (auto h1 : fHisto) {
    const G4String name = h1->GetName();

    G4String unitCategory;
    if (name[0] == 'E' ) { unitCategory = "Energy"; }
    if (name[0] == 'L' ) { unitCategory = "Length"; }

    G4cout << name
           << ": mean = " << G4BestUnit(h1->GetMean(), unitCategory)
           << " rms = " << G4BestUnit(h1->GetRMS(), unitCategory )
           << G4endl;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......




TFile *HistoManager::CreateSaveFile(G4String Name)
{

  G4cout<<"IN createsave"<<G4endl;

  // Perhaps later I implement an enviroment variable for this
  //  G4String pfad(getenv("KAOSDATADIR"));
    
  //    G4String pfad("./rootfiles/");
        
  G4String dataname;

  char buffer[100];
  time_t Now = time(0);	
  strftime((char*)&buffer,100,"%Y-%m-%d-%H-%M-%S",localtime(&Now));
  FileNameSuffix = buffer;

  FileNameSuffix = Name+ "_" + FileNameSuffix+".root";

  TFile *rootoutfile = new TFile(FileNameSuffix, "recreate");


  G4cout << "\n----> Output file is open in " << FileNameSuffix << G4endl;
  
  return rootoutfile;
}
