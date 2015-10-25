#ifndef TASKSCHEDULER_H
#define TASKSCHEDULER_H

#include <Helper/Vector.h>

class Task;

class TaskScheduler
{
public:
    void addTask(Task* task);

    void removeTask(Task* task);

    void loop();

    void iterate();

private:
    Vector<Task*> tasks;
    static int SLEEP_MS;

public:
    static TaskScheduler* instance;
};

#endif // TASKSCHEDULER_H
