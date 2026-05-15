#ifndef LADHodoHit_h
#define LADHodoHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "G4Threading.hh"

// namespace LADHodo
// {



class LADHodoHit : public G4VHit
{
public:
  LADHodoHit() = default;
  LADHodoHit(const LADHodoHit&) = default;
  ~LADHodoHit() override = default;

  // operators
  LADHodoHit& operator=(const LADHodoHit&) = default;
  G4bool operator==(const LADHodoHit&) const;

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

using LADHodoHitsCollection = G4THitsCollection<LADHodoHit>;

extern G4ThreadLocal G4Allocator<LADHodoHit>* LADHodoHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void* LADHodoHit::operator new(size_t)
{
  if (!LADHodoHitAllocator) {
    LADHodoHitAllocator = new G4Allocator<LADHodoHit>;
  }
  void *hit;
  hit = (void *) LADHodoHitAllocator->MallocSingle();
  return hit;
}

inline void LADHodoHit::operator delete(void *hit)
{
  if (!LADHodoHitAllocator) {
    LADHodoHitAllocator = new G4Allocator<LADHodoHit>;
  }
  LADHodoHitAllocator->FreeSingle((LADHodoHit*) hit);
}

inline void LADHodoHit::Add(G4double de, G4double dl) {
  fEdep += de;
  fTrackLength += dl;
}

inline G4double LADHodoHit::GetEdep() const {
  return fEdep;
}

inline G4double LADHodoHit::GetTrackLength() const {
  return fTrackLength;
}

//}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
