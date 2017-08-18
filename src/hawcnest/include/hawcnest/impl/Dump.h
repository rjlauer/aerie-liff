/*!
 * @file Dump.h
 * @author Colas Rivière
 * @brief Print the list of objects of the bag on the screen
 * 
 * Very short module to list the content of the bag, adapted
 * from John's ExamplePrintModule
 * @date 20 june 2013
 */

#ifndef DUMP_H_INCLUDED
#define DUMP_H_INCLUDED

#include <hawcnest/HAWCNest.h>

class Dump : public Module {

 public:

    typedef Module Interface;

    Result Process(BagPtr b);

};


#endif // DUMP_H_INCLUDED

