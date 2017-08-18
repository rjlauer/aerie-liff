#include <iostream>

using namespace std;

class Interface{
public:
  virtual void DoSomething() = 0;
};

void something_to_do_something(Interface* i){
  i->DoSomething();
}

class SomethingUseful : public Interface{
public:
  virtual void DoSomething() { cout<<"Something Useful...."<<endl;}
}

class SomethingElseUseful : public Interface{
public:
  virtual void DoSomething() { cout<<"Something Else Useful...."<<endl;}
};

int main(){
  Interface* i = new SomethingUseful();
  Interface* j = new SomethingElseUseful();
  
  something_to_do_something(i);
  something_to_do_something(j);
  
}
