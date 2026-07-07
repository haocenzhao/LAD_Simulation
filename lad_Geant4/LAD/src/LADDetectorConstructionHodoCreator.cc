#include "LADDetectorConstructionHodoCreator.hh"

#include "LADConstants.hh"
#include "LADVariables.hh"

#include "G4VisAttributes.hh"
#include "G4AssemblyVolume.hh"
#include "G4SubtractionSolid.hh"
#include "LADMaterials.hh"
//#include "G4SDManager.hh"

const G4double LADDetectorConstructionHodoCreator::inch = 25.4 * mm;


LADDetectorConstructionHodoCreator::LADDetectorConstructionHodoCreator()
{
  G4cout << "<LADDetectorConstructionHodoCreator> Info" << G4endl; 
}


void LADDetectorConstructionHodoCreator::BuildHodo(G4LogicalVolume *worldLV, LADMaterials *Materials)
{

  if ( ! Materials->defaultMaterial || ! Materials->absorberMaterial || ! Materials->gapMaterial ) {
    G4ExceptionDescription msg;
    msg << "Cannot retrieve materials already defined.";
    G4Exception("LADDetectorConstruction::DefineVolumes()",
		"MyCode0001", FatalException, msg);
  }

    
  // 11 Bars, but one frame has 10
  // Order changed for the construction. I could do it in the constructor, but this simpler

  thick  = Constants -> BarThick;// Z-axis
  width  = Constants -> BarWidth ; // X-axis
  length = Constants -> Barlength[Constants ->NoOfBars-1]*cm; // Y-axis
  //length = Constants -> Barlength[0]*cm;


// These are the dimensions of the Kapton, Aluminum and Lead sheets
  KaptonThick   = Constants -> KaptonThick *inch;
  AluminumThick = Constants -> AluminumThick *inch; 
  LeadThick     = Constants -> LeadThick *inch;

//  KaptonBoxThick = KaptonThick + (4*AluminumThick) + LeadThick; 

    
  //CONSTRUCTION OF THE WALL CONTAINING THE BARS

  // This extra cm is not well used. But in the future with more precise measurements,
  // the whole construction could be improved
 
  WallWidth = (Constants -> NoOfBars * (4*AluminumThick + 2*KaptonThick + width)) + 1*cm; //The total width of the Bars WITHOUT separation + 1 cm spac
 // WallWidth = (Constants -> NoOfBars * width) + 1*cm; //The total width of the Bars WITHOUT separation + 1 cm spac
  WallThick = thick + 1*cm;
  WallLength = length + 1*cm; // not cool! this only works if we have the Bars sorted from long to short


  G4cout<<"WallWidth: "<<WallWidth<<" WallLength: "<<WallLength<<G4endl;


  // This is VERY simplistic construction. In fact, the ID of each panel is not quite
  // well determined, thus it needs extra work on how to ID each Bar
  
  // In principle, wall containers have the same dimensions 
  wall
    = new G4Box("Wall",                                  // its name
		WallWidth/2 ,WallLength/2, WallThick/2); // its size 
  
  G4cout<<"WL: "<< WallLength/m<<" WW: "<< WallWidth/m<<" WT: "<< WallThick/m<<G4endl;
    
  
  wallSep = Constants -> WallSeparation;
  
  // The reference system is:
  // Z: direction of the beam (downstream)
  // Y: up to the hall
  // X: to the left looking downstream 

  // the central detector is at 127deg
  angleW[0] = (Variables -> centralWallAngle) + (Variables -> leftWallAngle);
  angleW[1] =  Variables -> centralWallAngle;
  angleW[2] = (Variables -> centralWallAngle) - (Variables -> rightWallAngle);
   
  distanceW[0] = (Variables -> centralWallDistance) + (Variables -> leftWallDistance);
  distanceW[1] =  Variables -> centralWallDistance;
  distanceW[2] = (Variables -> centralWallDistance) + (Variables -> rightWallDistance);


  
  for(G4int ii = 0; ii < 3; ii++)
    {
      // Angles are measured from Z -> X direction

      vSeparationX.push_back(distanceW[ii]*sin(angleW[ii]) );
      vSeparationY.push_back(0);
      vSeparationZ.push_back( distanceW[ii]*cos(angleW[ii]) );

      vSpaceX.push_back( wallSep*sin(angleW[ii]) );
      vSpaceY.push_back(0);
      vSpaceZ.push_back( wallSep*cos(angleW[ii]) );
    }


  // Each wall has to face the target, so they need to be rotated
  // the same angle as the wall was displaced. Due to the nature
  // of the rotation function, it is necesary to do a rotation per wall
  // independently.

  // I think this part of the code could be compacted, but not sure (CA)
    
  // rmW0.rotateY(-30.*deg);
  // rmW1.rotateY(-30.*deg);
  // rmW2.rotateY(-(23+30)*deg);
  // rmW3.rotateY(-(23+30)*deg);
  // rmW4.rotateY(-(23+23+30)*deg);
  
  rmW0.rotateY(angleW[0]);
  rmW1.rotateY(angleW[0]);
  rmW2.rotateY(angleW[1]);
  rmW3.rotateY(angleW[1]);
  rmW4.rotateY(angleW[2]);

  
  rmW.push_back(rmW0);
  rmW.push_back(rmW1);
  rmW.push_back(rmW2);
  rmW.push_back(rmW3);
  rmW.push_back(rmW4);
   
  // NOMENCLATURE FOR LAD:
  // 3 WALLS with
  // 2 SUBWALLS for a total of
  // 5 PANELS

  G4LogicalVolume *wallLV[Constants ->NoOfPanels];
  
  // probably this line is stupid, since the construction of this is quite rigid regarding the name of subpanels.
  // nevertheless, I'll keep the same philosophy
  G4int SubWalls = Constants -> NoOfSubWalls;

  for (G4int ww = 0; ww < Constants ->NoOfWalls; ww++)
    {
      if(ww>1) SubWalls = 1; // because we have 2 double walls and 1 single.
      //Maybe a while loop could be more efficient
      
      G4cout<<"Wall "<<ww<<G4endl;

	      for (G4int ws = 0; ws < SubWalls; ws++)
			{
		  G4cout<<"SubWall "<<ws<<G4endl;

		  G4int panelIndex = ww + ws + ww;

		  wallLV[panelIndex]
		    = new G4LogicalVolume(
					  wall,                       // its solid
					  Materials->defaultMaterial, // its material
					  "SciWall");                 // its name

		  G4ThreeVector wallPosition =
		    G4ThreeVector( vSeparationX[ww], vSeparationY[ww], vSeparationZ[ww]) +
		    G4ThreeVector( vSpaceX[ww]*ws, vSpaceY[ww]*ws, vSpaceZ[ww]*ws ) +
		    Variables->hodoShift[ww][ws];

		  G4RotationMatrix alignRot;
		  alignRot.rotateX(Variables->hodoRotate[ww][ws].x());
		  alignRot.rotateY(Variables->hodoRotate[ww][ws].y());
		  alignRot.rotateZ(Variables->hodoRotate[ww][ws].z());

		  G4RotationMatrix wallRotation = alignRot * rmW[panelIndex];

		  wallPV = new G4PVPlacement(G4Transform3D(
							   wallRotation,
							   wallPosition),
					     wallLV[panelIndex],               // its logical volume
					     "SciWall",                      // its name
					     worldLV,                        // its mother  volumeAluminumThick
					     false,                          // no boolean operation
					     panelIndex,                      // copy number
					     fCheckOverlaps);

          BuildPanel3Frame(worldLV, Materials, panelIndex, wallPosition, wallRotation);

	  
	  // There is something magic calling with the same name each solid and logic
	  // volume, although they are different on each loop, without deleting the
	  // previous one
	  
	  for (G4int pp = 0; pp < Constants ->NoOfBars; pp++)
	    {
        Kapton = new G4Box("KaptonBox", // its name
			       2*AluminumThick + KaptonThick + width/2, 
             Constants ->Barlength[pp]/2*cm, 
              (2*KaptonThick + 4*AluminumThick + LeadThick + thick)/2); // its size

        KaptonLV = new G4LogicalVolume(Kapton,       // its solid
					                            Materials->BC408,  // its material
					                            "KaptonLV");         //
        KaptonLV->SetVisAttributes(G4VisAttributes(G4Colour::Magenta()));

        //Need to recall the Kapton box for each Bar
        new G4PVPlacement(nullptr,                                                    // no rotation
					                G4ThreeVector( ((WallWidth/2 - 2*AluminumThick + KaptonThick - width/2 -0.5*cm)- ((2*AluminumThick + KaptonThick + width/2)*2)*pp),0, 0),   // its position
					                KaptonLV,                                                   // its logical volume
					                "KaptonPhy",                                                // its name
					                wallLV[panelIndex],                                          // its mother volume
					                false,                                                      // no boolean operation
					                (ww*10000)+(ws*100)+pp,                                     // copy number (for layers around the bar won´t be necessaery)
					                fCheckOverlaps);                                            // checking overlaps
	      //NOTE: with this copy number, each Bar has an unique ID


        LeadPlate = new G4Box("LeadPlate", // its name
			     width/2, Constants ->Barlength[pp]/2*cm, LeadThick/2); // its size


        LeadPlateLV = new G4LogicalVolume(LeadPlate,       // its solid
					                            Materials -> G4Pb,  // its material
					                            "LeadPlateLV");         //
        LeadPlateLV->SetVisAttributes(G4VisAttributes(G4Colour::Cyan()));

        //Need to recall the Kapton box for each Bar
        new G4PVPlacement(nullptr,                                                    // no rotation
				        //        (2*KaptonThick + 4*AluminumThick + LeadThick + thick)/2)G4ThreeVector( ((WallWidth/2 - width/2)- width*pp), 0, KaptonBoxThick/2 -  LeadThick/2),   // its position
                        G4ThreeVector( 0, 0, 
                        -((2*KaptonThick + 4*AluminumThick + LeadThick + thick)/2) + KaptonThick + LeadThick/2),   // its position
                        LeadPlateLV,                                                   // its logical volume
				                "LeadPlatePhy",                                                // its name
				                KaptonLV,                                           // its mother volume
				                false,                                                      // no boolean operation
				                (ww*10000)+(ws*100)+pp,                                     // copy number (for layers around the bar won´t be necessaery)
				                fCheckOverlaps);      




        AlumSheets = new G4Box("AlumSheets", // its name
			     width/2 + AluminumThick, 
           Constants ->Barlength[pp]/2*cm, 
           (thick + 4*AluminumThick)/2); // its size


        AlumSheetsLV = new G4LogicalVolume(AlumSheets,       // its solid
					                                    Materials -> Al,  // its material
					                                    "AlumSheetsLV");         //

     
      AlumSheetsLV ->SetVisAttributes(G4VisAttributes(G4Colour::Yellow()));

        //Need to recall the Kapton box for each Bar
        new G4PVPlacement(nullptr,                                                    // no rotation
				                 G4ThreeVector( 0, 0, LeadThick/2),   // its position
                         AlumSheetsLV,                                                   // its logical volume
				                "AlumSheetsPhy",                                                // its name
				                KaptonLV,                                           // its mother volume
				                false,                                                      // no boolean operation
				                (ww*10000)+(ws*100)+pp,                                     // copy number (for layers around the bar won´t be necessaery)
				                fCheckOverlaps);      





	      BarS = new G4Box("Bar_box", // its name
			       width/2, Constants ->Barlength[pp]/2*cm,  thick/2); // its size
	      
	      BarLV = new G4LogicalVolume(BarS,       // its solid
					  Materials->BC408,  // its material
					  "Bar_LV");         // its name
	      
	      //     G4cout<<"pos: "<<width*pp<<" wall: "<<ww+ws<<G4endl;
       
	      // The bars are placed in the same direction as we build
	      // the rest of the walls clockwise
	     
	      new G4PVPlacement(nullptr,                                                    // no rotation
				//G4ThreeVector( ((WallWidth/2 - width/2)- width*pp),0, 0),   // its position
				G4ThreeVector(0,0,0),
        //-KaptonBoxThick/2 + AluminumThick+ LeadThick),
        BarLV,                                                      // its logical volume
				"Bar_phy",                                                  // its name
        AlumSheetsLV,
				//wallLV[ww+ws+ww],                                           // its mother volume
				false,                                                      // no boolean operation
				(ww*10000)+(ws*100)+pp,                                     // copy number
				fCheckOverlaps);                                            // checking overlaps
	      //NOTE: with this copy number, each Bar has an unique ID

	    }
	  if(ww+ws+ww == 5) break; // just 5 walls
	}


   
      //      wallLV[ww]->SetVisAttributes(G4VisAttributes::GetInvisible()); //(G4VisAttributes(G4Colour::Green()));
      
    }

      wallLV[0]->SetVisAttributes(G4VisAttributes(G4Colour::Green()));
      wallLV[1]->SetVisAttributes(G4VisAttributes(G4Colour::Blue()));
      wallLV[2]->SetVisAttributes(G4VisAttributes(G4Colour::Red()));
      wallLV[3]->SetVisAttributes(G4VisAttributes(G4Colour::Yellow()));
      wallLV[4]->SetVisAttributes(G4VisAttributes(G4Colour::White()));

    

  //    BarLV->SetVisAttributes(G4VisAttributes(G4VisAttributes::GetInvisible() ));//>SetVisAttributes(G4VisAttributes(G4Colour::Cyan()));
      return;
}


void LADDetectorConstructionHodoCreator::BuildPanel3Frame(G4LogicalVolume *worldLV,
                                                          LADMaterials *Materials,
                                                          G4int panelIndex,
                                                          const G4ThreeVector &hodoCenterHall,
                                                          const G4RotationMatrix &hodoRotationHall)
{
  // Build the frame in its own local coordinate system first.  The origin is the
  // nominal Panel 3 frame center; later this whole assembly is moved/rotated
  // relative to the corrected hodo wall position in hall coordinates.
  G4AssemblyVolume *frameAssembly = new G4AssemblyVolume();

  BuildMountingPads(frameAssembly, Materials);
  BuildVerticalLegs(frameAssembly, Materials);
  BuildDetectorMountTubes(frameAssembly, Materials);
  BuildGussets(frameAssembly, Materials);
  BuildChannels(frameAssembly, Materials);

  // The hodo wall rests against Panel 3; their centers should therefore differ
  // along the local wall-normal direction.  This first approximation is kept as
  // explicit variables because the item-8/channel interface will be refined
  // later:
  //
  //   center spacing = 1/2 * (hodo wall thickness + mount tube width)
  //                  + item-8 channel width
  //
  // Local Z is the hodo-wall thickness direction in the current wall geometry.
  // Flip the sign of panel3FrameOffsetZ if visualization shows the frame on the
  // wrong side of the hodo wall.
  const G4double hodoWallThickness = WallThick;
  const G4double mountTubeWidth = 6.0 * inch;
  const G4double item8ChannelWidth = 1.625 * inch; // Unistrut P1000 nominal width
  const G4double panel3FrameOffsetZ =
    0.5 * (hodoWallThickness + mountTubeWidth) + item8ChannelWidth;

  G4ThreeVector frameOffsetLocal(0.0, 0.0, panel3FrameOffsetZ);
  G4ThreeVector frameCenterHall =
    hodoCenterHall + hodoRotationHall * frameOffsetLocal;
  G4RotationMatrix *frameRotationHall = new G4RotationMatrix(hodoRotationHall);

  frameAssembly->MakeImprint(worldLV,
                             frameCenterHall,
                             frameRotationHall,
                             160000 + panelIndex * 100,
                             fCheckOverlaps);
}


void LADDetectorConstructionHodoCreator::BuildMountingPads(G4AssemblyVolume *frameAssembly,
                                                           LADMaterials *Materials)
{
  // Item 16: MOUNTING PAD, ASTM A36.  The drawing labels eight pads per frame.
  const G4double padSize      = 12.0 * inch;
  const G4double padThickness = 0.75 * inch;

  const G4double frameHeight = 216.0 * inch;
  const G4double legInnerClearance = 94.50 * inch;
  const G4double legOuterX = 4.0 * inch;
  const G4double legCenterX = 0.5 * (legInnerClearance + legOuterX);

  const G4double topBottomPadX = legCenterX;
  const G4double topBottomPadY = 0.5 * frameHeight - 0.5 * padThickness;
  const G4double padZ = 0.0 * inch;

  // The side pads are the same 12-inch square plates mounted vertically on the
  // outside face of the two vertical legs.  The +/-74.83-inch value is the
  // repeated upper/lower pad station shown from the frame centerline in the
  // Panel 3 view.
  const G4double sidePadX = topBottomPadX + 0.5 * legOuterX + 0.5 * padThickness;
  const G4double sidePadY = 74.83 * inch;

  G4Box *padSolid = new G4Box("Panel3MountingPadSolid",
                              padSize / 2.0,
                              padThickness / 2.0,
                              padSize / 2.0);

  G4LogicalVolume *padLV = new G4LogicalVolume(padSolid,
                                               Materials->Steel,
                                               "Panel3MountingPadLV");
  padLV->SetVisAttributes(G4VisAttributes(G4Colour(0.45, 0.45, 0.45)));

  G4ThreeVector topBottomPads[4] = {
    G4ThreeVector(-topBottomPadX, -topBottomPadY, padZ),
    G4ThreeVector( topBottomPadX, -topBottomPadY, padZ),
    G4ThreeVector(-topBottomPadX,  topBottomPadY, padZ),
    G4ThreeVector( topBottomPadX,  topBottomPadY, padZ)
  };

  for (G4int ii = 0; ii < 4; ii++) {
    frameAssembly->AddPlacedVolume(padLV, topBottomPads[ii], nullptr);
  }

  G4RotationMatrix *sidePadRotation = new G4RotationMatrix();
  sidePadRotation->rotateZ(90.0 * deg);

  G4ThreeVector sidePads[4] = {
    G4ThreeVector(-sidePadX, -sidePadY, padZ),
    G4ThreeVector(-sidePadX,  sidePadY, padZ),
    G4ThreeVector( sidePadX, -sidePadY, padZ),
    G4ThreeVector( sidePadX,  sidePadY, padZ)
  };

  for (G4int ii = 0; ii < 4; ii++) {
    frameAssembly->AddPlacedVolume(padLV, sidePads[ii], sidePadRotation);
  }
}


void LADDetectorConstructionHodoCreator::BuildVerticalLegs(G4AssemblyVolume *frameAssembly,
                                                           LADMaterials *Materials)
{
  // Item 9: VERTICAL LEG, 6 x 4 x .25 wall, ASTM A500 Grade B.
  // Model it as a hollow rectangular tube using a Boolean subtraction.
  const G4double padThickness = 0.75 * inch;

  const G4double frameHeight = 216.0 * inch;
  const G4double legInnerClearance = 94.50 * inch;

  const G4double legOuterX = 4.0 * inch;
  const G4double legOuterZ = 6.0 * inch;
  const G4double legWall   = 0.25 * inch;
  const G4double legLength = frameHeight - 2.0 * padThickness;

  const G4double legInnerX = legOuterX - 2.0 * legWall;
  const G4double legInnerZ = legOuterZ - 2.0 * legWall;

  G4Box *legOuterSolid = new G4Box("Panel3VerticalLegOuterSolid",
                                   legOuterX / 2.0,
                                   legLength / 2.0,
                                   legOuterZ / 2.0);

  // Make the cutter slightly longer than the tube to avoid coplanar Boolean faces.
  G4Box *legInnerSolid = new G4Box("Panel3VerticalLegInnerSolid",
                                   legInnerX / 2.0,
                                   legLength / 2.0 + 0.1 * mm,
                                   legInnerZ / 2.0);

  G4SubtractionSolid *legSolid =
    new G4SubtractionSolid("Panel3VerticalLegSolid",
                           legOuterSolid,
                           legInnerSolid,
                           nullptr,
                           G4ThreeVector());

  G4LogicalVolume *legLV = new G4LogicalVolume(legSolid,
                                               Materials->Steel,
                                               "Panel3VerticalLegLV");
  legLV->SetVisAttributes(G4VisAttributes(G4Colour(0.35, 0.35, 0.35)));

  const G4double legCenterX = 0.5 * (legInnerClearance + legOuterX);
  const G4double legCenterZ = 0.0 * inch;

  G4ThreeVector leftLegPosition(-legCenterX, 0.0, legCenterZ);
  G4ThreeVector rightLegPosition(legCenterX, 0.0, legCenterZ);

  frameAssembly->AddPlacedVolume(legLV, leftLegPosition, nullptr);
  frameAssembly->AddPlacedVolume(legLV, rightLegPosition, nullptr);
}


void LADDetectorConstructionHodoCreator::BuildDetectorMountTubes(G4AssemblyVolume *frameAssembly,
                                                                 LADMaterials *Materials)
{
  // Item 10: PANEL 3, DETECTOR MOUNT TUBE, 6 x 6 x .25 wall,
  // ASTM A500 Grade B.  There are two slanted tubes per Panel 3 frame.
  const G4double tubeOuter = 6.0 * inch;
  const G4double tubeWall  = 0.25 * inch;
  const G4double tubeInner = tubeOuter - 2.0 * tubeWall;

  const G4double padThickness = 0.75 * inch;
  const G4double frameHeight = 216.0 * inch;
  const G4double legInnerClearance = 94.50 * inch;
  const G4double legOuterX  = 4.0 * inch;
  const G4double legOuterZ  = 6.0 * inch;
  const G4double legLength  = frameHeight - 2.0 * padThickness;
  const G4double legCenterX = 0.5 * (legInnerClearance + legOuterX);

  const G4double tubeLength = 96.0625 * inch; // 96 1/16 from the Panel 3 drawing
  const G4double tubeAngle  = 7.35 * deg;
  const G4double tubeCenterY = 74.83 * inch;
  const G4double tubeCenterZ = 0.0 * inch;

  G4Box *tubeOuterSolid = new G4Box("Panel3DetectorMountTubeOuterSolid",
                                    tubeLength / 2.0,
                                    tubeOuter / 2.0,
                                    tubeOuter / 2.0);

  // Make the cutter slightly longer than the tube to avoid coplanar Boolean faces.
  G4Box *tubeInnerSolid = new G4Box("Panel3DetectorMountTubeInnerSolid",
                                    tubeLength / 2.0 + 0.1 * mm,
                                    tubeInner / 2.0,
                                    tubeInner / 2.0);

  G4SubtractionSolid *tubeSolid =
    new G4SubtractionSolid("Panel3DetectorMountTubeSolid",
                           tubeOuterSolid,
                           tubeInnerSolid,
                           nullptr,
                           G4ThreeVector());

  // Use the vertical leg's outside envelope as a cutter.  Do not use the hollow
  // leg solid itself; otherwise the tube material inside the leg's hollow core
  // would remain.  The cutter is intentionally a little oversized to avoid
  // coplanar Boolean surfaces.
  G4Box *legEnvelopeCutter = new G4Box("Panel3DetectorMountTubeLegEnvelopeCutter",
                                       legOuterX / 2.0 + 0.1 * mm,
                                       legLength / 2.0 + 0.1 * mm,
                                       legOuterZ / 2.0 + 0.1 * mm);

  // Boolean transform of the leg cutter relative to the tube:
  //   cutter_in_tube = inverse(tube_in_frame) * leg_in_frame
  // In frame coordinates the vertical legs are not rotated, while the upper
  // and lower tubes are rotated by +/-tubeAngle around Z.
  G4RotationMatrix upperCutterRotation;
  upperCutterRotation.rotateZ(-tubeAngle);

  G4ThreeVector upperLeftCutterPosition =
    upperCutterRotation * G4ThreeVector(-legCenterX, -tubeCenterY, 0.0);
  G4ThreeVector upperRightCutterPosition =
    upperCutterRotation * G4ThreeVector( legCenterX, -tubeCenterY, 0.0);

  G4Transform3D upperLeftCutterTransform(upperCutterRotation,
                                         upperLeftCutterPosition);
  G4Transform3D upperRightCutterTransform(upperCutterRotation,
                                          upperRightCutterPosition);

  G4SubtractionSolid *upperTubeCutLeft =
    new G4SubtractionSolid("Panel3UpperDetectorMountTubeCutLeft",
                           tubeSolid,
                           legEnvelopeCutter,
                           upperLeftCutterTransform);

  G4SubtractionSolid *upperTubeSolid =
    new G4SubtractionSolid("Panel3UpperDetectorMountTubeSolid",
                           upperTubeCutLeft,
                           legEnvelopeCutter,
                           upperRightCutterTransform);

  G4RotationMatrix lowerCutterRotation;
  lowerCutterRotation.rotateZ(tubeAngle);

  G4ThreeVector lowerLeftCutterPosition =
    lowerCutterRotation * G4ThreeVector(-legCenterX, tubeCenterY, 0.0);
  G4ThreeVector lowerRightCutterPosition =
    lowerCutterRotation * G4ThreeVector( legCenterX, tubeCenterY, 0.0);

  G4Transform3D lowerLeftCutterTransform(lowerCutterRotation,
                                         lowerLeftCutterPosition);
  G4Transform3D lowerRightCutterTransform(lowerCutterRotation,
                                          lowerRightCutterPosition);

  G4SubtractionSolid *lowerTubeCutLeft =
    new G4SubtractionSolid("Panel3LowerDetectorMountTubeCutLeft",
                           tubeSolid,
                           legEnvelopeCutter,
                           lowerLeftCutterTransform);

  G4SubtractionSolid *lowerTubeSolid =
    new G4SubtractionSolid("Panel3LowerDetectorMountTubeSolid",
                           lowerTubeCutLeft,
                           legEnvelopeCutter,
                           lowerRightCutterTransform);

  G4LogicalVolume *upperTubeLV = new G4LogicalVolume(upperTubeSolid,
                                                     Materials->Steel,
                                                     "Panel3UpperDetectorMountTubeLV");
  upperTubeLV->SetVisAttributes(G4VisAttributes(G4Colour(0.30, 0.30, 0.30)));

  G4LogicalVolume *lowerTubeLV = new G4LogicalVolume(lowerTubeSolid,
                                                     Materials->Steel,
                                                     "Panel3LowerDetectorMountTubeLV");
  lowerTubeLV->SetVisAttributes(G4VisAttributes(G4Colour(0.30, 0.30, 0.30)));

  G4RotationMatrix *upperTubeRotation = new G4RotationMatrix();
  upperTubeRotation->rotateZ(tubeAngle);

  G4RotationMatrix *lowerTubeRotation = new G4RotationMatrix();
  lowerTubeRotation->rotateZ(-tubeAngle);

  G4ThreeVector upperTubePosition(0.0, tubeCenterY, tubeCenterZ);
  G4ThreeVector lowerTubePosition(0.0, -tubeCenterY, tubeCenterZ);

  frameAssembly->AddPlacedVolume(upperTubeLV, upperTubePosition, upperTubeRotation);
  frameAssembly->AddPlacedVolume(lowerTubeLV, lowerTubePosition, lowerTubeRotation);
}


void LADDetectorConstructionHodoCreator::BuildGussets(G4AssemblyVolume *frameAssembly,
                                                      LADMaterials *Materials)
{
  // Items 12 and 13: PANEL 3, FRAME GUSSET-1/2, 6 x 4 x .25 wall,
  // ASTM A500 Grade B.  The four diagonal gussets are 40-inch tubes at +/-45 deg.
  // Left pair: item 12.  Right pair: item 13.
  const G4double padThickness = 0.75 * inch;
  const G4double frameHeight = 216.0 * inch;
  const G4double legInnerClearance = 94.50 * inch;

  const G4double legOuterX = 4.0 * inch;
  const G4double legOuterZ = 6.0 * inch;
  const G4double legLength = frameHeight - 2.0 * padThickness;
  const G4double legCenterX = 0.5 * (legInnerClearance + legOuterX);

  const G4double mountTubeOuter = 6.0 * inch;
  const G4double mountTubeLength = 96.0625 * inch;
  const G4double mountTubeAngle = 7.35 * deg;
  const G4double mountTubeCenterY = 74.83 * inch;

  const G4double gussetLength = 40.0 * inch;
  const G4double gussetOuterY = 6.0 * inch;
  const G4double gussetOuterZ = 4.0 * inch;
  const G4double gussetWall = 0.25 * inch;
  const G4double gussetInnerY = gussetOuterY - 2.0 * gussetWall;
  const G4double gussetInnerZ = gussetOuterZ - 2.0 * gussetWall;
  const G4double gussetAngle = 45.0 * deg;
  const G4double halfProjection = 0.5 * gussetLength * cos(gussetAngle);

  G4Box *gussetOuterSolid = new G4Box("Panel3GussetOuterSolid",
                                      gussetLength / 2.0,
                                      gussetOuterY / 2.0,
                                      gussetOuterZ / 2.0);

  // Make the cutter slightly longer than the tube to avoid coplanar Boolean faces.
  G4Box *gussetInnerSolid = new G4Box("Panel3GussetInnerSolid",
                                      gussetLength / 2.0 + 0.1 * mm,
                                      gussetInnerY / 2.0,
                                      gussetInnerZ / 2.0);

  G4SubtractionSolid *gussetHollowSolid =
    new G4SubtractionSolid("Panel3GussetHollowSolid",
                           gussetOuterSolid,
                           gussetInnerSolid,
                           nullptr,
                           G4ThreeVector());

  // Use solid envelopes as cutters so the gussets do not overlap the vertical
  // legs or detector mount tubes.  The real weld/miter details are simplified.
  G4Box *legEnvelopeCutter = new G4Box("Panel3GussetLegEnvelopeCutter",
                                       legOuterX / 2.0 + 0.1 * mm,
                                       legLength / 2.0 + 0.1 * mm,
                                       legOuterZ / 2.0 + 0.1 * mm);

  G4Box *mountTubeEnvelopeCutter = new G4Box("Panel3GussetMountTubeEnvelopeCutter",
                                             mountTubeLength / 2.0 + 0.1 * mm,
                                             mountTubeOuter / 2.0 + 0.1 * mm,
                                             mountTubeOuter / 2.0 + 0.1 * mm);

  // Use a second mount-tube envelope as an additional cutter on the side away
  // from the frame center.  The second cutter is shifted along the frame Y
  // direction, not the tilted tube-local Y direction.  Since the mount tube is
  // rotated by +/-7.35 deg, the frame-Y shift is scaled by cos(angle) so the
  // two tilted cutters just touch.
  const G4double mountTubeSecondCutterNormalSeparation =
    2.0 * (mountTubeOuter / 2.0 + 0.1 * mm);

  auto MakeGussetSolid =
    [&](const G4String &name,
        const G4ThreeVector &gussetPosition,
        G4double gussetRotationAngle,
        const G4ThreeVector &legPosition,
        const G4ThreeVector &mountTubePosition,
        G4double mountTubeRotationAngle,
        G4double mountTubeOutwardSign) -> G4SubtractionSolid *
    {
      G4RotationMatrix gussetRotation;
      gussetRotation.rotateZ(gussetRotationAngle);

      G4RotationMatrix inverseGussetRotation(gussetRotation);
      inverseGussetRotation.invert();

      G4RotationMatrix legCutterRotation(inverseGussetRotation);
      G4ThreeVector legCutterPosition =
        inverseGussetRotation * (legPosition - gussetPosition);
      G4Transform3D legCutterTransform(legCutterRotation,
                                       legCutterPosition);

      G4SubtractionSolid *gussetCutLeg =
        new G4SubtractionSolid(name + "CutLeg",
                               gussetHollowSolid,
                               legEnvelopeCutter,
                               legCutterTransform);

      G4RotationMatrix mountTubeRotation;
      mountTubeRotation.rotateZ(mountTubeRotationAngle);
      G4RotationMatrix mountTubeCutterRotation =
        inverseGussetRotation * mountTubeRotation;
      G4ThreeVector mountTubeCutterPosition =
        inverseGussetRotation * (mountTubePosition - gussetPosition);
      G4Transform3D mountTubeCutterTransform(mountTubeCutterRotation,
                                             mountTubeCutterPosition);

      G4SubtractionSolid *gussetCutMountTube =
        new G4SubtractionSolid(name + "CutMountTube",
                               gussetCutLeg,
                               mountTubeEnvelopeCutter,
                               mountTubeCutterTransform);

      const G4double secondCutterOffsetFrameY =
        mountTubeSecondCutterNormalSeparation / cos(mountTubeRotationAngle);
      G4ThreeVector secondCutterOffsetInFrame(
        0.0,
        mountTubeOutwardSign * secondCutterOffsetFrameY,
        0.0);
      G4ThreeVector secondMountTubeCutterPosition =
        inverseGussetRotation *
        (mountTubePosition + secondCutterOffsetInFrame - gussetPosition);
      G4Transform3D secondMountTubeCutterTransform(
        mountTubeCutterRotation,
        secondMountTubeCutterPosition);

      return new G4SubtractionSolid(name + "Solid",
                                    gussetCutMountTube,
                                    mountTubeEnvelopeCutter,
                                    secondMountTubeCutterTransform);
    };

  G4ThreeVector leftLegPosition(-legCenterX, 0.0, 0.0);
  G4ThreeVector rightLegPosition(legCenterX, 0.0, 0.0);
  G4ThreeVector upperMountTubePosition(0.0, mountTubeCenterY, 0.0);
  G4ThreeVector lowerMountTubePosition(0.0, -mountTubeCenterY, 0.0);

  G4ThreeVector upperLeftGussetPosition(-legCenterX + halfProjection,
                                        mountTubeCenterY - halfProjection,
                                        0.0);
  G4ThreeVector upperRightGussetPosition(legCenterX - halfProjection,
                                         mountTubeCenterY - halfProjection,
                                         0.0);
  G4ThreeVector lowerLeftGussetPosition(-legCenterX + halfProjection,
                                        -mountTubeCenterY + halfProjection,
                                        0.0);
  G4ThreeVector lowerRightGussetPosition(legCenterX - halfProjection,
                                         -mountTubeCenterY + halfProjection,
                                         0.0);

  G4SubtractionSolid *upperLeftGussetSolid =
    MakeGussetSolid("Panel3Item12UpperLeftGusset",
                    upperLeftGussetPosition,
                    gussetAngle,
                    leftLegPosition,
                    upperMountTubePosition,
                    mountTubeAngle,
                    1.0);

  G4SubtractionSolid *upperRightGussetSolid =
    MakeGussetSolid("Panel3Item13UpperRightGusset",
                    upperRightGussetPosition,
                    -gussetAngle,
                    rightLegPosition,
                    upperMountTubePosition,
                    mountTubeAngle,
                    1.0);

  G4SubtractionSolid *lowerLeftGussetSolid =
    MakeGussetSolid("Panel3Item12LowerLeftGusset",
                    lowerLeftGussetPosition,
                    -gussetAngle,
                    leftLegPosition,
                    lowerMountTubePosition,
                    -mountTubeAngle,
                    -1.0);

  G4SubtractionSolid *lowerRightGussetSolid =
    MakeGussetSolid("Panel3Item13LowerRightGusset",
                    lowerRightGussetPosition,
                    gussetAngle,
                    rightLegPosition,
                    lowerMountTubePosition,
                    -mountTubeAngle,
                    -1.0);

  G4LogicalVolume *upperLeftGussetLV =
    new G4LogicalVolume(upperLeftGussetSolid,
                        Materials->Steel,
                        "Panel3Item12UpperLeftGussetLV");
  G4LogicalVolume *upperRightGussetLV =
    new G4LogicalVolume(upperRightGussetSolid,
                        Materials->Steel,
                        "Panel3Item13UpperRightGussetLV");
  G4LogicalVolume *lowerLeftGussetLV =
    new G4LogicalVolume(lowerLeftGussetSolid,
                        Materials->Steel,
                        "Panel3Item12LowerLeftGussetLV");
  G4LogicalVolume *lowerRightGussetLV =
    new G4LogicalVolume(lowerRightGussetSolid,
                        Materials->Steel,
                        "Panel3Item13LowerRightGussetLV");

  upperLeftGussetLV->SetVisAttributes(G4VisAttributes(G4Colour(0.25, 0.25, 0.25)));
  upperRightGussetLV->SetVisAttributes(G4VisAttributes(G4Colour(0.25, 0.25, 0.25)));
  lowerLeftGussetLV->SetVisAttributes(G4VisAttributes(G4Colour(0.25, 0.25, 0.25)));
  lowerRightGussetLV->SetVisAttributes(G4VisAttributes(G4Colour(0.25, 0.25, 0.25)));

  G4RotationMatrix *upperLeftGussetRotation = new G4RotationMatrix();
  upperLeftGussetRotation->rotateZ(gussetAngle);

  G4RotationMatrix *upperRightGussetRotation = new G4RotationMatrix();
  upperRightGussetRotation->rotateZ(-gussetAngle);

  G4RotationMatrix *lowerLeftGussetRotation = new G4RotationMatrix();
  lowerLeftGussetRotation->rotateZ(-gussetAngle);

  G4RotationMatrix *lowerRightGussetRotation = new G4RotationMatrix();
  lowerRightGussetRotation->rotateZ(gussetAngle);

  frameAssembly->AddPlacedVolume(upperLeftGussetLV,
                                 upperLeftGussetPosition,
                                 upperLeftGussetRotation);
  frameAssembly->AddPlacedVolume(upperRightGussetLV,
                                 upperRightGussetPosition,
                                 upperRightGussetRotation);
  frameAssembly->AddPlacedVolume(lowerLeftGussetLV,
                                 lowerLeftGussetPosition,
                                 lowerLeftGussetRotation);
  frameAssembly->AddPlacedVolume(lowerRightGussetLV,
                                 lowerRightGussetPosition,
                                 lowerRightGussetRotation);
}


void LADDetectorConstructionHodoCreator::BuildChannels(G4AssemblyVolume *,
                                                       LADMaterials *)
{
}


void LADDetectorConstructionHodoCreator::ConstructSDandField()
{

  G4cout<<" LADDetectorConstructionHodoCreator::ConstructSDandField() START"<<G4endl;
  /*
  G4SDManager::GetSDMpointer()->SetVerboseLevel(1);

  //
  // Sensitive detectors
  //
 LADHodoSD *barSD
   = new LADHodoSD("barSD", "LADbarsHitsCollection"); 
  G4SDManager::GetSDMpointer()->AddNewDetector(barSD);
  BarLV-> SetSensitiveDetector(barSD);

  */
}


LADDetectorConstructionHodoCreator::~LADDetectorConstructionHodoCreator()
{
    G4cout << "<~LADDetectorConstructionHodoCreator> --> DONE" << G4endl; 
}
