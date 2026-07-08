#ifndef LADDetectorConstructionFramesCreator_H
#define LADDetectorConstructionFramesCreator_H 1

// Panel 3 frame helper declarations for LADDetectorConstructionHodoCreator.
// This header is intentionally included inside the private section of
// LADDetectorConstructionHodoCreator.

void BuildPanel3Frame(G4LogicalVolume *worldLV, LADMaterials *Materials,
                      G4int panelIndex,
                      const G4ThreeVector &hodoCenterHall,
                      const G4RotationMatrix &hodoRotationHall);
void BuildMountingPads(G4AssemblyVolume *frameAssembly, LADMaterials *Materials);
void BuildVerticalLegs(G4AssemblyVolume *frameAssembly, LADMaterials *Materials);
void BuildDetectorMountTubes(G4AssemblyVolume *frameAssembly, LADMaterials *Materials);
void BuildGussets(G4AssemblyVolume *frameAssembly, LADMaterials *Materials);
void BuildChannels(G4AssemblyVolume *frameAssembly, LADMaterials *Materials);

#endif
