#include "LADEventAction.hh"
// #include "LADHodoSD.hh"
// #include "LADHodoHit.hh"
#include "LADVariables.hh"


#include "G4RunManager.hh"
#include "G4Event.hh"
// #include "G4SDManager.hh"
// #include "G4HCofThisEvent.hh"
#include "G4UnitsTable.hh"

#include "Randomize.hh"
#include <iomanip>



// Don't ask!! no clue how this piece of code works.
// I would like to modify it to make it more readable
// I understand it is the getter for the GEMs hitcollection
// but not sure

namespace {

  // Utility function which finds a hit collection with the given Id
  // and print warnings if not found
  G4VHitsCollection* GetHC(const G4Event* event, G4int collId) {
    auto hce = event->GetHCofThisEvent();
    if (!hce) {
      G4ExceptionDescription msg;
      msg << "No hits collection of this event found." << G4endl;
      G4Exception("EventAction::EndOfEventAction()",
                  "Code001", JustWarning, msg);
      return nullptr;
    }

    auto hc = hce->GetHC(collId);
    if ( ! hc) {
      G4ExceptionDescription msg;
      msg << "Hits collection " << collId << " of this event not found." << G4endl;
      G4Exception("EventAction::EndOfEventAction()",
		  "Code001", JustWarning, msg);
    }
    return hc;
  }

}











//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

LADEventAction::LADEventAction(LADRunAction* RunAction, HistoManager* histo, HodoAnalysis* HodoHandle)
  : fRunAct(RunAction)
{
  G4cout<<"LADEventAction::LADEventAction()"<<G4endl;

  // I have to pay more attention to the initializers. 
  // Not initializing the HistoManager was giving me problems
  // This is a way to initialize!
  if (histo!=NULL) fHistoManager=histo;

  if (HodoHandle!=NULL) AnalysisHodo=HodoHandle;


  // This make me think if I need to carry all the machinery of the handlers as I've been doing
  // G4cout<<">>>>>>>>>>>>>LUND generator (EA)<<<<<<<<<<<"<<Variables->GeneratorCase<<G4endl;
  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

LADEventAction::~LADEventAction()
{
  G4cout<<"LADEventAction::~LADEventAction()"<<G4endl;




}

// MY GOAL is having the Sensitive Detector machinery working, but 
// for some reason I've been unsuscessful.
// I keep these pieces of code for future improvements

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
/*
  LADHodoHitsCollection* 
  LADEventAction::GetHitsCollection(G4int hcID,
  const G4Event* event) const
  {
  G4cout<<"LADEventAction::GetHitsCollection:hcID "<<hcID<<G4endl;
  G4cout<<"LADEventAction::GetHitsCollection "<< event -> GetEventID() <<G4endl;

  auto hitsCollection 
  = static_cast<LADHodoHitsCollection*>(
  event->GetHCofThisEvent()->GetHC(hcID));
  
  if ( ! hitsCollection ) {
  G4ExceptionDescription msg;
  msg << "Cannot access hitsCollection ID " << hcID; 
  G4Exception("LADEventAction::GetHitsCollection()",
  "MyCode0003", FatalException, msg);
  }         

  return hitsCollection;
  }    

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


  */
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void LADEventAction::BeginOfEventAction(const G4Event* event)
{
  G4cout<<"LADEventAction::BeginOfEventAction ---- START: "<< event -> GetEventID() <<G4endl;

  EventID = event -> GetEventID();  

  // vEnergyDep.clear();
  // vPadNumber.clear();

  // G4cout<<"LADEventAction::BoE GOING TO ANALYSIS "<< G4endl;
  AnalysisHodo->BeginOfEventAction(event);


  // GEM Analysis data
  auto sdManager = G4SDManager::GetSDMpointer();

  // I want to modify these lines in order to have explicit names of the variables
  array<G4String, kDim> dHCName
    = {{ "chamber1/LADGEMHitsCollection", "chamber2/LADGEMHitsCollection" }};
 
  for (G4int iDet = 0; iDet < kDim; ++iDet)
    {
      fGEMHCID[iDet] = sdManager->GetCollectionID(dHCName[iDet]);
    }



 
 
   
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void LADEventAction::EndOfEventAction(const G4Event* event)
{
  G4cout<<"LADEventAction::EndOfEventAction **** "<< event -> GetEventID() <<G4endl;
  auto eventID = event->GetEventID();

  
  if(Variables->GeneratorCase == 2)
    {
      LundRead -> Clear(); 
    }

  auto printModulo = G4RunManager::GetRunManager()->GetPrintProgress();
  if ( ( printModulo > 0 ) && ( eventID % printModulo == 0 ) ) {
    G4cout << "---> End of event: " << eventID << G4endl;     
    
  }  

  // honestly, I don't like that for a particular case (LUND), I have to create a condition to general code (store),
  // when the case is not even involved. I leave as is, probably, it won't be needed. 
  
  //  if(eventID % printModulo == 0 || 'LUND condition'  )
  {
    
    // The Hodoscope data is handled in the HodoAnalysis 
    // G4cout<<"LADEventAction::EoE GOING TO ANALYSIS "<< G4endl;
    AnalysisHodo->EndOfEventAction(event);

    // Here is where we can extract the information of the GEMs
    // I will try make vectors.
 
    for (G4int iDet = 0; iDet < kDim; ++iDet)
      {
	auto hc = GetHC(event, fGEMHCID[iDet]);
	if ( ! hc ) return;
     
	auto nhit = hc->GetSize();
     
	for (unsigned long i = 0; i < nhit; ++i)
	  {
	    LADGEMHit *hit = static_cast<LADGEMHit*>(hc->GetHit(i));
	    G4ThreeVector localPos = hit->GetLocalPos();
	    G4ThreeVector worldPos = hit->GetWorldPos();

	    G4cout<<"Getting data: "<<localPos.x()<<", "<< localPos.y()<<", "<< hit->GetLevel()<<", "<<iDet<<G4endl;
	    fHistoManager -> AddXloc(localPos.x());
	    fHistoManager -> AddYloc(localPos.y());
	    fHistoManager -> AddZloc(localPos.z());

	    fHistoManager -> AddXglo(worldPos.x());
	    fHistoManager -> AddYglo(worldPos.y());
	    fHistoManager -> AddZglo(worldPos.z());


	    fHistoManager -> AddTchamber(hit -> GetTime());//time
	    fHistoManager -> AddgLevel(hit -> GetLevel() );
	    fHistoManager -> AddChamber(iDet);
	    fHistoManager -> AddgPDG( hit -> GetPDG() );

	 
	  }
      }
  
    fHistoManager -> FillGEM(eventID);
  }


  
}  


// void LADEventAction::FillEvent(G4int copy, G4double ene)
// {
// // Just filling the vectors with information from SteppingAction

//   vEnergyDep.push_back(ene);
//   vPadNumber.push_back(copy);
// }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
