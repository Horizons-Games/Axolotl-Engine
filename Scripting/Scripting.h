// The following ifdef block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the SCRIPTING_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// SCRIPTING_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef SCRIPTING_EXPORTS
#define SCRIPTING_API __declspec(dllexport)
#else
#define SCRIPTING_API __declspec(dllimport)
#endif

// This class is exported from the dll
class SCRIPTING_API CScripting {
public:
	CScripting(void);
	// TODO: add your methods here.
};

extern SCRIPTING_API int nScripting;

SCRIPTING_API int fnScripting(void);
