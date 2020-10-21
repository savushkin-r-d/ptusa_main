#include "stdafx.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <conio.h>
#include <windows.h>
#include <time.h>

#include "mlp.h"

const int mlp::MAX_BUFF_SIZE = 10000;

//------------------------------------------------------------------------------
char* to_next_line( char *stream )
    {
    while ( *stream != '\n')
        {
        stream++;
        }
    return ++stream;
    };
//------------------------------------------------------------------------------
char* to_next_word( char *stream )
    {
    while ( *stream != ' ' )
        {
        stream++;
        }
    while ( *stream == ' ' )
        {
        stream++;
        }

    return stream;
    };
//------------------------------------------------------------------------------
void check_range( float &x, float min_value, float max_value )
    {
    if ( x > max_value )
        {
        x = max_value;
        }

    if ( x < min_value )
        {
        x = min_value;
        }
    }
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
mlp::mlp( int inputs_cnt, int in_cnt, int outputs_cnt, int q  ):inputs_cnt( inputs_cnt ),
    hidden_cnt( in_cnt ), outputs_cnt( outputs_cnt ), q( q )
    {
    w = new float**[ 2 ];

    w[ L_INPUT ] = new float*[ inputs_cnt ];
    w[ L_HIDDEN ] = new float*[ in_cnt ];
        
    for ( int i = 0; i < inputs_cnt; i++ )
        {
        w[ L_INPUT ][ i ] = new float[ in_cnt ];
        }
    for ( int i = 0; i < in_cnt; i++ )
        {
        w[ L_HIDDEN ][ i ] = new float[ outputs_cnt ];
        }

    T = new float*[ 2 ]; 
    T[ L_INPUT ] = new float[ inputs_cnt ];
    T[ L_HIDDEN ] = new float[ in_cnt ];

    x = new float[ inputs_cnt ];
    y = new float[ outputs_cnt ];
    y_hidden = new float[ hidden_cnt ];
    }
//------------------------------------------------------------------------------
void mlp::init_weights()
    {
    srand( ( unsigned )time( NULL ) );

    int i;
    int j;
    for ( i = 0; i < inputs_cnt; i++ )
        {
        for ( j = 0; j < hidden_cnt; j++ )
            {
            w[ L_INPUT ][ i ][ j ] = ( float ) rand() / ( RAND_MAX + 1 ) * 2 - 1; 
            }        
        }
    for ( i = 0; i < hidden_cnt; i++ )
        {
        for ( j = 0; j < outputs_cnt; j++ )
            {
            w[ L_HIDDEN ][ i ][ j ] = ( float ) rand() / ( RAND_MAX + 1 ) * 2 - 1; 
            }        
        }

    for ( j = 0; j < hidden_cnt; j++ )
        {
        T[ L_INPUT ][ j ] = ( float ) rand() / ( RAND_MAX + 1 ) * 2 - 1; 
        }
    for ( j = 0; j < outputs_cnt; j++ )
        {
        T[ L_HIDDEN ][ j ] = ( float ) rand() / ( RAND_MAX + 1 ) * 2 - 1; 
        }        
    }
//------------------------------------------------------------------------------
void mlp::print()
    {
#ifdef _DEBUG
    printf_( " MLP %dx%dx%d\n", inputs_cnt, hidden_cnt, outputs_cnt );
    printf_( " w:\n" );
    int i;
    int j;
    for ( i = 0; i < inputs_cnt; i++ )
        {
        for ( j = 0; j < hidden_cnt; j++ )
            {
            printf_( "% .3f ",  w[ L_INPUT ][ i ][ j ] );
            }        
        printf_( "\n" );
        }
    printf_( "\n" );
    for ( i = 0; i < hidden_cnt; i++ )
        {
        for ( j = 0; j < outputs_cnt; j++ )
            {
            printf_( "% .3f ",  w[ L_HIDDEN ][ i ][ j ] );
            }        
        printf_( "\n" );
        }

    printf_( " T:\n" );
    for ( j = 0; j < hidden_cnt; j++ )
        {
        printf_( "% .3f ", T[ L_INPUT ][ j ] );        
        }
    printf_( "\n" );
    for ( j = 0; j < outputs_cnt; j++ )
        {
        printf_( "% .3f ", T[ L_HIDDEN ][ j ] );        
        }  
    printf_( "\n" );
#endif //_DEBUG
    }
//------------------------------------------------------------------------------
void mlp::learn_iteration( float *sample_x, float *sample_y_or_error, char type, 
    float &E_k, float **err, char is_solve_net_out )
    {
#ifdef USE_DIGIT_LED_DEBUG
    Show5DigitLedWithDot( 4, 1 );
    Show5DigitLed( 5, 0 );
#endif // USE_DIGIT_LED_DEBUG

    int i;
    int k;
    int j;

    if ( 1 == is_solve_net_out)
        {
        //1. Рассчитать выход каждого нейрона.
        solve_out( sample_x );
        }

    //2. Рассчитать среднеквадратичную ошибку. 
    if ( type == T_SAMPLE_Y )
        {
        for ( i = 0; i < outputs_cnt; i++ )
            {
            E_k += ( sample_y_or_error[ i ] - y[ i ] ) * 
                ( sample_y_or_error[ i ] - y[ i ] ) * 0.5f;
            }
        }
    else E_k += sample_y_or_error[ 0 ] ;

#ifdef USE_DIGIT_LED_DEBUG
    Show5DigitLed( 5, 1 );
#endif // USE_DIGIT_LED_DEBUG

    //3. Модифицируем веса и пороги.
    //  а. вычисляем ошибку
    for ( k = 0; k < outputs_cnt; k++ )
        {
        if ( type == T_SAMPLE_Y )
            {
            err[ L_OUTPUT ][ k ] = y[ k ] - sample_y_or_error[ k ] / q;
            }
        else err[ L_OUTPUT ][ k ] = sample_y_or_error[ k ] / q;
        }

#ifdef USE_DIGIT_LED_DEBUG
    Show5DigitLed( 5, 2 );
#endif // USE_DIGIT_LED_DEBUG

    for ( j = 0; j < hidden_cnt; j++ )
        {
        err[ L_HIDDEN ][ j ] = 0;
        for ( k = 0; k < outputs_cnt; k++ )
            {
            err[ L_HIDDEN ][ j ] +=  err[ L_OUTPUT ][ k ] * w[ L_HIDDEN ][ j ][ k ];
            }  
        }
    float alpha_k = .09f;
    float alpha_j = .09f;

    ////
    //for ( j = 0; j < hidden_cnt; j++ )
    //    {
    //    alpha_k += y_hidden[ j ] * y_hidden[ j ];
    //    }
    //alpha_k /= 1 + alpha_k;
    //if ( alpha_k > 0.9 )
    //    {
    //    alpha_k = 0.1f;
    //    }

    //
    //float a = 0;
    //float b = 0;
    //float c = 0;
    //for ( j = 0; j < hidden_cnt; j++ )
    //    {
    //    a += err[ L_HIDDEN ][ j ] * err[ L_HIDDEN ][ j ] *        
    //        y_hidden[ j ] * ( 1 - y_hidden[ j ] );

    //    b += err[ L_HIDDEN ][ j ] * err[ L_HIDDEN ][ j ] *
    //        y_hidden[ j ] * ( 1 - y_hidden[ j ] ) *
    //        y_hidden[ j ] * ( 1 - y_hidden[ j ] );
    //    }
    //for ( k = 0; k < outputs_cnt; k++ )
    //    {
    //    c += y[ k ] * y[ k ];
    //    }
    //if ( b < 0.00000000001 )
    //    {
    //    b = 0.00000000001;
    //    }

    //alpha_j = 4 * a /( ( 1 + c ) * b );
    //if ( alpha_j > 0.5f || alpha_j <= 0 )
    //    {
    //    alpha_j = 0.1f;
    //    }
    //

#ifdef USE_DIGIT_LED_DEBUG
    Show5DigitLed( 5, 3 );
#endif // USE_DIGIT_LED_DEBUG

    //  б. Модифицируем веса и пороги.
    for ( j = 0; j < hidden_cnt; j++ )
        {        
        for ( k = 0; k < outputs_cnt; k++ )
            {
            w[ L_HIDDEN ][ j ][ k ] -= alpha_k * err[ L_OUTPUT ][ k ] * y_hidden[ j ];
            check_range( w[ L_HIDDEN ][ j ][ k ], VR_MIN, VR_MAX ); 
            }
        }
    for ( k = 0; k < outputs_cnt; k++ )
        {        
        T[ L_HIDDEN ][ k ] += alpha_k * err[ L_OUTPUT ][ k ];
        check_range( T[ L_HIDDEN ][ k ], VR_MIN, VR_MAX ); 
        }

#ifdef USE_DIGIT_LED_DEBUG
    Show5DigitLed( 5, 4 );
#endif // USE_DIGIT_LED_DEBUG

    for ( i = 0; i < inputs_cnt; i++ )
        {        
        for ( j = 0; j < hidden_cnt; j++  )
            {
            w[ L_INPUT ][ i ][ j ] -= 
                alpha_j * err[ L_HIDDEN ][ j ] * sample_x[ i ] *
                y_hidden[ j ]*( 1 - y_hidden[ j ] );
            check_range( w[ L_INPUT ][ i ][ j ], VR_MIN, VR_MAX ); 
            }
        }
    for ( j = 0; j < hidden_cnt; j++ )
        {        
        T[ L_INPUT ][ j ] += 
            alpha_j * err[ L_HIDDEN ][ j ] *y_hidden[ j ]*( 1 - y_hidden[ j ] );
        check_range( T[ L_INPUT ][ j ], VR_MIN, VR_MAX ); 
        }
    }
//------------------------------------------------------------------------------
    int mlp::static_learn( 
        float e, i_learn_samples *sample, int max_iteration_cnt, 
        bool is_err /*= false*/, bool print_res /*= true */ )
        {
    //Проверка на соответствие структуры персептрона и обучающей выборки.
    if ( sample->get_inputs_cnt() != inputs_cnt )
        {
#ifdef _DEBUG
        printf( "Error! Sample has different inputs count %d != %d (MLP)!\n",
            sample->get_inputs_cnt(), inputs_cnt );
#endif // _DEBUG
        return -1;
        }
    if ( sample->get_outputs_cnt() != outputs_cnt )
        {
#ifdef _DEBUG
        printf( "Error! Sample has different outputs count %d != %d (MLP)!\n",
            sample->get_outputs_cnt(), outputs_cnt );
#endif // _DEBUG
        return -2;
        }

#ifdef _DEBUG
    if ( print_res )
    	{
        printf( "Start learning[%d]:\n", sample->get_samples_cnt() );
    	}    
#endif // _DEBUG

    float *sample_x = 0;
    float *sample_y = 0;
    float E_k = 10;

    float **err;
    err = new float*[ 3 ];
    err[ L_OUTPUT ] = new float[ outputs_cnt ];
    err[ L_HIDDEN ] = new float[ hidden_cnt ];

    int total_start_time = GetTickCount();
    int start_time = GetTickCount();
    int iteration_n = 0;
    while ( E_k > e )
        {  
#ifdef KEY_BREAK
        if ( _kbhit() ) 
            {
            _getch();
            break;        
            }
#endif // KEY_BREAK
        E_k = 0;        
        for ( int idx = 0; idx < sample->get_samples_cnt(); idx++ ) //Для всех образов.
            {    	
            sample_x = sample->get_sample_x( idx );
            sample_y = sample->get_sample_y( idx );

            if ( is_err )
            	{
                learn_iteration( sample_x, sample_y, T_ERROR, E_k, err, 1 );
            	}
            else
                {
                learn_iteration( sample_x, sample_y, T_SAMPLE_Y, E_k, err, 1 );
                }
            }
        iteration_n++;
        if ( iteration_n > max_iteration_cnt )
            {
#ifdef _DEBUG
            if ( print_res )
            	{
                printf( "Max iteration count has been reached - %d! Learning has stopped.\n",
                    max_iteration_cnt );
            	}
#endif // _DEBUG
            return -3;
            }

#ifdef _DEBUG
        if ( 0 == iteration_n % 500 && print_res )
            {           
            printf( "%6d %.10f - %.3f sec\n", iteration_n, E_k, 
                0.001 * ( GetTickCount() - start_time ) );
            start_time = GetTickCount();
            }
#endif // _DEBUG
        }

#ifdef _DEBUG
    if ( print_res )
    	{
        printf( "\nTotal %6d % .9f - %.3f sec\n", iteration_n, E_k, 
            0.001 * ( GetTickCount() - total_start_time ) );
    	}

#endif // _DEBUG

    return 0;
    }
//------------------------------------------------------------------------------
float* mlp::solve_layer_out( float *x_in, int layer_type, int activation_f_type )
    {
    // Рассчитать выход каждого нейрона.
    float *y_tmp;
    float **w_tmp;
    float *T_tmp;
    int in_cnt;
    int out_cnt;

    switch ( layer_type )
        {
    default:
    case L_INPUT:
        y_tmp = y_hidden;
        w_tmp = w[ L_INPUT ];
        T_tmp = T[ L_INPUT ];

        in_cnt = inputs_cnt;
        out_cnt = hidden_cnt;
        break;

    case L_HIDDEN:
        y_tmp = y;
        w_tmp = w[ L_HIDDEN ];
        T_tmp = T[ L_HIDDEN ];

        in_cnt = hidden_cnt;
        out_cnt = outputs_cnt;
        break;
        }
          

    for ( int i = 0; i < out_cnt; i++ )
        {
        y_tmp[ i ] = -T_tmp[ i ];
        for ( int j = 0; j < in_cnt; j++ )
            {
            if ( layer_type == L_INPUT )
                {
                y_tmp[ i ] += w_tmp[ j ][ i ] * x_in[ j ] / q;
                }
            else
                {
                y_tmp[ i ] += w_tmp[ j ][ i ] * x_in[ j ];
                }            
            }
        }

    switch ( activation_f_type )
        {
    case F_LINEAR:
        break;

    case F_SYGMOID:
        for ( int i = 0; i < out_cnt; i++ )
            {
            y_tmp[ i ] = 1 / ( 1  + exp( -y_tmp[ i ] ) ); //??
            }
        break;
        }

    return y_tmp;
    }
//------------------------------------------------------------------------------
float* mlp::solve_layer_out( float **x_in, int layer_type, int activation_f_type )
    {
    // Рассчитать выход каждого нейрона.
    float *y_tmp;
    float **w_tmp;
    float *T_tmp;
    int in_cnt;
    int out_cnt;

    switch ( layer_type )
        {
        default:
        case L_INPUT:
            y_tmp = y_hidden;
            w_tmp = w[ L_INPUT ];
            T_tmp = T[ L_INPUT ];

            in_cnt = inputs_cnt;
            out_cnt = hidden_cnt;
            break;

        case L_HIDDEN:
            y_tmp = y;
            w_tmp = w[ L_HIDDEN ];
            T_tmp = T[ L_HIDDEN ];

            in_cnt = hidden_cnt;
            out_cnt = outputs_cnt;
            break;
        }


    for ( int i = 0; i < out_cnt; i++ )
        {
        y_tmp[ i ] = -T_tmp[ i ];
        for ( int j = 0; j < in_cnt; j++ )
            {
            if ( layer_type == L_INPUT )
                {
                y_tmp[ i ] += w_tmp[ j ][ i ] * x_in[ j ][ 0 ] / q;
                }
            else
                {
                y_tmp[ i ] += w_tmp[ j ][ i ] * x_in[ j ][ 0 ];
                }          
            }
        }

    switch ( activation_f_type )
        {
        case F_LINEAR:
            break;

        case F_SYGMOID:
            for ( int i = 0; i < out_cnt; i++ )
                {
                y_tmp[ i ] = 1 / ( 1  + exp( -y_tmp[ i ] ) ); //??
                }
            break;
        }

    return y_tmp;
    }
//------------------------------------------------------------------------------
int mlp::prognose( int cnt, float *x_in, float *y_out )
    {
    int i;
    float* in_x_val = new float[ inputs_cnt ];
    for ( i = 0; i < inputs_cnt; i++ )
        {
        in_x_val[ i ] = x_in[ i ];
        }

#ifdef _DEBUG
    printf( "Prognosis\n" );
    for ( i = 0; i < inputs_cnt; i++ )
        {
        printf( " % .3f\n", x_in[ i ] );        
        }
    printf( "\n", x_in[ i ] );       
#endif // _DEBUG

    for ( int idx = 0; idx < cnt; idx++ )
        {
        // Рассчитать выход каждого нейрона.        
        solve_out( in_x_val );
#ifdef _DEBUG
        printf( " % .3f\n", y[ 0 ] );
#endif // _DEBUG

        //Сдвинуть окно.
        for ( i = 1; i < inputs_cnt; i++ )
            {
            in_x_val[ i - 1 ] = in_x_val[ i ];
            }
        in_x_val[ inputs_cnt - 1 ] = y[ 0 ];

        y_out[ idx ] = y[ 0 ];
        }

    return 0;
    }
//------------------------------------------------------------------------------
int mlp::prognose( int cnt, float k, float data [] )
    {    
    y_prognose      = new float[ cnt ];
    y_prognose_cnt  = cnt;

    x[ 0 ] = ( float ) k;
    for ( int i = 0; i < inputs_cnt - 1; i++ )
        {
        x[ i + 1 ] = data[ i ];
        }

    for ( int idx = 0; idx < cnt - 1; idx++ )
        {
        // Рассчитать выход.        
        solve_out( x );

        //Скопировать выход.
        y_prognose[ idx ] = y[ 0 ];

        //Сдвинуть окно.
        for ( int j = 0; j < inputs_cnt - 1; j++ )
            {
            x[ j ] = x[ j + 1 ];
            }
        x[ inputs_cnt - 1 ] = y[ 0 ];
        x[ 0 ] = ( float ) k;
        }

    return 0;
    }
//------------------------------------------------------------------------------
int mlp::save_prognose_to_file( char *file_name )
    {
    char *buff = new char[ 100000 ];
    int res = sprintf_s( buff, 50000, "%d \n", y_prognose_cnt );
    for ( int i = 0; i < y_prognose_cnt; i++ )
        {        
        res += sprintf_s( buff + res, 50000, "% 3f\n", y_prognose[ i ] );        
        }

    FILE *stream;
    fopen_s( &stream, file_name, "w" );
    fprintf_s( stream, "%s", buff );
    fclose( stream );

    return 0;
    }
//------------------------------------------------------------------------------
int mlp::prognose( int cnt, float **x_in, int out_col_n  )
    {
#ifdef _DEBUG
    printf( "Prognosis\n" );
#endif // _DEBUG

    for ( int idx = 0; idx < cnt - 1; idx++ )
        {
        // Рассчитать выход каждого нейрона.        
        solve_out( x_in[ idx ] );
#ifdef _DEBUG
        printf( " % .3f\n", y[ 0 ] );
#endif // _DEBUG

        //Сдвинуть окно.
        x_in[ idx + 1 ][ out_col_n ] = y[ 0 ];
        }

    return 0;
    }
//------------------------------------------------------------------------------
int mlp::save_prognose_to_file( char *file_name, float *y_out, int cnt )
    {
    char *buff = new char[ 100000 ];
    int res = sprintf_s( buff, 50000, "%d \n", cnt );
    for ( int i = 0; i < cnt; i++ )
        {        
        res += sprintf_s( buff + res, 50000, "% 3f\n", y_out[ i ] );        
        }

    FILE *stream;
    fopen_s( &stream, file_name, "w" );
    fprintf_s( stream, "%s", buff );
    fclose( stream );

    return 0;
    }
//------------------------------------------------------------------------------
int mlp::save_prognose_to_file( char *file_name, float **y_out, int cnt, int column_cnt,
    int column_window_size, int factor_k )
    {
    FILE *stream;
    fopen_s( &stream, file_name, "w" );
    fprintf( stream,"%s \t %s \t %s \t\n", "T гомог.", "Т пастер.", "VC100" );

    for ( int i = 0; i < cnt; i++ )
        {     
        for ( int j = 0; j < column_cnt / column_window_size; j++ )
            {
            fprintf( stream,"% f\t", 
                factor_k * y_out[ i ][ ( j + 1 ) * column_window_size - 1 ]  );        
            }         
        fprintf( stream, "\n" );        
        }

    fclose( stream );
    return 0;
    }
//------------------------------------------------------------------------------
int mlp::save_to_stream( char *stream )
    {
    int size = 0;
    size += sprintf_s( stream, MAX_BUFF_SIZE - size, "%d\n", inputs_cnt ); 
    size += sprintf_s( stream + size, MAX_BUFF_SIZE - size, "%d\n", hidden_cnt ); 
    size += sprintf_s( stream + size, MAX_BUFF_SIZE - size, "%d\n", outputs_cnt );

    size += sprintf_s( stream + size, MAX_BUFF_SIZE - size, "input [ w ] \n");        
    for ( int i = 0; i < inputs_cnt; i++ )
        {
        for ( int j = 0; j < hidden_cnt; j++ )
            {
            size += sprintf_s( stream + size, MAX_BUFF_SIZE - size, " % f", 
                w[ L_INPUT ][ i ][ j ] );
            }        
        size += sprintf_s( stream + size, MAX_BUFF_SIZE - size, "\n");        
        }
    size += sprintf_s( stream + size, MAX_BUFF_SIZE - size, "hidden [ w ] \n");        
    for ( int i = 0; i < hidden_cnt; i++ )
        {
        for ( int j = 0; j < outputs_cnt; j++ )
            {
            size += sprintf_s( stream + size, MAX_BUFF_SIZE - size, " % f", 
                w[ L_HIDDEN ][ i ][ j ] );
            }        
        size += sprintf_s( stream + size, MAX_BUFF_SIZE - size, "\n");        
        }

    size += sprintf_s( stream + size, MAX_BUFF_SIZE - size, "hidden [ T ]\n");
    for ( int j = 0; j < hidden_cnt; j++ )
        {
        size += sprintf_s( stream + size, MAX_BUFF_SIZE - size, " % f", T[ L_INPUT ][ j ] );        
        }
    size += sprintf_s( stream + size, MAX_BUFF_SIZE - size, "\n"); 
    size += sprintf_s( stream + size, MAX_BUFF_SIZE - size, "output [ T ]\n");
    for ( int j = 0; j < outputs_cnt; j++ )
        {
        size += sprintf_s( stream + size, MAX_BUFF_SIZE - size, " % f", T[ L_HIDDEN ][ j ] );        
        }
    size += sprintf_s( stream + size, MAX_BUFF_SIZE - size, "\n"); 

    return size;
    }
//------------------------------------------------------------------------------
int mlp::load_from_stream( char *stream )
    {
    int inputs_cnt_tmp;
    int hidden_cnt_tmp;
    int outputs_cnt_tmp;    

    sscanf_s( stream, "%d", &inputs_cnt_tmp );
    stream = to_next_line( stream );
    sscanf_s( stream, "%d", &hidden_cnt_tmp );
    stream = to_next_line( stream );
    sscanf_s( stream, "%d", &outputs_cnt_tmp );
    stream = to_next_line( stream );
    stream = to_next_line( stream );

    if ( inputs_cnt != inputs_cnt_tmp )
    	{
#ifdef _DEBUG
        printf( "Количество считанных входов (%d) отличается от заданного (%d).",
            inputs_cnt_tmp, inputs_cnt );
#endif // _DEBUG

        return -1;
    	}
    if ( hidden_cnt != hidden_cnt_tmp )
        {
#ifdef _DEBUG
        printf( "Количество считанных внутренних нейронов (%d) отличается от заданного (%d).",
            hidden_cnt_tmp, hidden_cnt );
#endif // _DEBUG

        return -1;
        }
    if ( outputs_cnt != outputs_cnt_tmp )
        {
#ifdef _DEBUG
        printf( "Количество считанных выходов (%d) отличается от заданного (%d).",
            outputs_cnt_tmp, outputs_cnt );
#endif // _DEBUG

        return -1;
        }


    if ( inputs_cnt && outputs_cnt && hidden_cnt )
        {    	
        int i;
        for ( i = 0; i < inputs_cnt; i++ )
            {
            delete [] w[ L_INPUT ][ i ];
            }
        for ( i = 0; i < hidden_cnt; i++ )
            {
            delete [] w[ L_HIDDEN ][ i ];
            }
        delete [] w[ L_INPUT ];
        delete [] w[ L_HIDDEN ];

        delete [] T[ L_INPUT ];
        delete [] T[ L_HIDDEN ];

        delete [] x;
        delete [] y;
        delete [] y_hidden;
        }

    inputs_cnt = inputs_cnt_tmp;
    hidden_cnt = hidden_cnt_tmp;
    outputs_cnt = outputs_cnt_tmp;

    w[ L_INPUT ] = new float*[ inputs_cnt ];
    w[ L_HIDDEN ] = new float*[ hidden_cnt ];

    int i;
    for ( i = 0; i < inputs_cnt; i++ )
        {
        w[ L_INPUT ][ i ] = new float[ hidden_cnt ];
        }
    for ( i = 0; i < hidden_cnt; i++ )
        {
        w[ L_HIDDEN ][ i ] = new float[ outputs_cnt ];
        }

    T[ L_INPUT ] = new float[ inputs_cnt ];
    T[ L_HIDDEN ] = new float[ hidden_cnt ];  

    x = new float[ inputs_cnt ];
    y = new float[ outputs_cnt ];
    y_hidden = new float[ hidden_cnt ];

    for ( int i = 0; i < inputs_cnt; i++ )
        {
        for ( int j = 0; j < hidden_cnt; j++ )
            {
            stream = to_next_word( stream );
            sscanf_s( stream, "%f", &w[ L_INPUT ][ i ][ j ] );            
            }                
        }
    stream = to_next_line( stream );
    stream = to_next_line( stream );
    for ( int i = 0; i < hidden_cnt; i++ )
        {
        for ( int j = 0; j < outputs_cnt; j++ )
            {
            stream = to_next_word( stream );
            sscanf_s( stream, "%f", &w[ L_HIDDEN ][ i ][ j ] );            
            }                
        }
    stream = to_next_line( stream );
    stream = to_next_line( stream );

    for ( int j = 0; j < hidden_cnt; j++ )
        {
        stream = to_next_word( stream );
        sscanf_s( stream, "%f", &T[ L_INPUT ][ j ] );        
        }
    stream = to_next_line( stream );
    stream = to_next_line( stream );
    for ( int j = 0; j < outputs_cnt; j++ )
        {
        stream = to_next_word( stream );
        sscanf_s( stream, "%f", &T[ L_HIDDEN ][ j ] );        
        }

    return 0;
    }
//------------------------------------------------------------------------------
int mlp::save_to_file( char *file_name )
    {
    char buff[ MAX_BUFF_SIZE ];
    save_to_stream( buff );

    FILE *stream;
    fopen_s( &stream, file_name, "w" );
    fprintf_s( stream, "%s", buff );
    fclose( stream );
    return 0;
    }
//------------------------------------------------------------------------------
int mlp::load_from_file( char *file_name )
    {
    char buff[ MAX_BUFF_SIZE ];

    char *tmp_buf = buff; 
    FILE *stream = nullptr;
    fopen_s( &stream, file_name, "r" );
    if ( stream == nullptr )
        {
#ifdef _DEBUG
        printf( "Can't find file \\%s\\!\n", file_name );
        return 1;
#endif // _DEBUG
        }

    while ( int read_cnt = fread( tmp_buf, sizeof( char ), 50, stream ) )
        {
        tmp_buf += read_cnt;
        }

    fclose( stream );

    load_from_stream( buff );
    return 0;
    }
//------------------------------------------------------------------------------
float* mlp::solve_out( float *x_in )
    {
    solve_layer_out( x_in, L_INPUT, F_SYGMOID );
    solve_layer_out( y_hidden, L_HIDDEN, F_SYGMOID );
       
    return y;
    }
//------------------------------------------------------------------------------
float* mlp::solve_out( float **x_in )
    {
    solve_layer_out( x_in, L_INPUT, F_SYGMOID );
    solve_layer_out( y_hidden, L_HIDDEN, F_LINEAR );
    return y;
    }
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
