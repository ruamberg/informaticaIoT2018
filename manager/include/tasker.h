#ifndef INCLUDE_TASKER_H
#define INCLUDE_TASKER_H

#include "Arduino.h"
#include "./taskmanager.h"

#define TOTAL_TASKS 10

namespace Tasker
{
    TaskManager manager(TOTAL_TASKS);

    inline void setActivationHandler(void (*handler)(String))
    {
        manager.setActivationHandler(handler);
    }

    inline void add(String event, void (*handler)(void), unsigned long time)
    {
        manager.add(event, handler, time);
    }

    inline void doTheLoop(void)
    {
        manager.doTheLoop();
    }
}


#endif