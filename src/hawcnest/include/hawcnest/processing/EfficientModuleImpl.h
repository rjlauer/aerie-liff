/*!
 * @file EfficientModuleImpl.h 
 * @brief Implementation of "efficient" modules which cache memory.
 * @author John Pretz 
 * @date 2 Mar 2010 
 * @version $Id: EfficientModuleImpl.h 15398 2013-06-03 18:34:03Z jbraun $
 */

#ifndef HAWCNEST_EFFICIENTMODULEIMPL_H_INCLUDED
#define HAWCNEST_EFFICIENTMODULEIMPL_H_INCLUDED

#include <hawcnest/processing/EfficientModule.h>
#include <hawcnest/Configuration.h>
#include <hawcnest/impl/Has.h>

#include <boost/tuple/tuple.hpp>

#include <vector>

// functions and macros for input/output parameter definition
// Use a template class -- may need partial specializations
template <class ModuleType, int param>
struct get_input_parameter_name_impl {
  static const char* name();
};

template <class ModuleType, int param>
struct get_output_parameter_name_impl {
  static const char* name();
};

#define INPUT(TYPE,NUMBER,NAME) template<> \
struct get_input_parameter_name_impl<TYPE,NUMBER> { \
  static const char* name() \
  {return BOOST_PP_STRINGIZE(NAME);} \
};

#define OUTPUT(TYPE,NUMBER,NAME) template<> \
struct get_output_parameter_name_impl<TYPE,NUMBER> { \
  static const char* name() \
  {return BOOST_PP_STRINGIZE(NAME);} \
};

/*
inline const char* get_input_parameter_name();

template <class ModuleType,int param>
inline const char* get_output_parameter_name();

#define INPUT(TYPE,NUMBER,NAME) template<> \
inline const char* get_input_parameter_name<TYPE,NUMBER>() \
{return BOOST_PP_STRINGIZE(NAME);}

#define OUTPUT(TYPE,NUMBER,NAME) template<> \
inline const char* get_output_parameter_name<TYPE,NUMBER>() \
{return BOOST_PP_STRINGIZE(NAME);}

*/

// compile-time iterative 'newing' of objects in a tuple
template <class tuple_type,int item>
class make_new_tuple_item{
public:
  typedef typename boost::tuples::element<item-1,tuple_type>::type 
    shared_ptr_type;
  typedef typename shared_ptr_type::element_type element_type;

  static void doit(tuple_type& thetuple){
    thetuple.template get<item-1>() = 
      boost::shared_ptr<element_type>(new element_type());
    make_new_tuple_item<tuple_type,item-1>::doit(thetuple);
  }
};

// terminating the iteration
template<class tuple_type>
class make_new_tuple_item<tuple_type,0>{
public:

  static void doit(tuple_type& thetuple){
  }
};

// A little function which 'news' all the objects in a boost::tuple
// requires that the elements be all shared pointers to some types
template <class tuple_type>
  void new_tuple_items(tuple_type& the_tuple){
  make_new_tuple_item<tuple_type,
                boost::tuples::length<tuple_type>::value>::doit(the_tuple);
}

// compile-time iteration for filling input names
template <class ModuleType,int item>
  class fill_input_names{
 public:
  static void doit(std::vector<std::string>& names){
    names.insert(names.begin(),
        get_input_parameter_name_impl<ModuleType,item-1>::name());
    fill_input_names<ModuleType,item-1>::doit(names);
  }
};

// terminating the iteration
template <class ModuleType>
class fill_input_names<ModuleType,0>{
 public:
  static void doit(std::vector<std::string>& names){
  }
};


// compile-time iteration for filling output names
template <class ModuleType,int item>
  class fill_output_names{
 public:
  static void doit(std::vector<std::string>& names){
    names.insert(names.begin(),
        get_output_parameter_name_impl<ModuleType,item-1>::name());
    fill_output_names<ModuleType,item-1>::doit(names);
  }
};

// terminating the iteration
template <class ModuleType>
class fill_output_names<ModuleType,0>{
 public:
  static void doit(std::vector<std::string>& names){
  }
};

// compile-time iteration for getting output from bag
template <class TupleType, int item>
  class fill_output_from_bag{
 public:
  static void doit(TupleType& thetuple,
                   BagPtr bag,
                   std::vector<std::string> names){
    typedef typename boost::tuples::element<item-1,TupleType>::type Bagged;
    thetuple.template get<item-1>() = 
      bag->Get<Bagged>(names[item-1]);
    fill_output_from_bag<TupleType,item-1>::doit(thetuple,bag,names);
  }
};


// terminating the iteration
template <class TupleType>
class fill_output_from_bag<TupleType,0>{
 public:
  static void doit(TupleType& thetuple,
                   BagPtr bag,
                   std::vector<std::string> names){
  }
};


// compile-time iteration for getting output from bag
template <class TupleType, int item>
  class fill_bag_input{
 public:
  static void doit(TupleType& thetuple,
                   BagPtr bag,
                   std::vector<std::string> names){
    bag->Put(names[item-1],thetuple.template get<item-1>());
    fill_bag_input<TupleType,item-1>::doit(thetuple,
                                           bag,
                                           names);
  }
};


// terminating the iteration
template <class TupleType>
class fill_bag_input<TupleType,0>{
 public:
  static void doit(TupleType& thetuple,
                   BagPtr bag,
                   std::vector<std::string> names){
  }
};

// The actual service, implementing the EfficientModule interface
template <class Wrapped>
class EfficientModuleImpl : public EfficientModule{
 public:
  typedef typename Wrapped::Input Input;
  typedef typename Wrapped::Output Output;
  typedef EfficientModule Interface;

  EfficientModuleImpl(){
    new_tuple_items(output_);

    fill_input_names<Wrapped,
      boost::tuples::length<Input>::value >::doit(inputParamNames_);
    fill_output_names<Wrapped,
      boost::tuples::length<Output>::value >::doit(outputParamNames_);
                         
  }

  Configuration DefaultConfiguration(){

    Configuration config = CallDefaultConfiguration(wrapped_);

    for(std::vector<std::string>::const_iterator iter = 
          inputParamNames_.begin();
        iter != inputParamNames_.end();
        iter++){

      // Add a configuration parameter if not already present
      // This allows the same parameter names in Efficient loop
      // as in the standard loop.
      if (!config.HasParameter(*iter)) {
        config.Parameter<std::string>(*iter);
      }
    }

    for(std::vector<std::string>::const_iterator iter = 
          outputParamNames_.begin();
        iter != outputParamNames_.end();
        iter++){

      // Add a configuration parameter if not already present
      if (!config.HasParameter(*iter)) {
        config.Parameter<std::string>(*iter);
      }
    }

    return config;
  }

  void Initialize(const Configuration& config){
    for(std::vector<std::string>::const_iterator iter = 
          inputParamNames_.begin() ; 
        iter != inputParamNames_.end() ; 
        iter++){
      std::string parametervalue;
      config.GetParameter(*iter,parametervalue);
      inputNames_.push_back(parametervalue);
    }

    for(std::vector<std::string>::const_iterator iter = 
          outputParamNames_.begin() ; 
        iter != outputParamNames_.end() ; 
        iter++){
      std::string parametervalue;
      config.GetParameter(*iter,parametervalue);
      outputNames_.push_back(parametervalue);
    }

    CallInitialize(wrapped_, config);
  }

  Module::Result Process(){
    return wrapped_.Process(input_,output_);
  }

  void TemplateBag(BagPtr bag){
    assert(boost::tuples::length<Input>::value == (int)inputParamNames_.size());
    assert(boost::tuples::length<Output>::value == 
           (int)outputParamNames_.size());

    fill_output_from_bag<Input,
      boost::tuples::length<Input>::value>::doit(input_,
                                                 bag,
                                                 inputNames_);

    fill_bag_input<Output,
      boost::tuples::length<Output>::value>::doit(output_,
                                                  bag,
                                                  outputNames_);
  }

  void Finish() {
    CallFinish(wrapped_);
  }

 private:
  Wrapped wrapped_;

  Input input_;
  std::vector<std::string> inputParamNames_;
  std::vector<std::string> inputNames_;
  Output output_;
  std::vector<std::string> outputParamNames_;
  std::vector<std::string> outputNames_;
};

#endif // HAWCNEST_EFFICIENTMODULEIMPL_H_INCLUDED

