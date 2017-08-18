

// A fake Reconstruction module... COM technique
class ReconstructionModule_COM : public Module
{
 public:
  typedef Module Interface;

  Result Process(BagPtr e){
    RandomNumberService& random = GetService<RandomNumberService>("rand"); 
    printf("reconstructing the event with COM technique\n"); 

    CoreReconstructionPtr core = make_shared<CoreReconstruction>();
    core->corex =random.Uniform(-2000,2000); 
    core->corey =random.Uniform(-2000,2000); 

    e->Put("Core_COM",core);
    
    return Continue;
  }
};
