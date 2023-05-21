@echo OFF 
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x86
echo "Starting Build"
echo .  
devenv "..\Source\Engine.sln" /build ReleaseGame
echo . 
echo "Build completed." 