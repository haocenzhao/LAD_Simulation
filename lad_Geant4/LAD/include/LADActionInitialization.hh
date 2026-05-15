#ifndef LADActionInitialization_h
#define LADActionInitialization_h 1

#include "G4VUserActionInitialization.hh"

#include "LADPrimaryGeneratorAction.hh"
#include "LADRunAction.hh"
#include "LADEventAction.hh"
#include "LADSteppingAction.hh"
#include "HistoManager.hh"

#include "HodoAnalysis.hh"

#include "LADLUNDReader.hh"

class HodoAnalysis;

class LADPrimaryGeneratorAction;
class LADDetectorConstruction;
class HistoManager;
class LADRunAction;
class LADEventAction;
class LADSteppingAction;


//class LADLUNDReader

/// Action initialization class.

class LADActionInitialization : public G4VUserActionInitialization
{
  public:
    LADActionInitialization(LADDetectorConstruction*) ;
    ~LADActionInitialization();
  //     ~LADActionInitialization() override = default;

    void BuildForMaster() const override;
    void Build() const override;

private: 
  LADDetectorConstruction* detConstruction;

 };

#endif


