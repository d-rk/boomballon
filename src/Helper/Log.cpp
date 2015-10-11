#include "Log.h"
#include <Arduino.h>

//-----------------------------------------------------------------------------

/**
 * @brief serial_putchar function that printf and related will use to print.
 * @param c char to write.
 * @param f file to write to.
 * @return returns if write was succesful.
 */
int serial_putchar(char c, FILE* f) {
    if (c == '\n') serial_putchar('\r', f);
    return Serial.write(c) == 1? 0 : 1;
}

//-----------------------------------------------------------------------------

FILE Log::serial_stdout;

//-----------------------------------------------------------------------------

void Log::setup() {
    Serial.begin(9600);

    // Set up stdout so that printf can be used.
    fdev_setup_stream(&serial_stdout, serial_putchar, NULL, _FDEV_SETUP_WRITE);
    stdout = &serial_stdout;
}

//-----------------------------------------------------------------------------


