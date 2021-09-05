#include <windows.h> 

int CALLBACK 
WinMain(HINSTANCE hInstance, 
        HINSTANCE hPrevInstance,
        PSTR lpCmdLine, 
        INT nCmdShow)
{
    MessageBoxA(0, "Running", "Simple-Platformer", MB_OK|MB_ICONINFORMATION);
    return 0;
}