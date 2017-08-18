/**
 * FILE main.cxx
 */

#include <iostream>
#include <hawcnest/HAWCNest.h>
#include <hawcnest/processing/SequentialMainLoop.h>
#include <vector>
#include <cmath>
#include "ExampleComponents.h"

using namespace std;


// showing off how to dig my favorite information out of the event
// stream
class MyModule : public Module{
public:
  typedef Module Interface;

  Result Process(BagPtr e){

    return Continue;
  }
};


int main(int argc,char** argv){
  HAWCNest nest;

  nest.Service<STDRandomNumberService>("rand")
    ("Seed",12345);

  nest.Service<DBCalibrationService>("calib")
    ("server","mildb.umd.edu")
    ("uname","milagro")
    ("password","topsecret");

  nest.Service<ExampleSource>("source")
    ("input","myinputfile.dat")
    ("maxevents",20);

  nest.Service<CalibrationModule>("calibmodule");

  nest.Service<ReconstructionModule_COM>("reco_com");

  nest.Service<ReconstructionModule_Gauss>("reco_gauss");

  nest.Service<PrintingModule>("print");

  nest.Service<MyModule>("mymodule");

  vector<string> modulechain;
  modulechain.push_back("print");       // <---
  modulechain.push_back("calibmodule"); //    |
  modulechain.push_back("reco_com");    //    |-- one module in two places  
  modulechain.push_back("reco_gauss");  //    |
  modulechain.push_back("print");       // <---
  modulechain.push_back("mymodule");
  nest.Service<SequentialMainLoop>("mainloop")
    ("source","source")
    ("modulechain",modulechain);

  nest.Configure();

  MainLoop& loop = GetService<MainLoop>("mainloop");

  loop.Execute();
  
  nest.Finish();
}

