#ifndef LADConstants_H
#define LADConstants_H 1

#include "globals.hh"
#include "G4ThreeVector.hh"
#include <vector>
#include "G4SystemOfUnits.hh"

using namespace std;

class LADConstants
{
public:
  LADConstants();
  ~LADConstants();

  G4int NoOfBars; // number of scintillator paddles per wall (Before it was declared static const)
  G4int NoOfWalls;
  G4int NoOfSubWalls;
  G4int NoOfPanels;

  G4double KaptonThick;
  G4double AluminumThick;
  G4double LeadThick;
  
  G4double BarThick;
  G4double BarWidth;
  G4double WallSeparation;
  
  vector<G4double> Barlength;
 
	
};

extern LADConstants *Constants;

#endif
