@echo off
py "%~dp0server.py" --open
if errorlevel 1 pause
