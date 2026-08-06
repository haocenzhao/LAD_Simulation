#include "LADDetectorConstructionHodoCreator.hh"

#include "G4AssemblyVolume.hh"
#include "G4SubtractionSolid.hh"
#include "G4VisAttributes.hh"
#include "LADMaterials.hh"

#include <cmath>


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

  G4RotationMatrix frameRotationInPanel3;

  PlacePanel3LocalAssembly(frameAssembly,
                           worldLV,
                           panelIndex,
                           160000,
                           hodoCenterHall,
                           hodoRotationHall,
                           frameRotationInPanel3);
}


void LADDetectorConstructionHodoCreator::BuildDoubleStand(
  G4LogicalVolume *worldLV,
  LADMaterials *Materials,
  G4int wallIndex,
  const G4ThreeVector &doubleWallCenterHall,
  const G4RotationMatrix &hodoRotationHall)
{
  // The local origin lies midway between the two corrected Panel 3 centers.
  // Build the complete rigid stand in that shared Panel 3 coordinate system
  // before applying the common double-wall alignment correction.
  G4AssemblyVolume *standAssembly = new G4AssemblyVolume();

  BuildDoubleStandItem11DetectorAttachmentPlates(standAssembly, Materials);
  BuildDoubleStandLongTube(standAssembly, Materials);
  BuildDoubleStandHorizontalItem6MidTubes(standAssembly, Materials);
  BuildDoubleStandItem10AttachmentPlates(standAssembly, Materials);
  BuildDoubleStandItem7And8VerticalFrames(standAssembly, Materials);
  BuildDoubleStandItem12LowerBasePlates(standAssembly, Materials);

  // Match the single-wall construction: draw the stand below Panel 3, then
  // rotate the completed stand before the Panel 3 drawing-orientation change.
  G4RotationMatrix standRotationInPanel3;
  standRotationInPanel3.rotateZ(180.0 * deg);

  PlacePanel3LocalAssembly(standAssembly,
                           worldLV,
                           wallIndex,
                           180000,
                           doubleWallCenterHall,
                           hodoRotationHall,
                           standRotationInPanel3);
}


void LADDetectorConstructionHodoCreator::BuildDoubleStandItem11DetectorAttachmentPlates(
  G4AssemblyVolume *standAssembly,
  LADMaterials *Materials)
{
  // Drawing 67506-00010, item 11: two 28 x 12 x 3/4-inch ASTM A36 detector
  // attachment plates.  Each plate touches the bottom pads of both Panel 3
  // layers without occupying the same volume.
  const G4double plateSizeX = 12.0 * inch;
  const G4double plateThicknessY = 0.75 * inch;
  const G4double plateSizeZ = 28.0 * inch;

  const G4double panel3FrameHeight = 216.0 * inch;
  const G4double panel3PadThickness = 0.75 * inch;
  const G4double panel3LegInnerClearance = 94.50 * inch;
  const G4double panel3LegOuterX = 4.0 * inch;

  const G4double plateCenterX =
    0.5 * (panel3LegInnerClearance + panel3LegOuterX);
  const G4double panel3BottomPadSurfaceY =
    -0.5 * panel3FrameHeight - panel3PadThickness;
  const G4double plateCenterY =
    panel3BottomPadSurfaceY - 0.5 * plateThicknessY;

  G4Box *plateSolid =
    new G4Box("DoubleStandItem11DetectorAttachmentPlateSolid",
              plateSizeX / 2.0,
              plateThicknessY / 2.0,
              plateSizeZ / 2.0);

  G4LogicalVolume *plateLV =
    new G4LogicalVolume(plateSolid,
                        Materials->ASTM_A36,
                        "DoubleStandItem11DetectorAttachmentPlateLV");
  plateLV->SetVisAttributes(G4VisAttributes(G4Colour(0.20, 0.20, 0.20)));

  G4ThreeVector leftPlatePosition(-plateCenterX, plateCenterY, 0.0);
  G4ThreeVector rightPlatePosition(plateCenterX, plateCenterY, 0.0);

  standAssembly->AddPlacedVolume(plateLV, leftPlatePosition, nullptr);
  standAssembly->AddPlacedVolume(plateLV, rightPlatePosition, nullptr);
}


void LADDetectorConstructionHodoCreator::BuildDoubleStandLongTube(
  G4AssemblyVolume *standAssembly,
  LADMaterials *Materials)
{
  // Drawing 67506-00010, item 5: one closed rectangular tube made from a
  // 6-inch-wide rectangular ring with a constant 3/8-inch wall.
  const G4double outerSizeX = 104.50 * inch;
  const G4double outerSizeY = 6.0 * inch;
  const G4double outerSizeZ = 22.0 * inch;
  const G4double openingSizeX = 92.50 * inch;
  const G4double openingSizeZ = 10.0 * inch;
  const G4double wallThickness = 0.375 * inch;
  const G4double booleanTolerance = 0.01 * inch;

  G4Box *outerBlock =
    new G4Box("DoubleStandItem5OuterBlock",
              outerSizeX / 2.0,
              outerSizeY / 2.0,
              outerSizeZ / 2.0);
  G4Box *centralOpening =
    new G4Box("DoubleStandItem5CentralOpening",
              openingSizeX / 2.0,
              outerSizeY / 2.0 + booleanTolerance,
              openingSizeZ / 2.0);
  G4SubtractionSolid *solidRing =
    new G4SubtractionSolid("DoubleStandItem5SolidRing",
                           outerBlock,
                           centralOpening);

  // The hollowing cutter is a second rectangular ring with every surface
  // offset inward by the specified wall thickness.
  const G4double cutterOuterSizeX = outerSizeX - 2.0 * wallThickness;
  const G4double cutterOuterSizeY = outerSizeY - 2.0 * wallThickness;
  const G4double cutterOuterSizeZ = outerSizeZ - 2.0 * wallThickness;
  const G4double cutterOpeningSizeX = openingSizeX + 2.0 * wallThickness;
  const G4double cutterOpeningSizeZ = openingSizeZ + 2.0 * wallThickness;

  G4Box *cutterOuterBlock =
    new G4Box("DoubleStandItem5CutterOuterBlock",
              cutterOuterSizeX / 2.0,
              cutterOuterSizeY / 2.0,
              cutterOuterSizeZ / 2.0);
  G4Box *cutterCentralOpening =
    new G4Box("DoubleStandItem5CutterCentralOpening",
              cutterOpeningSizeX / 2.0,
              cutterOuterSizeY / 2.0 + booleanTolerance,
              cutterOpeningSizeZ / 2.0);
  G4SubtractionSolid *hollowingCutter =
    new G4SubtractionSolid("DoubleStandItem5HollowingCutter",
                           cutterOuterBlock,
                           cutterCentralOpening);

  G4SubtractionSolid *item5Solid =
    new G4SubtractionSolid("DoubleStandItem5Solid",
                           solidRing,
                           hollowingCutter);
  G4LogicalVolume *item5LV =
    new G4LogicalVolume(item5Solid,
                        Materials->ASTM_A500_GradeB,
                        "DoubleStandItem5LV");
  item5LV->SetVisAttributes(G4VisAttributes(G4Colour(0.27, 0.27, 0.27)));

  G4ThreeVector item5Position(0.0, -112.50 * inch, 0.0);
  standAssembly->AddPlacedVolume(item5LV, item5Position, nullptr);
}


void LADDetectorConstructionHodoCreator::BuildDoubleStandHorizontalItem6MidTubes(
  G4AssemblyVolume *standAssembly,
  LADMaterials *Materials)
{
  // Drawing 67506-00010, item 6 in the horizontal weldment: two hollow square
  // tubes span the 10-inch central slot in item 5.
  const G4double tubeSizeX = 6.0 * inch;
  const G4double tubeSizeY = 6.0 * inch;
  const G4double tubeLengthZ = 10.0 * inch;
  const G4double wallThickness = 0.375 * inch;
  const G4double booleanTolerance = 0.01 * inch;

  G4Box *outerSolid =
    new G4Box("DoubleStandHorizontalItem6OuterSolid",
              tubeSizeX / 2.0,
              tubeSizeY / 2.0,
              tubeLengthZ / 2.0);
  G4Box *innerSolid =
    new G4Box("DoubleStandHorizontalItem6InnerSolid",
              (tubeSizeX - 2.0 * wallThickness) / 2.0,
              (tubeSizeY - 2.0 * wallThickness) / 2.0,
              tubeLengthZ / 2.0 + booleanTolerance);
  G4SubtractionSolid *tubeSolid =
    new G4SubtractionSolid("DoubleStandHorizontalItem6Solid",
                           outerSolid,
                           innerSolid);
  G4LogicalVolume *tubeLV =
    new G4LogicalVolume(tubeSolid,
                        Materials->ASTM_A500_GradeB,
                        "DoubleStandHorizontalItem6LV");
  tubeLV->SetVisAttributes(G4VisAttributes(G4Colour(0.27, 0.27, 0.27)));

  const G4double item5OuterLengthX = 104.50 * inch;
  const G4double item5EndToItem6EdgeX = 26.50 * inch;
  const G4double tubeCenterX =
    0.5 * item5OuterLengthX - item5EndToItem6EdgeX - 0.5 * tubeSizeX;
  const G4double tubeCenterY = -112.50 * inch;

  G4ThreeVector leftTubePosition(-tubeCenterX, tubeCenterY, 0.0);
  G4ThreeVector rightTubePosition(tubeCenterX, tubeCenterY, 0.0);
  standAssembly->AddPlacedVolume(tubeLV, leftTubePosition, nullptr);
  standAssembly->AddPlacedVolume(tubeLV, rightTubePosition, nullptr);
}


void LADDetectorConstructionHodoCreator::BuildDoubleStandItem10AttachmentPlates(
  G4AssemblyVolume *standAssembly,
  LADMaterials *Materials)
{
  // Drawing 67506-00010, item 10: the horizontal weldment uses two attachment
  // plates below item 5, and each of the two vertical weldments uses one more
  // directly above item 7.  All four have the same dimensions and X/Z centers.
  const G4double plateSizeX = 12.0 * inch;
  const G4double plateThicknessY = 0.75 * inch;
  const G4double plateSizeZ = 28.0 * inch;
  const G4double plateCenterX = 49.25 * inch;
  const G4double item5BottomSurfaceY = -115.50 * inch;
  const G4double horizontalPlateCenterY =
    item5BottomSurfaceY - 0.5 * plateThicknessY;
  const G4double verticalPlateCenterY =
    horizontalPlateCenterY - plateThicknessY;

  G4Box *plateSolid =
    new G4Box("DoubleStandItem10AttachmentPlateSolid",
              plateSizeX / 2.0,
              plateThicknessY / 2.0,
              plateSizeZ / 2.0);
  G4LogicalVolume *plateLV =
    new G4LogicalVolume(plateSolid,
                        Materials->ASTM_A36,
                        "DoubleStandItem10AttachmentPlateLV");
  plateLV->SetVisAttributes(G4VisAttributes(G4Colour(0.20, 0.20, 0.20)));

  G4ThreeVector leftHorizontalPlatePosition(
    -plateCenterX, horizontalPlateCenterY, 0.0);
  G4ThreeVector rightHorizontalPlatePosition(
    plateCenterX, horizontalPlateCenterY, 0.0);
  G4ThreeVector leftVerticalPlatePosition(
    -plateCenterX, verticalPlateCenterY, 0.0);
  G4ThreeVector rightVerticalPlatePosition(
    plateCenterX, verticalPlateCenterY, 0.0);

  standAssembly->AddPlacedVolume(
    plateLV, leftHorizontalPlatePosition, nullptr);
  standAssembly->AddPlacedVolume(
    plateLV, rightHorizontalPlatePosition, nullptr);
  standAssembly->AddPlacedVolume(
    plateLV, leftVerticalPlatePosition, nullptr);
  standAssembly->AddPlacedVolume(
    plateLV, rightVerticalPlatePosition, nullptr);
}


void LADDetectorConstructionHodoCreator::BuildDoubleStandItem7And8VerticalFrames(
  G4AssemblyVolume *standAssembly,
  LADMaterials *Materials)
{
  // Drawing 67506-00010: each vertical weldment combines one item-7 end tube
  // and two item-8 vertical side tubes into a continuous hollow U-shaped frame.
  const G4double frameSizeX = 6.0 * inch;
  const G4double frameSizeY = 38.0 * inch;
  const G4double frameSizeZ = 22.0 * inch;
  const G4double memberWidth = 6.0 * inch;
  const G4double centralOpeningSizeZ = 10.0 * inch;
  const G4double wallThickness = 0.375 * inch;
  const G4double booleanTolerance = 0.01 * inch;

  const G4double frameHalfY = frameSizeY / 2.0;
  const G4double centralOpeningTopY = frameHalfY - memberWidth;
  const G4double throughBottomY = -frameHalfY - 2.0 * booleanTolerance;

  G4Box *outerBlock =
    new G4Box("DoubleStandItem7And8OuterBlock",
              frameSizeX / 2.0,
              frameSizeY / 2.0,
              frameSizeZ / 2.0);

  const G4double centralOpeningSizeY =
    centralOpeningTopY - throughBottomY;
  const G4double centralOpeningCenterY =
    0.5 * (centralOpeningTopY + throughBottomY);
  G4Box *centralOpening =
    new G4Box("DoubleStandItem7And8CentralOpening",
              frameSizeX / 2.0 + booleanTolerance,
              centralOpeningSizeY / 2.0,
              centralOpeningSizeZ / 2.0);
  G4ThreeVector centralOpeningPosition(
    0.0, centralOpeningCenterY, 0.0);
  G4SubtractionSolid *solidFrame =
    new G4SubtractionSolid("DoubleStandItem7And8SolidFrame",
                           outerBlock,
                           centralOpening,
                           nullptr,
                           centralOpeningPosition);

  // Form one U-shaped cavity whose bottom extends beyond the two item-8 ends.
  // This leaves a constant wall on the top, front/back, outer and inner faces.
  const G4double cavityOuterSizeX = frameSizeX - 2.0 * wallThickness;
  const G4double cavityOuterSizeZ = frameSizeZ - 2.0 * wallThickness;
  const G4double cavityOuterTopY = frameHalfY - wallThickness;
  const G4double cavityOuterSizeY = cavityOuterTopY - throughBottomY;
  const G4double cavityOuterCenterY =
    0.5 * (cavityOuterTopY + throughBottomY);

  G4Box *cavityOuterBlock =
    new G4Box("DoubleStandItem7And8CavityOuterBlock",
              cavityOuterSizeX / 2.0,
              cavityOuterSizeY / 2.0,
              cavityOuterSizeZ / 2.0);

  const G4double cavityOpeningSizeZ =
    centralOpeningSizeZ + 2.0 * wallThickness;
  const G4double cavityOpeningTopY =
    centralOpeningTopY + wallThickness;
  const G4double cavityOpeningBottomY =
    throughBottomY - booleanTolerance;
  const G4double cavityOpeningSizeY =
    cavityOpeningTopY - cavityOpeningBottomY;
  const G4double cavityOpeningCenterY =
    0.5 * (cavityOpeningTopY + cavityOpeningBottomY);
  G4Box *cavityCentralOpening =
    new G4Box("DoubleStandItem7And8CavityCentralOpening",
              cavityOuterSizeX / 2.0 + booleanTolerance,
              cavityOpeningSizeY / 2.0,
              cavityOpeningSizeZ / 2.0);
  G4ThreeVector cavityOpeningPosition(
    0.0, cavityOpeningCenterY - cavityOuterCenterY, 0.0);
  G4SubtractionSolid *hollowingCutter =
    new G4SubtractionSolid("DoubleStandItem7And8HollowingCutter",
                           cavityOuterBlock,
                           cavityCentralOpening,
                           nullptr,
                           cavityOpeningPosition);

  G4ThreeVector hollowingCutterPosition(0.0, cavityOuterCenterY, 0.0);
  G4SubtractionSolid *item7And8Solid =
    new G4SubtractionSolid("DoubleStandItem7And8Solid",
                           solidFrame,
                           hollowingCutter,
                           nullptr,
                           hollowingCutterPosition);
  G4LogicalVolume *item7And8LV =
    new G4LogicalVolume(item7And8Solid,
                        Materials->ASTM_A500_GradeB,
                        "DoubleStandItem7And8LV");
  item7And8LV->SetVisAttributes(
    G4VisAttributes(G4Colour(0.27, 0.27, 0.27)));

  const G4double frameCenterX = 49.25 * inch;
  const G4double frameCenterY = -136.0 * inch;
  G4ThreeVector leftFramePosition(-frameCenterX, frameCenterY, 0.0);
  G4ThreeVector rightFramePosition(frameCenterX, frameCenterY, 0.0);
  standAssembly->AddPlacedVolume(item7And8LV, leftFramePosition, nullptr);
  standAssembly->AddPlacedVolume(item7And8LV, rightFramePosition, nullptr);
}


void LADDetectorConstructionHodoCreator::BuildDoubleStandItem12LowerBasePlates(
  G4AssemblyVolume *standAssembly,
  LADMaterials *Materials)
{
  // Drawing 67506-00010, item 12: one lower base plate below each vertical
  // weldment.  Its top face touches the open ends of the item-8 side tubes.
  const G4double plateSizeX = 12.0 * inch;
  const G4double plateThicknessY = 1.0 * inch;
  const G4double plateSizeZ = 28.0 * inch;
  const G4double plateCenterX = 49.25 * inch;
  const G4double verticalFrameBottomY = -155.0 * inch;
  const G4double plateCenterY =
    verticalFrameBottomY - 0.5 * plateThicknessY;

  G4Box *plateSolid =
    new G4Box("DoubleStandItem12LowerBasePlateSolid",
              plateSizeX / 2.0,
              plateThicknessY / 2.0,
              plateSizeZ / 2.0);
  G4LogicalVolume *plateLV =
    new G4LogicalVolume(plateSolid,
                        Materials->ASTM_A36,
                        "DoubleStandItem12LowerBasePlateLV");
  plateLV->SetVisAttributes(G4VisAttributes(G4Colour(0.20, 0.20, 0.20)));

  G4ThreeVector leftPlatePosition(-plateCenterX, plateCenterY, 0.0);
  G4ThreeVector rightPlatePosition(plateCenterX, plateCenterY, 0.0);
  standAssembly->AddPlacedVolume(plateLV, leftPlatePosition, nullptr);
  standAssembly->AddPlacedVolume(plateLV, rightPlatePosition, nullptr);
}


void LADDetectorConstructionHodoCreator::BuildSingleStand(
  G4LogicalVolume *worldLV,
  LADMaterials *Materials,
  G4int panelIndex,
  const G4ThreeVector &hodoCenterHall,
  const G4RotationMatrix &hodoRotationHall)
{
  // Build in the Panel 3 local coordinate system, whose origin is the
  // nominal center of Panel 3.  The complete stand is transformed only after
  // all of its local components have been added.
  G4AssemblyVolume *standAssembly = new G4AssemblyVolume();

  BuildSingleStandTopPlates(standAssembly, Materials);
  BuildSingleStandVerticalTubes(standAssembly, Materials);
  BuildSingleStandBottomPlates(standAssembly, Materials);
  BuildSingleStandBraceTubes(standAssembly, Materials);
  BuildSingleStandHorizontalMountingPlates(standAssembly, Materials);
  BuildSingleStandGussetMountingPlates(standAssembly, Materials);
  BuildSingleStandHorizontalWeldment(standAssembly, Materials);
  BuildSingleStandGussetWeldments(standAssembly, Materials);

  // The stand is drawn with negative local Y pointing below Panel 3.  Rotate
  // the completed stand in the Panel 3 frame so that the subsequent Panel 3
  // drawing-orientation correction preserves that final below-panel position.
  G4RotationMatrix standRotationInPanel3;
  standRotationInPanel3.rotateZ(180.0 * deg);

  PlacePanel3LocalAssembly(standAssembly,
                           worldLV,
                           panelIndex,
                           170000,
                           hodoCenterHall,
                           hodoRotationHall,
                           standRotationInPanel3);
}


void LADDetectorConstructionHodoCreator::BuildSingleStandTopPlates(
  G4AssemblyVolume *standAssembly,
  LADMaterials *Materials)
{
  // Drawing 67506-00006, item 13: two 12 x 12 x 1 inch ASTM A36 detector
  // mounting plates.  Their top faces touch the bottom faces of the Panel 3
  // mounting pads exactly; the solids do not occupy the same volume.
  const G4double plateSize = 12.0 * inch;
  const G4double plateThickness = 1.0 * inch;

  const G4double panel3FrameHeight = 216.0 * inch;
  const G4double panel3PadThickness = 0.75 * inch;
  const G4double panel3LegInnerClearance = 94.50 * inch;
  const G4double panel3LegOuterX = 4.0 * inch;

  const G4double plateCenterX =
    0.5 * (panel3LegInnerClearance + panel3LegOuterX);
  const G4double panel3BottomPadSurfaceY =
    -0.5 * panel3FrameHeight - panel3PadThickness;
  const G4double plateCenterY =
    panel3BottomPadSurfaceY - 0.5 * plateThickness;

  G4Box *plateSolid = new G4Box("SingleStandTopPlateSolid",
                                plateSize / 2.0,
                                plateThickness / 2.0,
                                plateSize / 2.0);

  G4LogicalVolume *plateLV =
    new G4LogicalVolume(plateSolid,
                        Materials->ASTM_A36,
                        "SingleStandTopPlateLV");
  plateLV->SetVisAttributes(G4VisAttributes(G4Colour(0.20, 0.20, 0.20)));

  G4ThreeVector leftPlatePosition(-plateCenterX, plateCenterY, 0.0);
  G4ThreeVector rightPlatePosition(plateCenterX, plateCenterY, 0.0);

  standAssembly->AddPlacedVolume(plateLV, leftPlatePosition, nullptr);
  standAssembly->AddPlacedVolume(plateLV, rightPlatePosition, nullptr);
}


void LADDetectorConstructionHodoCreator::BuildSingleStandVerticalTubes(
  G4AssemblyVolume *standAssembly,
  LADMaterials *Materials)
{
  // Drawing 67506-00006, item 6: one 6 x 6 x 3/8 inch wall ASTM A500
  // Grade B vertical tube per support leg weldment.  The tube spans from the
  // underside of the detector mounting plate to the top of the structure
  // mounting plate.
  const G4double tubeOuter = 6.0 * inch;
  const G4double tubeWall = 0.375 * inch;
  const G4double tubeInner = tubeOuter - 2.0 * tubeWall;

  const G4double standHeight = 47.25 * inch;
  const G4double topPlateThickness = 1.0 * inch;
  const G4double bottomPlateThickness = 1.0 * inch;
  const G4double tubeLength =
    standHeight - topPlateThickness - bottomPlateThickness;

  const G4double panel3FrameHeight = 216.0 * inch;
  const G4double panel3PadThickness = 0.75 * inch;
  const G4double panel3LegInnerClearance = 94.50 * inch;
  const G4double panel3LegOuterX = 4.0 * inch;

  const G4double tubeCenterX =
    0.5 * (panel3LegInnerClearance + panel3LegOuterX);
  const G4double panel3BottomPadSurfaceY =
    -0.5 * panel3FrameHeight - panel3PadThickness;
  const G4double topPlateCenterY =
    panel3BottomPadSurfaceY - 0.5 * topPlateThickness;
  const G4double topPlateBottomSurfaceY =
    topPlateCenterY - 0.5 * topPlateThickness;
  const G4double tubeCenterY =
    topPlateBottomSurfaceY - 0.5 * tubeLength;

  G4Box *tubeOuterSolid = new G4Box("SingleStandVerticalTubeOuterSolid",
                                    tubeOuter / 2.0,
                                    tubeLength / 2.0,
                                    tubeOuter / 2.0);

  // Extend the cutter slightly beyond both tube ends to avoid coplanar
  // surfaces in the Boolean subtraction.
  G4Box *tubeInnerSolid = new G4Box("SingleStandVerticalTubeInnerSolid",
                                    tubeInner / 2.0,
                                    tubeLength / 2.0 + 0.1 * mm,
                                    tubeInner / 2.0);

  G4SubtractionSolid *tubeSolid =
    new G4SubtractionSolid("SingleStandVerticalTubeSolid",
                           tubeOuterSolid,
                           tubeInnerSolid,
                           nullptr,
                           G4ThreeVector());

  G4LogicalVolume *tubeLV =
    new G4LogicalVolume(tubeSolid,
                        Materials->ASTM_A500_GradeB,
                        "SingleStandVerticalTubeLV");
  tubeLV->SetVisAttributes(G4VisAttributes(G4Colour(0.25, 0.25, 0.25)));

  G4ThreeVector leftTubePosition(-tubeCenterX, tubeCenterY, 0.0);
  G4ThreeVector rightTubePosition(tubeCenterX, tubeCenterY, 0.0);

  standAssembly->AddPlacedVolume(tubeLV, leftTubePosition, nullptr);
  standAssembly->AddPlacedVolume(tubeLV, rightTubePosition, nullptr);
}


void LADDetectorConstructionHodoCreator::BuildSingleStandBottomPlates(
  G4AssemblyVolume *standAssembly,
  LADMaterials *Materials)
{
  // Drawing 67506-00006, item 14: one 22 x 12 x 1 inch ASTM A36 structure
  // mounting plate per support leg.  The 22-inch dimension runs along local Z.
  const G4double plateSizeX = 12.0 * inch;
  const G4double plateThickness = 1.0 * inch;
  const G4double plateSizeZ = 22.0 * inch;

  const G4double standHeight = 47.25 * inch;
  const G4double panel3FrameHeight = 216.0 * inch;
  const G4double panel3PadThickness = 0.75 * inch;
  const G4double panel3LegInnerClearance = 94.50 * inch;
  const G4double panel3LegOuterX = 4.0 * inch;

  const G4double plateCenterX =
    0.5 * (panel3LegInnerClearance + panel3LegOuterX);
  const G4double panel3BottomPadSurfaceY =
    -0.5 * panel3FrameHeight - panel3PadThickness;
  const G4double plateCenterY =
    panel3BottomPadSurfaceY - standHeight + 0.5 * plateThickness;

  G4Box *plateSolid = new G4Box("SingleStandBottomPlateSolid",
                                plateSizeX / 2.0,
                                plateThickness / 2.0,
                                plateSizeZ / 2.0);

  G4LogicalVolume *plateLV =
    new G4LogicalVolume(plateSolid,
                        Materials->ASTM_A36,
                        "SingleStandBottomPlateLV");
  plateLV->SetVisAttributes(G4VisAttributes(G4Colour(0.20, 0.20, 0.20)));

  G4ThreeVector leftPlatePosition(-plateCenterX, plateCenterY, 0.0);
  G4ThreeVector rightPlatePosition(plateCenterX, plateCenterY, 0.0);

  standAssembly->AddPlacedVolume(plateLV, leftPlatePosition, nullptr);
  standAssembly->AddPlacedVolume(plateLV, rightPlatePosition, nullptr);
}


void LADDetectorConstructionHodoCreator::BuildSingleStandBraceTubes(
  G4AssemblyVolume *standAssembly,
  LADMaterials *Materials)
{
  // Drawing 67506-00006, item 7: two 3 x 3 x 1/4-inch-wall brace tubes per
  // support leg.  Build and cut each tube in its local frame before placement.
  const G4double braceLength = 25.125 * inch;
  const G4double braceOuter = 3.0 * inch;
  const G4double braceWall = 0.25 * inch;
  const G4double braceInner = braceOuter - 2.0 * braceWall;
  const G4double braceAngle = 15.0 * deg;
  const G4double cutterClearance = 0.1 * mm;

  const G4double legCenterX = 49.25 * inch;
  const G4double braceCenterY = -143.253785 * inch;
  const G4double braceCenterZ = 4.799697 * inch;

  const G4double verticalTubeOuter = 6.0 * inch;
  const G4double verticalTubeLength = 45.25 * inch;
  const G4double verticalTubeCenterY = -132.375 * inch;
  const G4double bottomPlateSizeX = 12.0 * inch;
  const G4double bottomPlateThickness = 1.0 * inch;
  const G4double bottomPlateSizeZ = 22.0 * inch;
  const G4double bottomPlateCenterY = -155.5 * inch;

  G4Box *braceOuterSolid = new G4Box("SingleStandBraceTubeOuterSolid",
                                     braceOuter / 2.0,
                                     braceOuter / 2.0,
                                     braceLength / 2.0);
  G4Box *braceInnerSolid = new G4Box("SingleStandBraceTubeInnerSolid",
                                     braceInner / 2.0,
                                     braceInner / 2.0,
                                     braceLength / 2.0 + cutterClearance);
  G4Box *verticalTubeCutter =
    new G4Box("SingleStandBraceVerticalTubeCutter",
              verticalTubeOuter / 2.0 + cutterClearance,
              verticalTubeLength / 2.0 + cutterClearance,
              verticalTubeOuter / 2.0 + cutterClearance);
  G4Box *bottomPlateCutter =
    new G4Box("SingleStandBraceBottomPlateCutter",
              bottomPlateSizeX / 2.0 + cutterClearance,
              bottomPlateThickness / 2.0 + cutterClearance,
              bottomPlateSizeZ / 2.0 + cutterClearance);

  G4RotationMatrix identityRotation;

  auto MakeBraceRotation =
    [](const G4ThreeVector &braceAxis) -> G4RotationMatrix
    {
      G4ThreeVector braceOutOfPlaneAxis(1.0, 0.0, 0.0);
      G4ThreeVector braceInPlaneAxis(0.0,
                                     braceAxis.z(),
                                     -braceAxis.y());
      return G4RotationMatrix(braceOutOfPlaneAxis,
                              braceInPlaneAxis,
                              braceAxis);
    };

  auto MakeCutBrace =
    [&](const G4String &name,
        G4double legSign,
        const G4ThreeVector &bracePosition,
        const G4ThreeVector &braceAxis) -> G4SubtractionSolid *
    {
      G4RotationMatrix braceRotation = MakeBraceRotation(braceAxis);
      G4RotationMatrix inverseBraceRotation(braceRotation);
      inverseBraceRotation.invert();

      G4ThreeVector verticalTubePosition(legSign * legCenterX,
                                         verticalTubeCenterY,
                                         0.0);
      G4ThreeVector bottomPlatePosition(legSign * legCenterX,
                                        bottomPlateCenterY,
                                        0.0);

      G4RotationMatrix verticalTubeRotationInBrace =
        inverseBraceRotation * identityRotation;
      G4ThreeVector verticalTubePositionInBrace =
        inverseBraceRotation * (verticalTubePosition - bracePosition);
      G4Transform3D verticalTubeTransform(verticalTubeRotationInBrace,
                                          verticalTubePositionInBrace);

      G4RotationMatrix bottomPlateRotationInBrace =
        inverseBraceRotation * identityRotation;
      G4ThreeVector bottomPlatePositionInBrace =
        inverseBraceRotation * (bottomPlatePosition - bracePosition);
      G4Transform3D bottomPlateTransform(bottomPlateRotationInBrace,
                                         bottomPlatePositionInBrace);

      G4SubtractionSolid *cutVerticalTube =
        new G4SubtractionSolid(name + "CutVerticalTube",
                               braceOuterSolid,
                               verticalTubeCutter,
                               verticalTubeTransform);
      G4SubtractionSolid *cutBottomPlate =
        new G4SubtractionSolid(name + "CutBottomPlate",
                               cutVerticalTube,
                               bottomPlateCutter,
                               bottomPlateTransform);
      G4ThreeVector innerCutterPosition;
      return new G4SubtractionSolid(name + "Solid",
                                    cutBottomPlate,
                                    braceInnerSolid,
                                    nullptr,
                                    innerCutterPosition);
    };

  const G4double sinBraceAngle = std::sin(braceAngle);
  const G4double cosBraceAngle = std::cos(braceAngle);

  G4ThreeVector leftBackPosition(-legCenterX,
                                 braceCenterY,
                                 -braceCenterZ);
  G4ThreeVector leftFrontPosition(-legCenterX,
                                  braceCenterY,
                                  braceCenterZ);
  G4ThreeVector rightBackPosition(legCenterX,
                                  braceCenterY,
                                  -braceCenterZ);
  G4ThreeVector rightFrontPosition(legCenterX,
                                   braceCenterY,
                                   braceCenterZ);

  G4ThreeVector backBraceAxis(0.0, cosBraceAngle, sinBraceAngle);
  G4ThreeVector frontBraceAxis(0.0, cosBraceAngle, -sinBraceAngle);

  G4SubtractionSolid *leftBackSolid =
    MakeCutBrace("SingleStandLeftBackBraceTube",
                 -1.0,
                 leftBackPosition,
                 backBraceAxis);
  G4SubtractionSolid *leftFrontSolid =
    MakeCutBrace("SingleStandLeftFrontBraceTube",
                 -1.0,
                 leftFrontPosition,
                 frontBraceAxis);
  G4SubtractionSolid *rightBackSolid =
    MakeCutBrace("SingleStandRightBackBraceTube",
                 1.0,
                 rightBackPosition,
                 backBraceAxis);
  G4SubtractionSolid *rightFrontSolid =
    MakeCutBrace("SingleStandRightFrontBraceTube",
                 1.0,
                 rightFrontPosition,
                 frontBraceAxis);

  G4LogicalVolume *leftBackLV =
    new G4LogicalVolume(leftBackSolid,
                        Materials->ASTM_A500_GradeB,
                        "SingleStandLeftBackBraceTubeLV");
  G4LogicalVolume *leftFrontLV =
    new G4LogicalVolume(leftFrontSolid,
                        Materials->ASTM_A500_GradeB,
                        "SingleStandLeftFrontBraceTubeLV");
  G4LogicalVolume *rightBackLV =
    new G4LogicalVolume(rightBackSolid,
                        Materials->ASTM_A500_GradeB,
                        "SingleStandRightBackBraceTubeLV");
  G4LogicalVolume *rightFrontLV =
    new G4LogicalVolume(rightFrontSolid,
                        Materials->ASTM_A500_GradeB,
                        "SingleStandRightFrontBraceTubeLV");

  G4VisAttributes braceVis(G4Colour(0.27, 0.27, 0.27));
  leftBackLV->SetVisAttributes(braceVis);
  leftFrontLV->SetVisAttributes(braceVis);
  rightBackLV->SetVisAttributes(braceVis);
  rightFrontLV->SetVisAttributes(braceVis);

  G4RotationMatrix backRotationValue = MakeBraceRotation(backBraceAxis);
  G4RotationMatrix frontRotationValue = MakeBraceRotation(frontBraceAxis);
  G4RotationMatrix *backRotation = new G4RotationMatrix(backRotationValue);
  G4RotationMatrix *frontRotation = new G4RotationMatrix(frontRotationValue);

  standAssembly->AddPlacedVolume(leftBackLV,
                                 leftBackPosition,
                                 backRotation);
  standAssembly->AddPlacedVolume(leftFrontLV,
                                 leftFrontPosition,
                                 frontRotation);
  standAssembly->AddPlacedVolume(rightBackLV,
                                 rightBackPosition,
                                 backRotation);
  standAssembly->AddPlacedVolume(rightFrontLV,
                                 rightFrontPosition,
                                 frontRotation);
}


void LADDetectorConstructionHodoCreator::BuildSingleStandHorizontalMountingPlates(
  G4AssemblyVolume *standAssembly,
  LADMaterials *Materials)
{
  // Drawing 67506-00006, item 10: one ASTM A36 horizontal mounting plate on
  // the inward face of each vertical tube.  Dimensions are X x Y x Z.
  const G4double plateThicknessX = 1.0 * inch;
  const G4double plateHeightY = 10.0 * inch;
  const G4double plateSizeZ = 6.0 * inch;
  const G4double plateBottomHeight = 28.125 * inch;

  const G4double standHeight = 47.25 * inch;
  const G4double tubeOuter = 6.0 * inch;
  const G4double panel3FrameHeight = 216.0 * inch;
  const G4double panel3PadThickness = 0.75 * inch;
  const G4double panel3LegInnerClearance = 94.50 * inch;
  const G4double panel3LegOuterX = 4.0 * inch;

  const G4double tubeCenterX =
    0.5 * (panel3LegInnerClearance + panel3LegOuterX);
  const G4double plateOffsetX =
    0.5 * tubeOuter + 0.5 * plateThicknessX;
  const G4double panel3BottomPadSurfaceY =
    -0.5 * panel3FrameHeight - panel3PadThickness;
  const G4double bottomPlateBottomSurfaceY =
    panel3BottomPadSurfaceY - standHeight;
  const G4double plateCenterY =
    bottomPlateBottomSurfaceY + plateBottomHeight + 0.5 * plateHeightY;

  G4Box *plateSolid =
    new G4Box("SingleStandHorizontalMountingPlateSolid",
              plateThicknessX / 2.0,
              plateHeightY / 2.0,
              plateSizeZ / 2.0);

  G4LogicalVolume *plateLV =
    new G4LogicalVolume(plateSolid,
                        Materials->ASTM_A36,
                        "SingleStandHorizontalMountingPlateLV");
  plateLV->SetVisAttributes(G4VisAttributes(G4Colour(0.22, 0.22, 0.22)));

  G4ThreeVector leftPlatePosition(-tubeCenterX + plateOffsetX,
                                  plateCenterY,
                                  0.0);
  G4ThreeVector rightPlatePosition(tubeCenterX - plateOffsetX,
                                   plateCenterY,
                                   0.0);

  standAssembly->AddPlacedVolume(plateLV, leftPlatePosition, nullptr);
  standAssembly->AddPlacedVolume(plateLV, rightPlatePosition, nullptr);
}


void LADDetectorConstructionHodoCreator::BuildSingleStandGussetMountingPlates(
  G4AssemblyVolume *standAssembly,
  LADMaterials *Materials)
{
  // Drawing 67506-00006, item 12: one ASTM A36 gusset mounting plate on the
  // inward face of each vertical tube.  Dimensions are X x Y x Z.
  const G4double plateThicknessX = 1.0 * inch;
  const G4double plateHeightY = 6.5 * inch;
  const G4double plateSizeZ = 6.0 * inch;
  const G4double plateBottomHeight = 9.75 * inch;

  const G4double standHeight = 47.25 * inch;
  const G4double tubeOuter = 6.0 * inch;
  const G4double panel3FrameHeight = 216.0 * inch;
  const G4double panel3PadThickness = 0.75 * inch;
  const G4double panel3LegInnerClearance = 94.50 * inch;
  const G4double panel3LegOuterX = 4.0 * inch;

  const G4double tubeCenterX =
    0.5 * (panel3LegInnerClearance + panel3LegOuterX);
  const G4double plateOffsetX =
    0.5 * tubeOuter + 0.5 * plateThicknessX;
  const G4double panel3BottomPadSurfaceY =
    -0.5 * panel3FrameHeight - panel3PadThickness;
  const G4double bottomPlateBottomSurfaceY =
    panel3BottomPadSurfaceY - standHeight;
  const G4double plateCenterY =
    bottomPlateBottomSurfaceY + plateBottomHeight + 0.5 * plateHeightY;

  G4Box *plateSolid =
    new G4Box("SingleStandGussetMountingPlateSolid",
              plateThicknessX / 2.0,
              plateHeightY / 2.0,
              plateSizeZ / 2.0);

  G4LogicalVolume *plateLV =
    new G4LogicalVolume(plateSolid,
                        Materials->ASTM_A36,
                        "SingleStandGussetMountingPlateLV");
  plateLV->SetVisAttributes(G4VisAttributes(G4Colour(0.22, 0.22, 0.22)));

  G4ThreeVector leftPlatePosition(-tubeCenterX + plateOffsetX,
                                  plateCenterY,
                                  0.0);
  G4ThreeVector rightPlatePosition(tubeCenterX - plateOffsetX,
                                   plateCenterY,
                                   0.0);

  standAssembly->AddPlacedVolume(plateLV, leftPlatePosition, nullptr);
  standAssembly->AddPlacedVolume(plateLV, rightPlatePosition, nullptr);
}


void LADDetectorConstructionHodoCreator::BuildSingleStandHorizontalWeldment(
  G4AssemblyVolume *standAssembly,
  LADMaterials *Materials)
{
  BuildSingleStandHorizontalTube(standAssembly, Materials);
  BuildSingleStandHorizontalEndPlates(standAssembly, Materials);
  BuildSingleStandHorizontalGussetMountingPlates(standAssembly, Materials);
}


void LADDetectorConstructionHodoCreator::BuildSingleStandHorizontalTube(
  G4AssemblyVolume *standAssembly,
  LADMaterials *Materials)
{
  // Drawing 67506-00006, item 5: 6 x 6 x 3/8 inch wall ASTM A500 Grade B
  // tube.  The 89-inch tube spans between the two 3/4-inch end plates in the
  // 90.50-inch horizontal weldment.
  const G4double weldmentLength = 90.50 * inch;
  const G4double endPlateThickness = 0.75 * inch;
  const G4double tubeLength =
    weldmentLength - 2.0 * endPlateThickness;
  const G4double tubeOuter = 6.0 * inch;
  const G4double tubeWall = 0.375 * inch;
  const G4double tubeInner = tubeOuter - 2.0 * tubeWall;

  const G4double standHeight = 47.25 * inch;
  const G4double horizontalPlateBottomHeight = 28.125 * inch;
  const G4double horizontalPlateHeight = 10.0 * inch;
  const G4double panel3FrameHeight = 216.0 * inch;
  const G4double panel3PadThickness = 0.75 * inch;

  const G4double panel3BottomPadSurfaceY =
    -0.5 * panel3FrameHeight - panel3PadThickness;
  const G4double bottomPlateBottomSurfaceY =
    panel3BottomPadSurfaceY - standHeight;
  const G4double tubeCenterY =
    bottomPlateBottomSurfaceY +
    horizontalPlateBottomHeight +
    0.5 * horizontalPlateHeight;

  G4Box *tubeOuterSolid = new G4Box("SingleStandHorizontalTubeOuterSolid",
                                    tubeLength / 2.0,
                                    tubeOuter / 2.0,
                                    tubeOuter / 2.0);

  // Extend the cutter slightly beyond both tube ends to avoid coplanar
  // surfaces in the Boolean subtraction.
  G4Box *tubeInnerSolid = new G4Box("SingleStandHorizontalTubeInnerSolid",
                                    tubeLength / 2.0 + 0.1 * mm,
                                    tubeInner / 2.0,
                                    tubeInner / 2.0);

  G4SubtractionSolid *tubeSolid =
    new G4SubtractionSolid("SingleStandHorizontalTubeSolid",
                           tubeOuterSolid,
                           tubeInnerSolid,
                           nullptr,
                           G4ThreeVector());

  G4LogicalVolume *tubeLV =
    new G4LogicalVolume(tubeSolid,
                        Materials->ASTM_A500_GradeB,
                        "SingleStandHorizontalTubeLV");
  tubeLV->SetVisAttributes(G4VisAttributes(G4Colour(0.25, 0.25, 0.25)));

  G4ThreeVector horizontalTubePosition(0.0, tubeCenterY, 0.0);
  standAssembly->AddPlacedVolume(tubeLV, horizontalTubePosition, nullptr);
}


void LADDetectorConstructionHodoCreator::BuildSingleStandHorizontalEndPlates(
  G4AssemblyVolume *standAssembly,
  LADMaterials *Materials)
{
  // Drawing 67506-00006, item 9: two 3/4 x 10 x 6 inch ASTM A36 end plates.
  const G4double weldmentLength = 90.50 * inch;
  const G4double plateThicknessX = 0.75 * inch;
  const G4double plateHeightY = 10.0 * inch;
  const G4double plateSizeZ = 6.0 * inch;

  const G4double standHeight = 47.25 * inch;
  const G4double horizontalPlateBottomHeight = 28.125 * inch;
  const G4double panel3FrameHeight = 216.0 * inch;
  const G4double panel3PadThickness = 0.75 * inch;

  const G4double panel3BottomPadSurfaceY =
    -0.5 * panel3FrameHeight - panel3PadThickness;
  const G4double bottomPlateBottomSurfaceY =
    panel3BottomPadSurfaceY - standHeight;
  const G4double plateCenterY =
    bottomPlateBottomSurfaceY +
    horizontalPlateBottomHeight +
    0.5 * plateHeightY;
  const G4double plateCenterX =
    0.5 * (weldmentLength - plateThicknessX);

  G4Box *plateSolid = new G4Box("SingleStandHorizontalEndPlateSolid",
                                plateThicknessX / 2.0,
                                plateHeightY / 2.0,
                                plateSizeZ / 2.0);

  G4LogicalVolume *plateLV =
    new G4LogicalVolume(plateSolid,
                        Materials->ASTM_A36,
                        "SingleStandHorizontalEndPlateLV");
  plateLV->SetVisAttributes(G4VisAttributes(G4Colour(0.22, 0.22, 0.22)));

  G4ThreeVector leftPlatePosition(-plateCenterX, plateCenterY, 0.0);
  G4ThreeVector rightPlatePosition(plateCenterX, plateCenterY, 0.0);

  standAssembly->AddPlacedVolume(plateLV, leftPlatePosition, nullptr);
  standAssembly->AddPlacedVolume(plateLV, rightPlatePosition, nullptr);
}


void LADDetectorConstructionHodoCreator::BuildSingleStandHorizontalGussetMountingPlates(
  G4AssemblyVolume *standAssembly,
  LADMaterials *Materials)
{
  // Drawing 67506-00006, item 12: two 6.5 x 1 x 6 inch ASTM A36 plates on the
  // underside of the horizontal tube.  Each center is 16.125 inches from the
  // nearest outer end of the horizontal weldment.
  const G4double weldmentLength = 90.50 * inch;
  const G4double plateEndToCenter = 16.125 * inch;
  const G4double plateSizeX = 6.5 * inch;
  const G4double plateThicknessY = 1.0 * inch;
  const G4double plateSizeZ = 6.0 * inch;
  const G4double tubeOuter = 6.0 * inch;

  const G4double standHeight = 47.25 * inch;
  const G4double horizontalPlateBottomHeight = 28.125 * inch;
  const G4double horizontalPlateHeight = 10.0 * inch;
  const G4double panel3FrameHeight = 216.0 * inch;
  const G4double panel3PadThickness = 0.75 * inch;

  const G4double panel3BottomPadSurfaceY =
    -0.5 * panel3FrameHeight - panel3PadThickness;
  const G4double bottomPlateBottomSurfaceY =
    panel3BottomPadSurfaceY - standHeight;
  const G4double tubeCenterY =
    bottomPlateBottomSurfaceY +
    horizontalPlateBottomHeight +
    0.5 * horizontalPlateHeight;
  const G4double plateCenterY =
    tubeCenterY - 0.5 * tubeOuter - 0.5 * plateThicknessY;
  const G4double plateCenterX =
    0.5 * weldmentLength - plateEndToCenter;

  G4Box *plateSolid =
    new G4Box("SingleStandHorizontalGussetMountingPlateSolid",
              plateSizeX / 2.0,
              plateThicknessY / 2.0,
              plateSizeZ / 2.0);

  G4LogicalVolume *plateLV =
    new G4LogicalVolume(plateSolid,
                        Materials->ASTM_A36,
                        "SingleStandHorizontalGussetMountingPlateLV");
  plateLV->SetVisAttributes(G4VisAttributes(G4Colour(0.22, 0.22, 0.22)));

  G4ThreeVector leftPlatePosition(-plateCenterX, plateCenterY, 0.0);
  G4ThreeVector rightPlatePosition(plateCenterX, plateCenterY, 0.0);

  standAssembly->AddPlacedVolume(plateLV, leftPlatePosition, nullptr);
  standAssembly->AddPlacedVolume(plateLV, rightPlatePosition, nullptr);
}


void LADDetectorConstructionHodoCreator::BuildSingleStandGussetWeldments(
  G4AssemblyVolume *standAssembly,
  LADMaterials *Materials)
{
  BuildSingleStandGussetEndPlates(standAssembly, Materials);
  BuildSingleStandGussetTubes(standAssembly, Materials);
}


void LADDetectorConstructionHodoCreator::BuildSingleStandGussetEndPlates(
  G4AssemblyVolume *standAssembly,
  LADMaterials *Materials)
{
  // Drawing 67506-00006, item 11: two 1/2 x 6.5 x 6 inch ASTM A36 end plates
  // per gusset weldment.  One end plate attaches to the vertical-tube item 12;
  // the other is the same part rotated onto the horizontal-weldment item 12.
  const G4double endPlateThickness = 0.5 * inch;
  const G4double endPlateSizeLong = 6.5 * inch;
  const G4double endPlateSizeZ = 6.0 * inch;

  const G4double standHeight = 47.25 * inch;
  const G4double verticalTubeOuter = 6.0 * inch;
  const G4double item12Thickness = 1.0 * inch;
  const G4double verticalItem12BottomHeight = 9.75 * inch;
  const G4double horizontalPlateBottomHeight = 28.125 * inch;
  const G4double horizontalPlateHeight = 10.0 * inch;
  const G4double horizontalWeldmentLength = 90.50 * inch;
  const G4double horizontalItem12EndToCenter = 16.125 * inch;
  const G4double panel3FrameHeight = 216.0 * inch;
  const G4double panel3PadThickness = 0.75 * inch;
  const G4double panel3LegInnerClearance = 94.50 * inch;
  const G4double panel3LegOuterX = 4.0 * inch;

  const G4double verticalTubeCenterX =
    0.5 * (panel3LegInnerClearance + panel3LegOuterX);
  const G4double verticalEndPlateCenterX =
    verticalTubeCenterX -
    0.5 * verticalTubeOuter -
    item12Thickness -
    0.5 * endPlateThickness;

  const G4double panel3BottomPadSurfaceY =
    -0.5 * panel3FrameHeight - panel3PadThickness;
  const G4double bottomPlateBottomSurfaceY =
    panel3BottomPadSurfaceY - standHeight;
  const G4double verticalEndPlateCenterY =
    bottomPlateBottomSurfaceY +
    verticalItem12BottomHeight +
    0.5 * endPlateSizeLong;

  const G4double horizontalTubeCenterY =
    bottomPlateBottomSurfaceY +
    horizontalPlateBottomHeight +
    0.5 * horizontalPlateHeight;
  const G4double horizontalItem12CenterY =
    horizontalTubeCenterY -
    0.5 * verticalTubeOuter -
    0.5 * item12Thickness;
  const G4double horizontalEndPlateCenterY =
    horizontalItem12CenterY -
    0.5 * item12Thickness -
    0.5 * endPlateThickness;
  const G4double horizontalEndPlateCenterX =
    0.5 * horizontalWeldmentLength - horizontalItem12EndToCenter;

  // Canonical item 11 orientation: thickness along X, 6.5 inches along Y.
  G4Box *plateSolid = new G4Box("SingleStandGussetEndPlateSolid",
                                endPlateThickness / 2.0,
                                endPlateSizeLong / 2.0,
                                endPlateSizeZ / 2.0);

  G4LogicalVolume *plateLV =
    new G4LogicalVolume(plateSolid,
                        Materials->ASTM_A36,
                        "SingleStandGussetEndPlateLV");
  plateLV->SetVisAttributes(G4VisAttributes(G4Colour(0.24, 0.24, 0.24)));

  G4ThreeVector leftVerticalPlatePosition(-verticalEndPlateCenterX,
                                          verticalEndPlateCenterY,
                                          0.0);
  G4ThreeVector rightVerticalPlatePosition(verticalEndPlateCenterX,
                                           verticalEndPlateCenterY,
                                           0.0);
  standAssembly->AddPlacedVolume(plateLV, leftVerticalPlatePosition, nullptr);
  standAssembly->AddPlacedVolume(plateLV, rightVerticalPlatePosition, nullptr);

  G4RotationMatrix *horizontalPlateRotation = new G4RotationMatrix();
  horizontalPlateRotation->rotateZ(90.0 * deg);

  G4ThreeVector leftHorizontalPlatePosition(-horizontalEndPlateCenterX,
                                            horizontalEndPlateCenterY,
                                            0.0);
  G4ThreeVector rightHorizontalPlatePosition(horizontalEndPlateCenterX,
                                             horizontalEndPlateCenterY,
                                             0.0);
  standAssembly->AddPlacedVolume(plateLV,
                                 leftHorizontalPlatePosition,
                                 horizontalPlateRotation);
  standAssembly->AddPlacedVolume(plateLV,
                                 rightHorizontalPlatePosition,
                                 horizontalPlateRotation);
}


void LADDetectorConstructionHodoCreator::BuildSingleStandGussetTubes(
  G4AssemblyVolume *standAssembly,
  LADMaterials *Materials)
{
  // Drawing 67506-00006, item 8: 4 x 2 x 1/4-inch-wall rectangular tube.
  // Start from the full 26-inch tube.  Oversized solid item-11 envelopes keep
  // the real item-11 contact faces fixed while removing everything beyond the
  // vertical and horizontal mating faces.
  const G4double tubeLength = 26.0 * inch;
  const G4double tubeOuterInPlane = 4.0 * inch;
  const G4double tubeOuterZ = 2.0 * inch;
  const G4double tubeWall = 0.25 * inch;
  const G4double tubeInnerInPlane = tubeOuterInPlane - 2.0 * tubeWall;
  const G4double tubeInnerZ = tubeOuterZ - 2.0 * tubeWall;
  const G4double cutterClearance = 0.1 * mm;

  const G4double oversizedItem11Thickness = 10.0 * inch;
  const G4double oversizedItem11HalfThickness =
    0.5 * oversizedItem11Thickness + cutterClearance;
  const G4double item11LongSize = 6.5 * inch;
  const G4double item11SizeZ = 6.0 * inch;

  const G4double verticalItem11ContactX = 44.75 * inch;
  const G4double verticalItem11CenterY = -143.0 * inch;
  const G4double horizontalItem11CenterX = 29.125 * inch;
  const G4double horizontalItem11ContactY = -127.375 * inch;
  const G4double verticalCutterCenterX =
    verticalItem11ContactX + oversizedItem11HalfThickness;
  const G4double horizontalCutterCenterY =
    horizontalItem11ContactY + oversizedItem11HalfThickness;

  const G4double gussetTubeCenterX = 36.976714 * inch;
  const G4double gussetTubeCenterY = -135.148286 * inch;
  const G4double inverseSqrtTwo = 1.0 / std::sqrt(2.0);

  // Local X is the 4-inch in-plane direction, local Y is the 2-inch stand-Z
  // direction, and local Z is the tube axis.
  G4Box *tubeOuterSolid = new G4Box("SingleStandGussetTubeOuterSolid",
                                    tubeOuterInPlane / 2.0,
                                    tubeOuterZ / 2.0,
                                    tubeLength / 2.0);
  G4Box *tubeInnerSolid = new G4Box("SingleStandGussetTubeInnerSolid",
                                    tubeInnerInPlane / 2.0,
                                    tubeInnerZ / 2.0,
                                    tubeLength / 2.0 + cutterClearance);
  G4Box *oversizedItem11Cutter =
    new G4Box("SingleStandGussetOversizedItem11Cutter",
              oversizedItem11HalfThickness,
              item11LongSize / 2.0 + cutterClearance,
              item11SizeZ / 2.0 + cutterClearance);

  G4RotationMatrix identityRotation;
  G4RotationMatrix horizontalItem11Rotation;
  horizontalItem11Rotation.rotateZ(90.0 * deg);

  auto MakeTubeRotation =
    [](const G4ThreeVector &tubeAxis) -> G4RotationMatrix
    {
      G4ThreeVector tubeWidthAxis(-tubeAxis.y(), tubeAxis.x(), 0.0);
      G4ThreeVector tubeDepthAxis(0.0, 0.0, 1.0);
      return G4RotationMatrix(tubeWidthAxis, tubeDepthAxis, tubeAxis);
    };

  auto MakeCutTube =
    [&](const G4String &name,
        G4double sideSign,
        const G4ThreeVector &tubePosition,
        const G4ThreeVector &tubeAxis) -> G4SubtractionSolid *
    {
      G4RotationMatrix tubeRotation = MakeTubeRotation(tubeAxis);

      G4RotationMatrix inverseTubeRotation(tubeRotation);
      inverseTubeRotation.invert();

      auto ToTubeFrame =
        [&](const G4ThreeVector &cutterPosition,
            const G4RotationMatrix &cutterRotation) -> G4Transform3D
        {
          G4RotationMatrix rotationInTubeFrame =
            inverseTubeRotation * cutterRotation;
          G4ThreeVector positionInTubeFrame =
            inverseTubeRotation * (cutterPosition - tubePosition);
          return G4Transform3D(rotationInTubeFrame, positionInTubeFrame);
        };

      G4ThreeVector verticalCutterPosition(
        sideSign * verticalCutterCenterX,
        verticalItem11CenterY,
        0.0);
      G4ThreeVector horizontalCutterPosition(
        sideSign * horizontalItem11CenterX,
        horizontalCutterCenterY,
        0.0);

      G4Transform3D verticalCutterTransform =
        ToTubeFrame(verticalCutterPosition, identityRotation);
      G4Transform3D horizontalCutterTransform =
        ToTubeFrame(horizontalCutterPosition, horizontalItem11Rotation);

      G4SubtractionSolid *cutVerticalEnd =
        new G4SubtractionSolid(name + "CutVerticalEnd",
                               tubeOuterSolid,
                               oversizedItem11Cutter,
                               verticalCutterTransform);
      G4SubtractionSolid *cutHorizontalEnd =
        new G4SubtractionSolid(name + "CutHorizontalEnd",
                               cutVerticalEnd,
                               oversizedItem11Cutter,
                               horizontalCutterTransform);
      G4ThreeVector innerCutterPosition;
      return new G4SubtractionSolid(name + "Solid",
                                    cutHorizontalEnd,
                                    tubeInnerSolid,
                                    nullptr,
                                    innerCutterPosition);
    };

  G4ThreeVector leftTubePosition(-gussetTubeCenterX,
                                 gussetTubeCenterY,
                                 0.0);
  G4ThreeVector rightTubePosition(gussetTubeCenterX,
                                  gussetTubeCenterY,
                                  0.0);
  G4ThreeVector leftTubeAxis(inverseSqrtTwo, inverseSqrtTwo, 0.0);
  G4ThreeVector rightTubeAxis(-inverseSqrtTwo, inverseSqrtTwo, 0.0);

  G4SubtractionSolid *leftTubeSolid =
    MakeCutTube("SingleStandLeftGussetTube",
                -1.0,
                leftTubePosition,
                leftTubeAxis);
  G4SubtractionSolid *rightTubeSolid =
    MakeCutTube("SingleStandRightGussetTube",
                1.0,
                rightTubePosition,
                rightTubeAxis);

  G4LogicalVolume *leftTubeLV =
    new G4LogicalVolume(leftTubeSolid,
                        Materials->ASTM_A500_GradeB,
                        "SingleStandLeftGussetTubeLV");
  G4LogicalVolume *rightTubeLV =
    new G4LogicalVolume(rightTubeSolid,
                        Materials->ASTM_A500_GradeB,
                        "SingleStandRightGussetTubeLV");
  leftTubeLV->SetVisAttributes(G4VisAttributes(G4Colour(0.27, 0.27, 0.27)));
  rightTubeLV->SetVisAttributes(G4VisAttributes(G4Colour(0.27, 0.27, 0.27)));

  G4RotationMatrix leftTubeRotationValue = MakeTubeRotation(leftTubeAxis);
  G4RotationMatrix rightTubeRotationValue = MakeTubeRotation(rightTubeAxis);
  G4RotationMatrix *leftTubeRotation =
    new G4RotationMatrix(leftTubeRotationValue);
  G4RotationMatrix *rightTubeRotation =
    new G4RotationMatrix(rightTubeRotationValue);

  standAssembly->AddPlacedVolume(leftTubeLV,
                                 leftTubePosition,
                                 leftTubeRotation);
  standAssembly->AddPlacedVolume(rightTubeLV,
                                 rightTubePosition,
                                 rightTubeRotation);
}


void LADDetectorConstructionHodoCreator::PlacePanel3LocalAssembly(
  G4AssemblyVolume *assembly,
  G4LogicalVolume *worldLV,
  G4int panelIndex,
  G4int copyNumberBase,
  const G4ThreeVector &hodoCenterHall,
  const G4RotationMatrix &hodoRotationHall,
  const G4RotationMatrix &assemblyRotationInPanel3)
{

  // Local +Z points from the origin toward the nominal wall center, so the
  // wall face nearer the origin is at -WallThick/2.  Before any dxyz/dangle
  // correction, place the Panel 3 center plane 10.5 inches from that face in
  // the +Z direction.
  const G4double hodoWallThickness = WallThick;
  const G4double wallFrontToPanel3Center = 10.5 * inch;
  const G4double panel3FrameOffsetZ =
    -0.5 * hodoWallThickness + wallFrontToPanel3Center;

  G4ThreeVector frameOffsetLocal(0.0, 0.0, panel3FrameOffsetZ);
  G4ThreeVector frameCenterHall =
    hodoCenterHall + hodoRotationHall * frameOffsetLocal;

  // The Panel 3 drawing orientation is opposite to the hodo-wall local X/Y
  // orientation used here.  Rotate the completed local frame by 180 deg about
  // its own center first, then move/rotate it into the hall with the hodo wall.
  G4RotationMatrix panel3LocalRotation;
  panel3LocalRotation.rotateZ(180.0 * deg);

  G4RotationMatrix *assemblyRotationHall =
    new G4RotationMatrix(hodoRotationHall *
                         panel3LocalRotation *
                         assemblyRotationInPanel3);

  assembly->MakeImprint(worldLV,
                        frameCenterHall,
                        assemblyRotationHall,
                        copyNumberBase + panelIndex * 100,
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
  const G4double topBottomPadY = 0.5 * frameHeight + 0.5 * padThickness;
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
                                               Materials->ASTM_A36,
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
  const G4double frameHeight = 216.0 * inch;
  const G4double legInnerClearance = 94.50 * inch;

  const G4double legOuterX = 4.0 * inch;
  const G4double legOuterZ = 6.0 * inch;
  const G4double legWall   = 0.25 * inch;
  const G4double legLength = frameHeight;

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
                                               Materials->ASTM_A500_GradeB,
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

  const G4double frameHeight = 216.0 * inch;
  const G4double legInnerClearance = 94.50 * inch;
  const G4double legOuterX  = 4.0 * inch;
  const G4double legOuterZ  = 6.0 * inch;
  const G4double legLength  = frameHeight;
  const G4double legCenterX = 0.5 * (legInnerClearance + legOuterX);

  const G4double tubeLength = 96.83644024 * inch;
  const G4double tubeAngle  = 7.35 * deg;
  const G4double tubeCenterX = 0.3837904539 * inch;
  const G4double tubeCenterY = 84.00861288 * inch;
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
  G4ThreeVector leftLegPosition(-legCenterX, 0.0, 0.0);
  G4ThreeVector rightLegPosition(legCenterX, 0.0, 0.0);
  G4ThreeVector upperTubePosition(tubeCenterX, tubeCenterY, tubeCenterZ);
  G4ThreeVector lowerTubePosition(tubeCenterX, -tubeCenterY, tubeCenterZ);

  G4RotationMatrix upperCutterRotation;
  upperCutterRotation.rotateZ(-tubeAngle);

  G4ThreeVector upperLeftCutterPosition =
    upperCutterRotation * (leftLegPosition - upperTubePosition);
  G4ThreeVector upperRightCutterPosition =
    upperCutterRotation * (rightLegPosition - upperTubePosition);

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
    lowerCutterRotation * (leftLegPosition - lowerTubePosition);
  G4ThreeVector lowerRightCutterPosition =
    lowerCutterRotation * (rightLegPosition - lowerTubePosition);

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
                                                     Materials->ASTM_A500_GradeB,
                                                     "Panel3UpperDetectorMountTubeLV");
  upperTubeLV->SetVisAttributes(G4VisAttributes(G4Colour(0.30, 0.30, 0.30)));

  G4LogicalVolume *lowerTubeLV = new G4LogicalVolume(lowerTubeSolid,
                                                     Materials->ASTM_A500_GradeB,
                                                     "Panel3LowerDetectorMountTubeLV");
  lowerTubeLV->SetVisAttributes(G4VisAttributes(G4Colour(0.30, 0.30, 0.30)));

  G4RotationMatrix *upperTubeRotation = new G4RotationMatrix();
  upperTubeRotation->rotateZ(tubeAngle);

  G4RotationMatrix *lowerTubeRotation = new G4RotationMatrix();
  lowerTubeRotation->rotateZ(-tubeAngle);

  frameAssembly->AddPlacedVolume(upperTubeLV, upperTubePosition, upperTubeRotation);
  frameAssembly->AddPlacedVolume(lowerTubeLV, lowerTubePosition, lowerTubeRotation);
}


void LADDetectorConstructionHodoCreator::BuildGussets(G4AssemblyVolume *frameAssembly,
                                                      LADMaterials *Materials)
{
  // Items 12 and 13: PANEL 3, FRAME GUSSET-1/2, 6 x 4 x .25 wall,
  // ASTM A500 Grade B.  The four diagonal gussets are 40-inch tubes at +/-45 deg.
  // Left pair: item 12.  Right pair: item 13.
  const G4double frameHeight = 216.0 * inch;
  const G4double legInnerClearance = 94.50 * inch;

  const G4double legOuterZ = 6.0 * inch;
  const G4double legLength = frameHeight;
  const G4double legCutterOuterX = 6.0 * inch;
  const G4double legCutterCenterX = 0.5 * (legInnerClearance + legCutterOuterX);

  const G4double mountTubeOuter = 6.0 * inch;
  const G4double mountTubeLength = 96.83644024 * inch;
  const G4double mountTubeAngle = 7.35 * deg;
  const G4double mountTubeCenterX = 0.3837904539 * inch;
  const G4double mountTubeCenterY = 84.00861288 * inch;

  const G4double gussetLength = 40.0 * inch;
  const G4double gussetOuterY = 6.0 * inch;
  const G4double gussetOuterZ = 4.0 * inch;
  const G4double gussetWall = 0.25 * inch;
  const G4double gussetInnerY = gussetOuterY - 2.0 * gussetWall;
  const G4double gussetInnerZ = gussetOuterZ - 2.0 * gussetWall;
  const G4double gussetAngle = 45.0 * deg;

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
  // legs or detector mount tubes.  The gusset leg cutter is intentionally
  // wider than the real 4-inch vertical leg.  Its inner faces are kept at the
  // same +/-47.25-inch locations, so the cutter center is based on the 94.5-inch
  // inner clearance and the 6-inch cutter width.
  G4Box *legEnvelopeCutter = new G4Box("Panel3GussetLegEnvelopeCutter",
                                       legCutterOuterX / 2.0 + 0.1 * mm,
                                       legLength / 2.0 + 0.1 * mm,
                                       legOuterZ / 2.0 + 0.1 * mm);

  G4Box *mountTubeEnvelopeCutter = new G4Box("Panel3GussetMountTubeEnvelopeCutter",
                                             mountTubeLength / 2.0 + 0.1 * mm,
                                             mountTubeOuter / 2.0 + 0.1 * mm,
                                             mountTubeOuter / 2.0 + 0.1 * mm);

  // Use a second mount-tube envelope as an additional cutter on the side away
  // from the frame center.  This removes small diagonal-tube tips that extend
  // beyond the real mating tube envelope after the first Boolean cut.
  const G4double mountTubeSecondCutterNormalSeparation =
    mountTubeOuter;

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
        mountTubeSecondCutterNormalSeparation / std::cos(mountTubeRotationAngle);
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

  G4ThreeVector leftLegPosition(-legCutterCenterX, 0.0, 0.0);
  G4ThreeVector rightLegPosition(legCutterCenterX, 0.0, 0.0);
  G4ThreeVector upperMountTubePosition(mountTubeCenterX,
                                       mountTubeCenterY,
                                       0.0);
  G4ThreeVector lowerMountTubePosition(mountTubeCenterX,
                                       -mountTubeCenterY,
                                       0.0);

  G4ThreeVector upperLeftGussetPosition(-35.22918472 * inch,
                                        66.48267017 * inch,
                                        0.0);
  G4ThreeVector upperRightGussetPosition(35.22918472 * inch,
                                         71.32569927 * inch,
                                         0.0);
  G4ThreeVector lowerLeftGussetPosition(-35.22918472 * inch,
                                        -66.48267017 * inch,
                                        0.0);
  G4ThreeVector lowerRightGussetPosition(35.22918472 * inch,
                                         -71.32569927 * inch,
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
                        Materials->ASTM_A500_GradeB,
                        "Panel3Item12UpperLeftGussetLV");
  G4LogicalVolume *upperRightGussetLV =
    new G4LogicalVolume(upperRightGussetSolid,
                        Materials->ASTM_A500_GradeB,
                        "Panel3Item13UpperRightGussetLV");
  G4LogicalVolume *lowerLeftGussetLV =
    new G4LogicalVolume(lowerLeftGussetSolid,
                        Materials->ASTM_A500_GradeB,
                        "Panel3Item12LowerLeftGussetLV");
  G4LogicalVolume *lowerRightGussetLV =
    new G4LogicalVolume(lowerRightGussetSolid,
                        Materials->ASTM_A500_GradeB,
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


void LADDetectorConstructionHodoCreator::BuildChannels(G4AssemblyVolume *frameAssembly,
                                                       LADMaterials *Materials)
{
  // Item 8: detector channel.  Simplified as a 94 x 1-5/8 x 1-5/8 inch
  // U-channel with one long side opened.  The channels are parallel to the
  // upper/lower detector mount tubes.
  const G4double channelLength = 94.0 * inch;
  const G4double channelOuter  = 1.625 * inch;
  const G4double channelWall   = 0.125 * inch; // provisional simplified wall
  const G4double channelAngle  = 7.35 * deg;

  const G4double channelCenterX = -0.125 * inch;
  const G4double channelCenterY = 82.9380496 * inch;
  const G4double channelCenterZ = -3.8125 * inch;

  G4Box *channelOuterSolid = new G4Box("Panel3ChannelOuterSolid",
                                       channelLength / 2.0,
                                       channelOuter / 2.0,
                                       channelOuter / 2.0);

  // Open the -local-Z face while leaving the back web against the mount tube.
  G4Box *channelOpenCutter = new G4Box("Panel3ChannelOpenCutter",
                                       channelLength / 2.0 + 0.1 * mm,
                                       (channelOuter - 2.0 * channelWall) / 2.0,
                                       (channelOuter - channelWall) / 2.0 + 0.1 * mm);
  G4ThreeVector openCutterPosition(0.0,
                                   0.0,
                                   -channelWall / 2.0);

  G4SubtractionSolid *channelSolid =
    new G4SubtractionSolid("Panel3ChannelSolid",
                           channelOuterSolid,
                           channelOpenCutter,
                           nullptr,
                           openCutterPosition);

  G4LogicalVolume *channelLV = new G4LogicalVolume(channelSolid,
                                                   Materials->Unistrut_P1000_Steel,
                                                   "Panel3ChannelLV");
  channelLV->SetVisAttributes(G4VisAttributes(G4Colour(0.32, 0.32, 0.32)));

  G4RotationMatrix *upperChannelRotation = new G4RotationMatrix();
  upperChannelRotation->rotateZ(channelAngle);

  G4RotationMatrix *lowerChannelRotation = new G4RotationMatrix();
  lowerChannelRotation->rotateZ(-channelAngle);

  G4ThreeVector upperChannelPosition(channelCenterX,
                                     channelCenterY,
                                     channelCenterZ);
  G4ThreeVector lowerChannelPosition(channelCenterX,
                                     -channelCenterY,
                                     channelCenterZ);

  frameAssembly->AddPlacedVolume(channelLV,
                                 upperChannelPosition,
                                 upperChannelRotation);
  frameAssembly->AddPlacedVolume(channelLV,
                                 lowerChannelPosition,
                                 lowerChannelRotation);
}
