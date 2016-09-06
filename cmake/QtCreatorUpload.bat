@echo off
@REM Simple script needed for QT Creator to upload to arduino without warning.
@REM avrdude, which uploads to arduino, writes output to stderr even though everything went fine
@REM during upload. QtCreator missinterprets this as an error and shows a warning.
@REM ---
@REM This script runs the upload, and redirects the output to stderr/stdout depending on the return code.

@REM close putty so that serial port is open (and generate no output from taskkill)
taskkill /F /IM putty.exe /f >nul 2>&1

@REM force reset of arduino by opening serial connection with baud 1200 and directly close it again.
start putty.exe -serial com8 -sercfg 1200
taskkill /F /IM putty.exe >nul

@REM wait a second so that the virtual com port can open
echo Wait for com port switch...
ping 127.0.0.1 -n 2 > NUL

@REM Run command and pipe all output to tmp file
mingw32-make upload > tmp 2>&1
set errCode=%errorlevel%

if %errCode% == 0 (
	@REM UPLOAD SUCCESSFUL
	@REM Print tmp file in stdout and delete it
	type tmp
	IF EXIST tmp del /F tmp
) else (
	@REM ERROR DURING UPLOAD
	@REM Print tmp file in stderr and delete it
	type tmp 1>&2
	IF EXIST tmp del /F tmp
)

@REM wait a second so that the com port can go back to initial state.
echo Wait for com port switch...
ping 127.0.0.1 -n 2 > NUL
	
exit /b %errCode%






