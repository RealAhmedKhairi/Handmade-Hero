#include <Windows.h>
#include <stdint.h>

#define internal static
#define local static
#define global_variable static 


global_variable bool Running;

global_variable BITMAPINFO BitmapInfo;
global_variable void *BitmapMemory;
global_variable int BitmapWidth;
global_variable int BitmapHeight;

internal void 
RenderPixels(int X_Offset, int Y_Offset) {
   
   int BytesPerPixel = 4;
   int Width = BitmapWidth;
   int Height = BitmapHeight;

   int Pitch = Width * BytesPerPixel;
   uint8_t *Row = (uint8_t *)BitmapMemory;
   for(int Y = 0; Y < BitmapHeight; Y++) 
   {
      uint8_t *Pixel = (uint8_t *)Row;
      for (int X = 0; X < BitmapWidth; X++) 
      {
         *Pixel = (uint8_t) X + X_Offset;
         Pixel++;
         
         *Pixel = (uint8_t) Y + Y_Offset;
         Pixel++;

         *Pixel = 0;
         Pixel++;

         *Pixel = 0;
         Pixel++;
      }
      Row += Pitch;
   }
}

internal void
Win32ResizeDIBSection(int Width, int Height)
{
   // We will try later to check if we don't free first but allocate.
   
   BitmapWidth = Width;
   BitmapHeight = Height;
   
   if(BitmapMemory) {
      VirtualFree(BitmapMemory, 0, MEM_RELEASE);
   }
   
   BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);  
   BitmapInfo.bmiHeader.biWidth = BitmapWidth;
   BitmapInfo.bmiHeader.biHeight = -BitmapHeight;
   BitmapInfo.bmiHeader.biPlanes = 1;
   BitmapInfo.bmiHeader.biBitCount = 32;
   BitmapInfo.bmiHeader.biCompression = BI_RGB;
   

   const int BytesPerPixel = 4;
   int BitmapMemorySize = BytesPerPixel * BitmapWidth * BitmapHeight;

   BitmapMemory = VirtualAlloc(0, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);

   
}
internal void
Win32UpdateWindow(HDC DeviceContext, RECT *WindowRect, int X, int Y, int Width, int Height) 
{
   int WindowWidth = WindowRect->right - WindowRect->left;
   int WindowHeight = WindowRect->bottom - WindowRect->top;

   StretchDIBits(DeviceContext, 
                 0, 0, BitmapWidth, BitmapHeight,
                 0, 0, WindowWidth, WindowHeight,
               //   X, Y, Width, Height, 
               //   X, Y, Width, Height,
                 BitmapMemory,
                 &BitmapInfo,
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
         RECT ClientRect;
         GetClientRect(Window, &ClientRect);
         int Height = ClientRect.bottom - ClientRect.top;
         int Width = ClientRect.right - ClientRect.left;
         Win32ResizeDIBSection(Width, Height);
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
         RECT ClientRect;
         GetClientRect(Window, &ClientRect);

         PAINTSTRUCT Paint;
         HDC DeviceContext = BeginPaint(Window, &Paint);
         int X = Paint.rcPaint.left;
         int Y = Paint.rcPaint.top;
         int Height = Paint.rcPaint.bottom -= Paint.rcPaint.top;
         int Width = Paint.rcPaint.right -= Paint.rcPaint.left;
         Win32UpdateWindow(DeviceContext, &ClientRect, X, Y, Width, Height);
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
   WNDCLASS WindowClass = {};
   WindowClass.style = CS_OWNDC;
   WindowClass.lpfnWndProc = MainWindowCallback;
   WindowClass.hInstance = hInstance;
//  WindowClass.hIcon = ;
   WindowClass.lpszClassName = "HandmadeHeroWindowClass";
   if (RegisterClass(&WindowClass)) 
   {
      HWND WindowHandle = 
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
      if(WindowHandle) 
      {
         int X_Offset = 0, Y_Offset = 0;
         Running = true;
         while(Running)
         {
            while(PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
            {
               if(Message.message == WM_QUIT) 
               {
                  Running = false;
               }
               TranslateMessage(&Message);
               DispatchMessage(&Message);
            }
            HDC DeviceContext = GetDC(WindowHandle);
            RECT ClientRect;
            GetClientRect(WindowHandle, &ClientRect);
            int Width = ClientRect.right - ClientRect.left;
            int Height = ClientRect.bottom - ClientRect.top;
            Win32UpdateWindow(DeviceContext, &ClientRect, 0, 0, Width, Height);
            RenderPixels(X_Offset, Y_Offset);
            X_Offset++;
         }  
      }
   }

   return(0);
}
