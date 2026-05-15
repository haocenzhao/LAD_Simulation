#ifndef LADGEMHit_h
#define LADGEMHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "G4Threading.hh"

/*
  class LADGEMHit : public G4VHit
  {
  public:
  LADGEMHit() = default;
  LADGEMHit(const LADGEMHit&) = default;
  ~LADGEMHit() override = default;

  // operators
  LADGEMHit& operator=(const LADGEMHit&) = default;
  G4bool operator==(const LADGEMHit&) const;

  inline void* operator new(size_t);
  inline void  operator delete(void*);

  // methods from base class
  void Draw()  override{}
  void Print() override;

  // methods to handle data
  void Add(G4double de, G4double dl);

  // get methods
  G4double GetEdep() const;
  G4double GetTrackLength() const;

  private:
  G4double fEdep = 0.;        ///< Energy deposit in the sensitive volume
  G4double fTrackLength = 0.; ///< Track length in the  sensitive volume
  };

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  using GEMHitsCollection = G4THitsCollection<LADGEMHit>;

  extern G4ThreadLocal G4Allocator<LADGEMHit>* LADGEMHitAllocator;

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  inline void* LADGEMHit::operator new(size_t)
  {
  if (!LADGEMHitAllocator) {
  LADGEMHitAllocator = new G4Allocator<LADGEMHit>;
  }
  void *hit;
  hit = (void *) LADGEMHitAllocator->MallocSingle();
  return hit;
  }

  inline void LADGEMHit::operator delete(void *hit)
  {
  if (!LADGEMHitAllocator) {
  LADGEMHitAllocator = new G4Allocator<LADGEMHit>;
  }
  LADGEMHitAllocator->FreeSingle((LADGEMHit*) hit);
  }

  inline void LADGEMHit::Add(G4double de, G4double dl) {
  G4cout<<"fEdep (1): "<<fEdep<<G4endl;
  G4cout<<"de: "<<de<<G4endl;
  fEdep += de;
  G4cout<<"fEdep (2): "<<fEdep<<G4endl;

  fTrackLength += dl;
  }

  inline G4double LADGEMHit::GetEdep() const {
  return fEdep;
  }

  inline G4double LADGEMHit::GetTrackLength() const {
  return fTrackLength;
  }



  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  #endif

*/


/// Drift chamber hit
///
/// It records:
/// - the layer ID
/// - the particle time
/// - the particle local and global positions

class LADGEMHit : public G4VHit
{
public:
  LADGEMHit() = default;
  LADGEMHit(G4int layerID);
  LADGEMHit(const LADGEMHit &right) = default;
  ~LADGEMHit() override = default;

  LADGEMHit& operator=(const LADGEMHit &right) = default;
  G4bool operator==(const LADGEMHit &right) const;

  inline void *operator new(size_t);
  inline void operator delete(void *aHit);

  void Draw() override;
  const std::map<G4String,G4AttDef>* GetAttDefs() const override;
  std::vector<G4AttValue>* CreateAttValues() const override;
  void Print() override;

  void SetLayerID(G4int z) { fLayerID = z; }
  G4int GetLayerID() const { return fLayerID; }

  void SetTime(G4double t) { fTime = t; }
  G4double GetTime() const { return fTime; }

  void SetLocalPos(G4ThreeVector xyz) { fLocalPos = xyz; }
  G4ThreeVector GetLocalPos() const { return fLocalPos; }

  void SetWorldPos(G4ThreeVector xyz) { fWorldPos = xyz; }
  G4ThreeVector GetWorldPos() const { return fWorldPos; }

  void Add(G4double de);
  G4double GetEdep() const;

  void SetLevel(G4int xyz) { iLevel = xyz; }
  G4int GetLevel() const { return iLevel; }

  void SetPDG(G4int xyz) { iPDG = xyz; }
  G4int GetPDG() const { return iPDG; }

private:
  G4int fLayerID = -1;

  G4double fTime = 0.;
  G4ThreeVector fLocalPos;
  G4ThreeVector fWorldPos;
  G4double fEdep = 0.;

  G4int iLevel = 0;
  G4int iPDG = 0;

 
  

  
};

using LADGEMHitsCollection = G4THitsCollection<LADGEMHit>;

extern G4ThreadLocal G4Allocator<LADGEMHit>* LADGEMHitAllocator;

inline void* LADGEMHit::operator new(size_t)
{
  if (!LADGEMHitAllocator) {
    LADGEMHitAllocator = new G4Allocator<LADGEMHit>;
  }
  return (void*)LADGEMHitAllocator->MallocSingle();
}

inline void LADGEMHit::operator delete(void* aHit)
{
  LADGEMHitAllocator->FreeSingle((LADGEMHit*) aHit);
}

inline G4double LADGEMHit::GetEdep() const {
  return fEdep;
}

inline void LADGEMHit::Add(G4double de)
{
  // G4cout<<"fEdep (1): "<<fEdep<<G4endl;
  //  G4cout<<"de: "<<de<<G4endl;
  fEdep += de;
  //  G4cout<<"fEdep (2): "<<fEdep<<G4endl;

}



#endif
