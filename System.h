#ifndef System_h
#define System_h

#include "Data.h"

class System {
  public:
    System(DateTime* now,bool* bypassFlag):now(now), bypassFlag(bypassFlag){}
    virtual ~System(){}
    virtual void initialize() = 0;
    virtual void run() = 0;
  protected:
    DateTime* now;
    bool* bypassFlag;
    bool enabled = true;
};

#endif
