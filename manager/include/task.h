#include "Arduino.h"

#ifndef INCLUDE_TASK_H
#define INCLUDE_TASK_H

class Task
{
    public:
        Task(unsigned long period);
        void setHandler(String name, void (*handler)(void));
        bool activate(void);
        String getEventName(void);

    private:
        String eventName;
        unsigned long timestamp;
        unsigned long period;
        void (*handler)(void);
};

inline Task::Task(unsigned long period)
{
    this->eventName = "";
    this->handler = NULL;
    this->period = period;
    this->timestamp = millis();
}

inline void Task::setHandler(String eventName, void (*handler)(void))
{
    this->eventName = eventName;
    this->handler = handler;
}

inline bool Task::activate(void)
{
    if(handler != NULL)
    {
        if((millis() - this->timestamp) > this->period)
        {
            handler();
            this->timestamp = millis();
            return true;
        }
    }

    return false;
}

inline String Task::getEventName(void)
{
    return this->eventName;
}

#endif