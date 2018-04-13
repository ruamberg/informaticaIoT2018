#include "Arduino.h"
#include "./task.h"

#ifndef INCLUDE_TASKMANAGER_H
#define INCLUDE_TASKMANAGER_H

class TaskManager
{
  public:
    TaskManager(int size);
    void add(String name, void (*handler)(void), unsigned long period);
    void setActivationHandler(void (*activationHandler)(String name));
    void doTheLoop(void);

  private:
    int size;
    int pointer;
    Task **tasks;
    void (*activationHandler)(String name);
};

inline TaskManager::TaskManager(int size)
{
    this->size = size;
    this->pointer = 0;
    this->activationHandler = NULL;
    this->tasks = (Task **) malloc(this->size * sizeof(Task*));
}

inline void TaskManager::add(String name, void (*handler)(void), unsigned long period)
{
    if(this->pointer >= this->size)
        return;

    Task *t = new Task(period);
    t->setHandler(name, handler);

    this->tasks[this->pointer++] = t;
}

inline void TaskManager::setActivationHandler(void (*activationHandler)(String name))
{
    this->activationHandler = activationHandler;
}

inline void TaskManager::doTheLoop(void)
{
    for(int i = 0; i < this->pointer; i++)
    {
        Task *t = this->tasks[i];
        if(t->activate())
        {
            if (this->activationHandler != NULL)
            {
                this->activationHandler(t->getEventName());
            }
        }
    }
}

#endif