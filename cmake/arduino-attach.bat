@echo off

echo SERIAL_PORT: %SERIAL_PORT%
echo BOARD: %BOARD%

putty.exe -serial %SERIAL_PORT%