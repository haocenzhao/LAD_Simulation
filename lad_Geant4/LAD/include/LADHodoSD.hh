#ifndef LADHodoSD_h
#define LADHodoSD_h 1

#include "G4VSensitiveDetector.hh"

#include "LADHodoHit.hh"

#include <vector>

class G4Step;
class G4HCofThisEvent;

// namespace B4c
// {

/// Calorimeter sensitive detector class
///
/// In Initialize(), it creates one hit for each calorimeter layer and one more
/// hit for accounting the total quantities in all layers.
///
/// The values are accounted in hits in ProcessHits() function which is called
/// by Geant4 kernel at each step.

class LADHodoSD : public G4VSensitiveDetector
{
public:
  LADHodoSD(const G4String& name,
	    const G4String& hitsCollectionName,
	    G4int nofCells);
  ~LADHodoSD() override = default;

  // methods from base class
  void   Initialize(G4HCofThisEvent* hitCollection) override;
  G4bool ProcessHits(G4Step* step, G4TouchableHistory* history) override;
  void   EndOfEvent(G4HCofThisEvent* hitCollection) override;

private:
  LADHodoHitsCollection* fHitsCollection = nullptr;
  G4int fNofCells = 0;
};

//}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

