#ifndef TASK
#define TASK

class Task {
public:
    virtual ~Task() {}

    virtual bool applyChange(unsigned long timeMs) = 0;
};

#endif // TASK

