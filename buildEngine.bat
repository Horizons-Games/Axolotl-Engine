@echo OFF 
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x86
echo "Starting Build"
echo .  
devenv "Dependencies\Axolotl-Engine\Source\Engine.sln" /build ReleaseEngine
echo . 
echo "Build completed." 
pause