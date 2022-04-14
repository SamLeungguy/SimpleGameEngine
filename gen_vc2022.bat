@REM #change current directory to this file

@rem change driver letter
@%~d0

@rem change current directory
@cd %~dp0

cmake -G "Visual Studio 17 2022"  -B build/SimpleGameEngine-x64-windows .

@pause