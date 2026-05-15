#ifndef LADLUNDReader_h
#define LADLUNDReader_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"
#include "G4RunManager.hh"
#include "LADRunAction.hh"
#include "G4Run.hh"


#include <fstream>
#include <cstdlib>
#include <sstream>
#include <iostream>
#include <vector>

using namespace std;


class LADRunAction; 

class LADLUNDReader
{
public:
  LADLUNDReader();
  ~LADLUNDReader();

  void  OpenFile();
  void  CloseFile();
  bool  ReadFile();
  void  Clear();



  
  string fileName;
  ifstream cur_file;

  G4int GetnPart(){return nPart;}

  G4Run *runptr;
  
  vector<G4int> Getindex(){return index;}
  vector<G4double> Gett_live(){return t_live;} // lifetime [ns]
  vector<G4int> Gettype(){return type ;} // 1=active
  vector<G4int> Getpid(){return pid;}
  vector<G4int> GetparentInd(){return parentInd ;}
  vector<G4int> GetdaughtInd(){return daughtInd;}
  vector<G4double> Getpx(){return px ;}
  vector<G4double> Getpy(){return py ;}
  vector<G4double> Getpz(){return pz ;}
  vector<G4double> GetE(){return E ;}
  vector<G4double> Getm(){return m ;}
  vector<G4double> Getvx(){return vx;}
  vector<G4double> Getvy(){return vy;}
  vector<G4double> Getvz(){return vz;}


  
private:

  LADRunAction *RunAction;
 
  // https://gemc.jlab.org/gemc/html/documentation/generator/lund.html


  // HEADER
  string  it1, // Number of particles
    it2, // Mass number of the target (UD)
    it3, // Atomic number oif the target (UD)
    it4, // Target polarization (UD)
    it5, // Beam Polarization
    it6, // Beam type, electron=11, photon=22” (UD)
    it7, // Beam energy (GeV) (UD)
    it8, // Interacted nucleon ID (2212 or 2112) (UD)
    it9, // Process ID (UD)
    it10; // Event weight (UD)

  
  vector<string> particle;
  // Except the first variable, needed for looping later
  // the rest is more information of how the event is generated
  G4int nPart; 
  G4double A_Targ;
  G4int Z_Targ;
  G4double pol_targ;
  G4double pol_beam;
  G4int beamType; // 11 Electron, 22 Photon
  G4double Eb;
  G4int G4interNuclID; // G4interacted Nucleon ID
  G4int ProcessID;
  G4double EvWeight;

  
  // const G4int nMax = 50;
  
  // G4int index[nMax];
  // G4double t_live[nMax]; // lifetime [ns]
  // G4int type[nMax]; // 1=active
  // G4int pid[nMax];
  // G4int parentInd[nMax];
  // G4int daughtInd[nMax];
  // G4double px[nMax];
  // G4double py[nMax];
  // G4double pz[nMax];
  // G4double E[nMax];
  // G4double m[nMax];
  // G4double vx[nMax];
  // G4double vy[nMax];
  // G4double vz[nMax];

  vector<G4int> index; 
  vector<G4double> t_live; // lifetime [ns]
  vector<G4int> type; // 1=active
  vector<G4int> pid;
  vector<G4int> parentInd;
  vector<G4int> daughtInd;
  vector<G4double> px;// momentum x [GeV]
  vector<G4double> py;// momentum y [GeV]
  vector<G4double> pz;// momentum z [GeV]
  vector<G4double> E;// Energy of the particle [GeV] (
  vector<G4double> m;//Mass of the particle [GeV]
  vector<G4double> vx;// vertex x [cm]
  vector<G4double> vy;// vertex y [cm]
  vector<G4double> vz;// vertex z [cm]
};

extern LADLUNDReader *LundRead;



#endif
