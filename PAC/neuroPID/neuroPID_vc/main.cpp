// neuroPID.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "main.h"

#include "vector"

#include "PID.h"
#include "plant.h"

#include "mlp.h"
#include "nn_manager.h"

#include <fcntl.h>
#include <io.h>


bool g_is_start = false;

//Эмулятор.
nn_manager* nn_emul_manager = new nn_manager( 100, 10 );

#ifdef SAVE_DATA_FOR_LEARNING
FILE *emulator_learning_data_stream;
#endif 
//----------------------------------------------------------------------------
void DrawSeries( HDC hdc, int x0, int y0, int x1, int y1, int scale,
                std::vector<float> *data_y, std::vector<float> *data_y2,
                std::vector<float> *data_y3 )
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

    const float SCALE_Y = 2;
    const float SCALE_X = 10;

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
    int y_val = 0;
    WCHAR y_str[ 10 ];
    while ( y_ > D + ARROW_SIZE )
        {
        MoveToEx( hdc, CENTER_X, y_, NULL );
        LineTo( hdc, CENTER_X + 5, y_ );

        swprintf( y_str, 10, L"%d", y_val );

        if ( y_val >= 10 )
            {
            TextOut( hdc, CENTER_X + 5, y_ - 8, y_str, wcslen( y_str ) );
            }        

        y_ -= ( int ) ( 10 * SCALE_Y );   
        y_val += 10;       
        }


    //Lines x
    int x_ = CENTER_X;    
    while ( x_ < x1 - D - ARROW_SIZE )
        {
        MoveToEx( hdc, x_, y1 - D, NULL );
        LineTo( hdc, x_, y1 - D - 5 );
        x_ += ( int ) SCALE_X;
        }
    x_ = CENTER_X;  
    while ( x_ > D )
        {
        MoveToEx( hdc, x_, y1 - D, NULL );
        LineTo( hdc, x_, y1 - D - 5 );
        x_ -= ( int ) SCALE_X;
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

    ////Массив точек для кривой Безье.
    //POINT* lpPoints = new POINT[ data_y->size() ];
    //for ( unsigned int i = 0; i < data_y->size(); i++ )
    //    {
    //    POINT tmp = 
    //        { CENTER_X - ( int ) SCALE_X * i, y1 - D - ( int ) ( SCALE_Y * data_y[ 0 ][ i ] ) };
    //    lpPoints[ i ] = tmp;
    //    }
    //PolyBezierTo( hdc, lpPoints, data_y->size() );

    for ( unsigned int i = 0; i < data_y->size(); i++ )
        {
        LineTo( hdc, CENTER_X - ( int ) SCALE_X * i, 
            y1 - D - ( int ) ( SCALE_Y * data_y[ 0 ][ i ] ) );
        }
    DeleteObject(hPen); 

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
    DeleteObject(hPen2); 

    //Строим линию 3 влево.
    LOGBRUSH lb3;    
    lb3.lbStyle = BS_SOLID; 
    lb3.lbColor = RGB( 0, 0, 0 ); 
    lb3.lbHatch = 0;

    HGDIOBJ hPen3 = ExtCreatePen(PS_GEOMETRIC | PS_DASH, 2, &lb3, 0, NULL);    
    hPenOld = SelectObject(hdc, hPen3); 

    MoveToEx( hdc, CENTER_X, 
        y1 - D - ( int ) ( SCALE_Y * data_y3[ 0 ][ 0 ] ), NULL );

    for ( unsigned int i = 0; i < data_y3->size(); i++ )
        {
        LineTo( hdc, CENTER_X - ( int ) SCALE_X * i, 
            y1 - D - ( int ) ( SCALE_Y * data_y3[ 0 ][ i ] ) );
        }
    DeleteObject(hPen3); 

    }
//-----------------------------------------------------------------------------
void init( HWND hWnd )
    {
    if (AllocConsole())
        {
        int hCrt = _open_osfhandle((long)GetStdHandle(STD_OUTPUT_HANDLE), 4);
        *stdout = *(::_fdopen(hCrt, "w"));
        ::setvbuf(stdout, NULL, _IONBF, 0);
        *stderr = *(::_fdopen(hCrt, "w"));
        ::setvbuf(stderr, NULL, _IONBF, 0);
        std::ios::sync_with_stdio();
        }  
    SetConsoleCP ( 1251 );
    SetConsoleOutputCP ( 1251 );   

#ifdef SAVE_DATA_FOR_LEARNING
    fopen_s( &emulator_learning_data_stream, "..\\PID_tuner_learning.data", "w+" );

    fprintf_s( emulator_learning_data_stream, "%d\n%d\n%d\n", 5, 1000, 120 );
#endif 
    }
//----------------------------------------------------------------------------
void final()
    {  
#ifdef SAVE_DATA_FOR_LEARNING
    fclose( emulator_learning_data_stream );
    emulator_learning_data_stream = 0;
#endif 
    }
//----------------------------------------------------------------------------
void eval()
    {
    nn_emul_manager->eval();

#ifdef SAVE_DATA_FOR_LEARNING
    fprintf_s( emulator_learning_data_stream, "%f\t%f\t%f\t%f\t%f\n", 
        nn_emul_manager->get_plant()->get_current_out(),
        nn_emul_manager->get_plant()->get_current_control_v(),
        nn_emul_manager->get_PID()->get_p(), nn_emul_manager->get_PID()->get_i(),
        nn_emul_manager->get_PID()->get_d() );
#endif

    nn_emul_manager->get_PID()->get_p();
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

    init( 0 ); 

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
WNDPROC DefEditProc1;
WNDPROC DefEditProc2;

WNDPROC DefStartProc;
//---------------------------------------------------------------------------
LRESULT CALLBACK NewStartProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
    switch(message)
        {
    case WM_COMMAND:
        if( wParam == VK_RETURN )
            {

            return( 0 );
            }
        }
    return((LRESULT)CallWindowProc((WNDPROC)DefStartProc,hwnd,message,wParam,lParam));
    }
//---------------------------------------------------------------------------

LRESULT CALLBACK NewEditProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
    switch(message)
        {
        case WM_CHAR:
            if( wParam == VK_RETURN )
                {
                WCHAR str[255];   
               
                GetWindowText( hwnd, str, 255 );
                float new_val = ( float ) _wtof( str );
                nn_emul_manager->get_plant()->set_k1( new_val );
                
                return( 0 );
                }
        }
    return((LRESULT)CallWindowProc((WNDPROC)DefEditProc1,hwnd,message,wParam,lParam));
    }
//---------------------------------------------------------------------------
LRESULT CALLBACK NewEditProc2(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
    switch(message)
        {
        case WM_CHAR:
            if( wParam == VK_RETURN )
                {
                WCHAR str[255];   

                GetWindowText( hwnd, str, 255 );
                float new_val = ( float ) _wtof( str );
                nn_emul_manager->get_PID()->SetZ( new_val );
                return( 0 );
                }
        }
    return((LRESULT)CallWindowProc((WNDPROC)DefEditProc2,hwnd,message,wParam,lParam));
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

    //Start
    HWND hwnd_start = CreateWindow(        
        L"Button",        // Predefined class; Unicode assumed 
        L"Start",         // Button text 
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON ,  // Styles 
        530,             // x position 
        500,            // y position 
        100,            // Button width
        20,             // Button height
        hWnd,           // Parent window
        (HMENU)10100,   // No menu.
        (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), 
        NULL);          // Pointer not needed.

    if (!hwnd_start)
        {
        return FALSE;
        }

    DefStartProc = (WNDPROC)GetWindowLongPtr(hwnd_start,GWLP_WNDPROC);
    SetWindowLongPtr( hwnd_start,GWLP_WNDPROC,(LPARAM)NewStartProc);


    //k1
    int k = nn_emul_manager->get_plant()->get_k1();
    wchar_t str[ 10 ];        
    _itow( k, str, 10 );
    
    HWND hwnd_k1_edit = CreateWindow(        
        L"Edit",        // Predefined class; Unicode assumed 
        str,         // Button text 
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

    DefEditProc1 = (WNDPROC)GetWindowLongPtr(hwnd_k1_edit,GWLP_WNDPROC);
    SetWindowLongPtr(hwnd_k1_edit,GWLP_WNDPROC,(LPARAM)NewEditProc);

    //k1 label.
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

    //Use learning label.
    HWND hwnd_MLP_learn_label = CreateWindow(        
        L"Static",        // Predefined class; Unicode assumed 
        L"Online learning", // Button text 
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | ES_LEFT | WS_BORDER,  // Styles 
        200,            // x position 
        500,            // y position 
        120,            // Button width
        20,             // Button height
        hWnd,           // Parent window
        (HMENU)0,       // No menu.
        (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), 
        NULL);          // Pointer not needed.

    //Use learning checkbox.
    HWND hwnd_MLP_learn_check_box = CreateWindow(        
        L"BUTTON",      // Predefined class; Unicode assumed 
        L"NN2 emulator learn",         // Button text 
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX | BST_CHECKED,  // Styles 
        330,             // x position 
        500,            // y position 
        15,             // Button width
        20,             // Button height
        hWnd,           // Parent window
        (HMENU)10001,   // ID.
        (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), 
        NULL);          // Pointer not needed.

    Button_SetCheck( hwnd_MLP_learn_check_box, BST_CHECKED );

    //z
    HWND hwnd_z_edit = CreateWindow(        
        L"Edit",        // Predefined class; Unicode assumed 
        L"90",         // Button text 
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | ES_RIGHT | WS_BORDER,  // Styles 
        30,             // x position 
        530,            // y position 
        100,            // Button width
        20,             // Button height
        hWnd,           // Parent window
        (HMENU)10010,   // No menu.
        (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), 
        NULL);          // Pointer not needed.

    if (!hwnd_k1_edit)
        {
        return FALSE;
        }

    DefEditProc2 = (WNDPROC)GetWindowLongPtr(hwnd_z_edit,GWLP_WNDPROC);
    SetWindowLongPtr(hwnd_z_edit,GWLP_WNDPROC,(LPARAM)NewEditProc2);

    //z label.
    HWND hwnd_z_label = CreateWindow(        
        L"Static",        // Predefined class; Unicode assumed 
        L"z",         // Button text 
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | ES_RIGHT | WS_BORDER,  // Styles 
        5,              // x position 
        530,            // y position 
        20,             // Button width
        20,             // Button height
        hWnd,           // Parent window
        (HMENU)10011,   // No menu.
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

                case 10001: //Checkbox
                    {
                    // Получаем HWND нашего chechbox'а.
                    HWND hwndCheck = GetDlgItem(hWnd, 10001);
                    // Выясняем текущее состояние chechbox'а.
                    LRESULT res = SendMessage (hwndCheck, BM_GETCHECK, 0, 0);
                    // Если галочка стоит.
                    if(res == BST_CHECKED)
                        {
                        nn_emul_manager->set_learning( true );

                        //SetWindowText( hWnd, L"Checked" );
                        }
                    // Если галочка не стоит.
                    if(res == BST_UNCHECKED)
                        {
                        //SetWindowText( hWnd, L"Unchecked" );

                        nn_emul_manager->set_learning( false );
                        }
                    break;
                    }
                case 10100:
                    g_is_start = ! g_is_start;
                    break;


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
            if ( g_is_start )
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
                    10, nn_emul_manager->get_plant_data(),
                    nn_emul_manager->get_PID_data(), 
                    //emulator_output );
                    nn_emul_manager->get_nn2_emul_data() );

                ReleaseDC( hWnd, hdc );

                eval();
                }

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
