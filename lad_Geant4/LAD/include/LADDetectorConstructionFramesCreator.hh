#ifndef LADDetectorConstructionFramesCreator_H
#define LADDetectorConstructionFramesCreator_H 1

// Panel 3 frame and wall-stand helper declarations for
// LADDetectorConstructionHodoCreator.
// This header is intentionally included inside the private section of
// LADDetectorConstructionHodoCreator.

void BuildPanel3Frame(G4LogicalVolume *worldLV, LADMaterials *Materials,
                      G4int panelIndex,
                      const G4ThreeVector &hodoCenterHall,
                      const G4RotationMatrix &hodoRotationHall);
void BuildDoubleStand(G4LogicalVolume *worldLV, LADMaterials *Materials,
                      G4int wallIndex,
                      const G4ThreeVector &doubleWallCenterHall,
                      const G4RotationMatrix &hodoRotationHall);
void BuildDoubleStandItem11DetectorAttachmentPlates(
  G4AssemblyVolume *standAssembly, LADMaterials *Materials);
void BuildDoubleStandLongTube(G4AssemblyVolume *standAssembly,
                              LADMaterials *Materials);
void BuildDoubleStandHorizontalItem6MidTubes(
  G4AssemblyVolume *standAssembly, LADMaterials *Materials);
void BuildDoubleStandItem10AttachmentPlates(
  G4AssemblyVolume *standAssembly, LADMaterials *Materials);
void BuildDoubleStandItem7And8VerticalFrames(
  G4AssemblyVolume *standAssembly, LADMaterials *Materials);
void BuildDoubleStandItem12LowerBasePlates(
  G4AssemblyVolume *standAssembly, LADMaterials *Materials);
void BuildSingleStand(G4LogicalVolume *worldLV, LADMaterials *Materials,
                      G4int panelIndex,
                      const G4ThreeVector &hodoCenterHall,
                      const G4RotationMatrix &hodoRotationHall);
void BuildSingleStandTopPlates(G4AssemblyVolume *standAssembly,
                               LADMaterials *Materials);
void BuildSingleStandVerticalTubes(G4AssemblyVolume *standAssembly,
                                   LADMaterials *Materials);
void BuildSingleStandBottomPlates(G4AssemblyVolume *standAssembly,
                                  LADMaterials *Materials);
void BuildSingleStandBraceTubes(G4AssemblyVolume *standAssembly,
                                LADMaterials *Materials);
void BuildSingleStandHorizontalMountingPlates(G4AssemblyVolume *standAssembly,
                                              LADMaterials *Materials);
void BuildSingleStandGussetMountingPlates(G4AssemblyVolume *standAssembly,
                                          LADMaterials *Materials);
void BuildSingleStandHorizontalWeldment(G4AssemblyVolume *standAssembly,
                                        LADMaterials *Materials);
void BuildSingleStandHorizontalTube(G4AssemblyVolume *standAssembly,
                                    LADMaterials *Materials);
void BuildSingleStandHorizontalEndPlates(G4AssemblyVolume *standAssembly,
                                         LADMaterials *Materials);
void BuildSingleStandHorizontalGussetMountingPlates(
  G4AssemblyVolume *standAssembly, LADMaterials *Materials);
void BuildSingleStandGussetWeldments(G4AssemblyVolume *standAssembly,
                                     LADMaterials *Materials);
void BuildSingleStandGussetEndPlates(G4AssemblyVolume *standAssembly,
                                     LADMaterials *Materials);
void BuildSingleStandGussetTubes(G4AssemblyVolume *standAssembly,
                                 LADMaterials *Materials);
void PlacePanel3LocalAssembly(G4AssemblyVolume *assembly,
                              G4LogicalVolume *worldLV,
                              G4int panelIndex,
                              G4int copyNumberBase,
                              const G4ThreeVector &hodoCenterHall,
                              const G4RotationMatrix &hodoRotationHall,
                              const G4RotationMatrix &assemblyRotationInPanel3);
void BuildMountingPads(G4AssemblyVolume *frameAssembly, LADMaterials *Materials);
void BuildVerticalLegs(G4AssemblyVolume *frameAssembly, LADMaterials *Materials);
void BuildDetectorMountTubes(G4AssemblyVolume *frameAssembly, LADMaterials *Materials);
void BuildGussets(G4AssemblyVolume *frameAssembly, LADMaterials *Materials);
void BuildChannels(G4AssemblyVolume *frameAssembly, LADMaterials *Materials);

#endif
