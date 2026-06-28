#include <Windows.h>

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
         OutputDebugStringA("WM_SIZE\n");
      } break;

      case WM_DESTROY:
      {
         OutputDebugStringA("WM_DESTROY\n");
      } break;

      case WM_CLOSE: 
      {
         OutputDebugStringA("WM_CLOSE\n");
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
         PatBlt(DeviceContext, X , Y, Height, Width , WHITENESS);
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
         for(;;)
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
