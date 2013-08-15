// neuroPID.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "neuroPID.h"

#include "vector"

#include "PID.h"
#include "plant.h"

#include "mlp.h"

plant   p_object;
PID     pid1;

std::vector<float> *plant_data = new std::vector<float>();
std::vector<float> *PID_data = new std::vector<float>();

//Данные работы храним в файлах.
FILE *work_data_stream;   

#ifdef SAVE_DATA_FOR_EMULATOR_LEARNING
//FILE *emulator_learning_data_stream;  
#endif // SAVE_DATA_FOR_EMULATOR_LEARNING

//Эмулятор.
const int INPUTS_COUNT = 10;
mlp *emulator = new mlp( 2 * INPUTS_COUNT, 10, 1 );
std::vector<float> *emulator_output = new std::vector<float>();
std::vector<float> *emulator_input = new std::vector<float>();

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
float **err_; 

void init( HWND hWnd )
    {
    err_ = new float* [ 3 ];

    err_[ mlp::L_OUTPUT ] = new float[ 1 ];
    err_[ mlp::L_HIDDEN ] = new float[ 10 ];

    pid1.par[ PID::PAR_k ] = 1;             //1 Параметр k.
    pid1.par[ PID::PAR_Ti ] = 2;            //2 Параметр Ti.
    pid1.par[ PID::PAR_Td ] = 0.01f;         //3 Параметр Td.
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

    fopen_s( &work_data_stream, "..\\simul_system_data.prn", "w" );

    emulator->init_weights();
    emulator->load_from_file( "..\\emul_k.data" );
    for ( int i = 0; i < 2 * INPUTS_COUNT; i++ )
        {
        emulator_input->push_back( 0 );
        }   
    emulator_output->push_back( 0 );

#ifdef SAVE_DATA_FOR_EMULATOR_LEARNING
//    fopen_s( &emulator_learning_data_stream, "..\\emul_learning.data", "w" );

//    fprintf_s( emulator_learning_data_stream, "%f\n%f\n%f\n", 2., 1000., 200. );
#endif // SAVE_DATA_FOR_EMULATOR_LEARNING
    }

void final()
    {
    fclose( work_data_stream );

#ifdef SAVE_DATA_FOR_EMULATOR_LEARNING
//    fclose( emulator_learning_data_stream );
#endif // SAVE_DATA_FOR_EMULATOR_LEARNING
    }
//----------------------------------------------------------------------------
void eval()
    {
    static float err[1];     

    static int dt = 0;
    dt += 1;

    float new_control_val = pid1.Eval( p_object.get_current_out() );
    float new_val = p_object.get_new_out( new_control_val );

    //Обучение эмулятора в процессе работы при необходимости.


    float Ek = 0;

    if ( dt > 2 )
        {
        err[ 0 ] = new_val - emulator_output[ 0 ][ 0 ];
        if ( abs( err[ 0 ] ) > 2 )
            {
            err[ 0 ] /= 120;

            for ( int k = 0; k < 3; k++ )
                {
                emulator->learn_iteration(
                    emulator_input->data(), err, mlp::T_SAMPLE_Y, Ek, err_, 1 );
                }
            }
        }

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


    plant_data[ 0 ][ 0 ] = new_val; 
    PID_data[ 0 ][ 0 ] = new_control_val;

    fprintf_s( work_data_stream, "%f\t%f\n", new_val, new_control_val );

    //Работа эмулятора.
    //Сдвигаем данные входной выборки.
    for ( int i = 0; i < 2 * INPUTS_COUNT - 1; i++ )
        {
         emulator_input[ 0 ][ i ] = emulator_input[ 0 ][ i + 1 ];
        }
    
    //Добавляем новые данные.
    emulator_input[ 0 ][ INPUTS_COUNT - 1 ] = new_val / 120;
    emulator_input[ 0 ][ 2 * INPUTS_COUNT - 1 ] = new_control_val / 120;

    //Вычисляем выход эмулятора.
    float *new_emulator_output = emulator->solve_out( emulator_input->data() );

    //Добавляем в вектор новую точку при необходимости и сдвигаем данные.   
    if ( emulator_output->size() < MAX_VECTOR_SIZE )
        {
        emulator_output->push_back( 0 );       
        }
    for ( int i = emulator_output->size() - 1; i > 0; i-- )
        {
        emulator_output[ 0 ][ i ] = emulator_output[ 0 ][ i - 1 ];
        }
    emulator_output[ 0 ][ 0 ] = *new_emulator_output * 120;



    

#ifdef SAVE_DATA_FOR_EMULATOR_LEARNING
#endif //SAVE_DATA_FOR_EMULATOR_LEARNING
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
               
                GetWindowText( hwnd, str, 255 );
                float new_val = ( float ) _wtof( str );
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
                10, plant_data, PID_data, emulator_output );

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
