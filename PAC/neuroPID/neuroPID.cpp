// neuroPID.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "neuroPID.h"

#include "vector"

#include "PID.h"
#include "plant.h"

plant   p_object;
PID     pid1;

std::vector<double> *plant_data = new std::vector<double>();
std::vector<double> *PID_data = new std::vector<double>();

//Данные работы храним в файлах.
FILE *work_data_stream;    

//----------------------------------------------------------------------------
void DrawSeries( HDC hdc, int x0, int y0, int x1, int y1, int scale,
                std::vector<double> *data_y, std::vector<double> *data_y2 )
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

    const double SCALE_Y = 2;
    const double SCALE_X = 10;

    //y
    MoveToEx( hdc, CENTER_X, y1 - D, NULL );
    LineTo( hdc, CENTER_X, y0 + D + ARROW_SIZE );
    //Arrow.
    MoveToEx( hdc, CENTER_X, y0 + D, NULL );   
    LineTo( hdc, CENTER_X + ARROW_SIZE / 3, y0 + D + ARROW_SIZE );
    LineTo( hdc, CENTER_X - ARROW_SIZE / 3, y0 + D + ARROW_SIZE );
    LineTo( hdc, CENTER_X, y0 + D );     

    //Lines y
    int y_ = y1 - D;    
    while ( y_ > D + ARROW_SIZE )
        {
        MoveToEx( hdc, CENTER_X, y_, NULL );
        LineTo( hdc, CENTER_X + 5, y_ );
        y_ -= 10 * SCALE_Y;
        }

    //Lines x
    int x_ = CENTER_X;    
    while ( x_ < x1 - D - ARROW_SIZE )
        {
        MoveToEx( hdc, x_, y1 - D, NULL );
        LineTo( hdc, x_, y1 - D - 5 );
        x_ += SCALE_X;
        }
    x_ = CENTER_X;  
    while ( x_ > D )
        {
        MoveToEx( hdc, x_, y1 - D, NULL );
        LineTo( hdc, x_, y1 - D - 5 );
        x_ -= SCALE_X;
        }


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
    SelectObject(hdc, hPen); 

    //MoveToEx( hdc, CENTER_X, CENTER_Y, NULL );

    
    //Строим линию 1 влево.
    MoveToEx( hdc, CENTER_X, 
        y1 - D - ( int ) ( SCALE_Y * data_y[ 0 ][ 0 ] ), NULL );

    for ( unsigned int i = 0; i < data_y->size(); i++ )
        {
        LineTo( hdc, CENTER_X - ( int ) SCALE_X * i, 
            y1 - D - ( int ) ( SCALE_Y * data_y[ 0 ][ i ] ) );
        }

    //Строим линию 2 влево.
    LOGBRUSH lb2;    
    lb2.lbStyle = BS_SOLID; 
    lb2.lbColor = RGB( 255, 0, 0); 
    lb2.lbHatch = 0;

    HGDIOBJ hPen2 = ExtCreatePen(PS_GEOMETRIC | PS_SOLID, 2, &lb2, 0, NULL);    
    HGDIOBJ hPenOld = SelectObject(hdc, hPen2); 

    MoveToEx( hdc, CENTER_X, 
        y1 - D - ( int ) ( SCALE_Y * data_y2[ 0 ][ 0 ] ), NULL );

    for ( unsigned int i = 0; i < data_y2->size(); i++ )
        {
        LineTo( hdc, CENTER_X - ( int ) SCALE_X * i, 
            y1 - D - ( int ) ( SCALE_Y * data_y2[ 0 ][ i ] ) );
        }
    }
//-----------------------------------------------------------------------------
void init( HWND hWnd )
    {
    pid1.par[ PID::PAR_k ] = 1;             //1 Параметр k.
    pid1.par[ PID::PAR_Ti ] = 2;            //2 Параметр Ti.
    pid1.par[ PID::PAR_Td ] = 0.01;         //3 Параметр Td.
    pid1.par[ PID::PAR_dt ] = 1;            //4 Интервал расчёта
    pid1.par[ PID::PAR_dmax ] = 200;        //5 Мax значение входной величины.
    pid1.par[ PID::PAR_dmin ] = 0;          //6 Мin значение входной величины.
    pid1.par[ PID::PAR_AccelTime ] = 0;     //7 Время выхода на режим регулирования.
    pid1.par[ PID::PAR_IsManualMode ] = 0;  //8 Ручной режим.
    pid1.par[ PID::PAR_UManual ] = 0;       //9 Заданное ручное значение выходного сигнала.

    pid1.Reset( 0 );
    pid1.SetZ( 90 );
    pid1.On();

    plant_data->push_back( 0 );   
    PID_data->push_back( 0 );

    fopen_s( &work_data_stream, "simul_system_data.prn", "w" );
    }

void final()
    {
    fclose( work_data_stream );
    }
//----------------------------------------------------------------------------
void eval()
    {
    static int dt = 0;
    dt += 1;

    //Добавляем в вектор новую точку при необходимости и сдвигаем данные.
    const int MAX_VECTOR_SIZE = 1000;
    if ( plant_data->size() < MAX_VECTOR_SIZE )
        {
        plant_data->push_back( 0 );       
        }
    for ( int i = plant_data->size() - 1; i > 0; i-- )
        {
        plant_data[ 0 ][ i ] = plant_data[ 0 ][ i - 1 ];
        }

    if ( PID_data->size() < MAX_VECTOR_SIZE )
        {
        PID_data->push_back( 0 );       
        }
    for ( int i = PID_data->size() - 1; i > 0; i-- )
        {
        PID_data[ 0 ][ i ] = PID_data[ 0 ][ i - 1 ];
        }


    //Добавляем новую точку.
    //double new_val = 0.99 * PID_data[ 0 ][ 0 ];// + 0.001 * PID_data[ 0 ][ 0 ] * PID_data[ 0 ][ 0 ];
    //PID_data[ 0 ][ 0 ] = new_val;
    //+ 0.001 * PID_data[ 0 ][ i - 1 ] * PID_data[ 0 ][ i - 1 ] + sin( 10 );

    double new_control_val = pid1.Eval( p_object.get_current_out() );
    double new_val = p_object.get_new_out( new_control_val );
    plant_data[ 0 ][ 0 ] = new_val; 
    PID_data[ 0 ][ 0 ] = new_control_val;

    fprintf_s( work_data_stream, "%f\t%f\n", new_val, new_control_val );
    }
//----------------------------------------------------------------------------
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
//----------------------------------------------------------------------------
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
//----------------------------------------------------------------------------
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
//----------------------------------------------------------------------------
WNDPROC DefEditProc;

//---------------------------------------------------------------------------
LRESULT CALLBACK NewEditProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
    switch(message)
        {
        case WM_CHAR:
            if( wParam == VK_RETURN )
                {
                WCHAR str[255];   
                WCHAR *stopstring;

                GetWindowText( hwnd, str, 255 );
                double new_val = wcstod( str, &stopstring );
                p_object.set_k( new_val );

                return(0);
                }
        }
    return((LRESULT)CallWindowProc((WNDPROC)DefEditProc,hwnd,message,wParam,lParam));
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

    //k1
    HWND hwnd_k1_edit = CreateWindow(        
        L"Edit",        // Predefined class; Unicode assumed 
        L"0.9",         // Button text 
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | ES_RIGHT | WS_BORDER,  // Styles 
        30,             // x position 
        500,            // y position 
        100,            // Button width
        20,             // Button height
        hWnd,           // Parent window
        (HMENU)10000,   // No menu.
        (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), 
        NULL);          // Pointer not needed.
    
    if (!hwnd_k1_edit)
        {
        return FALSE;
        }

    DefEditProc = (WNDPROC)GetWindowLongPtr(hwnd_k1_edit,GWLP_WNDPROC);
    SetWindowLongPtr(hwnd_k1_edit,GWLP_WNDPROC,(LPARAM)NewEditProc);

    //k1
    HWND hwnd_k1_label = CreateWindow(        
        L"Static",        // Predefined class; Unicode assumed 
        L"k1",         // Button text 
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | ES_RIGHT | WS_BORDER,  // Styles 
        5,              // x position 
        500,            // y position 
        20,             // Button width
        20,             // Button height
        hWnd,           // Parent window
        (HMENU)10000,   // No menu.
        (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), 
        NULL);          // Pointer not needed.


    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
    }
//----------------------------------------------------------------------------
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
            {
                SetTimer( hWnd, idTimer = 1, 100, NULL); 

                init( hWnd ); 
                break;
            }

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

                //case 10000:


                //    if( EN_CHANGE == wmEvent)
                //        {          
                //        WCHAR str[255];
                //        int a = GetDlgItemText( hWnd, 10000, str, 255 );
                //        a++;
                //        }

                //    break;             

                default:
                    return DefWindowProc(hWnd, message, wParam, lParam);
                }
            break;

        case WM_ERASEBKGND:
            ////Очищаем окно.
            //hdc = GetDC( hWnd );

            //RECT rect;
            //GetClientRect (hWnd, &rect);
            ////rect.bottom /= 2;
            //FillRect ( hdc, &rect, (HBRUSH)(COLOR_WINDOW+1));

            return 1;
            break;

        case WM_TIMER:
            {
            //Очищаем окно.
            hdc = GetDC( hWnd );

            //Здесь рисуем в hdc
            RECT rect;
            GetClientRect (hWnd, &rect);
            rect.bottom /= 2;
            FillRect ( hdc, &rect, (HBRUSH)(COLOR_WINDOW+1));

            //Рисуем график ПИД.
            DrawSeries( hdc, rect.left, rect.top, rect.right, rect.bottom,
                10, plant_data, PID_data );

            ReleaseDC( hWnd, hdc );
                       
            eval();

            break;
            }

        case WM_PAINT:
            {

            //static int x0 = 0;
            //x0 += 10;

            hdc = BeginPaint(hWnd, &ps);

            RECT rect;
            GetClientRect (hWnd, &rect);            
            ExcludeClipRect( hdc, rect.left, rect.top, rect.right, rect.bottom / 2 );
            FillRect ( hdc, &rect, (HBRUSH)(COLOR_WINDOW+1));
            HRGN hrgn = CreateRectRgn( 
                rect.left, rect.top, rect.right, rect.bottom / 2 ); 
            SelectClipRgn(hdc, hrgn); 
            
            //Очищаем окно.
            //RECT rect;
            //GetClientRect (hWnd, &rect);            
            //FillRect ( hdc, &rect, (HBRUSH)(COLOR_WINDOW+1));

            //MoveToEx( hdc, 0, 0, NULL );
            //LineTo( hdc, x0, x0 );
            EndPaint(hWnd, &ps);
            break;
            }

        case WM_DESTROY:
            final();

            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    return 0;
    }
//----------------------------------------------------------------------------
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
//----------------------------------------------------------------------------
