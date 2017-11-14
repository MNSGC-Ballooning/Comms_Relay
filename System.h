#ifndef System_h
#define System_h

#include "Data.h"

class System {
  public:
    System(DateTime* now):now(now){}
    virtual void initialize() = 0;
    virtual void run() = 0;
  protected:
    DateTime* now;
};

#endif
