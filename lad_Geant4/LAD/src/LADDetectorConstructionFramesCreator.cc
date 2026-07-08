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

  // The Panel 3 drawing orientation is opposite to the hodo-wall local X/Y
  // orientation used here.  Rotate the completed local frame by 180 deg about
  // its own center first, then move/rotate it into the hall with the hodo wall.
  G4RotationMatrix panel3LocalRotation;
  panel3LocalRotation.rotateZ(180.0 * deg);

  G4RotationMatrix *frameRotationHall =
    new G4RotationMatrix(hodoRotationHall * panel3LocalRotation);

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
                                                   Materials->Steel,
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
