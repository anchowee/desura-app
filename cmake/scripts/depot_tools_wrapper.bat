@ECHO OFF
set DEPOT_TOOLS_PATH=%1%
set PATH=%PATH%;%DEPOT_TOOLS_PATH:"=%;%~dp2%
%2 %3 %4 %5 %6 %7 %8 %9