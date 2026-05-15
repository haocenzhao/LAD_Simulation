#ifndef LADVariables_H
#define LADVariables_H 1

#include <iostream>
using namespace std;

#include "globals.hh"
#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"


class LADVariables
{
public:
  LADVariables();
  ~LADVariables();
  
  G4int LoadFromFile(G4String FileName);
  G4int SaveToFile(G4String FileName);


  G4int MultiTh;

  G4double centralWallAngle;
  G4double leftWallAngle;  // wrt to the centralWallAngle
  G4double rightWallAngle; // wrt to the centralWallAngle

  // this is the distance from the center of the hall to the front face of the wall
  G4double centralWallDistance; // wrt to the center of the hall
  G4double leftWallDistance;  // wrt to the centralWall
  G4double rightWallDistance; // wrt to the centralWallAngle
  G4bool G4GUI; // G4 terminal or GUI

  G4int GeneratorCase;
  G4String ScanParticle;
  G4double ScanMomentumParticle;

  G4String DISrootFile;
  
private:
	G4bool ReadBoolean(G4String Value);
	
};

ostream & operator << (ostream& s, const LADVariables v);

extern LADVariables *Variables;

#endif
