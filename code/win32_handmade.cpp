#include <Windows.h>

#define internal_function static
#define local_persist static
#define global_variable static 

global_variable bool Running;

global_variable BITMAPINFO BitmapInfo;
global_variable void *BitMapMemory;
global_variable HDC BitmapDeviceContext;
global_variable HBITMAP BitmapHandle;

internal_function void
Win32ResizeDIBSection(int Width, int Height)
{
   // We will try later to check if we don't free first but allocate.

   if(BitmapHandle) 
   {
      DeleteObject(BitmapHandle);
   } 
   if(BitmapDeviceContext) 
   {
      HDC BitmapDeviceContext = CreateCompatibleDC(0);
   }

   BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);  
   BitmapInfo.bmiHeader.biWidth = Width;
   BitmapInfo.bmiHeader.biHeight = Height;
   BitmapInfo.bmiHeader.biPlanes = 1;
   BitmapInfo.bmiHeader.biBitCount = 32;
   BitmapInfo.bmiHeader.biCompression = BI_RGB;


   BitmapHandle = CreateDIBSection(
      BitmapDeviceContext,
      BitmapInfo,
      DIB_RGB_COLORS,
      &BitMapMemory,
      0, 0);

}
internal_function void
Win32UpdateWindow(HDC DeviceContext, int X, int Y, int Width, int Height) 
{
   int StretchDIBits(DeviceContext, X, Y, Width, Height, 
                     X, Y, Width, Height
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
         BOOL GetClientRect(Window, &ClientRect);
         int Height = ClientRect.bottom -= ClientRect.top;
         int Width = ClientRect.right -= ClientRect.left;
         Win32ResizeDIBSection(Width, Height);
         OutputDebugStringA("WM_SIZE\n");
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
         PAINTSTRUCT Paint;
         HDC DeviceContext = BeginPaint(Window, &Paint);
         int X = Paint.rcPaint.left;
         int Y = Paint.rcPaint.top;
         int Height = Paint.rcPaint.bottom -= Paint.rcPaint.top;
         int Width = Paint.rcPaint.right -= Paint.rcPaint.left;
         Win32UpdateWindow(DeviceContext, X, Y, Width, Height);
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
         Running = true;
         while(Running)
         {
            BOOL MessageResult = GetMessage(&Message, 0, 0, 0);
            if (MessageResult > 0) 
            {
               TranslateMessage(&Message);
               DispatchMessage(&Message);
            } else 
            {
               break;
            }
         }  
      }
   }

   return(0);
}
