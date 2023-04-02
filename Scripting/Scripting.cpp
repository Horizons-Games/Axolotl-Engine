// Scripting.cpp : Defines the exported functions for the DLL.
//

#include "pch.h"
#include "framework.h"
#include "Scripting.h"


// This is an example of an exported variable
SCRIPTING_API int nScripting;

// This is an example of an exported function.
SCRIPTING_API int fnScripting()
{
    //press P to test it in engine
    nScripting += 10;
    return nScripting;
}

// This is the constructor of a class that has been exported.
CScripting::CScripting()
{
    return;
}
