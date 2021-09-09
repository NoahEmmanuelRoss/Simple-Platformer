#include <windows.h> 
#include <stdint.h>

#define internal static 
#define local_persist static  
#define global_variable static

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;


struct win32_offscreen_buffer 
{
    BITMAPINFO Info;
    void *Memory;
    int Width;
    int Height;
    int Pitch;
    int BytesPerPixel;
};

global_variable bool Running;
global_variable win32_offscreen_buffer GlobalBackbuffer;

struct win32_window_dimension
{
    int Width;
    int Height;
};

win32_window_dimension 
Win32GetWindowDimension(HWND Window)
{
    win32_window_dimension Result;
    RECT ClientRect;
    GetClientRect(Window, &ClientRect);
    Result.Height = ClientRect.bottom - ClientRect.top;
    Result.Width = ClientRect.right - ClientRect.left;
    
    return(Result);
}


internal void 
RenderWeirdGradient(win32_offscreen_buffer Buffer, int XOffset, int YOffset)
{
    int Width = Buffer.Width;
    int Height = Buffer.Height; 
    
    
    uint8 *Row = (uint8 *)Buffer.Memory;
    for(int Y = 0;
        Y < Buffer.Height;
        ++Y)
    {
        uint32 *Pixel = (uint32 *)Row;
        for(int X = 0;
            X < Buffer.Width;
            ++X)
        {
            uint8 Blue = (X + XOffset);
            uint8 Green = (Y + YOffset);
            
            *Pixel++ = ((Green << 8) | Blue);
            
            
        }
        
        Row += Buffer.Pitch;
    }
}

internal void 
Win32ResizeDIBSection(win32_offscreen_buffer *Buffer, int Width, int Height)
{
    if(Buffer->Memory)
    {
        VirtualFree(Buffer->Memory, 0, MEM_RELEASE);
    }
    
    Buffer->Width = Width;
    Buffer->Height = Height;
    Buffer->BytesPerPixel = 4;
    
    
    Buffer->Info.bmiHeader.biSize = sizeof(Buffer->Info.bmiHeader); 
    Buffer->Info.bmiHeader.biWidth = Buffer->Width;
    Buffer->Info.bmiHeader.biHeight = -Buffer->Height;
    Buffer->Info.bmiHeader.biPlanes = 1;
    Buffer->Info.bmiHeader.biBitCount = 32;
    Buffer->Info.bmiHeader.biCompression = BI_RGB;
    
    
    
    int BitmapMemorySize = (Buffer->Width * Buffer->Height) * Buffer->BytesPerPixel;
    Buffer->Memory = VirtualAlloc(0, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);
    
    Buffer->Pitch = Width*Buffer->BytesPerPixel;
}

internal void 
Win32DisplayBuffer(HDC DeviceContext, int WindowWidth, int WindowHeight,
                   win32_offscreen_buffer Buffer,
                   int X, int Y, int Width, int Height)
{
    
    StretchDIBits(DeviceContext,
                  //X, Y, Width, Height,
                  //X, Y, Width, Height,
                  
                  0, 0, WindowWidth, WindowHeight,
                  0, 0, Buffer.Width, Buffer.Height,
                  Buffer.Memory, &Buffer.Info,
                  DIB_RGB_COLORS, SRCCOPY);
}

LRESULT CALLBACK 
Win32MainWindowCallback(HWND   Window,
                        UINT   Message,
                        WPARAM WParam,
                        LPARAM LParam)
{
    LRESULT Result = 0;
    
    switch(Message)
    {
        
        case WM_SIZE:
        {
            
            
        } break;
        
        case WM_CLOSE:
        {
            //TODO: Handle with message
            Running = false;
            
        } break;
        
        case WM_DESTROY:
        {
            //TODO: Handle with error 
            Running = false;
        } break;
        
        case WM_ACTIVATEAPP:
        {
            OutputDebugStringA("WM_ACTIVATEAPP\n");
        } break;
        
        case WM_PAINT:
        {
            PAINTSTRUCT Paint;
            HDC DeviceContext = BeginPaint(Window, &Paint);
            int X = Paint.rcPaint.left;
            int Y = Paint.rcPaint.top;
            int Height = Paint.rcPaint.bottom - Paint.rcPaint.top;
            int Width = Paint.rcPaint.right - Paint.rcPaint.left;
            
            win32_window_dimension Dimension = Win32GetWindowDimension(Window);
            
            Win32DisplayBuffer(DeviceContext, Dimension.Width, Dimension.Height,
                               GlobalBackbuffer, X, Y, Width, Height);
            EndPaint(Window, &Paint);
        } break;
        
        default:
        {
            //OutputDebugStringA("DEFAULT\n");
            Result = DefWindowProc(Window, Message, WParam, LParam);
        } break;
    }
    
    return Result;
}

int CALLBACK 
WinMain(HINSTANCE Instance, 
        HINSTANCE PrevInstance,
        PSTR CommandLine, 
        INT ShowCode)
{
    WNDCLASS WindowClass = {}; //set the window class to 0
    
    Win32ResizeDIBSection(&GlobalBackbuffer, 1280, 720);
    
    WindowClass.style = CS_HREDRAW|CS_VREDRAW; 
    WindowClass.lpfnWndProc = Win32MainWindowCallback; //specify procedure that handles windows messages
    WindowClass.hInstance = Instance;
    WindowClass.lpszClassName = "PlatformerWindowClass";
    
    if(RegisterClass(&WindowClass))
    {
        HWND Window = CreateWindowEx(0,
                                     WindowClass.lpszClassName,
                                     "Simple Platformer",
                                     WS_OVERLAPPEDWINDOW|WS_VISIBLE,
                                     CW_USEDEFAULT,
                                     CW_USEDEFAULT,
                                     CW_USEDEFAULT,
                                     CW_USEDEFAULT,
                                     0,
                                     0,
                                     Instance,
                                     0);
        if(Window)
        {
            int XOffset = 0;
            int YOffset = 0;
            Running = true;
            while(Running)
            {
                
                MSG Message;
                while(PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
                {
                    if (Message.message == WM_QUIT)
                    {
                        Running = false;
                    }
                    TranslateMessage(&Message);
                    DispatchMessage(&Message);
                    
                    
                    
                    
                }
                
                RenderWeirdGradient(GlobalBackbuffer, XOffset, YOffset);
                
                HDC DeviceContext = GetDC(Window);
                
                win32_window_dimension Dimension = Win32GetWindowDimension(Window);
                Win32DisplayBuffer(DeviceContext, Dimension.Width, Dimension.Height, GlobalBackbuffer, 
                                   0, 0, Dimension.Width, Dimension.Height);
                ReleaseDC(Window, DeviceContext);
                
                YOffset += 5;
            }
            
            
        }
        else
        {
            //TODO: loggin
        }
    } 
    else
    {
    }
    
    return 0;
}