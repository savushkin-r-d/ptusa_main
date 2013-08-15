#include "stdafx.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#ifdef WIN32
#include <conio.h>
#include <windows.h>
#include <time.h>
#endif // WIN32

#ifdef PAC
#include "plc.h"
#endif // PAC

#include "mlp.h"

#ifdef WIN32
const int slp::MAX_BUFF_SIZE = 10000;
#endif // WIN32
const int mlp::MAX_BUFF_SIZE = 10000;

//---------------------------------------------------------------------------------------
char* to_next_line( char *stream )
    {
    while ( *stream != '\n')
        {
        stream++;
        }
    return ++stream;
    };
//---------------------------------------------------------------------------------------
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
//---------------------------------------------------------------------------------------
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
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
#ifdef WIN32
int i_learn_samples::get_inputs_cnt() const
    {
    return window_size;
    }
//---------------------------------------------------------------------------------------
int i_learn_samples::get_outputs_cnt() const
    {
    return 1;
    }
//---------------------------------------------------------------------------------------
int i_learn_samples::get_samples_cnt() const
    {
    return samples_cnt;
    }
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
slp::slp( int inputs_cnt, int outputs_cnt ):inputs_cnt( inputs_cnt ),
    outputs_cnt( outputs_cnt )
    {
    w = new float*[ inputs_cnt ];
    for ( int i = 0; i < inputs_cnt; i++ )
        {
        w[ i ] = new float[ outputs_cnt ];
        }

    x = new float[ inputs_cnt ];
    y = new float[ outputs_cnt ];
    T = new float[ outputs_cnt ]; 
    }
//---------------------------------------------------------------------------------------
void slp::init_weights()
    {
    int j;
    for ( int i = 0; i < inputs_cnt; i++ )
        {
        for ( j = 0; j < outputs_cnt; j++ )
            {
            w[ i ][ j ] = ( float ) rand() / ( RAND_MAX + 1 ); 
            }        
        }

    for ( j = 0; j < outputs_cnt; j++ )
        {
        T[ j ] = ( float ) rand() / ( RAND_MAX + 1 ); 
        }        
    }
//---------------------------------------------------------------------------------------
void slp::print()
    {
    printf( " SLP %dx%d\n", inputs_cnt, outputs_cnt );
    printf( " w:\n" );
    for ( int i = 0; i < inputs_cnt; i++ )
        {
        for ( int j = 0; j < outputs_cnt; j++ )
            {
            printf( "% f ", w[ i ][ j ] );
            }        
        printf( "\n" );
        }

    printf( " T:\n" );
    for ( int j = 0; j < outputs_cnt; j++ )
        {
        printf( "% f ", T[ j ] );
        }
    printf( "\n" );
    }
//---------------------------------------------------------------------------------------
int slp::learn( float e, i_learn_samples *sample, int max_iteration_cnt )
    {
    //Проверка на соответствие структуры персептрона и обучающей выборки.
    if ( sample->get_inputs_cnt() != inputs_cnt )
        {
#ifdef _DEBUG
        printf( "Error! Sample has different inputs count %d != %d (SLP)!\n",
            sample->get_inputs_cnt(), inputs_cnt );
#endif // _DEBUG
        return -1;
        }
    if ( sample->get_outputs_cnt() != outputs_cnt )
        {
#ifdef _DEBUG
        printf( "Error! Sample has different outputs count %d != %d (SLP)!\n",
            sample->get_outputs_cnt(), outputs_cnt );
#endif // _DEBUG
        return -2;
        }

#ifdef _DEBUG
    printf( "Start learning:\n" );
#endif // _DEBUG

    float alpha =  0.1f; //Скорость обучения.
    int i;
    int j;

    float *sample_x = 0;
    float *sample_y = 0;
    float E_k = 10;

    int iteration_n = 0;
    while ( E_k > e )
        {    
        E_k = 0;
        for ( int idx = 0; idx < sample->get_samples_cnt(); idx++ ) //Для всех образов.
            {    	
            sample_x = sample->get_sample_x( idx );
            sample_y = sample->get_sample_y( idx );

            alpha = 0;
            for ( j = 0; j < inputs_cnt; j++ )
                {
                alpha += sample_x[ j ] * sample_x[ j ];
                }
            alpha = 1 / ( 1 + alpha );
            //1. Рассчитать выход каждого нейрона.
            solve_out( sample_x );

            //2. Рассчитать среднеквадратичную ошибку.        
            for ( i = 0; i < outputs_cnt; i++ )
                {
                E_k += ( sample_y[ i ] - y[ i ] ) * ( sample_y[ i ] - y[ i ] ) * 0.5f;
                }

            //3. Модифицируем веса и пороги.
            for ( i = 0; i < outputs_cnt; i++ )
                {        
                for ( j = 0; j < inputs_cnt; j++ )
                    {
                    w[ j ][ i ] -= alpha * ( y[ i ] - sample_y[ i ] ) * sample_x[ j ];
                    }
                }
            for ( i = 0; i < outputs_cnt; i++ )
                {        
                T[ i ] += alpha * ( y[ i ] - sample_y[ i ] );
                }
            }
        iteration_n++;
        if ( iteration_n > max_iteration_cnt )
            {
#ifdef _DEBUG
            printf( "Max iteration count has been reached - %d! Learning has stopped.\n",
                max_iteration_cnt );
#endif // _DEBUG
            return -3;
            }

#ifdef _DEBUG
        printf( "%4d % f\n", iteration_n, E_k );
#endif // _DEBUG
        }

    return 0;
    }
//---------------------------------------------------------------------------------------
int slp::prognose( int cnt, float *x_in, float *y_out )
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
//---------------------------------------------------------------------------------------
float* slp::solve_out( float *x_in )
    {
    // Рассчитать выход каждого нейрона.
    for ( int i = 0; i < outputs_cnt; i++ )
        {
        y[ i ] = -T[ i ];
        for ( int j = 0; j < inputs_cnt; j++ )
            {
            y[ i ] += w[ j ][ i ] * x_in[ j ];
            }
        }
    return y;
    }
//---------------------------------------------------------------------------------------
int slp::save_to_stream( char *stream )
    {
    int size = 0;
    size += sprintf_s( stream, MAX_BUFF_SIZE - size, "%d\n", inputs_cnt );    
    size += sprintf_s( stream + size, MAX_BUFF_SIZE - size, "%d\n", outputs_cnt );

    size += sprintf_s( stream + size, MAX_BUFF_SIZE - size, "[ w ] \n");        
    for ( int i = 0; i < inputs_cnt; i++ )
        {
        for ( int j = 0; j < outputs_cnt; j++ )
            {
            size += sprintf_s( stream + size, MAX_BUFF_SIZE - size, " % f", w[ i ][ j ] );
            }        
        size += sprintf_s( stream + size, MAX_BUFF_SIZE - size, "\n");        
        }

    size += sprintf_s( stream + size, MAX_BUFF_SIZE - size, "[ T ]\n");        
    for ( int j = 0; j < outputs_cnt; j++ )
        {
        size += sprintf_s( stream + size, MAX_BUFF_SIZE - size, " % f", T[ j ] );        
        }
    size += sprintf_s( stream + size, MAX_BUFF_SIZE - size, "\n");        

    return size;
    }
//---------------------------------------------------------------------------------------
int slp::save_to_file( char *file_name )
    {
    char buff[ MAX_BUFF_SIZE ];
    save_to_stream( buff );

    FILE *stream;
    fopen_s( &stream, file_name, "w" );
    fprintf_s( stream, "%s", buff );
    fclose( stream );
    return 0;
    }
//---------------------------------------------------------------------------------------
int slp::load_from_stream( char *stream )
    {    
    int inputs_cnt_tmp;
    int outputs_cnt_tmp;

    sscanf_s( stream, "%d", &inputs_cnt_tmp );
    stream = to_next_line( stream );
    sscanf_s( stream, "%d", &outputs_cnt_tmp );
    stream = to_next_line( stream );
    stream = to_next_line( stream );

    if ( inputs_cnt && outputs_cnt )
        {    	
        for ( int i = 0; i < inputs_cnt; i++ )
            {
            delete [] w[ i ];
            }
        delete [] w;
        delete [] T;
        }

    inputs_cnt = inputs_cnt_tmp;
    outputs_cnt = outputs_cnt_tmp;

    w = new float* [ inputs_cnt ]; 
    for ( int i = 0; i < inputs_cnt; i++ )
        {
        w[ i ] = new float[ outputs_cnt ];
        }

    for ( int i = 0; i < inputs_cnt; i++ )
        {
        for ( int j = 0; j < outputs_cnt; j++ )
            {
            stream = to_next_word( stream );
            sscanf_s( stream, "%f", &w[ i ][ j ] );            
            }                
        }

    T = new float [ outputs_cnt ]; 
    stream = to_next_line( stream );  
    for ( int j = 0; j < outputs_cnt; j++ )
        {
        stream = to_next_word( stream );
        sscanf_s( stream, "%f", &T[ j ] );        
        }

    return 0;
    }
//---------------------------------------------------------------------------------------
int slp::load_from_file( char *file_name )
    {
    char buff[ MAX_BUFF_SIZE ];

    char *tmp_buf = buff; 
    FILE *stream;
    fopen_s( &stream, file_name, "r" );
    while ( int read_cnt = fread( tmp_buf, sizeof( char ), 50, stream ) )
        {
        tmp_buf += read_cnt;
        }

    fclose( stream );

    load_from_stream( buff );
    return 0;
    }
//---------------------------------------------------------------------------------------
int slp::save_prognose_to_file( char *file_name, float *y_out, int cnt )
    {    
    char *buff = new char[ 10000 ];
    int res = sprintf_s( buff, 5000, "%d \n", cnt );
    for ( int i = 0; i < cnt; i++ )
        {        
        res += sprintf_s( buff + res, 5000, "% 3f\n", y_out[ i ] );        
        }

    FILE *stream;
    fopen_s( &stream, file_name, "w" );
    fprintf_s( stream, "%s", buff );
    fclose( stream );    

    return 0;
    }
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
function_sample::function_sample( float x_start, float x_end, float dx, int window_size )
    {
    this->window_size = window_size;

    int elements_cnt = ( int ) ( ( x_end - x_start ) / dx );
    float *f_y = new float[ elements_cnt ];

#ifdef _DEBUG
    printf( "f( x ) = sin( x ), ");
    printf( "range [ % .3f, % .3f ], dx = %.3f, total elements = %d\n", 
        x_start, x_end, elements_cnt );
    printf( "Function values:\n" );
#endif // _DEBUG

    float x_i = x_start;
    for ( int i = 0; i < elements_cnt; i++ )
        {       
        //f_y [ i ] = 0.1f * cos( 0.1f * x_i ) + 0.01f * sin( 0.1f * x_i );
        //f_y [ i ] = 0.1f * cos( 0.1f * x_i );
        f_y [ i ] = 0.5f * sin( 0.9f * x_i ) + 0.1f * cos( 0.2f * x_i );

#ifdef _DEBUG        
        printf( "%2d: [ % 2.3f, % 2.3f ]\n", i, x_i, f_y[ i ] );
#endif // _DEBUG        
        x_i += dx;
        }
#ifdef _DEBUG        
    printf( "\n" );
#endif // _DEBUG     

    samples_cnt = elements_cnt - window_size;
    x = new float*[ samples_cnt ];
    y = new float*[ samples_cnt ];

    //Формирование обучающих выборок.
#ifdef _DEBUG        
    printf( "Learning sample (total %d):\n", samples_cnt );
#endif // _DEBUG     
    float *tmp_f_y = f_y;
    for ( int i = 0; i < samples_cnt; i++ )
        {
        x[ i ] = new float[ window_size ];
#ifdef _DEBUG        
        printf( "%2d: [ ", i );
#endif // _DEBUG     
        for ( int j = 0; j < window_size; j++ )
            {
            x[ i ][ j ] = tmp_f_y[ j ];
#ifdef _DEBUG        
            printf( "% 2.3f ", x[ i ][ j ] );
#endif // _DEBUG     
            }
        y[ i ] = new float[ 1 ];
        y[ i ][ 0 ] = tmp_f_y[ window_size ];
#ifdef _DEBUG        
        printf( " ] -> % 2.3f \n", y[ i ][ 0 ] );
#endif // _DEBUG     
        tmp_f_y++;        
        }

    delete[] f_y;
    }
//---------------------------------------------------------------------------------------
float* function_sample::get_sample_x( int index )
    {
    if ( index >= samples_cnt )
        {
        return 0;
        }

    return x[ index ];
    }
//---------------------------------------------------------------------------------------
float* function_sample::get_sample_y( int index )
    {
    if ( index >= samples_cnt )
        {
        return 0;
        }

    return y[ index ] ;
    }
//---------------------------------------------------------------------------------------
void function_sample::print()
    {
    printf( "Learning sample (total %d):\n", samples_cnt );
    for ( int i = 0; i < samples_cnt; i++ )
        {
        printf( "%2d: [ ", i );
        for ( int j = 0; j < window_size; j++ )
            {
            printf( "% 2.3f ", x[ i ][ j ] );
            }
        printf( " ] -> % 2.3f \n", y[ i ][ 0 ] );
        }
    return;
    }
//---------------------------------------------------------------------------------------
int function_sample::load_from_file( char *file_name )
    {
    const int MAX_BUFF_SIZE = 100000;
    char buff[ MAX_BUFF_SIZE ];

    char *tmp_buf = buff; 
    char *stream = buff;
    FILE *file_stream;
    fopen_s( &file_stream, file_name, "r" );
    while ( int read_cnt = fread( tmp_buf, sizeof( char ), 50, file_stream ) )
        {
        tmp_buf += read_cnt;
        }
    fclose( file_stream );

    if ( samples_cnt && window_size )
        {    	
        for ( int i = 0; i < samples_cnt; i++ )
            {
            delete [] x[ i ];
            delete [] y[ i ];
            }
        delete [] x;
        delete [] y;
        }

    sscanf_s( stream, "%d", &samples_cnt );
    stream = to_next_line( stream );
    sscanf_s( stream, "%d", &window_size );
    stream = to_next_line( stream );

    x = new float*[ samples_cnt ];
    y = new float*[ samples_cnt ];

#ifdef _DEBUG        
    printf( "\nLearning sample (total %d):\n", samples_cnt );
#endif // _DEBUG     
    //Считывание обучающих выборок.    
    for ( int i = 0; i < samples_cnt; i++ )
        {
#ifdef _DEBUG        
        printf( "%2d: [ ", i );
#endif // _DEBUG     
        x[ i ] = new float[ window_size ];
        for ( int j = 0; j < window_size; j++ )
            {
            stream = to_next_word( stream );    
            sscanf_s( stream, "%f", &x[ i ][ j ] );  
            x[ i ][ j ] = x[ i ][ j ] / 200;
#ifdef _DEBUG        
            printf( "% 2.3f ", x[ i ][ j ] );
#endif // _DEBUG     
            }

        y[ i ] = new float[ 1 ];
        stream = to_next_word( stream );    
        sscanf_s( stream, "%f", &y[ i ][ 0 ] );
        y[ i ][ 0 ] = y[ i ][ 0 ] / 200;
#ifdef _DEBUG        
        printf( " ] -> % 2.3f \n", y[ i ][ 0 ] );
#endif // _DEBUG     
        }

    return 0;
    }
//---------------------------------------------------------------------------------------
int function_sample::save_to_file( char *file_name )
    {
    char buff[ 100000 ];

    int res = sprintf_s( buff, 50000, "%d \n", samples_cnt );
    res += sprintf_s( buff + res, 50000, "%d \n", window_size );
    for ( int i = 0; i < samples_cnt; i++ )
        {        
        for ( int j = 0; j < window_size; j++ )
            {
            res += sprintf_s( buff + res, 50000, " % f; ", x[ i ][ j ] );
            }
        res += sprintf_s( buff + res, 50000, "% f\n", y[ i ][ 0 ] );        
        }

    FILE *stream;
    fopen_s( &stream, file_name, "w" );
    fprintf_s( stream, "%s", buff );
    fclose( stream );    

    return 0;
    }
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
stored_sample::stored_sample()
    {
    window_size = 0;
    samples_cnt = 0;
    }
//---------------------------------------------------------------------------------------
float* stored_sample::get_sample_x( int index )
    {
    if ( index >= samples_cnt )
        {
        return 0;
        }

    return x[ index ];
    }
//---------------------------------------------------------------------------------------
float** stored_sample::get_sample_x()
    {
    return x;
    }
//---------------------------------------------------------------------------------------
float* stored_sample::get_sample_y( int index )
    {
    if ( index >= samples_cnt )
        {
        return 0;
        }

    return y[ index ] ;
    }
//---------------------------------------------------------------------------------------
int stored_sample::load_from_file( char *file_name )
    {
    return load_from_file( file_name, 0 );
    }
//---------------------------------------------------------------------------------------
int stored_sample::load_from_file( char *file_name, char is_ignore_first_column )
    {      
    FILE *file_stream;
    fopen_s( &file_stream, file_name, "r" );

    if ( file_stream == NULL )
        {
        return 1;
        }

    fscanf_s( file_stream, "%d", &column_cnt );
    fscanf_s( file_stream, "%d", &row_cnt );    
    fscanf_s( file_stream, "%d", &factor_k );
    if ( is_ignore_first_column ) column_cnt--;

    data = new float* [ row_cnt ];
    for ( int i = 0; i < row_cnt; i++ )
        {
        data[ i ] = new float[ column_cnt ];
        }
    for ( int i = 0; i < row_cnt; i++ )
        {
        if ( is_ignore_first_column )
            {
            fscanf_s( file_stream, "%f" );
            }

        for ( int j = 0; j < column_cnt; j++ )
            {            
            fscanf_s( file_stream, "%f", &data[ i ][ j ] );
            }
        }    

#ifdef _DEBUG_SS
    for ( int i = 0; i < row_cnt; i++ )
        {
        printf( "%d : ", i );
        for ( int j = 0; j < column_cnt; j++ )
            {  
            printf( "%8.3f ", data[ i ][ j ] );
            }
        printf( "\n" );
        }
    printf( "\n" );
#endif // _DEBUG_SS 

    fclose( file_stream );

    return 0;
    }
//---------------------------------------------------------------------------------------
int stored_sample::save_to_file( char *file_name )
    {
    FILE *stream;
    fopen_s( &stream, file_name, "w" );
    fprintf( stream, "%d\n", samples_cnt );
    for ( int i = 0; i < samples_cnt; i++ )
        {
        for ( int j = 0; j < window_size; j++ )
            {
            fprintf( stream, "%f\t", x[ i ][ j ] );
            }  
        fprintf( stream, "\n" );
        }
    fclose( stream );
    return 0;
    }
//---------------------------------------------------------------------------------------
int stored_sample::create_learn_sample( int window_size, int learn_samples_count /*= -1*/, 
    int exclude_every_i /*= 1*/, 
    int etalon_column_number /*= 0 */ )
    {
    //#define _DEBUG_SS

    this->window_size = window_size;

    if ( window_size % column_cnt )
        {
#ifdef DEBUG
        printf( "Windows size [%d] must be multiple of columns count [%d]!\n",
            window_size, column_cnt );
#endif // DEBUG
        return -1;
        }
    int window_column_size = window_size / column_cnt;

    float **data_tmp;
    samples_cnt = row_cnt / exclude_every_i;
    if ( row_cnt % exclude_every_i )
        {
        samples_cnt++;
        }
    data_tmp = new float*[ samples_cnt ];
    for ( int i = 0; i < row_cnt; i++ )
        {
        if ( i % exclude_every_i != 0  )
            {
            continue;
            }
        data_tmp[ i / exclude_every_i ] = data[ i ]; 
#ifdef _DEBUG_SS        
        printf( "%d\n", i );
#endif // _DEBUG_SS 
        }
#ifdef _DEBUG_SS        
    printf( "\n" );
#endif

    samples_cnt = samples_cnt - window_column_size;
    if ( samples_cnt <= 0 )
        {
        return 0;
        }

    x = new float*[ samples_cnt ];
    y = new float*[ samples_cnt ];

    //Формирование обучающих выборок.
#ifdef _DEBUG        
    printf( "Creating learning sample (total %d):\n", samples_cnt );
#endif // _DEBUG     

    for ( int i = 0; i < samples_cnt; i++ )
        {

        x[ i ] = new float[ window_column_size * column_cnt ];
#ifdef _DEBUG_SS        
        printf( "%2d: [ ", i );
#endif // _DEBUG_SS     

        for ( int k = 0; k < column_cnt; k++ )
            {
            for ( int j = 0; j < window_column_size; j++ )
                {                                    
                x[ i ][ k * window_column_size + j ] = 
                    data_tmp[ i + j ][ k ] / factor_k;                
                }
            }    
#ifdef _DEBUG_SS  
        for ( int j = 0; j < window_column_size * column_cnt; j++ )
            {
            printf( "%5.2f ", x[ i ][ j ] );
            }        
#endif // _DEBUG_SS 

        y[ i ] = new float[ 1 ];
        if ( 0 == etalon_column_number )
            {
            y[ i ][ 0 ] = data_tmp[ i + window_column_size ][ column_cnt - 1 ] / factor_k;
            }
        else
            {
            y[ i ][ 0 ] = data_tmp[ i + window_column_size ][ etalon_column_number - 1 ] / factor_k;
            }

#ifdef _DEBUG_SS        
        printf( "] -> %5.2f \n", y[ i ][ 0 ] );
#endif // _DEBUG_SS             
        }
#ifdef _DEBUG_SS        
    printf( "\n" );
#endif

    if ( learn_samples_count > 0 &&
        learn_samples_count <= samples_cnt )
        {
        samples_cnt = learn_samples_count;
        }

    return 0;    
    }
//---------------------------------------------------------------------------------------
int stored_sample::get_factor_k() const
    {
    return factor_k;
    }
//---------------------------------------------------------------------------------------
int stored_sample::get_column_window_size() const
    {
    return window_size / column_cnt;
    }
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
mlp::mlp( int inputs_cnt, int in_cnt, int outputs_cnt ):inputs_cnt( inputs_cnt ),
    hidden_cnt( in_cnt ), outputs_cnt( outputs_cnt )
    {
    w = new float**[ 2 ];

    w[ L_INPUT ] = new float*[ inputs_cnt ];
    w[ L_HIDDEN ] = new float*[ in_cnt ];

    int i;
    for ( i = 0; i < inputs_cnt; i++ )
        {
        w[ L_INPUT ][ i ] = new float[ in_cnt ];
        }
    for ( i = 0; i < in_cnt; i++ )
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
//---------------------------------------------------------------------------------------
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
#endif // WIN32
//---------------------------------------------------------------------------------------
#ifdef PAC
mlp::mlp( char *filename ) : w( 0 ),
    T( 0 ), 
    x( y ), 
    y( 0 ), 
    y_hidden( 0 )
    {       
    mlp_filename[ 0 ] = 0;

#ifdef DEBUG
    Print( "Reading MLP description from %s....\n\r", filename );
#endif // DEBUG

    char far *stream = 0;
    stream = ( char far *) GetFilePositionByName( filename );

    if ( stream != NULL ) 
        {
        strcpy( filename, mlp_filename );

        w = new float**[ 2 ];
        T = new float*[ 2 ];

        sscanf( stream, "%d", &inputs_cnt );
        stream = to_next_line( stream );
        sscanf( stream, "%d", &hidden_cnt );
        stream = to_next_line( stream );
        sscanf( stream, "%d", &outputs_cnt );
        stream = to_next_line( stream );
        stream = to_next_line( stream );

        w[ L_INPUT ] = new float*[ inputs_cnt ];
        w[ L_HIDDEN ] = new float*[ hidden_cnt ];

        int i;
        int j;
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

        for ( i = 0; i < inputs_cnt; i++ )
            {
            for ( j = 0; j < hidden_cnt; j++ )
                {
                stream = to_next_word( stream );
                sscanf( stream, "%f", &w[ L_INPUT ][ i ][ j ] );                
                }    
            }

        stream = to_next_line( stream );
        stream = to_next_line( stream );
        for (  i = 0; i < hidden_cnt; i++ )
            {
            for ( j = 0; j < outputs_cnt; j++ )
                {
                stream = to_next_word( stream );
                sscanf( stream, "%f", &w[ L_HIDDEN ][ i ][ j ] );            
                }                
            }
        stream = to_next_line( stream );
        stream = to_next_line( stream );

        for ( j = 0; j < hidden_cnt; j++ )
            {
            stream = to_next_word( stream );
            sscanf( stream, "%f", &T[ L_INPUT ][ j ] );        
            }
        stream = to_next_line( stream );
        stream = to_next_line( stream );
        for ( j = 0; j < outputs_cnt; j++ )
            {
            stream = to_next_word( stream );
            sscanf( stream, "%f", &T[ L_HIDDEN ][ j ] );        
            }
        }
    else
        {
#ifdef DEBUG
        Print( "Can't find file [%s]!\n\r", filename );
#endif // DEBUG
        while ( 1 )
            {
            SetGlobalError( EC_DEVICE_DEFINITION, 
                ES_WRONG_MLP_DEFINITION_FILE, 0 );
            ShowErrors();
            }
        }
    }
#endif // PAC
//---------------------------------------------------------------------------------------
void mlp::print()
    {
#ifdef DEBUG
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
#endif // DEBUG
    }
//---------------------------------------------------------------------------------------
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
            err[ L_OUTPUT ][ k ] = y[ k ] - sample_y_or_error[ k ];
            }
        else err[ L_OUTPUT ][ k ] = sample_y_or_error[ k ];
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

#ifdef USE_DIGIT_LED_DEBUG
#ifdef DEBUG
    Delay( 200 );
#endif // DEBUG

    Show5DigitLedWithDot( 4, 16 );
    Show5DigitLed( 5, 16 );
#endif // USE_DIGIT_LED_DEBUG
    }
//---------------------------------------------------------------------------------------
#ifdef WIN32
int mlp::static_learn( float e, i_learn_samples *sample, int max_iteration_cnt )
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
    printf( "Start learning[%d]:\n", sample->get_samples_cnt() );
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

            learn_iteration( sample_x, sample_y, T_SAMPLE_Y, E_k, err, 1 );
            }
        iteration_n++;
        if ( iteration_n > max_iteration_cnt )
            {
#ifdef _DEBUG
            printf( "Max iteration count has been reached - %d! Learning has stopped.\n",
                max_iteration_cnt );
#endif // _DEBUG
            return -3;
            }

        //#ifdef _DEBUG
        if ( 0 == iteration_n % 500 )
            {           
            printf( "%6d % f - %.3f sec\n", iteration_n, E_k, 
                0.001 * ( GetTickCount() - start_time ) );
            start_time = GetTickCount();
            }
        //#endif // _DEBUG
        }

    printf( "\nTotal %6d % .9f - %.3f sec\n", iteration_n, E_k, 
        0.001 * ( GetTickCount() - total_start_time ) );
    return 0;
    }
#endif // WIN32
//---------------------------------------------------------------------------------------
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
            y_tmp[ i ] += w_tmp[ j ][ i ] * x_in[ j ];
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
//---------------------------------------------------------------------------------------
#ifdef WIN32
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
//---------------------------------------------------------------------------------------
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
//---------------------------------------------------------------------------------------
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
//---------------------------------------------------------------------------------------
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
//---------------------------------------------------------------------------------------
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
//---------------------------------------------------------------------------------------
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
//---------------------------------------------------------------------------------------
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
//---------------------------------------------------------------------------------------
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
//---------------------------------------------------------------------------------------
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
//---------------------------------------------------------------------------------------
int mlp::load_from_file( char *file_name )
    {
    char buff[ MAX_BUFF_SIZE ];

    char *tmp_buf = buff; 
    FILE *stream;
    fopen_s( &stream, file_name, "r" );
    while ( int read_cnt = fread( tmp_buf, sizeof( char ), 50, stream ) )
        {
        tmp_buf += read_cnt;
        }

    fclose( stream );

    load_from_stream( buff );
    return 0;
    }
#endif // WIN32
//---------------------------------------------------------------------------------------
float* mlp::solve_out( float *x_in )
    {
#ifdef USE_DIGIT_LED_DEBUG
    Show5DigitLedWithDot( 4, 2 );
    Show5DigitLed( 5, 0 );
#endif // USE_DIGIT_LED_DEBUG

    solve_layer_out( x_in, L_INPUT, F_SYGMOID );
#ifdef USE_DIGIT_LED_DEBUG
    Show5DigitLed( 5, 1 );
#endif // USE_DIGIT_LED_DEBUG
    solve_layer_out( y_hidden, L_HIDDEN, F_LINEAR );

#ifdef USE_DIGIT_LED_DEBUG
    Show5DigitLedWithDot( 4, 16 );
    Show5DigitLed( 5, 16 );
#endif // USE_DIGIT_LED_DEBUG
    return y;
    }
//---------------------------------------------------------------------------------------
#ifdef PAC
int mlp::set_default_weight()
    {
#ifdef DEBUG
    Print( "Setting default MLP description from %s....\n\r", mlp_filename );
#endif // DEBUG

    char far *stream = 0;
    stream = ( char far *) GetFilePositionByName( mlp_filename );

    if ( stream != NULL ) 
        {
        int tmp;
        sscanf( stream, "%d", &tmp );
        stream = to_next_line( stream );
        sscanf( stream, "%d", &tmp );
        stream = to_next_line( stream );
        sscanf( stream, "%d", &tmp );
        stream = to_next_line( stream );
        stream = to_next_line( stream );

        int i;
        int j;
        for ( i = 0; i < inputs_cnt; i++ )
            {
            for ( j = 0; j < hidden_cnt; j++ )
                {
                stream = to_next_word( stream );
                sscanf( stream, "%f", &w[ L_INPUT ][ i ][ j ] );                
                }    
            }

        stream = to_next_line( stream );
        stream = to_next_line( stream );
        for (  i = 0; i < hidden_cnt; i++ )
            {
            for ( j = 0; j < outputs_cnt; j++ )
                {
                stream = to_next_word( stream );
                sscanf( stream, "%f", &w[ L_HIDDEN ][ i ][ j ] );            
                }                
            }
        stream = to_next_line( stream );
        stream = to_next_line( stream );

        for ( j = 0; j < hidden_cnt; j++ )
            {
            stream = to_next_word( stream );
            sscanf( stream, "%f", &T[ L_INPUT ][ j ] );        
            }
        stream = to_next_line( stream );
        stream = to_next_line( stream );
        for ( j = 0; j < outputs_cnt; j++ )
            {
            stream = to_next_word( stream );
            sscanf( stream, "%f", &T[ L_HIDDEN ][ j ] );        
            }        
        }
    else
        {
#ifdef DEBUG
        Print( "Can't find file [%s]!\n\r", mlp_filename );
#endif // DEBUG
        return 1;
        }

    return 0;
    }
#endif // PAC
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
#ifdef PAC
MLP_sample::MLP_sample( int window_size, int variables_count, 
    int max_var_value ):window_size( window_size ),
    variables_count( variables_count ),
    max_var_value( max_var_value )
    {
    data = new float[ variables_count * window_size ];

    for ( int i = 0; i < variables_count * window_size ; i++ )
        {
        data[ i ] = 0;
        }
    }
//---------------------------------------------------------------------------------------
float* MLP_sample::get_sample_for_prognose()
    {
    return data;
    }
//---------------------------------------------------------------------------------------
int MLP_sample::add_new_data( int variable_number, float value )
    {
    if ( variable_number < 1 )
        {
#ifdef DEBUG
        Print( "Error MLP_sample::add_new_data(...) - variable_number[%d] < 1!\n",
            variable_number );
#endif // DEBUG
        return 1;
        }
    if ( variable_number > variables_count )
        {
#ifdef DEBUG
        Print( "Error MLP_sample::add_new_data(...) - variable_number[%d] > variables_count[%d]!\n",
            variable_number, variables_count );
#endif // DEBUG
        return 1;
        }

    int start_pos = ( variable_number - 1 ) * window_size;
    int end_pos = variable_number * window_size - 1;

    for ( int i = start_pos; i < end_pos; i++ )
        {
        data[ i ] = data[ i + 1 ];
        }

    data[ end_pos ] = value / max_var_value;
    //-Проверка на корректность новых данных.
    check_range( data[ end_pos ], -1, 1 );  

    return 0;
    }
//---------------------------------------------------------------------------------------
#ifdef DEBUG
void MLP_sample::print()
    {
    Print( "MLP_sample: window_size = %d, var count = %d, max_var_value = %d\n",
        window_size, variables_count, max_var_value );
    for ( int i = 0; i < variables_count; i++ )
        {
        for ( int j = 0; j < window_size; j++ )
            {
            Print( "%.3f ", data[ i * window_size + j ] );
            }
        Print( "\n" );
        }
    }
#endif // DEBUG
//---------------------------------------------------------------------------------------
int MLP_sample::get_max_var_value() const
    {
    return max_var_value;
    }
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
#endif // PAC


test_sample::test_sample( float x_start, float x_end, float dx, int window_size )
    {
    this->window_size = window_size + 1;

    int elements_cnt = ( int ) ( ( x_end - x_start ) / dx );
    float *f_y = new float[ 2 * elements_cnt ];

#ifdef _DEBUG
    printf( "f( x ) = sin( x ), ");
    printf( "range [ % .3f, % .3f ], dx = %.3f, total elements = %d\n", 
        x_start, x_end, elements_cnt );
    printf( "Function values:\n" );
#endif // _DEBUG

    float x_i = x_start;
    for ( int i = 0; i < elements_cnt; i++ )
        {                     
        f_y [ i ] = sin( x_i );

#ifdef _DEBUG        
        printf( "%2d: [ % 2.3f, % 2.3f ]\n", i, x_i, f_y[ i ] );
#endif // _DEBUG        
        x_i += dx;
        }
#ifdef _DEBUG        
    printf( "\n" );
#endif // _DEBUG     

    x_i = x_start;
    for ( int i = elements_cnt; i < 2 * elements_cnt; i++ )
        {                     
        f_y [ i ] = cos( x_i );

#ifdef _DEBUG        
        printf( "%2d: [ % 2.3f, % 2.3f ]\n", i, x_i, f_y[ i ] );
#endif // _DEBUG        
        x_i += dx;
        }
#ifdef _DEBUG        
    printf( "\n" );
#endif // _DEBUG


    samples_cnt = 2 * elements_cnt - 2 * window_size;
    x = new float*[ samples_cnt ];
    y = new float*[ samples_cnt ];

    //Формирование обучающих выборок.
#ifdef _DEBUG        
    printf( "Learning sample (total %d):\n", samples_cnt );
#endif // _DEBUG     
    float *tmp_f_y = f_y;

    for ( int i = 0; i < elements_cnt - window_size; i++ )
        {
        x[ i ] = new float[ window_size + 1 ];
#ifdef _DEBUG        
        printf( "%2d: [ ", i );
#endif // _DEBUG     
        x[ i ][ 0 ] = 0.1;
        printf( "% 2.3f ", x[ i ][ 0 ] );

        for ( int j = 1; j < window_size + 1; j++ )
            {
            x[ i ][ j ] = tmp_f_y[ j - 1 ];
#ifdef _DEBUG        
            printf( "% 2.3f ", x[ i ][ j ] );
#endif // _DEBUG     
            }

        y[ i ] = new float[ 1 ];
        y[ i ][ 0 ] = tmp_f_y[ window_size ];
#ifdef _DEBUG        
        printf( " ] -> % 2.3f \n", y[ i ][ 0 ] );
#endif // _DEBUG     
        tmp_f_y++;        
        }

    tmp_f_y++;
    tmp_f_y++;
    tmp_f_y++;
    tmp_f_y++;

#ifdef _DEBUG        
    printf( "\n" );
#endif // _DEBUG 

    for ( int i = elements_cnt - window_size; i < 2 * elements_cnt - 2 * window_size; i++ )
        {
        x[ i ] = new float[ window_size + 1 ];
#ifdef _DEBUG        
        printf( "%2d: [ ", i );
#endif // _DEBUG     
        x[ i ][ 0 ] = 0.9;
        printf( "% 2.3f ", x[ i ][ 0 ] );

        for ( int j = 1; j < window_size + 1; j++ )
            {
            x[ i ][ j ] = tmp_f_y[ j  - 1 ];
#ifdef _DEBUG        
            printf( "% 2.3f ", x[ i ][ j ] );
#endif // _DEBUG     
            }
        y[ i ] = new float[ 1 ];
        y[ i ][ 0 ] = tmp_f_y[ window_size ];
#ifdef _DEBUG        
        printf( " ] -> % 2.3f \n", y[ i ][ 0 ] );
#endif // _DEBUG     
        tmp_f_y++;        
        }

    delete[] f_y;
    }

function_sample_with_time::function_sample_with_time( float x_start, float x_end, float dx, int window_size )
    {
    this->window_size = window_size + 1;

    int elements_cnt = ( int ) ( ( x_end - x_start ) / dx );
    float *f_y = new float[ elements_cnt ];

#ifdef _DEBUG
    printf( "f( x ) = sin( x ), ");
    printf( "range [ % .3f, % .3f ], dx = %.3f, total elements = %d\n", 
        x_start, x_end, elements_cnt );
    printf( "Function values:\n" );
#endif // _DEBUG

    float x_i = x_start;
    for ( int i = 0; i < elements_cnt; i++ )
        {       
        //f_y [ i ] = 0.1f * cos( 0.1f * x_i ) + 0.01f * sin( 0.1f * x_i );
        //f_y [ i ] = 0.1f * cos( 0.1f * x_i );
        f_y [ i ] = sin( x_i );

#ifdef _DEBUG        
        printf( "%2d: [ % 2.3f, % 2.3f ]\n", i, x_i, f_y[ i ] );
#endif // _DEBUG        
        x_i += dx;
        }
#ifdef _DEBUG        
    printf( "\n" );
#endif // _DEBUG     

    samples_cnt = elements_cnt - window_size;
    x = new float*[ samples_cnt ];
    y = new float*[ samples_cnt ];

    //Формирование обучающих выборок.
#ifdef _DEBUG        
    printf( "Learning sample (total %d):\n", samples_cnt );
#endif // _DEBUG     
    float *tmp_f_y = f_y;
    int t = 0;
    for ( int i = 0; i < samples_cnt; i++ )
        {
        x[ i ] = new float[ window_size + 1 ];
#ifdef _DEBUG        
        printf( "%2d: [ ", i );
#endif // _DEBUG     

        x[ i ][ 0 ] = t++;
#ifdef _DEBUG        
        printf( "% 2.3f ", x[ i ][ 0 ] );
#endif // _DEBUG     

        for ( int j = 1; j < window_size + 1; j++ )
            {
            x[ i ][ j ] = tmp_f_y[ j - 1 ];
#ifdef _DEBUG        
            printf( "% 2.3f ", x[ i ][ j ] );
#endif // _DEBUG     
            }
        y[ i ] = new float[ 1 ];
        y[ i ][ 0 ] = tmp_f_y[ window_size ];
#ifdef _DEBUG        
        printf( " ] -> % 2.3f \n", y[ i ][ 0 ] );
#endif // _DEBUG     
        tmp_f_y++;        
        }

    delete[] f_y;
    }
