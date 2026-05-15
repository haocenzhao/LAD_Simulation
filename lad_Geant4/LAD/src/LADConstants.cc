#include "LADConstants.hh"

LADConstants::LADConstants()
{
  // In an ideal world, all of these could be read from a DB.
  // Here we use brute force. Normally, these guys are constant along the experiment
  
  
  // HODO SPECIFICATION AND  DIMENSIONS

  NoOfBars = 11;

  NoOfWalls = 3;
  NoOfSubWalls = 2;
  NoOfPanels = 5;

  
  
  BarThick = 5.08 *cm;
  BarWidth = 22.0 *cm;

// These numbers come from the NIM paper
// units in inches, should be converted to mm. (it is done in the constructor)
  KaptonThick = 0.0094;
  AluminumThick = 0.001;
  LeadThick = 0.005;

  WallSeparation = 40.6*cm;

  Barlength  = {387.5,
		393.3,
		399.0,
		404.8,
		410.5,
		416.3,
		422.0,
		427.8,				  
		433.6,
		439.3,
		445.1 };


  // Barlength  = {445.1,
  // 		439.3,
  // 		433.6,
  // 		427.8,
  // 		422.0,
  // 		416.3,
  // 		410.5,
  // 		404.8,
  // 		399.0,
  // 		393.3,
  // 		387.5 };

  

  
  
}

LADConstants::~LADConstants()
{
	G4cout << "<LADConstants::~LADConstants>: End Read" << G4endl;
}

LADConstants *Constants=NULL;
