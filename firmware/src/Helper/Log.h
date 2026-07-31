#ifndef LOG_H
#define LOG_H

#include <stdio.h>

/**
 * @brief The Log class.
 * For now just enables usage of printf().
 *
 */
class Log
{
public:
    static void setup();

private:
    Log() {}
    static FILE serial_stdout;
};

#endif // LOG_H
