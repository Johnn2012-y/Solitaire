@echo off
echo Compiling Solitaire...
g++ -o solitaire main.cpp src/*.cpp -I include
if %errorlevel% neq 0 (
    echo Compilation failed!
    pause
    exit /b %errorlevel%
)
echo Compilation successful!
echo Running Solitaire...
solitaire.exe
pause 