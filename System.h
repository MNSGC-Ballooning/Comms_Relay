#ifndef System_h
#define System_h

#include "Data.h"

//Superclass to define common traits of all driving system classes
class System {
  public:
    System(DateTime* now,bool* bypassFlag):now(now), bypassFlag(bypassFlag){}
    virtual ~System(){}
    //Functions that define what a given system does during setup() and each iteration of loop()
    virtual void initialize() = 0;
    virtual void run() = 0;
  protected:
    DateTime* now; //pointer to common DateTime object used for timestamping. Updated by GPS
    bool* bypassFlag; //pointer to common bypass flag triggered by push switch. Used to skip parts of setup if needed
    bool enabled = true; //allows each system to be separately enabled or disabled in event of possible errors
};

#endif
