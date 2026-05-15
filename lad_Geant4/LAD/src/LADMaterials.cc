#include "LADMaterials.hh"
// #include "LADVariablen.hh"
// #include "LADSettings.hh"

#include "G4Element.hh"
#include "G4Material.hh"

LADMaterials::LADMaterials()
{
  G4String name, symbol;             //a=mass of a mole;
  G4double a, z, density;            //z=mean number of protons;
  
  G4int    ncomponents, natoms;
  G4double fractionmass;
  G4double temperature, pressure;
  G4int nel; //check if it is the same as ncomponents

  //
  // Chemist Elements
  //
  
  G4Element* H  = new G4Element(name="Hydrogen",symbol="H" , z= 1., a=1.01*g/mole);
  G4Element* C  = new G4Element(name="Carbon"  ,symbol="C" , z= 6., a=12.01*g/mole);
  G4Element* N  = new G4Element(name="Nitrogen",symbol="N" , z= 7., a=14.01*g/mole);
  G4Element* O  = new G4Element(name="Oxygen"  ,symbol="O" , z= 8., a=16.00*g/mole);
  G4Element* Si = new G4Element(name="Silicon", symbol="Si" ,z= 14.,a=28.09*g/mole);
  Al = new G4Material(name="Aluminium", z=13., a=26.98*g/mole,  density=2.70*g/cm3);
  G4Material * Fe = new G4Material(name="Iron",      z=26., a=55.845*g/mole, density=7.87*g/cm3);

  G4Material * W = new G4Material(name="Tungsten",    z=74., a=183.84*g/mole, density=19.3*g/cm3);

  //
  // Physic Vacuum
  //
  Vacuum = new G4Material("Vacuum", z=1., a=1.01*g/mole, density= universe_mean_density,
                           kStateGas, 2.73*kelvin, 3.e-18*pascal);
 

  // ----  gas and liquid deuterium (Lucas/Tyler)

  // Deuteron isotope
  G4Isotope* deuteron  = new G4Isotope("deuteron", 1, 2, 2.0141018*g/mole);
  
  // Deuterium element
  G4Element* deuterium = new G4Element("deuterium", "deuterium", 1);
  deuterium->AddIsotope(deuteron, 1);
  
  // Liquid Deuterium
  D2_liquid = new G4Material("LD2", 0.174*g/cm3, 1, kStateLiquid, 19.0*kelvin);
  D2_liquid->AddElement(deuterium, 2);
  
  // Hydrogen Gas (T = 19.5 K, P = 470 mTorr)
  H2_gas = new G4Material("H2_gas", density = 0.47 / 760.0 * 273.15 / 19.5 * 0.08988 * mg / cm3, 
			  ncomponents = 1, kStateGas, 19.5 * kelvin, 0.47 / 760.0 * atmosphere);
  H2_gas->AddElement(H, natoms = 2);



  
  //
  // Hydrogen\
  //
  
  /* REAL lH2 */
  G4Material* lH2 = new G4Material(name="lH2", density=67.9347 * kg/m3, ncomponents=1,
				   kStateLiquid,temperature=21.2*kelvin,pressure= 1.950*bar);
  lH2->AddElement(H, 2);

  
  //
  // Water
  //
  G4Material* matH2O = new G4Material("Water", density=1.000*g/cm3, 2);
  matH2O->AddElement(H,2);
  matH2O->AddElement(O,1);
  
  
  // Lead material defined using NIST Manager
  G4NistManager *nistManager = G4NistManager::Instance();
  G4Pb = nistManager->FindOrBuildMaterial("G4_Pb");

  G4Element *elH = nistManager -> FindOrBuildElement("H"); //Define hydrogen with natural isotopic abundances
  G4Element *elC = nistManager -> FindOrBuildElement("C");

  
  liquidArgon = new G4Material("liquidArgon", z=18., a= 39.95*g/mole, density= 1.390*g/cm3);
         // The argon by NIST Manager is a gas with a different density

 // Get materials
  defaultMaterial = Vacuum;
  absorberMaterial = G4Pb;
  gapMaterial = liquidArgon;

  
  // FROM G4SBS (modified) for BC-408
  // Let's start simple and later we can add optical properties
  
  G4double d_PolyVinylToluene = 0.57*g/cm3;
  G4double d_Anthracene = 1.28*g/cm3;
  G4double BC408density = 1.032*g/cm3;
  
  G4Material* PolyVinylToluene = new G4Material( "PolyVinylToluene", d_PolyVinylToluene, 2 );
  PolyVinylToluene->AddElement(elC, fractionmass = 0.91471);
  PolyVinylToluene->AddElement(elH, fractionmass = 0.08529);
  
  G4Material* Anthracene = new G4Material( "Anthracene", d_Anthracene, 2 );
  Anthracene->AddElement(elC, fractionmass = 0.943447);
  Anthracene->AddElement(elH, fractionmass = 0.056553);


  BC408 = new G4Material( "BC408", BC408density,  2 );
  BC408 -> AddMaterial(PolyVinylToluene, fractionmass = 0.36);
  BC408 -> AddMaterial(Anthracene, fractionmass = 0.64);
  
  
  // GEM Materials from g4sbs (when finish, put all the materials together)

  G4Element *elO  = nistManager -> FindOrBuildElement("O");
  G4Element *elN  = nistManager -> FindOrBuildElement("N");
  G4Element *elSi = nistManager -> FindOrBuildElement("Si");
  G4Element* elCl = nistManager -> FindOrBuildElement("Cl");
  G4Element* elAr = nistManager -> FindOrBuildElement("Ar");

  Air = nistManager ->FindOrBuildMaterial("G4_AIR");
    
  NEMAG10 = new G4Material("NEMAG10", 1.70*g/cm3, nel=4);
  NEMAG10 -> AddElement(elSi, 1);
  NEMAG10 -> AddElement(elO , 2);
  NEMAG10 -> AddElement(elC , 3);
  NEMAG10 -> AddElement(elH , 3);

  NOMEX_pure = new G4Material("NOMEX_pure", density = 1.38*g/cm3, 5);
  NOMEX_pure -> AddElement(elH,0.04);
  NOMEX_pure -> AddElement(elC,0.54);
  NOMEX_pure -> AddElement(elN,0.09);
  NOMEX_pure -> AddElement(elO,0.10);
  NOMEX_pure -> AddElement(elCl,0.23);
  
  NOMEX = new G4Material("NOMEX",density = 0.04*g/cm3, 2);
  NOMEX -> AddMaterial(NOMEX_pure,0.45);
  NOMEX -> AddMaterial(Air,0.55);
  
  Copper = nistManager->FindOrBuildMaterial( "G4_Cu" );
  
  Kapton = new G4Material("Kapton",   density = 1.42*g/cm3, nel=4);
  Kapton -> AddElement(elH, 0.026362);
  Kapton -> AddElement(elC, 0.691133);
  Kapton -> AddElement(elN, 0.073270);
  Kapton -> AddElement(elO, 0.209235);


  G4double density_Ar = 1.7823*mg/cm3 ;
  G4Material* Argon = new G4Material("Argon"  , density_Ar, nel=1);
  Argon->AddElement(elAr, 1);
  
  G4double density_CO2 = 1.977*mg/cm3;
  G4Material* CO2 = new G4Material("CO2", density_CO2, nel=2);
  CO2->AddElement(elC, 1);
  CO2->AddElement(elO, 2);

  G4double density_ArCO2 = .7*density_Ar + .3*density_CO2;
  // Use ArCO2
  GEMgas= new G4Material("GEMgas", density_ArCO2, nel=2);
  GEMgas -> AddMaterial(Argon, 0.7*density_Ar/density_ArCO2) ;
  GEMgas -> AddMaterial(CO2, 0.3*density_CO2/density_ArCO2) ;




  
  G4cout << "<LADMaterials::LADMaterials> --- Debugging ---"      << G4endl;



    
}

LADMaterials::~LADMaterials()
{
    G4cout << "<~LADMaterials> --> DONE" << G4endl; 
}
