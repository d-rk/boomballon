@echo off
@REM Simple script needed for QT Creator to upload to arduino without warning.
@REM avrdude, which uploads to arduino, writes output to stderr even though everything went fine
@REM during upload. QtCreator missinterprets this as an error and shows a warning.
@REM ---
@REM This script runs the upload, and redirects the output to stderr/stdout depending on the return code.

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

exit /b %errCode%






