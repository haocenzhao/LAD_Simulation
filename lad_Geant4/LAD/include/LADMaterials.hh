#ifndef LADMaterials_H
#define LADMaterials_H 1
#include "globals.hh"
#include "G4SystemOfUnits.hh"
#include "G4NistManager.hh"
#include "G4PhysicalConstants.hh"


class G4Material;

class LADMaterials
{
public:
  
  LADMaterials();
  ~LADMaterials();

  G4Material*        defaultMaterial;
  G4Material*        Vacuum;
  G4Material*        absorberMaterial;
  G4Material*        gapMaterial;
  
  G4Material*        liquidArgon;

  G4Material * Al;

  //From Lucas/Tyler code
  G4Material * H2_gas;
  G4Material * D2_liquid;
  
  //Using NIST manager
  G4Material         *G4Pb;
  G4Element          *elH;
  G4Element          *elC;

  G4Material*        BC408;

  // GEMs materials
  G4Material* NEMAG10;
  G4Material* NOMEX_pure;
  G4Material* NOMEX;
  G4Material *Copper;
  G4Material *Kapton;
  G4Material *GEMgas;
  G4Material *Air;
  
private:

  G4int              verbose;

};

#endif
