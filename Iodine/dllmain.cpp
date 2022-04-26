// dllmain.cpp : Defines the entry point for the DLL application.
#include "Dumper.h"

void InitCore() {
    //Open Console
    AllocConsole();
    FILE* pFile;
    freopen_s(&pFile, ("CONOUT$"), "w", stdout);
    //Init Stuff
    GVersion = 14.60f;
    FreeAddr = Util::FindPattern(Free_G);
    if (FreeAddr == 0) {
        FreeAddr = Util::FindPattern(Free_420);
        if (FreeAddr == 0) {
            FreeAddr = Util::FindPattern(Free_OLD);
            if (FreeAddr == 0) {
                MessageBoxA(0, "Free Address is INVALID!\nMake sure your version is supported!", "Iodine", MB_OK);
                Sleep(5000);
                ExitProcess(69);
            }
        }
    }
    FNameToStringAddr = Util::FindPattern(FNTS_C2);
    if (FNameToStringAddr == 0) {
        GVersion = 10.40f;
        FNameToStringAddr = Util::FindPattern(FNTS_C1);
        if (FNameToStringAddr == 0) {
            GVersion = 4.1f;
            FNameToStringAddr = Util::FindPattern(FNTS_OLD);
            if (FNameToStringAddr == 0) {
                MessageBoxA(0, "FName::ToString Address is INVALID!\nMake sure your version is supported!", "Iodine", MB_OK);
                Sleep(5000);
                ExitProcess(69);
            }
        }
    }
    ObjsAddr = Util::FindPattern(GObjects_G, true, 10);
    if (ObjsAddr == 0) {
        ObjsAddr = Util::FindPattern(ObjObjects_420, true, 3);
        if (ObjsAddr == 0) {
            ObjsAddr = Util::FindPattern(ObjObjects_416, true, 3);
            if (ObjsAddr == 0) {
                MessageBoxA(0, "GObjects\ObjObjects Address is INVALID!\nMake sure your version is supported!", "Iodine", MB_OK);
                Sleep(5000);
                ExitProcess(69);
            }
        }
    }
    ProcessEventAddr = Util::FindPattern(ProcessEvent_G);
    if (ProcessEventAddr == 0) {
        ProcessEventAddr = Util::FindPattern(ProcessEvent_OLD);
        if (ObjsAddr == 0) {
            MessageBoxA(0, "ProcessEvent Address is INVALID!\nProcessEvent will be excluded in your SDK!", "Iodine", MB_OK);
            IncludePE = false;
        }
    }
    Dumper::InitDumper();
    MessageBoxA(0, "Setup Complete!\nPress OK to Dump!", "Iodine", MB_OK);
    Dumper::Dump();
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        InitCore();
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

