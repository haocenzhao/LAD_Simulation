#include "LADLUNDReader.hh"




using namespace std;

LADLUNDReader::LADLUNDReader()
{
  G4cout<<"LADLUNDReader::LADLUNDReader()"<<G4endl;
  
}


LADLUNDReader::~LADLUNDReader()
{
  
}
void LADLUNDReader::OpenFile()
{

  fileName ="LUND_example_2.txt";
  cur_file.open(fileName);
  
  G4cout<<fileName<<G4endl;
  // need to add a control in case file doesn't exist

  
}

void LADLUNDReader::CloseFile()
{
  
  G4cout<<"LADLUNDReader Closing file"<<G4endl;
  cur_file.close();

    
}


bool LADLUNDReader::ReadFile()
{
  G4cout<<"Reading the file"<<G4endl;

  Clear();

  string line1;
  string line2;
  getline(cur_file, line1);  
  //  G4cout<<"Line: "<<line1<<G4endl;

  // runptr = RunAction->GetaRun();
 // G4cout<< runptr->GetNumberOfEvent()<<G4endl;
  if (cur_file.eof())
    {
      G4ExceptionDescription msg;
      msg << "End of File. Exceed number of run/beamOn events" << G4endl;
      G4Exception("LADLUNDReader::ReadFile()",
                  "Code001", RunMustBeAborted, msg);
      exit(0);// yep!! radical approach!!
      return false ;
      
    }
  // The strategy:
  // - read the first line ONLY corresponding to the header
  // - get the number of particle in the event
  // - loop to read the number of lines retrieve from the header
  // - move out of the event
  
  

  // get the first line, which is the header
  // this will set how many lines I need to read afterwards
  // for the whole event


  // I prefer to do the readout like this for clarity of each variable in the header
  istringstream iss1(line1);

  iss1 >> it1 >> it2 >> it3 >> it4 >> it5 >> it6 >> it7 >> it8 >> it9 >> it10;

  nPart = stoi(it1); // needed for the loop over the particles, next

  G4cout<<"nPart: "<< nPart<<G4endl;

  // for(G4int kk = 0; kk < nPart ; kk++) 

    G4int kk = 0;

  while (kk <nPart)
    {
      getline(cur_file, line2);
      istringstream iss2(line2);
      //  


        
      // Here I use std:vectors, because will be easy
      // store the info,for an unknown number of particles
      //      for (std::string s2; iss2 >> s2;)
      for (std::string s2; iss2 >> s2;)
	{
	  particle.push_back(s2);
	  //	    G4cout<<s2<<" ";
	}
      
      index.push_back( atoi(particle.at(0).c_str() ));
      t_live.push_back( atof(particle.at(1).c_str()) );
      type.push_back( atoi(particle.at(2).c_str()) );
      pid.push_back( atoi(particle.at(3).c_str()) );
      parentInd.push_back( atoi(particle.at(4).c_str()) );
      daughtInd.push_back( atoi(particle.at(5).c_str()) );
      px.push_back( atof(particle.at(6).c_str()) );
      py.push_back( atof(particle.at(7).c_str()) );
      pz.push_back( atof(particle.at(8).c_str()) );
      E.push_back( atof(particle.at(9).c_str()) );
      m.push_back( atof(particle.at(10).c_str()) );
      vx.push_back( atof(particle.at(11).c_str()) );
      vy.push_back( atof(particle.at(12).c_str()) );
      vz.push_back( atof(particle.at(13).c_str())) ;        

      particle.clear();
      kk++;
    }
  G4cout<<"End of reading file"<<G4endl;

  return true ;
}


void LADLUNDReader::Clear()
{

  nPart = -1; //just in case
  it1 = it2 = it3 = it4 = it5 = it6 = it7 = it8 = it9 = it10 = -1;//actually IT is not necessary, but helps with debugging
  particle.clear();

  index.clear();
  t_live.clear();
  type.clear();
  pid.clear();
  parentInd.clear();
  daughtInd.clear();
  px.clear();
  py.clear();
  pz.clear();
  E.clear();
  m.clear();
  vx.clear();
  vy.clear();
  vz.clear();        
	  
}



LADLUNDReader *LundRead=NULL;
