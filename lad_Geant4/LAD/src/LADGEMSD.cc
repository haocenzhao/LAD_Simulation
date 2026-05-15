#include "LADGEMSD.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// LADGEMSD::LADGEMSD(const G4String& name,
//                              const G4String& hitsCollectionName,
//                              G4int nofCells)
//  : G4VSensitiveDetector(name),
//    fNofCells(nofCells)
// {
//   G4cout << "<LADGEMSD> Info" << G4endl; 
//   collectionName.insert(hitsCollectionName);
// }


//DriftChamberSD::DriftChamberSD(G4String name
LADGEMSD::LADGEMSD(G4String name)
: G4VSensitiveDetector(name)
{
  G4cout << "<LADGEMSD> Info" << G4endl; 
  collectionName.insert("LADGEMHitsCollection");
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void LADGEMSD::Initialize(G4HCofThisEvent* hce)
{
  G4cout << "<LADGEMSD> Initialize" << G4endl;


  fHitsCollection
    = new LADGEMHitsCollection(SensitiveDetectorName,collectionName[0]);

  if (fHCID<0) {
     fHCID = G4SDManager::GetSDMpointer()->GetCollectionID(fHitsCollection);
  }
  hce->AddHitsCollection(fHCID,fHitsCollection);

  /*
  
  // Create hits collection
  fHitsCollection
    = new GEMHitsCollection(SensitiveDetectorName, collectionName[0]);

  // Add this collection in hce
  auto hcID
    = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
  hce->AddHitsCollection( hcID, fHitsCollection );

  // The number of cell == the number of SD layers in the GEM
  //  G4cout<<"fNofCells: "<<fNofCells<<G4endl;
  
  // Create hits
  // fNofCells for cells + one more for total sums
  for (G4int i=0; i<fNofCells+1; i++ )
    {
      fHitsCollection->insert(new LADGEMHit()); //<----- THIS GUY WAS CAUSING PROBLEMS!!!
    }
  */
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool LADGEMSD::ProcessHits(G4Step* step,
                                     G4TouchableHistory*)
{
 
  G4cout << "<LADGEMSD> ProcessHits" << G4endl;

  // From Example B5
  // This is a driftchamber SD
  auto charge = step->GetTrack()->GetDefinition()->GetPDGCharge();
  if (charge==0.) return true;

  auto preStepPoint = step->GetPreStepPoint();

  auto touchable = step->GetPreStepPoint()->GetTouchable();
  auto motherPhysical = touchable->GetVolume(1); // mother
  auto copyNo = motherPhysical->GetCopyNo();

  
  auto worldPos = preStepPoint->GetPosition();
  auto localPos
    = touchable->GetHistory()->GetTopTransform().TransformPoint(worldPos);

  // energy deposit
  auto edep = step->GetTotalEnergyDeposit();


  G4int ParticleLevel = step->GetTrack()-> GetTrackID();
  G4int PDGID = step->GetTrack()->GetDefinition()-> GetPDGEncoding();
  
  auto hit = new LADGEMHit(copyNo);


  // this is similar to the SteppingAction analysis
  // But I don't know how this behave with multiple
  // interactions in a given event
  
  hit->SetWorldPos(worldPos);
  hit->SetLocalPos(localPos);
  hit->SetTime(preStepPoint->GetGlobalTime());

  hit->Add(edep);

  hit->SetPDG(PDGID);
  hit->SetLevel(ParticleLevel);

  fHitsCollection->insert(hit);

  return true;


  /*
  
  G4cout<< "1"<<G4endl;  
  
  // energy deposit
  auto edep = step->GetTotalEnergyDeposit();

  G4cout<< "2 "<< edep<<G4endl;
  
  // step length
  G4double stepLength = 0.;
  if ( step->GetTrack()->GetDefinition()->GetPDGCharge() != 0. ) {
    stepLength = step->GetStepLength();
  }


  if ( edep==0. && stepLength == 0. )
    {
      return false;
    }

  
  auto touchable = (step->GetPreStepPoint()->GetTouchable());
  G4cout<< "3"<<G4endl;
  // Get calorimeter cell id
  auto layerNumber = touchable->GetReplicaNumber(1);
  G4cout<< "4: "<<layerNumber<<G4endl;

  
  // Get hit accounting data for this cell
  auto hit = (*fHitsCollection)[layerNumber];


  if ( ! hit ) {
    G4ExceptionDescription msg;
    msg << "Cannot access hit " << layerNumber;
    G4Exception("LADGEMSD::ProcessHits()",
      "MyCode0004", FatalException, msg);
  }

  // Get hit for total accounting
  auto hitTotal
    = (*fHitsCollection)[fHitsCollection->entries()-1];


  G4cout<<"entries: "<<fHitsCollection->entries()<<G4endl;
  // Add values
  hit->Add(edep, stepLength);
  hitTotal->Add(edep, stepLength);
  G4cout<<  hit->GetEdep()<<" "<<  hitTotal->GetEdep()<<G4endl;
  return true;
  */
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void LADGEMSD::EndOfEvent(G4HCofThisEvent*)
{
  G4cout << "<LADGEMSD> EndOfEvent" << G4endl; 

  
  //  if ( verboseLevel>1 )
    {
     auto nofHits = fHitsCollection->entries();
     G4cout
       << G4endl
       << "-------->Hits Collection: in this event they are " << nofHits
       << " hits in the tracker chambers: " << G4endl;
     for ( std::size_t i=0; i<nofHits; ++i ) (*fHitsCollection)[i]->Print();
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


