#ifndef LADDetectorConstructionHodoCreator_H
#define LADDetectorConstructionHodoCreator_H 1
#include "globals.hh"

#include "G4SystemOfUnits.hh"

#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4Transform3D.hh"
#include "G4RotationMatrix.hh"
#include <vector>
// #include "G4VSensitiveDetector.hh"
// #include "LADHodoSD.hh"
#include "LADMaterials.hh"



using namespace std;


class G4VPhysicalVolume;

//class LADMaterials;


class LADDetectorConstructionHodoCreator
{
public:
  
   LADDetectorConstructionHodoCreator();
  ~LADDetectorConstructionHodoCreator();
  
  void BuildHodo(G4LogicalVolume *worldLV, LADMaterials *Materials);
  void ConstructSDandField();
  static const G4double inch;

private:

  G4double thick;
  G4double width;
  G4double length; // the largest Bar
  

  G4double KaptonThick;
  G4double AluminumThick;
  G4double LeadThick;

  G4double KaptonBoxThick;

   
  
  //A container to place the scintillator Bars
  // static const G4int NoOfWalls = 5;
  // static const G4int NoOfPanels = 3;
  
  G4int SubPanels;
  
  G4double WallWidth; //The total width of the Bars WITHOUT separation + 1 cm space
  G4double WallLength;
  G4double WallThick;

  G4Box *wall;
 
  G4VPhysicalVolume *wallPV;

  // Placement of the double walls
  G4double wallSep;

  G4double angleW[3];//{150*deg,
  // 		       (150-23)*deg,
  // 		       (150-23-23)*deg};

  G4double distanceW[3]; // = {6.15*m,
			 //   5.23*m,
			 //   6.15*m};

  // A set of vector containers needed to translate and rotate the walls
  vector<G4double> vSeparationX;
  vector<G4double> vSeparationY;
  vector<G4double> vSeparationZ;

  // These are the separation between each of the double walls
  vector<G4double> vSpaceX;
  vector<G4double> vSpaceY;
  vector<G4double> vSpaceZ;

    
  // The rotation of each of the walls  
  G4RotationMatrix rmW0;
  G4RotationMatrix rmW1;
  G4RotationMatrix rmW2;
  G4RotationMatrix rmW3;
  G4RotationMatrix rmW4;

  vector<G4RotationMatrix> rmW;

  G4Box* Kapton; //solid
  G4LogicalVolume* KaptonLV;//logic volume

  G4Box* LeadPlate; //solid
  G4LogicalVolume* LeadPlateLV;//logic volume

  G4Box* AlumSheets;
  G4LogicalVolume* AlumSheetsLV;//logic volume


  G4Box* BarS; //solid
  G4LogicalVolume* BarLV;//logic volume

 
  G4bool fCheckOverlaps = false; // option to activate checking of volumes overlaps
  
};

#endif
