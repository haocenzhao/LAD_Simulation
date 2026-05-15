#include "LADDetectorConstruction.hh"
#include "LADMaterials.hh"

#include "G4Material.hh"
#include "G4NistManager.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4GlobalMagFieldMessenger.hh"
#include "G4AutoDelete.hh"

#include "G4SDManager.hh"
#include "G4SDChargedFilter.hh"
#include "G4MultiFunctionalDetector.hh"
#include "G4VPrimitiveScorer.hh"
#include "G4PSEnergyDeposit.hh"
#include "G4PSTrackLength.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"



LADDetectorConstruction::LADDetectorConstruction()
 {
  Materials = new LADMaterials();
  HodoCreator = new LADDetectorConstructionHodoCreator();
  
 }

 
LADDetectorConstruction::~LADDetectorConstruction()
 {} 
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ThreadLocal
G4GlobalMagFieldMessenger* LADDetectorConstruction::fMagFieldMessenger = nullptr;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Note that this method is called by the Geant4 engine. It is mandatory!
G4VPhysicalVolume* LADDetectorConstruction::Construct()
{
   return DefineVolumes();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* LADDetectorConstruction::DefineVolumes()
{
  auto  worldSizeXY = 40*m;
  auto  worldSizeZ  = 40*m;
  
  //
  // World
  //
  auto worldS
    = new G4Box("World",           // its name
                 worldSizeXY/2, worldSizeXY/2, worldSizeZ/2); // its size

  auto worldLV
    = new G4LogicalVolume(
                 worldS,           // its solid
		 //                 Materials->defaultMaterial,  // vacuum
		 Materials->Air,  // G4_Air
                 "World");         // its name

  auto worldPV = new G4PVPlacement(nullptr,  // no rotation
    G4ThreeVector(),                         // at (0,0,0)
    worldLV,                                 // its logical volume
    "World",                                 // its name
    nullptr,                                 // its mother  volume
    false,                                   // no boolean operation
    0,                                       // copy number
    fCheckOverlaps);                         // checking overlaps


  G4cout<<"Building Hodoscope"<<G4endl;
  //HodoCreator = new LADDetectorConstructionHodoCreator(worldLV, Materials);

  HodoCreator -> BuildHodo(worldLV, Materials);

  GEMCreator = new LADDetectorConstructionGEMCreator();
  GEMCreator ->BuildGEM(worldLV, Materials);


/*
 //----------------------------------------------------------------------------------------------------------
  // TARGET CHAMBER

  double r_chamber = 40. * cm;
  double t_chamber = 0.406 * mm;

  G4Tubs *target_mother_tubs = 
          new G4Tubs("targ_mother_tubs", 
                      0., 
                      50. * cm, 
                      50. * cm, 
                      0. * deg, 
                      360. * deg);

  G4LogicalVolume *target_mother_log =
          new G4LogicalVolume(target_mother_tubs,  
                            Materials-> Vacuum,
                            "target_mother_logical", 0, 0, 0);
  
  G4RotationMatrix *rotX_neg90 = new G4RotationMatrix();
  rotX_neg90->rotateX(-90. * deg);
  G4RotationMatrix *rotX_pos90 = new G4RotationMatrix();
  rotX_pos90->rotateX(90. * deg);

  G4VPhysicalVolume *target_mother_phys =
          new G4PVPlacement(rotX_neg90, 
                            G4ThreeVector(), 
                            target_mother_log, 
                            "target_mother_physical", 
                            worldLV, 
                            false, 
                            0);

  // Target chamber exit window
  G4Tubs *chamber_tubs = 
          new G4Tubs("chamber_tubs", 
                      r_chamber, 
                      r_chamber + t_chamber, 
                      20. * cm, 
                      0. * deg, 
                      360. * deg);
 
  G4LogicalVolume *chamber_log = 
          new G4LogicalVolume(chamber_tubs, 
                              Materials->Al, 
                              "chamber_logical", 0, 0, 0);
  
  // // original
  G4VPhysicalVolume *chamber_phys =
          new G4PVPlacement(0, 
                            G4ThreeVector(), 
                            chamber_log, 
                            "chamber_physical", 
                            target_mother_log, 
                            false, 
                            0);



  G4double length = 20.0 * cm;
  G4double radius = 0.5 * cm;
  G4double window = 0.125 * mm;
  G4double wall   = 0.2 * mm;
  wall = 1.0 * mm;



  // Target cell
  G4Tubs *cell_tubs            = new G4Tubs("cell_tubs", 0., radius + wall, length / 2. + window, 0. * deg, 360. * deg);
  G4Tubs *cell_empt            = new G4Tubs("cell_empt", 0., radius, length / 2., 0. * deg, 360. * deg);
  G4SubtractionSolid *cell_sub = new G4SubtractionSolid("cell_sub", cell_tubs, cell_empt);
  // G4LogicalVolume *cell_log    = new G4LogicalVolume(cell_sub, fMaterial->vacuum, "cell_logical", 0, 0, 0);
  G4LogicalVolume *cell_log = new G4LogicalVolume(cell_sub, Materials->Al, "cell_logical", 0, 0, 0); //
  // original
  G4VPhysicalVolume *cell_phys =
      new G4PVPlacement(rotX_pos90, G4ThreeVector(), cell_log, "cell_physical", target_mother_log, false, 0);


 // Target liquid volume
  G4Tubs *liquid_tubs         = new G4Tubs("liquie_tubs", 0., radius, length / 2., 0. * deg, 360. * deg);
  // G4LogicalVolume *liquid_log = new G4LogicalVolume(liquid_tubs, fMaterial->vacuum, "liquid_logical", 0, 0, 0);
  G4LogicalVolume *liquid_log = new G4LogicalVolume(liquid_tubs, Materials->D2_liquid, "liquid_logical", 0, 0, 0); //
  // original
  G4VisAttributes *liquid_vis = new G4VisAttributes(G4Colour(0., 0., 1.));
  liquid_log->SetVisAttributes(liquid_vis);
  G4VPhysicalVolume *liquid_phys =
      new G4PVPlacement(rotX_pos90, G4ThreeVector(), liquid_log, "liquid_physical", target_mother_log, false, 0);

// Why was this line here?
//G4VPhysicalVolume *target_mother_phys =
//   new G4PVPlacement(rotX_neg90, G4ThreeVector(), target_mother_log, "target_mother_physical", worldLV, false, 0);

//target_mother_log->SetVisAttributes(G4VisAttributes(G4Colour::Grey()));
target_mother_log->SetVisAttributes(G4VisAttributes::GetInvisible());
chamber_log->SetVisAttributes(G4VisAttributes(G4Colour::Red()));
cell_log->SetVisAttributes(G4VisAttributes(G4Colour::Grey()));

///----------------------------------------------------------------------------------------------------------
// Target chamber end
///----------------------------------------------------------------------------------------------------------
  
*/

worldLV->SetVisAttributes(G4VisAttributes::GetInvisible());
return worldPV;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void LADDetectorConstruction::ConstructSDandField()
{
  G4SDManager::GetSDMpointer()->SetVerboseLevel(1);


  GEMCreator -> ConstructSDandField();
  //
  // Scorers
  //
  //  G4cout<<"LADDetectorConstruction::ConstructSDandField() Calling HODO"<<G4endl;
  // HodoCreator -> ConstructSDandField();
  // // declare Absorber as a MultiFunctionalDetector scorer
  // //
  // auto absDetector = new G4MultiFunctionalDetector("Absorber");
  // G4SDManager::GetSDMpointer()->AddNewDetector(absDetector);

  // G4VPrimitiveScorer* primitive;
  // primitive = new G4PSEnergyDeposit("Edep");
  // absDetector->RegisterPrimitive(primitive);

  // primitive = new G4PSTrackLength("TrackLength");
  // auto charged = new G4SDChargedFilter("chargedFilter");
  // primitive ->SetFilter(charged);
  // absDetector->RegisterPrimitive(primitive);

  // // SetSensitiveDetector("AbsoLV",absDetector);

  // // declare Gap as a MultiFunctionalDetector scorer
  // //
  // auto gapDetector = new G4MultiFunctionalDetector("Gap");
  // G4SDManager::GetSDMpointer()->AddNewDetector(gapDetector);

  // primitive = new G4PSEnergyDeposit("Edep");
  // gapDetector->RegisterPrimitive(primitive);

  // primitive = new G4PSTrackLength("TrackLength");
  // primitive ->SetFilter(charged);
  // gapDetector->RegisterPrimitive(primitive);

  // //  SetSensitiveDetector("GapLV",gapDetector);

  // //
  // // Magnetic field
  // //
  // // Create global magnetic field messenger.
  // // Uniform magnetic field is then created automatically if
  // // the field value is not zero.
  // G4ThreeVector fieldValue;
  // fMagFieldMessenger = new G4GlobalMagFieldMessenger(fieldValue);
  // fMagFieldMessenger->SetVerboseLevel(1);

  // // Register the field messenger for deleting
  // G4AutoDelete::Register(fMagFieldMessenger);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

