// Scripting.cpp : Defines the exported functions for the DLL.
//

#include "pch.h"
#include "framework.h"
#include "Scripting.h"


// This is an example of an exported variable
SCRIPTING_API int nScripting=0;

// This is an example of an exported function.
SCRIPTING_API int fnScripting(void)
{
    return 0;
}

// This is the constructor of a class that has been exported.
CScripting::CScripting()
{
    return;
}
