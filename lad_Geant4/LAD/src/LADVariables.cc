#include <sstream>
#include <fstream>
using namespace std;

#include "LADVariables.hh"
#include "G4SystemOfUnits.hh"


#define READBOOL(myx) \
				if (buf == #myx) \
				{ \
					ss >> word; \
					myx = ReadBoolean(word); \
				}

#define READDOUBLE(x,unit) \
				if (buf == #x) \
				{ \
					ss >> doubleword; \
					x = doubleword * unit; \
				}

#define READINT(x) \
				if (buf == #x) \
				{ \
					ss >> intword; \
					x = intword; \
				}

#define READSTRING(x) \
				if (buf == #x) \
				{ \
					ss >> x; \
				}

#define READVECTOR(v) \
				if (buf == #v) \
				{ \
					G4double x,y,z; \
					if (ss >> x >> y >> z) \
					v = G4ThreeVector(x * m, y * m, z * m); \
				}



LADVariables::LADVariables()
{
	G4cout << "<LADVariables::LADVariables>: Start" << G4endl;

	//LAD DEFAULT VALUES
	
	centralWallAngle = 127 *deg;
	leftWallAngle    =  23 *deg; // wrt to the centralWallAngle
	rightWallAngle   =  23 *deg; // wrt to the centralWallAngle

	centralWallDistance = 523 *cm; // wrt to the center of the hall
	leftWallDistance    =  92 *cm;  // wrt to the centralWall
	rightWallDistance   =  92 *cm; // wrt to the centralWallAngle

	DISrootFile = "";

}

LADVariables::~LADVariables()
{
	G4cout << "<LADVariables::~LADVariables>: Finished" << G4endl;
}

G4int 
LADVariables::LoadFromFile(G4String FileName)
{
  G4cout << "<LADVariables::LADVariables>: reading file" << G4endl;
  
	ifstream infile;
	G4String line;
	infile.open(FileName);
	if (infile)
	{
		while (!infile.eof())
		{
			if (getline(infile,line))
			{
				G4String buf;
				G4String word;
				G4double doubleword;
				G4int intword;
				stringstream ss(line);
//				G4cout << ">" << line << G4endl;
				ss >> buf;
//				G4cout << ">" << ss.str() << "<" << G4endl;

				READDOUBLE(centralWallAngle, deg);
				READDOUBLE(leftWallAngle, deg); // wrt to the centralWallAngle
				READDOUBLE(rightWallAngle, deg); // wrt to the centralWallAngle

				READDOUBLE(centralWallDistance, cm); // wrt to the center of the hall
				READDOUBLE(leftWallDistance, cm);  // wrt to the centralWall
				READDOUBLE(rightWallDistance, cm); // wrt to the centralWallAngle

				READBOOL(G4GUI);//GUI on/off

				READINT(MultiTh);//multithread. DO NOT CHANGE IT!!

				READINT(GeneratorCase);//particle generator case

				READSTRING(ScanParticle);
				READDOUBLE(ScanMomentumParticle, MeV); 

				READSTRING(DISrootFile);
				
			} // if getline
		} // if not end of file
		return 0;
	} // if infile
	else
	{
		G4cerr << "Konnte Datei nicht zum lesen �ffnen!" << G4endl;
		return -1;
	} // if infile else
}


G4int 
LADVariables::SaveToFile(G4String FileName)
{
	ofstream outfile;
	outfile.open(FileName);
	if (outfile)
	{
		outfile << *this;
		G4cout << "Folgendes wurde ins File geschrieben:" << G4endl << *this;
		outfile.close();
		return 0;
	}
	else
	{
		return -1;
	}
}

ostream & operator << (ostream & os, const LADVariables v) {
	return os 
	
;
}

ostream & operator << (ostream & os, const LADVariables *v) {
	return os 
	
;
}


G4bool
LADVariables::ReadBoolean(G4String Value)
{
	if ((Value=="true")
		|| (Value=="True")
		|| (Value=="1"))
	{return true;}
	else
	{return false;}
}


LADVariables *Variables=NULL;
