// dllmain.cpp : Defines the entry point for the DLL application.
#include <windows.h>

BOOL APIENTRY DllMain(HMODULE a_hModule, DWORD a_ulReasonForCall, LPVOID a_lpReserved)
{
    switch (a_ulReasonForCall)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
