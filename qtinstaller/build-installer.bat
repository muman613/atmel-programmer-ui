@echo off
REM Build the Installer

REM Build the help system
ECHO Building the help system...
PUSHD ..\help
call build
POPD

REM Copy files from deploy directory
ECHO Removing extaneous files from installer directory...
DEL ..\installer\*.log

REM Copy assisstant to install directory
ECHO Copying QtAssistant into installer directory...
COPY C:\Qt\5.12.4\mingw73_64\bin\assistant.exe ..\installer

REM Copy all files into the package data directory
ECHO Copying all files into package...
XCOPY ..\installer\* packages\com.wunderbar.multiprogrammer\data /E /Y
REM Copy help file
COPY ..\help\programmer.qhc packages\com.wunderbar.multiprogrammer\data /Y


ECHO Copying all Qt related libraries and support into package...
PUSHD ..\installer
windeployqt AtmelProgrammer.exe
windeployqt assistant.exe
POPD

REM Start installer creation
C:\Qt\Tools\QtInstallerFramework\3.2\bin\binarycreator.exe -v -c config\config.xml -p packages AtmelProgrammer-Installer.exe