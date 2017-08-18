# importing the needed classes and functions
from HAWCNest import HAWCNest
from HAWCNest import load
from HAWCNest import GetService_MainLoop

# loading shared libraries that might be needed for processing
load("libhawcnest")
load("libexample-hawcnest")

# This is nearly identical to the C++ interface, except it uses python
# syntax
nest = HAWCNest()

# adding and configuring services
nest.Service("STDRandomNumberService","rand")(
    ("Seed",12345)
    )
nest.Service("DBCalibrationService","calib")(
    ("server","mildb.umd.edu")
    ("uname","milagro")
    ("password","topsecret")
    )
nest.Service("ExampleSource","source")(
    ("input","myinputfile.dat")
    ("maxevents",20)
    )
nest.Service("CalibrationModule","calibmodule")
nest.Service("ReconstructionModule_COM","reco_com")
nest.Service("ReconstructionModule_Gauss","reco_gauss")
nest.Service("PrintingModule","print")
nest.Service("SequentialMainLoop","mainloop")(
    ("source","source")
    ("modulechain",["print","calibmodule","reco_com",
                    "reco_gauss","print","mymodule"]),
    )
nest.Configure()
main = GetService_MainLoop("mainloop")
loop.Execute()
nest.Finish()

