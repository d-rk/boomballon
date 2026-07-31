#include "TaskScheduler.h"
#include <Arduino.h>
#include <Tasks/Task.h>

//-----------------------------------------------------------------------------

TaskScheduler* TaskScheduler::instance = NULL;

//-----------------------------------------------------------------------------

int TaskScheduler::SLEEP_MS = 20;

//-----------------------------------------------------------------------------

TaskScheduler::TaskScheduler() : tasks(Vector<Task*>(5))
{
}

//-----------------------------------------------------------------------------

void TaskScheduler::addTask(Task* task) {
    tasks.push_back(task);
}

//-----------------------------------------------------------------------------

void TaskScheduler::removeTask(Task* task) {
    Vector<Task*> tasksOld = tasks;
    tasks.clear();

    for (Vector<Task*>::iterator it = tasksOld.begin(); it != tasksOld.end(); it++) {
        if ((*it) != task) {
            tasks.push_back(*it);
        }
    }
}

//-----------------------------------------------------------------------------

void TaskScheduler::loop() {
    bool activeTasks = !tasks.empty();

    while(activeTasks) {
        activeTasks = false;

        unsigned long timeStart = millis();

        for (Vector<Task*>::iterator it = tasks.begin(); it != tasks.end(); it++) {
            activeTasks |= (*it)->applyChange(millis());
        }

        unsigned long timeSleep = SLEEP_MS - (millis() - timeStart);

        if (timeSleep <= SLEEP_MS) {
            delay(timeSleep);
        }
    }

    tasks.clear();
}

//-----------------------------------------------------------------------------

void TaskScheduler::iterate() {

    bool activeTasks = !tasks.empty();

    for (Vector<Task*>::iterator it = tasks.begin(); it != tasks.end(); it++) {
        activeTasks |= (*it)->applyChange(millis());
    }

    if (!activeTasks) {
        tasks.clear();
    }
}

//-----------------------------------------------------------------------------

