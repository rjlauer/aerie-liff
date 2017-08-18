/*!
 * @file EMS.cc 
 * @brief Interface to data from the EMS system.
 * @author Andreas Tepe 
 * @date 25 Jan 2012 
 * @version $Id: EMS.cc 14880 2013-04-28 12:19:08Z sybenzvi $
 */

#ifndef SCALER_CLASSES_NOT_FOUND

#include <data-structures/detector/EMS.h>

#include <hawcnest/HAWCUnits.h>

using namespace det;


EMS::EMS():
  TypeID_    (0),
  SensorID_  (0),
  RawValue_  (-2e10),
  PhysValue_ (-2e10),
  Valid_     (false)
{
}

bool EMS::SetSensor(const std::string &Sensor)
{
  std::stringstream
    Help;
//  unsigned int
  size_t
    Pos (0);

  Help.clear();
  Help.str("");

  Pos = Sensor.find('.');
  if (Pos == std::string::npos) return false;
  Help << Sensor.substr(0,Pos);
  Help >> TypeID_;
  
  Help.clear();
  Help.str("");

  Help << Sensor.substr(Pos+1);
  Help >> SensorID_;
  
  return (TypeID_ > 0 && SensorID_ > 0);
}

std::string EMS::GetSensorString() const
{
  std::stringstream
    Help;
  
  Help.clear();
  Help.str("");
  
  Help << TypeID_ << "." << SensorID_;
  
  return Help.str();
}

unsigned short EMS::GetTypeID() const
{
  return TypeID_;
}

unsigned short EMS::GetSensorID() const
{
  return SensorID_;
}

bool EMS::SetRawValue(const std::string &Value)
{
  if (Value == "+9.99999E+9")
  {
    SetRawValue(-1e-10);
    SetValid(false);
  }
  else
  {
    std::stringstream Help;
    
    Help.clear();
    Help.str("");
    
    Help << Value;
    
    Help >> RawValue_;

    SetValid(true);
  }
  
  return IsValid();
}

void EMS::SetPhysicsValue(const std::string &Value)
{
  std::stringstream Help;
  
  Help.clear();
  Help.str("");
  
  Help << Value;
  
  Help >> PhysValue_;
}

void EMS::SetRawValue(double Value)
{
  RawValue_ = Value;
}

void EMS::SetPhysicsValue(double Value)
{
  PhysValue_ = Value;
}

bool EMS::SetValues(const std::string &Raw, const std::string &Phys)
{
  SetPhysicsValue(Phys);

  return SetRawValue(Raw);
}

double EMS::GetRawValue() const
{
  return RawValue_;
}

double EMS::GetPhysicsValue() const
{
  return PhysValue_;
}

void EMS::SetValid (bool Valid)
{
  Valid_ = Valid;
}

bool EMS::IsValid () const
{
  return Valid_;
}

void EMS::Clear()
{
  TypeID_    = 0;
  SensorID_  = 0;
  RawValue_  =-1e-10;
  PhysValue_ =-1e-10;
  Valid_     = false;
  Time_.Clear();
}


#endif

