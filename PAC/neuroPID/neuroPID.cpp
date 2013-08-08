// neuroPID.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "neuroPID.h"

#include "vector"

void DrawSeries( HDC hdc, int x0, int y0, int x1, int y1, int scale,
                std::vector<double> *data_x,  std::vector<double> *data_y )
    {    
    //Считаем очищенной заданную область.
    //RECT rect;
    //SetRect( &rect, x0, y0, x1, y1 );    
    //FillRect (hdc, &rect, (HBRUSH)(COLOR_WINDOW+1));
           
    //Рисуем оси.
    const int D        = 5;
    const int ARROW_SIZE = 15;
    const int CENTER_X = ( x1 - x0 ) / 2;
    const int CENTER_Y = ( y1 - y0 ) / 2;

    //y
    MoveToEx( hdc, CENTER_X, y1 - D, NULL );
    LineTo( hdc, CENTER_X, y0 + D + ARROW_SIZE );
    //Arrow.
    MoveToEx( hdc, CENTER_X, y0 + D, NULL );   
    LineTo( hdc, CENTER_X + ARROW_SIZE / 3, y0 + D + ARROW_SIZE );
    LineTo( hdc, CENTER_X - ARROW_SIZE / 3, y0 + D + ARROW_SIZE );
    LineTo( hdc, CENTER_X, y0 + D );     

    //x
    MoveToEx( hdc, x0 + D, y1 - D, NULL );
    LineTo( hdc, x1 - D - ARROW_SIZE, y1 - D );    
    //Arrow.
    MoveToEx( hdc, x1 - D, y1 - D, NULL );   
    LineTo( hdc, x1 - ARROW_SIZE - D, y1 - D - ARROW_SIZE / 3 );
    LineTo( hdc, x1 - ARROW_SIZE - D, y1 - D + ARROW_SIZE / 3 );
    LineTo( hdc, x1 - D, y1 - D );

    //Строим график.
    LOGBRUSH lb;    
    lb.lbStyle = BS_SOLID; 
    lb.lbColor = RGB(0, 0, 0); 
    lb.lbHatch = 0; 

    HGDIOBJ hPen = ExtCreatePen(PS_GEOMETRIC | PS_SOLID, 2, &lb, 0, NULL);    
    HGDIOBJ hPenOld = SelectObject(hdc, hPen); 

    //MoveToEx( hdc, CENTER_X, CENTER_Y, NULL );

    //Строим график.
    MoveToEx( hdc, CENTER_X + ( int ) data_x[ 0 ][ 0 ], 
        CENTER_Y - ( int ) data_y[ 0 ][ 0 ], NULL );

    for ( unsigned int i = 0; i < data_x->size(), i < data_y->size(); i++ )
        {
         LineTo( hdc, CENTER_X + ( int ) data_x[ 0 ][ i ], 
             CENTER_Y - ( int ) data_y[ 0 ][ i ] );
        }
    }


std::vector<double> *PID_data = new std::vector<double>();
std::vector<double> *time_data = new std::vector<double>();

void init_PID_data()
    {
    for ( int i = 0; i < 1000; i++ )
        {
    //    PID_data->push_back( 100 * sin( i * 0.1 ) );
        time_data->push_back( i );
        }

    PID_data->push_back( 100 );
    PID_data->push_back( 100 );   
    }

void roll_PID_data()
    {
    static int dt = 0;
    dt += 1;
    
    double tmp_val1 = PID_data[ 0 ][ 1 ];
    //double tmp_val2 = time_data[ 0 ][ 0 ];

    PID_data->clear();
    //time_data->clear();

    PID_data->push_back( tmp_val1 );
    //time_data->push_back( tmp_val2 );
    
    for ( int i = 1; i < 1000; i++ )
        {
        //PID_data->push_back( 100 * sin( ( dt + i ) * 0.1 ) );
      
        double new_val = 0.99 * PID_data[ 0 ][ i - 1 ];
           //+ 0.001 * PID_data[ 0 ][ i - 1 ] * PID_data[ 0 ][ i - 1 ] + sin( 10 );

        PID_data->push_back( new_val );
        //time_data->push_back( dt + i );
        }
    }

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                       _In_opt_ HINSTANCE hPrevInstance,
                       _In_ LPTSTR    lpCmdLine,
                       _In_ int       nCmdShow)
    {
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.
    MSG msg;
    HACCEL hAccelTable;

    // Initialize global strings
    LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadString(hInstance, IDC_NEUROPID, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
        {
        return FALSE;
        }

    hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_NEUROPID));

    // Main message loop:
    while (GetMessage(&msg, NULL, 0, 0))
        {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
            {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            }
        }

    return (int) msg.wParam;
    }



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
    {
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style			= CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc	= WndProc;
    wcex.cbClsExtra		= 0;
    wcex.cbWndExtra		= 0;
    wcex.hInstance		= hInstance;
    wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_NEUROPID));
    wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_NEUROPID);
    wcex.lpszClassName	= szWindowClass;
    wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassEx(&wcex);
    }

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
    {
    HWND hWnd;

    hInst = hInstance; // Store instance handle in our global variable

    hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

    if (!hWnd)
        {
        return FALSE;
        }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
    }

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
    int wmId, wmEvent;
    PAINTSTRUCT ps;
    HDC hdc;
    int idTimer;

    switch (message)
        {
        case WM_CREATE:
            SetTimer( hWnd, idTimer = 1, 200, NULL); 

            init_PID_data();
                 
            break;

        case WM_COMMAND:
            wmId    = LOWORD(wParam);
            wmEvent = HIWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
                {
                case IDM_ABOUT:
                    DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                    break;
                case IDM_EXIT:
                    DestroyWindow(hWnd);
                    break;
                default:
                    return DefWindowProc(hWnd, message, wParam, lParam);
                }
            break;

        case WM_ERASEBKGND:
            return 1;
            break;

        case WM_TIMER:
            {
            //Очищаем окно.
            hdc = GetDC( hWnd );


            RECT rect;
            GetClientRect (hWnd, &rect);
            FillRect (hdc, &rect, (HBRUSH)(COLOR_WINDOW+1));

            //Рисуем график ПИД.
            DrawSeries(hdc, rect.left, rect.top, rect.right, rect.bottom / 2,
              10, time_data, PID_data );

            ReleaseDC( hWnd, hdc );

            roll_PID_data();

            break;
            }

        case WM_PAINT:
            {

            //static int x0 = 0;
            //x0 += 10;

            hdc = BeginPaint(hWnd, &ps);

            //MoveToEx( hdc, 0, 0, NULL );
            //LineTo( hdc, x0, x0 );
            EndPaint(hWnd, &ps);
            break;
            }

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    return 0;
    }

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
    {
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
        {
        case WM_INITDIALOG:
            return (INT_PTR)TRUE;

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
                {
                EndDialog(hDlg, LOWORD(wParam));
                return (INT_PTR)TRUE;
                }
            break;
        }
    return (INT_PTR)FALSE;
    }


