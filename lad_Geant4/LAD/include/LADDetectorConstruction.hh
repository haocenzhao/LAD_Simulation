#ifndef LADDetectorConstruction_h
#define LADDetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

#include "LADDetectorConstructionHodoCreator.hh"
#include "LADDetectorConstructionGEMCreator.hh"
#include "G4SubtractionSolid.hh"

class G4VPhysicalVolume;
class G4GlobalMagFieldMessenger;

class LADMaterials;


/// Detector construction class to define materials and geometry.
/// The calorimeter is a box made of a given number of layers. A layer consists
/// of an absorber plate and of a detection gap. The layer is replicated.
///
/// Four parameters define the geometry of the calorimeter :
///
/// - the thickness of an absorber plate,
/// - the thickness of a gap,
/// - the number of layers,
/// - the transverse size of the calorimeter (the input face is a square).
///
/// In ConstructSDandField() sensitive detectors of G4MultiFunctionalDetector
/// type with primitive scorers are created and associated with the Absorber
/// and Gap volumes.  In addition a transverse uniform magnetic field is defined
/// via G4GlobalMagFieldMessenger class.

class LADDetectorConstruction : public G4VUserDetectorConstruction
{
  public:
  LADDetectorConstruction();
  ~LADDetectorConstruction();

  public:
    G4VPhysicalVolume* Construct() override;
  void ConstructSDandField() override;

  private:

  LADMaterials *Materials;
  LADDetectorConstructionHodoCreator *HodoCreator;
  LADDetectorConstructionGEMCreator *GEMCreator;






  // methods
  //
  G4VPhysicalVolume* DefineVolumes();
  
  // data members
  //
    static G4ThreadLocal G4GlobalMagFieldMessenger*  fMagFieldMessenger;
  // magnetic field messenger
  
  G4bool fCheckOverlaps = true; // option to activate checking of volumes overlaps
};


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
