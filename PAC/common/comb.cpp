#include "comb.h"
#include <memory.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


//--------------------------------------------------------------------------------
comb::comb( char rows,
    char cols ):
    cmd( 0 ),
    rows_cnt( rows ),
    columns_cnt( cols ),
    rt_par_uint( run_time_params_u_int_4( 6 ) )
    {   
    if ( rows_cnt > 0 && columns_cnt > 0 )
        {               

        com_dev = new complex_device( 1, "PATH_COMB", 2, i_complex_device::COMPLEX_DEV );
        com_dev->sub_dev[ 0 ] = new complex_state( "CMD", 1, &cmd );

        com_dev->sub_dev[ 1 ] = &rt_par_uint;

        mMatrix = new comb_element*[ rows_cnt ];	    
        for ( char i = 0; i < rows_cnt; i++ )
            {
            mMatrix[ i ] = new comb_element[ columns_cnt ];  
            }               
        }
    }
//--------------------------------------------------------------------------------
void comb::init()
    {
    int i, j;
    pathCount = ( rows_cnt == 0 ) && ( columns_cnt == 0 ) ? 0 : MAX_PATH_COUNT;

    path_list = new comb_path[ pathCount ];  
    for ( i = 0; i < pathCount; i++ )
      {
      path_list[ i ].in = comb_point( ( char ) -1, ( char ) -1 );
      path_list[ i ].out = comb_point( ( char ) -1, ( char ) -1 );
      }  
      
    for ( i = 0; i < rows_cnt; i++ )
        {  
        for ( j = 0; j < columns_cnt; j++ )
            {
            switch ( mMatrix[ i ][ j ].type )
                {
                case COMB_TANK:
                case COMB_IN:
                case COMB_OUT:
                case COMB_MCA_IN:
                case COMB_MCA_OUT:
                case COMB_DRAINAGE:
                case COMB_WATER:
                case COMB_POST:
                    mMatrix[ i ][ j ].valve = 0;
                    break;

                case COMB_V_CLOSE:
                case COMB_V_OPEN:
                case COMB_V_DRAIN_CLOSE:
                case COMB_V_DRAIN_OPEN:
                case COMB_V_MIX_PROOF:
                    mMatrix[ i ][ j ].valve = V( mMatrix[ i ][ j ].id );
                    mMatrix[ i ][ j ].valve->set_state( 0 );
                    break;    
                }  
            mMatrix[ i ][ j ].status = comb_element::CES_NOT_USED;
            mMatrix[ i ][ j ].is_opened = 0;

            }   //  for ( j = 0; j < columns_cnt; j++ )
        }   //  for ( i = 0; i < rows_cnt; i++ )      
    }
//--------------------------------------------------------------------------------
//	Проверка на корректность( возможность открытия ) маршрута.
int comb::check_path( char x1, char y1, char x2, char y2 )
    {
    if ( rows_cnt > 0 )
        {
#ifdef DEBUG
        Print("\n\nStart CHECK ...\n\n");
#endif
        //Проверяем правильный диапазон параметров.
        if ( x1 > columns_cnt || y1 > rows_cnt || x2 > columns_cnt || y2 > rows_cnt )
            {		
#ifdef DEBUG
            Print( "comb::check_path(...) error - not permissible parameters!\n" );
            
            if ( x1 > columns_cnt ) 
                Print( "xin = %d > columns_cnt = %d\n", x1, columns_cnt );
            if ( y1 > rows_cnt ) 
                Print( "yin = %d > rows_cnt = %d\n", y1, rows_cnt );
            if ( x2 > columns_cnt ) 
                Print( "xout = %d > columns_cnt = %d\n", x2, columns_cnt );
            if ( y2 > rows_cnt ) 
                Print( "yout = %d > rows_cnt = %d\n", y2, rows_cnt );
                
            Print( "Press any key..." );
            get_char();
            Print( "\n" );
#endif  
            return GE_INCORRECT_PATH;
            }

        int i;
        comb_point in( x1, y1 );
        comb_point out( x2, y2 );

        //Проверяем, являются ли граничные элементы правильными.
        if ( !( 
            ( mMatrix[ in.y ][ in.x ].type >= COMB_TANK && 
              mMatrix[ in.y ][ in.x ].type <= COMB_POST ) && 
            ( mMatrix[ out.y ][ out.x ].type >= COMB_TANK && 
              mMatrix[ out.y ][ out.x ].type <= COMB_POST ) )  
          && !(( in.x == 0 ) && ( in.y == 0 ) && ( out.x == 0 ) && ( out.y == 0 ))
            ) 
            {
#ifdef DEBUG
            Print( "comb::check_path(...) error - not permissible parameters!\n" );
            if ( mMatrix[ in.y ][ in.x ].type < COMB_TANK ) 
                Print( "mMatrix y1[%d]x1[%d].type[%d] < COMB_TANK[%d]\n", 
                in.y, in.x, mMatrix[ in.y ][ in.x ].type, COMB_TANK );
            if ( mMatrix[ in.y ][ in.x ].type > COMB_POST ) 
                Print( "mMatrix y1[%d]x1[%d].type[%d] > COMB_POST[%d]\n", 
                in.y, in.x, mMatrix[ in.y ][ in.x ].type, COMB_POST );

            if ( mMatrix[ out.y ][ out.x ].type < COMB_TANK ) 
                Print( "mMatrix y2[%d]x2[%d].type[%d] < COMB_TANK[%d]\n", 
                out.y, out.x, mMatrix[ out.y ][ out.x ].type, COMB_TANK );
            if ( mMatrix[ out.y ][ out.x ].type > COMB_POST ) 
                Print( "mMatrix y2[%d]x2[%d].type[%d] > COMB_POST[%d]\n", 
                out.y, out.x, mMatrix[ out.y ][ out.x ].type, COMB_POST );
            Print( "Press any key..." );
            get_char();
            Print( "\n" );
#endif
            return GE_INCORRECT_PATH; 
            }

#ifdef DEBUG
        Print( "comb::check_path( y1[ %d ]x1[ %d ].status = %d, y2[ %d ]x2[ %d ].status = %d\n",
            in.y, in.x, mMatrix[ in.y ][ in.x ].status, 
            out.y, out.x, mMatrix[ out.y ][ out.x ].status );
#endif // DEBUG

        if ( get_path_points( comb_point( x1, y1 ), comb_point( x2, y2 ) ) )
            {
#ifdef DEBUG
            Print("\n       Incorrect PATH ...\n");
#endif                 
            return GE_INCORRECT_PATH;	 
            }

        //Проверка на наличие уже включенного режима.
        for ( i = 0; i < pathCount; i++ )
            {
            if (( path_list[ i ].in == in && path_list[ i ].out == out  )
             && !( in == out )) // Защита от нулевых координат - т.е. не заданных маршрутов
                {   
#ifdef DEBUG
            Print("\n    ALREADY USED PATH - %d ... \n", i );
                                                    
            Print("    in = ( %d, %d ), out = ( %d, %d ) \n", in.x, in.y, out.x, out.y );
                                                    
            Print("    PL_in = ( %d, %d ), PL_out = ( %d, %d ),  \n", 
                   path_list[ i ].in.x, 
                   path_list[ i ].in.y, 
                   path_list[ i ].out.x, 
                   path_list[ i ].out.y );                                                                                                        
#endif                                                 
                return GE_PATH_ALREADY_USED;
                }
            }

        //Проверка отсечных клапанов при угловом маршруте.
        if ( current_path.path_type == comb_path::PT_G_TYPE )
            {
            int res = find_block_element( current_path.turn.x, current_path.turn.y, 
                current_path.in.x, current_path.in.y, 0 );
            res += find_block_element( current_path.turn.x, current_path.turn.y, 
                current_path.out.x, current_path.out.y, 0 );

            res += find_block_element( current_path.in_.x, current_path.in_.y, 
                current_path.turn.x, current_path.turn.y, 1 );
            res += find_block_element( current_path.out_.x, current_path.out_.y, 
                current_path.turn.x, current_path.turn.y, 1 );
            if ( res )
             {
#ifdef DEBUG
            Print("\n       CHECK -> BLOCK MODE ...\n");
#endif 
             return GE_BLOCK_MODE;
             }
            }

        //Проверяем, включен ли кокой-либо режим, запрещающий все другие режимы.
        if ( mMatrix[ in.y ][ in.x ].status == comb_element::CES_MONOPOLY_USED || 
            mMatrix[ out.y ][ out.x ].status == comb_element::CES_MONOPOLY_USED ||
            //Для режимов мойки проверяем на наличие других режимов.
            mMatrix[ y1 ][ x1 ].type == COMB_MCA_IN ||
            mMatrix[ y2 ][ x2 ].type == COMB_MCA_OUT )                    
            {                
            //Находим блокирующий режим.
            for ( i = 0; i < pathCount; i++ )
                {
                if ( path_list[ i ].in == in || path_list[ i ].out == out  )
                    {   
                    //Устанавливаем соответствующие параметры для сообщения об ошибке.
                    in = path_list[ i ].in; 
                    out = path_list[ i ].out;     
                    rt_par_uint[ CP_ERRORS ] = GE_BLOCK_MODE;
                    rt_par_uint[ CP_CONFLICTED_OBJECT1_TYPE ] = mMatrix[ in.y ][ in.x ].type;
                    rt_par_uint[ CP_CONFLICTED_OBJECT1_N ] = mMatrix[ in.y ][ in.x ].id;
                    rt_par_uint[ CP_CONFLICTED_OBJECT2_TYPE ] = mMatrix[ out.y ][ out.x ].type;
                    rt_par_uint[ CP_CONFLICTED_OBJECT2_N ] = mMatrix[ out.y ][ out.x ].id;
                    return GE_BLOCK_MODE;
                    }
                }
            if ( mMatrix[ in.y ][ in.x ].status == comb_element::CES_MONOPOLY_USED || 
                mMatrix[ out.y ][ out.x ].status == comb_element::CES_MONOPOLY_USED )
                {
#ifdef DEBUG
                Print( "comb::check_path(...) error!\n" );
                Print( "Could't find blocking mode!\n" );
                Print( "Press any key..." );
                get_char();
                Print( "\n" );
#endif // DEBUG
                return GE_BLOCK_MODE;
                }
            } // if ( mMatrix[ in.y ][ in.x ].status == comb_element::CES_MONOPOLY_USED ||  
        } // if ( rows_cnt > 0 )

    return GE_OK;     
    } 
//--------------------------------------------------------------------------------   
void comb::mark_path( comb_path *marked_path, char is_unmark )
    {
    //Работа с сквозным маршрутом.
    if ( marked_path->path_type == comb_path::PT_STRAIGHT )
        {
        mark_path_elements( marked_path->in.x, marked_path->in.y,
            marked_path->out.x, marked_path->out.y, 1, is_unmark );
        return;
        }

    //Включаем клапан на пересечении, если надо. 
    if ( mMatrix[ marked_path->turn.y ][ marked_path->turn.x ].type >= COMB_V_CLOSE &&
        mMatrix[ marked_path->turn.y ][ marked_path->turn.x ].type <= COMB_V_MIX_PROOF )
        {
        mMatrix[ marked_path->turn.y ][ marked_path->turn.x ].status = comb_element::CES_USED;
        mMatrix[ marked_path->turn.y ][ marked_path->turn.x ].is_opened = 1;   
        if ( is_unmark )
            {    
            mMatrix[ marked_path->turn.y ][ marked_path->turn.x ].status = 
                comb_element::CES_NOT_USED;
            mMatrix[ marked_path->turn.y ][ marked_path->turn.x ].is_opened = 0;       
            mMatrix[ marked_path->turn.y ][ marked_path->turn.x ].valve->off();
            }
        }  

//#ifdef DEBUG    
//    Print( "mark_path +++++ CLIP( %d, %d ) \n", marked_path->turn.y, marked_path->turn.x );
//#endif  //   DEBUG 

    Clip( marked_path->turn.y, marked_path->turn.x );

    mark_path_elements( marked_path->in.x, marked_path->in.y,
        marked_path->turn.x, marked_path->turn.y, 1, is_unmark );
    mark_path_elements( marked_path->out.x, marked_path->out.y,
        marked_path->turn.x, marked_path->turn.y, 1, is_unmark );
        
/*
    mark_path_elements( marked_path->in_.x, marked_path->in_.y,
        marked_path->turn.x, marked_path->turn.y, 0, is_unmark );
    mark_path_elements( marked_path->out_.x, marked_path->out_.y,
        marked_path->turn.x, marked_path->turn.y, 0, is_unmark );
*/        
    }
//--------------------------------------------------------------------------------    
//	Обновление гребёнки;
void comb::refresh()
    {
    //Сбрасываем все помеченные клапана.
    for ( int j = 1; j < rows_cnt - 1; j++ )       //Кроме граничных элементов.
        {
        for ( int k = 1; k < columns_cnt - 1; k++ )
            {
            mMatrix[ j ][ k ].status = comb_element::CES_NOT_USED;
            mMatrix[ j ][ k ].is_opened = 0;
            }
        }

    //-Из списка маршрутов последовательно выбираем маршруты. 
    for ( int i = 0; i < pathCount; i++ )
        {
        if ( path_list[ i ].in.x != ( char ) -1 )    //Нашли маршрут.
            {	
            //-Проверяем маршрут на автоотключение.
            if ( path_list[ i ].finish_time )
                {         
                if ( path_list[ i ].start_time + path_list[ i ].finish_time < get_millisec() )
                    {               
                    path_list[ i ].object->set_mode(
                        path_list[ i ].object_mode_n, 0 );
                    close_path( path_list[ i ].in.x, path_list[ i ].in.y,
                        path_list[ i ].out.x, path_list[ i ].out.y );
                    continue;
                    }
                }   //  if ( path_list[ i ].finish_time )
            //-
            //Помечаем используемые клапана.
            mark_path( &path_list[ i ] );            
            }
        }

    //-Включаем необходимые клапана.
    for ( int j = 1; j < rows_cnt - 1; j++ )
        {
        for ( int k = 1; k < columns_cnt - 1; k++ )
            {
            if ( mMatrix[ j ][ k ].type < COMB_V_CLOSE ||
                mMatrix[ j ][ k ].type > COMB_V_MIX_PROOF )
                {
                continue;
                }

            if ( comb_element::CES_USED == mMatrix[ j ][ k ].status )
                {                
                if ( 0 == mMatrix[ j ][ k ].is_opened )
                    {
                    mMatrix[ j ][ k ].valve->off();
                    }
                else
                    {
                    mMatrix[ j ][ k ].valve->on();
                    }
                } 
            }
        }
    }
//--------------------------------------------------------------------------------   
//	Производит открытие или закрытие клапанов маршрута.
int comb::open_path( char x1, char y1, char x2, char y2,
                    tech_object *object, int object_mode_n )
    {
     
    if ( rows_cnt > 0 )
        {  
        int i;      


        get_path_points( comb_point( x1, y1 ), comb_point( x2, y2 ) );

        current_path.object = object;
        current_path.object_mode_n = object_mode_n;

        //Находим связанные режимы.
        for ( i = 0; i < pathCount; i++ )
            {  
            //  Работа с постами          
            if ( path_list[ i ].in == comb_point( x1, y1 ) )
                {
                //Для поста разрешен одновременно только один режим. 
                if ( mMatrix[ y1 ][ x1 ].type == COMB_POST )
                    {                    
                    path_list[ i ].start_time = get_millisec();
                    path_list[ i ].finish_time = rt_par_uint[ CP_SWICTH_TIME ];
                    }
                }
                
            //  Работа с линиями
            if ( path_list[ i ].out == comb_point( x2, y2 ) )
                {
                //Для линии разрешен одновременно только один режим. 
                if ( mMatrix[ y2 ][ x2 ].type == COMB_OUT )
                    {                    
                    path_list[ i ].start_time = get_millisec();
                    path_list[ i ].finish_time = rt_par_uint[ CP_SWICTH_TIME ];
                    }
                }                
                
            }

        //	Находим свободную строку в таблице маршрутов и записываем туда текущий маршрут.
        i = 0;
        while ( i < pathCount && path_list[ i ].in.x != ( char ) -1 )
            { 
            i++;
            }   

        if ( i >= pathCount )	
            {
#ifdef DEBUG            
            Print( "comb::open_close_path(...) - no free line in path_list!\n" );
#endif  
            return GE_PATH_TABLE_IS_FULL;	 //Нет свободной строки для хранения маршрута.
            }

        path_list[ i ] =  current_path; 

        //-Для режимов мойки помечаем, что граничный элемент монопольно занят.
        if ( mMatrix[ y1 ][ x1 ].type == COMB_MCA_IN )
            {                    
            mMatrix[ y2 ][ x2 ].status = comb_element::CES_MONOPOLY_USED;
            }
        if ( mMatrix[ y2 ][ x2 ].type == COMB_MCA_OUT )
            {                    
            mMatrix[ y1 ][ x1 ].status = comb_element::CES_MONOPOLY_USED;
            }
#ifdef DEBUG
        mark_path( &current_path );
        Print( "----- &current_path : %d, %d  \n ", current_path.turn.y, current_path.turn.x );
//        print( 1 );
//        get_char();
#endif // DEBUG

        refresh();
        }//  if ( rows_cnt > 0    

    return GE_OK;
    }
//--------------------------------------------------------------------------------  
int comb::close_path( char x1, char y1, char x2, char y2 )
    {
    if ( rows_cnt > 0 && columns_cnt > 0 )
        {
        int i_pl = 0;
        //-Находим нужный маршрут в таблице маршрутов.        
        while ( i_pl < pathCount && 
            !( path_list[ i_pl ].in == comb_point( x1, y1 ) &&
            path_list[ i_pl ].out == comb_point( x2, y2 ) ) )
            {        
            i_pl++;
            }
        if ( i_pl >= pathCount ) 
            {
#ifdef DEBUG
        Print( "GE_IS_NOT_OPEN_PATH (y,x) = (%d, %d) -> (%d, %d)  \n ", y1, x1, y2, x2 );
        print( 1 );
#endif // DEBUG                        
            return GE_IS_NOT_OPEN_PATH; 
            }

        //Помечаем как свободный маршрут.
        path_list[ i_pl ].in = comb_point( ( char ) -1, ( char ) -1 );
        path_list[ i_pl ].out = comb_point( ( char ) -1, ( char ) -1 );

        //Помечаем, что граничный элемент уже свободен.
        mMatrix[ y1 ][ x1 ].status = comb_element::CES_NOT_USED;  
        mMatrix[ y2 ][ x2 ].status = comb_element::CES_NOT_USED;   

        get_path_points( comb_point ( x1, y1 ), comb_point ( x2, y2 ) );
        mark_path( &current_path, 1 );        //Убираем отметку об использовании.
        refresh();        
        }

    return GE_OK;
    }
//--------------------------------------------------------------------------------  
//  Вывод на экран структуры объекта comb и его содержимого.
void comb::print( int type )
    {    
#ifdef DEBUG
    int i, j;

    Print( "Comb.\n" );
    Print( "     [ Valve status ]\n" );
    for ( i = 0; i < rows_cnt; i++ )
        {  
        for ( int j = 0; j < columns_cnt; j++ )
            {
            Print( "%d ", mMatrix[ i ][ j ].status );                   
            }
        Print( "\n" );  
        } 
        
    Print( "     pathCount [%d]\n", pathCount );
    for ( i = 0; i < pathCount; i++ ) 
        {          
        Print( "path_list[ %d ] : in( %d, %d )   out( %d, %d ) \n", 
            i,
            path_list[ i ].in.x, path_list[ i ].in.y, 
            path_list[ i ].out.x, path_list[ i ].out.y );            
        }
    Print( "\n" );
            
    if ( 1 == type ) return;                 
        
    Print( "     [ type ]\n" );
    for ( i = 0; i < rows_cnt; i++ )
        {
        for ( j = 0; j < columns_cnt; j++ )
            {				
            Print( "%2d  ", mMatrix[ i ][ j ].type );  	
            }
        Print( "\n" );
        }
        
    Print( "     [ valve ID ]\n" );
    for ( i = 0; i < rows_cnt; i++ )
        {
        for ( j = 0; j < columns_cnt; j++ )
            {
            Print( "%u	", mMatrix[ i ][ j ].id ); 	
            }
        Print( "\n" );
        }
        
#else  // DEBUG
    type++;     //Чтобы не было Warning'а.
#endif // DEBUG
    }
//--------------------------------------------------------------------------------  

//  Проверка элементов ответвления
void comb::Clipping( char y, char x, char reg, int increment, 
                     int tube1, int tube2, int tube3, int tube4, int tube5 )
    {
    if ( reg == 'y' )
        {
        y += increment;
        }
    else
        {
        x += increment;
        }

//#ifdef DEBUG 
//        Print( "    switch ??? ( mMatrix[ %d ][ %d ].type ) = %d \n ", y, x, mMatrix[ y ][ x ].type );
//#endif // DEBUG 

    while ( ( mMatrix[ y ][ x ].type != COMB_NULL )
        &&( mMatrix[ y ][ x ].type < 21 )
        &&( mMatrix[ y ][ x ].type != tube1 )
        &&( mMatrix[ y ][ x ].type != tube2 )
        &&( mMatrix[ y ][ x ].type != tube3 )
        &&( mMatrix[ y ][ x ].type != tube4 )
        &&( mMatrix[ y ][ x ].type != tube5 )
        &&(( y < rows_cnt - 1 )||( y < columns_cnt - 1 ))
        &&( y > 0 )
        &&(( x < rows_cnt - 1 )||( x < columns_cnt - 1 ))
        &&( x > 0 ) )
        {
        
//#ifdef DEBUG 
//        Print( "    switch OK ( mMatrix[ %d ][ %d ].type ) = %d \n ", y, x, mMatrix[ y ][ x ].type );
//#endif // DEBUG        
        
        
        switch ( mMatrix[ y ][ x ].type )
            {
            case COMB_V_MIX_PROOF:
//                if ( mMatrix[ y ][ x ].status == comb_element::CES_NOT_USED ) // 2
//                    {
                    
                mMatrix[ y ][ x ].status = comb_element::CES_USED;    //  0                    
                //mMatrix[ y ][ x ].is_opened = 0;
                
//#ifdef DEBUG                    
//                    Print( "%d,  ", mMatrix[ y ][ x ].id );
//#endif // DEBUG                    
//                    }			
                break;

            //  Рассматриваем ответвления от маршрута
            case COMB_T_VERTIC_RIGHT:
//#ifdef DEBUG                 
//                        Print( "----- Clip( %d, %d ); \n ", y, x );
//#endif // DEBUG             
                Clip( y, x );                       
                break;

            case COMB_T_VERTIC_LEFT:
//#ifdef DEBUG                 
//                        Print( "----- Clip( %d, %d ); \n ", y, x );
//#endif // DEBUG                
                Clip( y, x );               
                break;

            case COMB_T_HORIZ_UP:
//#ifdef DEBUG                 
//                        Print( "----- Clip( %d, %d ); \n ", y, x );
//#endif // DEBUG              
                Clip( y, x );                 
                break;

            case COMB_T_HORIZ_DOWN:    
//#ifdef DEBUG                 
//                        Print( "----- Clip( %d, %d ); \n ", y, x );
//#endif // DEBUG                      
                Clip( y, x );                 
                break;
            }
   
        //  Переходим к новому элементу
        if ( reg == 'y' )
            {
            y += increment;
            }
        else
            {
            x += increment;
            }
        }

    switch ( mMatrix[ y ][ x ].type )
        {
       case COMB_V_CLOSE:
       case COMB_V_OPEN:
                mMatrix[ y ][ x ].status = comb_element::CES_USED;   //  0
                mMatrix[ y ][ x ].is_opened =  mMatrix[ y ][ x ].type - 11; //  для уменьшения программы    
                
//#ifdef DEBUG                    
//                    Print( "%d,  ", mMatrix[ y ][ x ].id );
//#endif // DEBUG                 
                
            break;         
        }  
    }
//--------------------------------------------------------------------------------    

//  Анализируем ответвления от включаемого маршрута
void comb::Clip( char baseY, char baseX )
    { 
        if ( baseY == 0 )	//	Прямой маршрут по Y
            {
            Clipping( baseY, baseX, 'y', 1, COMB_T_HORIZ, COMB_T_HORIZ_UP, COMB_T_HORIZ_DOWN, COMB_NULL, COMB_NULL );
            }
        else               //	Угловой маршрут; проверка по Y
            {
            Clipping( baseY, baseX, 'y', -1, COMB_T_HORIZ, COMB_T_HORIZ_UP, COMB_T_HORIZ_DOWN, COMB_V_CLOSE, COMB_V_OPEN );
            Clipping( baseY, baseX, 'y', 1, COMB_T_HORIZ, COMB_T_HORIZ_UP, COMB_T_HORIZ_DOWN, COMB_V_CLOSE, COMB_V_OPEN );
            }

        if ( baseX == 0 )	//	Прямой маршрут по X
            {
            Clipping( baseY, baseX, 'x', 1, COMB_T_VERTICAL, COMB_T_VERTIC_RIGHT, COMB_T_VERTIC_LEFT, COMB_NULL, COMB_NULL );
            }
        else               //	Угловой маршрут; проверка по X
            {
            Clipping( baseY, baseX, 'x', -1, COMB_T_VERTICAL, COMB_T_VERTIC_RIGHT, COMB_T_VERTIC_LEFT, COMB_V_CLOSE, COMB_V_OPEN );
            Clipping( baseY, baseX, 'x', 1, COMB_T_VERTICAL, COMB_T_VERTIC_RIGHT, COMB_T_VERTIC_LEFT, COMB_V_CLOSE, COMB_V_OPEN );
            }
    }
//--------------------------------------------------------------------------------    

int comb::mark_path_elements( char x1, char y1, char x2, char y2, 
                             char is_open, char is_unmark /* = 0 */ )
    {
    char mode;
    char x;
    char y;
    int length;
    if ( x1 == x2 )
        {
        mode = 'y';
        x = x1;
        y = y1 < y2 ? y1 : y2;
        if ( y1 > y2 ) length = y1 - y2;
        else length = y2 - y1;
        }
    else
        {
        if ( y1 == y2 )
            {
            mode = 'x';
            y = y1;
            x = x1 < x2 ? x1 : x2;
            if ( x1 > x2 ) length = x1 - x2;
            else length = x2 - x1;
            }
        else
            {
#ifdef DEBUG
            Print( "mark_path_elements(...) error - must be x1[%d]==x2[%d] or y1[%d]==y2[%d]! \n",
                x1, x2, y1, y2 );   
            get_char();
#endif // DEBUG
            return 1;
            }
        }

    while ( length >= 0 )
        {
        switch ( mMatrix[ y ][ x ].type )
            {
            case COMB_V_MIX_PROOF:
                mMatrix[ y ][ x ].status = comb_element::CES_USED;
                if ( is_unmark )
                    {
                    mMatrix[ y ][ x ].status = comb_element::CES_NOT_USED;
                    }
                break;

            case COMB_V_CLOSE:
                mMatrix[ y ][ x ].status = comb_element::CES_USED;
                mMatrix[ y ][ x ].is_opened = 0;
                if ( is_open )
                   {
                    mMatrix[ y ][ x ].is_opened = 1;
                   }    

                if ( is_unmark )
                    {
                    mMatrix[ y ][ x ].status = comb_element::CES_NOT_USED;
                    mMatrix[ y ][ x ].is_opened = 0;
                    mMatrix[ y ][ x ].valve->off();
                    }
                break;
                
			case COMB_V_OPEN:
				mMatrix[ y ][ x ].status = comb_element::CES_USED;
				mMatrix[ y ][ x ].is_opened = 0;
				if ( is_open )
					{
					mMatrix[ y ][ x ].is_opened = 0;
					}    

				if ( is_unmark )
					{
					mMatrix[ y ][ x ].status = comb_element::CES_NOT_USED;
					mMatrix[ y ][ x ].is_opened = 0;
					mMatrix[ y ][ x ].valve->off();
					}
				break;                
            }

        if ( 'y' == mode ) y++;            
        else x++;

        length--;
        }

    return 0;
    }
//--------------------------------------------------------------------------------
int comb::get_path_points( comb_point in, comb_point out )
    {        
    current_path.in = in;
    current_path.out = out;

//#ifdef DEBUG
//    Print( "get_path_points : in[ %d; %d ]  out[ %d; %d ] \n",
//        current_path.in.x, current_path.in.y,
//        current_path.out.x, current_path.out.y );
//#endif // DEBUG

    if ( in.x == out.x || in.y == out.y )
        {
        current_path.path_type = comb_path::PT_STRAIGHT;
        return 0;
        }
    else
        {
        current_path.path_type = comb_path::PT_G_TYPE;
        }

    //Находим координаты пересечения при угловом переходе.
    if ( in.x == 0 || in.x == columns_cnt - 1 )
        {
        current_path.turn.x = out.x;
        current_path.turn.y = in.y;
        }
    if ( in.y == 0 || in.y == rows_cnt - 1 )
        {
        current_path.turn.x = in.x;
        current_path.turn.y = out.y;
        }
    if ( out.x == 0 || out.x == columns_cnt - 1 )
        {
        current_path.turn.x = in.x;
        current_path.turn.y = out.y;
        }
    if ( out.y == 0 || out.y == rows_cnt - 1 )
        {
        current_path.turn.x = out.x;
        current_path.turn.y = in.y;
        }

    char direction_x;
    char direction_y;
    //Находим остальные границы маршрута.
    if ( in.y == 0 || out.y == 0 )   //Маршрут примыкает к верхней стороне.
        {
        direction_y = +1;
        }
    else direction_y = ( char ) -1;
    if ( in.x == 0 || out.x == 0 )   //Маршрут примыкает к левой стороне.
        {
        direction_x = +1;
        }
    else direction_x = ( char ) -1;

    char x = current_path.turn.x;
    while ( mMatrix[ current_path.turn.y ][ x ].type != COMB_T_VERTICAL &&
        mMatrix[ current_path.turn.y ][ x ].type != COMB_T_VERTIC_RIGHT &&
        mMatrix[ current_path.turn.y ][ x ].type != COMB_T_VERTIC_LEFT &&
        mMatrix[ current_path.turn.y ][ x ].type != COMB_NULL &&
        mMatrix[ current_path.turn.y ][ x ].type != COMB_V_CLOSE &&
        x > 0 && x < columns_cnt ) 
        {
        x += direction_x;
        }
    if ( in.y == current_path.turn.y )
        {
        current_path.in_.x = x;
        current_path.in_.y = current_path.turn.y;
        }
    else
        {
        current_path.out_.x = x;
        current_path.out_.y = current_path.turn.y;
        }
    char y = current_path.turn.y;
    while ( mMatrix[ y ][ current_path.turn.x ].type != COMB_T_HORIZ &&
        mMatrix[ y ][ current_path.turn.x ].type != COMB_T_HORIZ_DOWN &&
        mMatrix[ y ][ current_path.turn.x ].type != COMB_T_HORIZ_UP &&
        mMatrix[ y ][ current_path.turn.x ].type != COMB_NULL &&
        mMatrix[ y ][ current_path.turn.x ].type != COMB_V_CLOSE &&
        y > 0 && y < rows_cnt ) 
        {
        y += direction_y;
        }
    if ( in.x == current_path.turn.x )
        {
        current_path.in_.x = current_path.turn.x;
        current_path.in_.y = y;
        }
    else
        {
        current_path.out_.x = current_path.turn.x;
        current_path.out_.y = y;
        }
#ifdef DEBUG
    Print( "Make path : in[ %d; %d ] in_[ %d; %d ] turn[ %d; %d ] out[ %d; %d ] out_[ %d; %d ]\n",
        current_path.in.x, current_path.in.y, current_path.in_.x, current_path.in_.y,
        current_path.turn.x, current_path.turn.y,
        current_path.out.x, current_path.out.y, current_path.out_.x, current_path.out_.y );
#endif // DEBUG
    return 0;
    }
//--------------------------------------------------------------------------------
int comb::find_block_element( char x1, char y1, char x2, char y2, char is_open )
    {
//#ifdef DEBUG
//    Print( "comb::find_block_element\n" );
//#endif // DEBUG

    //-Находим блокирующий режим.
    int res = 0;
    comb_point blocking_point;

    char mode;
    char x;
    char y;
    int length;
    if ( x1 == x2 )
        {
        blocking_point.x = x1;
        if ( y1 > y2 ) blocking_point.y = rows_cnt - 1;
        else blocking_point.y = 0;

        mode = 'y';
        x = x1;
        y = y1 < y2 ? y1 : y2;
        if ( y1 > y2 ) 
            {
            length = y1 - y2;
            }
        else length = y2 - y1;
        }
    else
        {
        if ( y1 == y2 )
            {
            blocking_point.y = y1;
            if ( x1 > x2 ) blocking_point.x = columns_cnt - 1;
            else blocking_point.x = 0;

            mode = 'x';
            y = y1;
            x = x1 < x2 ? x1 : x2;
            if ( x1 > x2 ) length = x1 - x2;
            else length = x2 - x1;
            }
        else
            {
#ifdef DEBUG
            Print( "find_block_element(...) error - must be x1[%d]==x2[%d] or y1[%d]==y2[%d]!",
                x1, x2, y1, y2 );   
            get_char();
#endif // DEBUG
            return 0;
            }
        }
#ifdef DEBUG
    Print( "blocking_point x[%d]y[%d]\n", blocking_point.x, blocking_point.y );
#endif // DEBUG


#ifdef DEBUG
    Print( " is_open = %d\n", is_open );
#endif // DEBUG

    while ( length >= 0 )
        {
        if ( COMB_V_CLOSE == mMatrix[ y ][ x ].type && 
            comb_element::CES_USED == mMatrix[ y ][ x ].status )
            {
            if ( 1 == is_open ) //Ищем открытые клапана.
                {
                if ( mMatrix[ y ][ x ].is_opened == 1 )
                    {
                    res = 1;
#ifdef DEBUG
                    Print( " Is block mode 1\n" );
#endif // DEBUG
                    break;
                    }
                }
            else
                {
                if ( mMatrix[ y ][ x ].is_opened == 0 )
                    {
                    res = 1;
#ifdef DEBUG
                    Print( " Is block mode 2\n" );
#endif // DEBUG
                    break;
                    }
                }
            }

        if ( 'y' == mode ) y++;            
        else x++;

        length--;

//#ifdef DEBUG
//        Print( " x%d y%d  V[%d] status[%d] is_opened[%d]\n", 
//            x, y, mMatrix[ y ][ x ].type, mMatrix[ y ][ x ].status, mMatrix[ y ][ x ].is_opened );
//#endif // DEBUG
        }

    if ( res )
        {
        for ( int i = 0; i < pathCount; i++ )
            {
            if ( path_list[ i ].in == blocking_point || 
                path_list[ i ].out == blocking_point )
                { 
                //Устанавливаем соответствующие параметры для сообщения об ошибке.
                //comb_point in = path_list[ i ].in; 
                //comb_point out = path_list[ i ].out;     
                //par->setParamM( CP_ERRORS, GE_BLOCK_MODE );
                //par->setParamM( CP_CONFLICTED_OBJECT1_TYPE, mMatrix[ in.y ][ in.x ].type );
                //par->setParamM( CP_CONFLICTED_OBJECT1_N, mMatrix[ in.y ][ in.x ].id );
                //par->setParamM( CP_CONFLICTED_OBJECT2_TYPE, mMatrix[ out.y ][ out.x ].type );
                //par->setParamM( CP_CONFLICTED_OBJECT2_N, mMatrix[ out.y ][ out.x ].id );
                
                //Данные о блокирующем режиме, передаваемые в параметры гребенки.
                rt_par_uint[ CP_CONFLICTED_OBJECT1_TYPE ] = mMatrix[ blocking_point.y ][ blocking_point.x ].type;
                rt_par_uint[ CP_CONFLICTED_OBJECT1_N ] = mMatrix[ blocking_point.y ][ blocking_point.x ].id;
                return GE_BLOCK_MODE;
                }
            }
        }

    return 0;
    }
