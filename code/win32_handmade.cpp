#include <Windows.h>
#include <stdint.h>

#define internal static
#define local static
#define global_variable static 


typedef struct win32_buffer {
   BITMAPINFO Info;
   void *Memory;
   int Width;
   int Height;
   int Pitch;
   int BytesPerPixel;
} Win32_buffer;

typedef struct win32_window_dimension {
   int Width;
   int Height;
} Win32_window_dimension;


global_variable bool Running;
global_variable Win32_buffer BackBuffer;

internal
Win32_window_dimension Win32GetWindowDimension(HWND Window) {

   Win32_window_dimension Result;

   RECT ClientRect;
   GetClientRect(Window, &ClientRect);
   Result.Height = ClientRect.bottom - ClientRect.top;
   Result.Width = ClientRect.right - ClientRect.left;
   
   return (Result);
} 

internal void 
RenderPixels(Win32_buffer Buffer, int X_Offset, int Y_Offset) {
   
   int Width = Buffer.Width;
   int Height = Buffer.Height;

   uint8_t *Row = (uint8_t *)Buffer.Memory;
   for(int Y = 0; Y < Height; Y++) 
   {
      uint32_t *Pixel = (uint32_t *)Row;
      // Note that here the first pixel is blue, becuase of the little Endiane CPU architecture
      // 0 x xxRRGGBB -> Start the the least significant bit. 
      
      for (int X = 0; X < Width; X++) 
      {
         if (((X / 32) + (Y / 32)) % 2 == 0) 
         {
            *Pixel = 0xffffff;
            Pixel++;   
         } else {
            *Pixel = 0x000000 ;
            Pixel++;
         }
         
      }
      Row += Buffer.Pitch;
   }
}

internal void
Win32ResizeDIBSection(Win32_buffer *Buffer, int Width, int Height)
{
   // We will try later to check if we don't free first but allocate.
   
   Buffer->Width = Width;
   Buffer->Height = Height;
   Buffer->BytesPerPixel = 4;

   if(Buffer->Memory) {
      VirtualFree(Buffer->Memory, 0, MEM_RELEASE);
   } 
   
   Buffer->Info.bmiHeader.biSize = sizeof(Buffer->Info.bmiHeader);  
   Buffer->Info.bmiHeader.biWidth = Buffer->Width;
   // -ve height forces the bitmap to draw top to bottom not the other way around.
   // Refer to documentation for further info.
   Buffer->Info.bmiHeader.biHeight = -Buffer->Height;
   Buffer->Info.bmiHeader.biPlanes = 1;
   Buffer->Info.bmiHeader.biBitCount = 32;
   Buffer->Info.bmiHeader.biCompression = BI_RGB;
   
   
   int BitmapMemorySize = (Buffer->BytesPerPixel) * (Buffer->Width) * (Buffer->Height);
   
   Buffer->Memory = VirtualAlloc(0, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);
   
   Buffer->Pitch = Width * Buffer->BytesPerPixel;
   
}
internal void
Win32CopyBufferToWindow(HDC DeviceContext, int WindowWidth, int WindowHeight, 
                        Win32_buffer Buffer) 
{
   // TODO: Update aspect ratio to stay the same after resizing
   StretchDIBits(DeviceContext, 
               0, 0, WindowWidth, WindowHeight,
               0, 0, Buffer.Width, Buffer.Height,
               //   X, Y, Width, Height, 
               //   X, Y, Width, Height,
                 Buffer.Memory,
                 &Buffer.Info,
                 DIB_RGB_COLORS, SRCCOPY);
}

LRESULT CALLBACK 
MainWindowCallback(HWND Window,
                   UINT Message,
                   WPARAM wParam,
                   LPARAM lParam)
{
   LRESULT Result = 0;

   switch(Message)
   {
      case WM_SIZE:
      {      
      } break;

      case WM_DESTROY:
      {
         // Has to be changed for an error later.
         Running = false;
      } break;

      case WM_CLOSE: 
      {
         // Has to be changed for a message later.
         Running = false;
      } break;

      case WM_ACTIVATEAPP: 
      {
         OutputDebugStringA("WM_ACTIVATEAPP\n");
      } break;
      case WM_PAINT:
      {
         Win32_window_dimension Dimension;
         Dimension = Win32GetWindowDimension(Window);         

         PAINTSTRUCT Paint;
         HDC DeviceContext = BeginPaint(Window, &Paint);
         int X = Paint.rcPaint.left;
         int Y = Paint.rcPaint.top;

         Win32CopyBufferToWindow(DeviceContext, Dimension.Width, Dimension.Height, BackBuffer);
         EndPaint(Window, &Paint);
      } break;
      default:
      {
         Result = DefWindowProc(Window, Message, wParam, lParam);
      } break;
   }
   return(Result);
}
int CALLBACK 
WinMain(HINSTANCE hInstance,
        HINSTANCE hPrevInstance,
        LPSTR lpCmdLine,
        int nCmdShow)
{
   // WNDCLASS is a struct that defines a certain blueprint for your program.
   // We used it to reference MainWindowCallback which is essential for the message we pop from the windows message q.
   WNDCLASS WindowClass = {};
   // since we are going to strech the image to fit, we will need to redraw the entire thing instead of
   // just redrawing the new part. 
   Win32ResizeDIBSection(&BackBuffer, 1280, 720);
   WindowClass.style = CS_HREDRAW|CS_VREDRAW;
   WindowClass.lpfnWndProc = MainWindowCallback;
   // this is the pointer that contains that address of the process (the function)
   WindowClass.hInstance = hInstance;
//  WindowClass.hIcon = ;
   WindowClass.lpszClassName = "HandmadeHeroWindowClass";
   if (RegisterClass(&WindowClass)) 
   {
      HWND Window = 
         CreateWindowEx(
         0,
         WindowClass.lpszClassName,
         "Handmade Hero",
         WS_OVERLAPPEDWINDOW|WS_VISIBLE,
         CW_USEDEFAULT,
         CW_USEDEFAULT,
         CW_USEDEFAULT,
         CW_USEDEFAULT,
         0,
         0,
         hInstance,
         0);
      
         
      MSG Message;
      if(Window) 
      {
         int X_Offset = 0;
         int Y_Offset = 0;
         Running = true;
         while(Running)
         {
            // PeekMessage is a non-blocking function instead of GetMessage()
            while(PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
            {
               if(Message.message == WM_QUIT) 
               {
                  Running = false;
               }
               TranslateMessage(&Message);
               DispatchMessage(&Message);
            }
            HDC DeviceContext = GetDC(Window);
            Win32_window_dimension Dimension;
            Dimension = Win32GetWindowDimension(Window);
                
            Win32CopyBufferToWindow(DeviceContext, Dimension.Width, Dimension.Height, BackBuffer);
            RenderPixels(BackBuffer, X_Offset, Y_Offset);
            X_Offset++;
         }  
      }
   }

   return(0);
}
