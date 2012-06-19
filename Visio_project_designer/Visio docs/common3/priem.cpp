#include <mem.h>
#include <string.h>

#include "priem.h"

#ifdef USE_SIMPLE_DEV_ERRORS
#include "snprintf.h"
#endif // USE_SIMPLE_DEV_ERRORS

extern unsigned long    MyGetMS();

extern TMyComb      **g_combs;
extern TTank        **g_tanks;
extern TVirtualDO   *g_tvirt;

#ifndef USE_NO_COMB
extern comb         *g_greb;
#endif // USE_NO_COMB

int TTank::TankCnt = 0;				
int TMyComb::CombCnt = 0;

int	UseatOn;
int	LseatOn;

//-----------------------------------------------------------------------------

int IsAnyMode( int mode, int lastTankN ) 
    {    
    int res = -1;
    int lastTankN1;
    lastTankN1 = TTank::TankCnt < lastTankN ? TTank::TankCnt : lastTankN; 

    for ( int i = 0; i < lastTankN1; i++ ) 
        {
        if ( g_tanks[ i ]->GetMode( mode ) ) return i;
        }
    return res;
    }
//-----------------------------------------------------------------------------
int IsAnyModeC( int mode1, int mode2 ) 
    {    
    int res = -1;
    for ( int i = 0; i< TTank::TankCnt; i++ ) 
        {
        if ( g_tanks[ i ]->GetMode( mode1 ) &&
            g_tanks[ i ]->GetMode( mode2 ) ) return i;
        }
    return res;
    }
//-----------------------------------------------------------------------------

int IsAnyModeT( int mode1, char TankType ) 
    {    
    int res = -1;
    for ( int i = 0; i< TTank::TankCnt; i++ ) 
        {        
        if ( ( g_tanks[ i ]->PodTip == TankType ) &&
             ( g_tanks[ i ]->GetMode( mode1 ) ) ) return i;
        }
    return res;
    }
//-----------------------------------------------------------------------------

TVDEV *DEV( dev_elem dev )
	{
	if ( dev.no != 0 )
		{
		switch ( dev.type )
			{
		case dtV:
		case dtV2DO:
		case dtV1DO1DI:
		case dtV1DO2DI:
		case dtV2DO2DI:
		case dtMix:
		case dtV1DO3DI: 
		case dtV1DO2DI_S: //2 FB на одно из состояний.
		case dtASMix:     //Клапан микспруф с AS интерфейсом.
			return V( dev.no );
			//		break; 

		case dtN1DO1DI:
			return N( dev.no );
			//		break; 

		case dtM1DO1DI:
			return M( dev.no );
			//		break; 

		case dtLS: 
		case dtLS_ex:     //Настраиваемый предельный уровень. 
			return LS( dev.no );
			//		break; 

		case dtTE:   
			return TE( dev.no );
			//		break; 

		case dtFE:  
			return FE( dev.no );
			//		break; 

		case dtFS:  
			return FS( dev.no );
			//		break; 

		case dtCTR:  
			return CTR( dev.no );
			//		break; 

		case dtAO:   
			return AO( dev.no );
			//		break; 

		case dtLE1:    
		case dtLE2:    
		case dtLE3:    
		case dtLE4:  
			return LE( dev.no );
			//		break; 

		case dtOS:    
			return FB( dev.no );
			//		break; 	 

		case dtUpr:   
			return UPR( dev.no );
			//		break; 

		case dtQE:     
			return QE( dev.no );
			//		break; 

		case dtAI:
			return AI( dev.no );
			//		break; 
			}
		}

	return g_tvirt;
	}

//-----------------------------------------------------------------------------

int Check_Block_Mode( OBJ_TYPE obj, int nom, int mode, int err_par )
	{
	int res;

	switch ( obj )
		{
	case COMB: 
		if ( g_combs[ nom ]->GetMode( mode ) )
			{                              
			g_combs[ 0 ]->par->setParamM( err_par, mode + 1 );
			return 1;
			}
		break;

	case TANK: 
		//	Определяем по номеру танка его порядковый номер
		for ( int i = 0; i < TTank::TankCnt ; i ++ )
			{
			if ( nom == g_tanks[ i ]->no )
				{
				res = i;
				break;
				}
			}

		if ( g_tanks[ res ]->GetMode( mode ) ) 
			{               
			g_combs[ g_tanks[ res ]->comb_n ]->par->setParamM( 
				err_par, 1000 + nom * 100 + mode );
			return 1;
			}
		break; 

	case TANK_TYPE:
		if ( (res = IsAnyModeT( mode, nom )) >= 0 ) 
			{               
			g_combs[ g_tanks[ res ]->comb_n ]->par->setParamM( 
				err_par, 1000 + g_tanks[ res ]->no * 100 + mode );
			return 1;
			}
		break;	   

	case TANK_ALL:
		if ( (res = IsAnyMode( mode )) >= 0 ) 
			{               
			g_combs[ g_tanks[ res ]->comb_n ]->par->setParamM( 
				err_par, 1000 + g_tanks[ res ]->no * 100 + mode );
			return 1;
			}
		break;			    
		} 

	return 0;            
	}  
//-----------------------------------------------------------------------------
int ProcMainParams( int argc, char *argv[] )
    {
    if ( argc > 1 )
        {
        if ( strcmp( argv[ 1 ], "rcrc" ) == 0 )
            {
#ifdef DEBUG
            Print( "\nReset CRC..." );
#endif
            TParams::ResetCRC();      
#ifdef DEBUG
            Print( " Ok!\n" );
#endif
            }

        if ( strcmp( argv[ 1 ], "reseteeprom" ) == 0 )
            {
#ifdef DEBUG
            Print( "\nStarting reset total EEPROM..." );
#endif
            EnableEEP();
#if defined I7186_E || defined uPAC5000 
            for ( short i = 0; i < 4096; i++ )
                {
                WriteEEP( i/256+48, i%256, 0);
                }
#else
#ifdef I7188_E
            for ( short i = 0; i < 1792; i++ )
                {
                WriteEEP( i/256, i%256, 0);
                }
#else
            for ( short i = 0; i < 2048; i++ )
                {
                WriteEEP( i/256, i%256, 0);
                }
#endif
#endif
            ProtectEEP();
#ifdef DEBUG
            Print( " Ok!\n" );
#endif
            }
#ifdef MCA_RECIPES
		if ( strcmp( argv[ 1 ], "resetsram" ) == 0 )
			{
			unsigned char nullarray[BLOCK_SIZE];
			memset(nullarray,0,BLOCK_SIZE);
			for (int i = 0; i< 4096; i++)
				{
				X608_WriteFn(i*BLOCK_SIZE,BLOCK_SIZE, nullarray);
				}
			}
#endif
        } 

    if ( strcmp( argv[ 1 ], "fb" ) == 0 )
        {
        extern int g_is_FB;
        g_is_FB = 0;
        }

    if ( strcmp( argv[ 1 ], "nofb" ) == 0 )
        {
        extern int g_is_FB;
        g_is_FB = 1;
        }
    
    return 0;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int TTank::SetMode( int mode, int newm ) 
    {
#ifdef DEBUG
            Print ("Start TTank[N%d]::SetMode mode = %d [%d], st = %lu.\n", 
                no, mode, newm, state );
#endif 

    int i, res;

    if ( newm != 0 ) newm = 1; //newm может равняться только 0 или 1.
    if ( mode < 0 || mode > 31 ) res = 3;
    else
        {
        if ( GetMode( mode ) == newm ) 
			{
			res = 1;
			}
        else
            {
            if ( newm == 0 ) 
                {  //off mode
                state = state & ( ~( 1L << mode ) );
                FinalMode( mode );
                res = mode + 2000;
                } 
            else 
                {  
#ifdef USE_SIMPLE_DEV_ERRORS
                //Проверка режима на проверку ОС устройств.
                char res_str[ 41 ] = { 'о', 'б', 'р', '.', ' ', 'с', 'в', 'я', 'з', 'ь', ' ', 0 };
                
                int res = modes_manager->check_mode_devices( mode, res_str + 
                    strlen( res_str ), 41 - strlen( res_str ) );
                if ( res )
                    {
                    if ( is_check_mode( mode ) == 1 )
                        {
                        set_err_msg( res_str, mode );
                        return res = mode + 1000;
                        }

                    set_err_msg( res_str, mode, ERR_ON_WITH_ERRORS ); 
                    }
                //Проверка режима на проверку ОС устройств.
#endif // USE_SIMPLE_DEV_ERRORS

                //try to set
				res = 4;
                if ( ( res = InitMode( mode ) ) == 0 ) 
                    {
                    state = state | 1L << mode;
                    res = mode + 1000;
                    }        
                }
#ifdef DEBUG
            Print ("TTank[N%d]::SetMode mode = %d,[%d], res = %d, st = %lu.\n", 
                no, mode, newm, res, state );
#endif               
            }            
        }
    return res;
    }
//-----------------------------------------------------------------------------
int TTank::GetMode( int mode ) 
    {
    return ( int )( ( unsigned long )( state >> mode ) & 1 );
    }
//-----------------------------------------------------------------------------
TTank::TTank( TParams *tpar, int n, int tmrCnt ): tmrCnt( tmrCnt ), 
    no( n ),
    par( tpar ),
    cmd( 0 )

#ifdef USE_SIMPLE_DEV_ERRORS
	,
	is_err( 0 ),
	is_lvl_err( 0 ),
    LL( 0 ), LH ( 0 )
#endif // USE_SIMPLE_DEV_ERRORS
    {  
    int i;
    int j;

#if defined NO_TANKS_MODE
    modes_manager = new mode_manager( 1 );
#else
    modes_manager = new mode_manager( 32 );
#endif //NO_TANKS_MODE    

    modes_manager->set_param( tpar );

#if defined I7188_E || defined I7186_E
    active_dev = new active_device( this, active_device::AT_TANK );
#endif // defined I7188_E || defined I7186_E

    com_dev = new complex_device( n, "TANK", 4, i_complex_device::COMPLEX_DEV );
    com_dev->sub_dev[ 0 ] = new single_state( "SINGLE_STATE", n, &state, this, 
        single_state::T_TANK ); 
    com_dev->sub_dev[ 1 ] = new complex_state( "STATE", n, &state, this, 
        single_state::T_TANK ); 
    com_dev->sub_dev[ 2 ] = new complex_state( "CMD", n, &cmd, this, 
        single_state::T_TANK ); 
    com_dev->sub_dev[ 3 ] = new dev_param( "PARAM", n, par );

#ifdef USE_SIMPLE_DEV_ERRORS
    memset( err_str, 0, sizeof( err_str ) );
#endif // USE_SIMPLE_DEV_ERRORS
	
    TMR = new TTimer*[ tmrCnt ];
    for ( i = 0; i < tmrCnt; i++) TMR[ i ] = new TTimer;

    state = 0;   
    modeStartTime[ 32 ] = MyGetMS( );

#ifdef USE_SIMPLE_DEV_ERRORS
    for ( i = 0; i < 32; i++ )
        {
        mode_name[ i ] = 0;
        } 
#endif // USE_SIMPLE_DEV_ERRORS  
    }
//-----------------------------------------------------------------------------
TTank::TTank()
    {
    }
//-----------------------------------------------------------------------------
TTank::~TTank() 
    {
    int i;
    for ( i = 0; i < tmrCnt; i++ ) delete TMR[ i ];
    delete TMR;
    }
//-----------------------------------------------------------------------------
int TTank::InitParams( )
    { 
    for ( int j = par->WORK_PARAM_CNT; j < par->getParamCnt(); j++ ) 
        par->setParamM( j, 0 );

	//--------------------------------------
	par->setParamM( CROS_PROC_TIME, 5 );

	//--------------------------------------

    return 0;
    }
//-----------------------------------------------------------------------------
int TTank::InitWorkParams( )
    {    
    for ( int j = 0; j < par->WORK_PARAM_CNT; j++ ) par->setParamM( j, 0 );

    return 0;
    }
//-----------------------------------------------------------------------------

int TTank::InitMode( int mode )
    {
	int i;

#ifdef DEBUG
	//modes_manager->print();
	//Getch();
#endif

	//	Включение	--------------------------------------------------------------
	//	Проверка на наличие всех необходимых для включения режима сигналов
	for ( i = 0; i < paths[ mode ].FB_need_Cnt; i++ ) 
		{	
		if ( DEV( paths[ mode ].FB_need[ i ] )->State() == 0 )
			{

#ifdef USE_SIMPLE_DEV_ERRORS
			char err_msg[ 50 ] = { 0 };
			snprintf( err_msg, sizeof( err_msg ), "Нет OC %d \"%.25s\"",
				int ( DEV( paths[ mode ].FB_need[ i ] )->get_n() ), 
				DEV( paths[ mode ].FB_need[ i ] )->get_name() );
			set_err_msg( err_msg, mode );
#endif // USE_SIMPLE_DEV_ERRORS

			return ( -paths[ mode ].FB_need[ i ].no );
			}
		}

	//	Блокирующие режимы гребенок
	for ( i = 0; i < paths[ mode ].Block_cm_cnt; i++ )
		{
		//	Преобразовываем код режима в номер объекта и номер режима
		int obj_idx;
		int obj_no = paths[ mode ].Block_cm[ i ] / 1000;
		int mode_no = paths[ mode ].Block_cm[ i ] % 1000;

		//	Ищем объект по его номеру
		for ( int j = 0; j < TMyComb::CombCnt; j++ )
			{
			if ( g_combs[ j ]->no == obj_no )
				{
				obj_idx = j;
				break;
				}
			}

		//	Проверяем, если найденный режим найденного объекта включен,
		//	то блокируем включение нашего режима и выдаем сообщение 
		if (	( obj_idx < TMyComb::CombCnt )
			&&	( mode_no < g_combs[ obj_idx ]->statesCnt )
			&&	g_combs[ obj_idx ]->GetMode( mode_no ) )
			{
			// 			sprintf( err_str, //sizeof( err_str ),
			// 				"Включен блокирующий режим %s танка %d",
			// 				g_combs[ obj_idx ]->get_mode_name( mode_no ),
			// 				g_combs[ obj_idx ]->no );
			// 
			// 			set_err_msg( err_str, mode, ERR_CANT_ON );
			return 1;
			}
		}

	//	Блокирующие режимы танков
	for ( i = 0; i < paths[ mode ].Block_tm_cnt; i++ )
		{ 
		//	Преобразовываем код режима в номер объекта и номер режима
		int obj_idx;
		int obj_no = paths[ mode ].Block_tm[ i ] / 1000;
		int mode_no = paths[ mode ].Block_tm[ i ] % 1000;

		//	Ищем объект по его номеру
		for ( int j = 0; j < TankCnt; j++ )
			{
			if ( g_tanks[ j ]->no == obj_no )
				{
				obj_idx = j;
				break;
				}
			}

		//	Проверяем, если найденный режим найденного объекта включен,
		//	то блокируем включение нашего режима и выдаем сообщение 
		if (	( obj_idx < TankCnt )
			&&	( mode_no < g_tanks[ obj_idx ]->mode_cnt )
			&&	g_tanks[ obj_idx ]->GetMode( mode_no ) )
			{
			// 			sprintf( err_str, //sizeof( err_str ),
			// 				"Включен блокирующий режим %s танка %d",
			// 				g_tanks[ obj_idx ]->get_mode_name( mode_no ),
			// 				g_tanks[ obj_idx ]->no );
			// 
			// 			set_err_msg( err_str, mode, ERR_CANT_ON );
			return 1;
			}
		}

	//	Блокирующие устройства
	for ( i = 0; i < paths[ mode ].Block_dev_Cnt; i++ )
		{
		if( DEV( paths[ mode ].Block_dev[ i ] )->State() == 1 )
			{
//			char* str;
// 			sprintf( str, "Включено блокирующее устройство %d !", 
// 				paths[ mode ].Block_dev[ i ].no );
// 			set_err_msg( str, 0, ERR_SIMPLE );

			return 1;
			}
		}
		
	//	Работа		--------------------------------------------------------------
	//	Если для этого режима задан ключ, то задаем началные параметры
	if ( paths[ mode ].Key_signal_Cnt > 0 )
		{
		paths[ mode ].lamp_blink_start_time = MyGetMS();
		paths[ mode ].DT_time = MyGetMS( );
		}

	//	Если настроено ожидание, то сбрасываем переменную времени начала переключения
	if ( paths[ mode ].Wait_modes_Cnt > 0 )
		{
		start_cross_period = 0;
		}

	//	Параметры	--------------------------------------------------------------
	//	Если задан параметр времени работы режима
	if (	paths[ mode ].work_time_par < par->getParamCnt() 
		&&	par->getParam( paths[ mode ].work_time_par ) > 0 )
		{
		SetModeTime( mode, par->getParam( paths[ mode ].work_time_par ) * 1000 * 60 );
		}

	//	Проверяем какие режимы нужно ВКЛючить
	for ( i = 0; i < paths[ mode ].On_mode_Cnt; i++ )
		{
		//	Преобразовываем код режима в номер объекта и номер режима
		int obj_idx;
		int obj_no = paths[ mode ].On_mode[ i ] / 1000;
		int mode_no = paths[ mode ].On_mode[ i ] % 1000;

		//	Ищем объект по его номеру
		for ( int j = 0; j < TankCnt; j++ )
			{
			if ( g_tanks[ j ]->no == obj_no )
				{
				obj_idx = j;
				break;
				}
			}

		//	Включаем режимы
		if (	( obj_idx < TankCnt )
			&&	( mode_no < g_tanks[ obj_idx ]->mode_cnt )
			&&	!g_tanks[ obj_idx ]->GetMode( mode_no ) )
			{
			g_tanks[ obj_idx ]->SetMode( mode_no, 1 );
			}		  
		}

	//	Проверяем какие режимы нужно ВЫКЛючить
	for ( i = 0; i < paths[ mode ].Off_mode_Cnt; i++ )
		{
		//	Преобразовываем код режима в номер объекта и номер режима
		int obj_idx;
		int obj_no = paths[ mode ].Off_mode[ i ] / 1000;
		int mode_no = paths[ mode ].Off_mode[ i ] % 1000;

		//	Ищем объект по его номеру
		for ( int j = 0; j < TankCnt; j++ )
			{
			if ( g_tanks[ j ]->no == obj_no )
				{
				obj_idx = j;
				break;
				}
			}

		//	Выключаем режимы
		if (	( obj_idx < TankCnt )
			&&	( mode_no < g_tanks[ obj_idx ]->mode_cnt )
			&&	g_tanks[ obj_idx ]->GetMode( mode_no ) )
			{
			//	Выключаем не сразу, а задаем время для выключения
			g_tanks[ obj_idx ]->modeEvaluationTime[ mode_no ] = 
					( MyGetMS( ) - modeStartTime[ mode_no ] ) + 
					g_tanks[ obj_idx ]->par->getParam( CROS_PROC_TIME ) * 1000;
			//g_tanks[ obj_idx ]->SetMode( mode_no, 0 );
			}		  
		}
			
			 
	//	Задаем начальные параметры времени для режима
	if ( mode < 33 )
		{
		//Инициализация времени  режима.
		modeStartTime[ mode ] = MyGetMS( );
		isModeEvaluationTimeLeft[ mode ] = 0;
		}

#ifdef DEBUG
    else Print( "Error TTank::InitMode m = %d!\n", mode );
#endif

	Prev_LH = LH->State();
	Prev_LL = LL->State();

    return 0;
    }
//-----------------------------------------------------------------------------

int TTank::Evaluate( )
    {
    int i, j;  
	
	//	Выключаем флипование во всех устройствах (см. по таймеру)
	for ( i = 0; i < mode_cnt; i++ )
		{
		if ( GetMode( i ) && ( UseatOn == 0 ) && ( LseatOn == 0 ) )
			{
			for ( j = 0; j<paths[ i ].USCnt; j++) DEV(paths[ i ].US[ j ])->Off();
			for ( j = 0; j<paths[ i ].LSCnt; j++) DEV(paths[ i ].LS[ j ])->Off();
			}
		}	
	  
    for ( i = 0; i < mode_cnt; i++ ) 
        {
        if ( GetMode( i ) ) 
            {
            int currentMode = i;

            //-Работа с временем режима.
            if ( modeEvaluationTime[ currentMode ] > 0 && 
                MyGetMS( ) - modeStartTime[ currentMode ] >
                modeEvaluationTime[ currentMode ] ) 
				{
                isModeEvaluationTimeLeft[ currentMode ] = 1;    
				SetMode( currentMode, 0 );
				continue;
				}

#if !defined NO_TANKS_MODE
            modes_manager->evaluate( i );
#endif // NO_TANKS_MODE

			//***************************************************
			//	Включаем заданные устройства
			for ( j = 0; j < paths[ i ].OVCnt; j++ )
				{
				DEV( paths[ i ].OV[ j ] )->On();
				}

			//	Выключаем заданные устройства
			for ( j = 0; j < paths[ i ].CVCnt; j++ )
				{
				DEV( paths[ i ].CV[ j ] )->Off();
				}

			//	Флипование верхними седлами микспруфов
			for ( j = 0; j < paths[ i ].USCnt; j++ )
				{
				if ( UseatOn == 1 ) DEV( paths[ i ].US[ j ] )->Set( 4 );
				}

			//	Флипование нижними седлами микспруфов
			for ( j = 0; j < paths[ i ].LSCnt; j++ )
				{
				if ( LseatOn == 1 ) DEV( paths[ i ].LS[ j ] )->Set( 5 );
				} 

			//	Подаем управляющие сигналы
			for ( j = 0; j < paths[ i ].UPRCnt; j++)
				{                    
				DEV( paths[ i ].UPR[ j ] )->On( );                    
				}

			//	Управляющие устройствами сигналы          
			for ( j = 0; j < paths[ i ].FB_control_Cnt; j++ )
				{							
				// В зависимости от сигнала обратной связи вкл/выкл устройство 
				if ( DEV( paths[ i ].FB_control[ j ] )->State( ) )
					{
					DEV( paths[ i ].FB_control_dev[ j ] )->On( );
					}
				else
					{
					DEV( paths[ i ].FB_control_dev[ j ] )->Off( );
					}		
				}

			//	Зависящие от устройств сигналы
			for ( j = 0; j < paths[ i ].UPR_control_Cnt; j++ )
				{
				if ( DEV( paths[ i ].UPR_control_dev[ j ] )->State() == -1 ) 
					{
					if ( MyGetMS() - paths[ i ].time_for_off_UPR[ j ] > 2000 )
						{
						//	Снимаем управляющие сигналы
						DEV( paths[ i ].UPR_control[ j ] )->Off();
						}	
					}
				else
					{
					paths[ i ].time_for_off_UPR[ j ] = MyGetMS();
					}
				}		     

			//		Выключающие режим устройства (сигналы)
			for ( j = 0; j < paths[ i ].Dev_off_Cnt; j++ )
				{
				if ( DEV( paths[ i ].Dev_off[ j ] )->State() == 1 ) 
					{
					if ( MyGetMS() - paths[ i ].time_for_off_mode[ j ] > 2000 )
						{
						SetMode( i, 0 );
						}	
					}
				else
					{
					paths[ i ].time_for_off_mode[ j ] = MyGetMS();
					}
				}

			//	Работа с ожиданием
			//------------------------------------------
			//	Создаем флаг (можно переходить в режимы работы (0) или нет(>0) )
			char go_work = 0;

			if ( paths[ i ].Wait_modes_Cnt > 0 )
				{
				switch ( modes_manager->get_active_step( i ) )
					{
				case 0:	//	Шаг ожидания
					//	Проверяем на включенность режимы, которые мы ждем
					for ( int k = 0; k < paths[ i ].Wait_modes_Cnt; k++ )
						{
						//	Преобразовываем код режима в номер объекта и номер режима
						int obj_idx;
						int obj_no = paths[ i ].Wait_modes[ k ] / 1000;
						int mode_no = paths[ i ].Wait_modes[ k ] % 1000;

						//	Ищем объект по его номеру
						for ( int j = 0; j < TankCnt; j++ )
							{
							if ( g_tanks[ j ]->no == obj_no )
								{
								obj_idx = j;
								break;
								}
							}

						//	Проверяем режимы
						//	1 - Номер объекта в допустимых пределах?
						//	2 - Номер режима в допустимых пределах?
						if (	( obj_idx < TankCnt )
							&&	( mode_no < g_tanks[ obj_idx ]->mode_cnt ))
							{
							//	3 - Режим данного объекта включен?
							//	4 - Включенный режим не в шаге 0-ожидание, а в шагах работы (>0)? 
							if (	g_tanks[ obj_idx ]->GetMode( mode_no )
								&&  g_tanks[ obj_idx ]->modes_manager->get_active_step( mode_no ) > 0 )
								{
								go_work++;

								//	Если задан флаг-время выключения режимов по уровню, то разрешаем вкл. работу
								if ( g_tanks[ obj_idx ]->start_cross_period > 0 )
									{
									go_work--;
									}
								}
							}
						}
					break;

				//case 1:	//	Шаги работы
				default:	
					if (	start_cross_period > 0
						&&	 getDeltaT( start_cross_period ) > ( par->getParam( CROS_PROC_TIME ) * 1000 ) )
						{
						//	По флагу-времени выключаем режим
						start_cross_period = 0;
						SetMode( i, 0 );
						}
					break;
					}
				}

			if ( go_work == 0 )
				{
				from_wait_to_work( i );
				}
			//------------------------------------------
			
			//	Если задан параметр времени работы режима
			if (	paths[ i ].work_time_par < par->getParamCnt() 
				&&	par->getParam( paths[ i ].work_time_par ) > 0 )
				{
				if ( isModeEvaluationTimeLeft[ i ] == 1 )
					{
					SetMode( i, 0 );
					}
				}

			//	Фиксируем номер текущего шага
			par->setParamM( STEP_NUMBER, i * 1000 + modes_manager->get_active_step( i ) + 1 );

			par->setParamM( PROCESS_TIME, ( MyGetMS() - modeStartTime[i] ) /1000 );
            } 
		else
			{
			//	Проверяем, чтобы не было сигналов выключающих режим
			for ( j = 0; j < paths[ i ].Dev_off_Cnt; j++ )
				{
				if ( DEV( paths[ i ].Dev_off[ j ] )->State() == 1 ) 
					{
					break;
					}
				}

			//	Если их нет, то проверяем "Включающие режим сигналы"
			if ( j == paths[ i ].Dev_off_Cnt )
				{
				//	Включающие режим устройства (сигналы)			
				for ( j = 0; j < paths[ i ].FB_on_mode_Cnt; j++ )
					{
					if ( DEV( paths[ i ].FB_on_mode[ j ] )->State() == 1 ) 
						{
						if ( MyGetMS() - paths[ i ].time_for_on_mode[ j ] > 2000 )
							{
							SetMode( i, 1 );
							}	
						}
					else
						{
						paths[ i ].time_for_on_mode[ j ] = MyGetMS();
						}	
					}
				}

			}	//	if GetMode()	       
        }	//	for i

	
	//	Если нет включеных режимов, то значение текущего шага == -1
		if ( state == 0 )
			{
			par->setParamM( STEP_NUMBER, 0 );
			}
	
		
#ifdef USE_SIMPLE_DEV_ERRORS
    if ( LH && LL )
        {
        // Ошибка предельных уровней.   
        if ( LH->State() == LIS && LL->State() == LNO )
            {
            if ( 0 == is_lvl_err )
                {
                set_err_msg( "ошибка предельных уровней", -1, ERR_SIMPLE );
                is_lvl_err = 1;
                }        
            }
        else
            {
            is_lvl_err = 0;
            }
        }
#endif // USE_SIMPLE_DEV_ERRORS

	//	Контролируем уровни (выключение по уровню)
	if (	( Prev_LL == 1 && LL->State() == 0 )
		||	( Prev_LH == 1 && LH->State() == 0 )
		)
		{
		//	Фиксируем флаг-время начала завершения режима
		start_cross_period = MyGetMS();
		set_err_msg( "Сработал предельный уровень!", -1, ERR_SIMPLE );
		}

	//	Работа с временем режима
	if ( state == 0 ) par->setParamM( PROCESS_TIME, ( MyGetMS() - modeStartTime[32] ) /1000 );

	Prev_LH = LH->State();
	Prev_LL = LL->State();

    return 0;
    }
//-----------------------------------------------------------------------------

void TTank::from_wait_to_work( int i )
	{
	//	Работа с ключом
	if ( paths[ i ].Key_signal_Cnt >= 2 )
		{
		//mode_manager
		Key_Work( i );
		}
	else
		{
		//	Если ключ не задан, то
		//	Если в режиме настроено ожидание
		//		и заданы 2 шага (шаг 0 Ожидание и шаг 1 Работа),
		//		и текущий шаг 0-Ожидание,
		// то переходим к шагу 1-Работа
		if (	paths[ i ].Wait_modes_Cnt > 0
			&&	modes_manager->steps_cnt[ i ] >= 2
			&&	modes_manager->get_active_step( i ) == 0   
			)
			{
			modes_manager->final( i );
			modes_manager->init( i, 1, this );
			start_cross_period = 0;
			}
		}
	}
//-----------------------------------------------------------------------------

int TTank::FinalMode( int mode )
	{
	int i, j;

	//Инициализация времени  режима.
	if ( 0 == state ) modeStartTime[ 32 ] = MyGetMS( );
	isModeEvaluationTimeLeft[ mode ] = 0;    


	//	Включаемые устройства
	for ( j = 0; j < paths[ mode ].OVCnt; j++ )
		{
		DEV( paths[ mode ].OV[ j ] )->Off();
		}

	//	Флипование верхними седлами микспруфов
	for ( j = 0; j < paths[ mode ].USCnt; j++ )
		{
		DEV( paths[ mode ].US[ j ] )->Off();
		}

	//	Флипование нижними седлами микспруфов
	for ( j = 0; j < paths[ mode ].LSCnt; j++ )
		{
		DEV( paths[ mode ].LS[ j ] )->Off();
		} 

	//	Подаем управляющие сигналы
	for ( j = 0; j < paths[ mode ].UPRCnt; j++)
		{                    
		DEV( paths[ mode ].UPR[ j ] )->Off();                    
		}

	//	Управляющие устройствами сигналы          
	for ( j = 0; j < paths[ mode ].FB_control_Cnt; j++ )
		{							
		DEV( paths[ mode ].FB_control_dev[ j ] )->Off( );
		}

	//	Включаемые по завершению устройства
	for ( j = 0; j < paths[ mode ].FVCnt; j++ ) 
		{
		DEV( paths[ mode ].FV[ j ])->On();
		}

	//	Включение следующего режима (автоматическое)
	if (	( paths[ mode ].next_mode > 0 )
		 &&	( paths[ mode ].next_mode < mode_cnt )
		 &&	!GetMode( paths[ mode ].next_mode )
		)
		{
		SetMode( paths[ mode ].next_mode, 1 );
		}

	//	Проверяем какие режимы нужно было ВКЛючить - теперь мы их ВЫКЛючаем
	for ( i = 0; i < paths[ mode ].On_mode_Cnt; i++ )
		{
		//	Преобразовываем код режима в номер объекта и номер режима
		int obj_idx;
		int obj_no = paths[ mode ].On_mode[ i ] / 1000;
		int mode_no = paths[ mode ].On_mode[ i ] % 1000;

		//	Ищем объект по его номеру
		for ( int j = 0; j < TankCnt; j++ )
			{
			if ( g_tanks[ j ]->no == obj_no )
				{
				obj_idx = j;
				break;
				}
			}

		//	Выключаем режимы
		if (	( obj_idx < TankCnt )
			&&	( mode_no < g_tanks[ obj_idx ]->mode_cnt )
			&&	g_tanks[ obj_idx ]->GetMode( mode_no ) )
			{
			g_tanks[ obj_idx ]->SetMode( mode_no, 0 );
			}		  
		}

#if !defined NO_TANKS_MODE
	modes_manager->final( mode );
#endif // NO_TANKS_MODE
	return 0;
	}
//-----------------------------------------------------------------------------

void TTank::Key_Work( int i )
	{
	key_btn_Drain	= DEV( paths[ i ].Key_signal[ 0 ] ); 
	key_btn_Tank	= DEV( paths[ i ].Key_signal[ 1 ] );

	if ( paths[ i ].Key_signal_Cnt > 2 )
		{
		key_lamp	= DEV( paths[ i ].Key_signal[ 2 ] );
		}

	//	Организуем переключения между шагами по ключу и работу светодиода (общий код для режимов)
	switch ( modes_manager->get_active_step( i ) )
		{
	case 0:		//	Ожидание сигнала с ключа
		if ( key_btn_Drain->State() == ON )
			{
			modes_manager->final( i );
			modes_manager->init( i, 1, this );
			}

		if ( key_btn_Tank->State() == ON ) 
			{
			modes_manager->final( i );
			modes_manager->init( i, 3, this );
			}

		//	Мигание лампочки
		if ( MyGetMS( ) - paths[ i ].lamp_blink_start_time > 1000 )
			{
			key_lamp->Set( !key_lamp->State( ) );
			paths[ i ].lamp_blink_start_time = MyGetMS( );
			}
		break;

	case 1:	//	В линию вытеснения
		//	Мигание лампочки
		if ( MyGetMS( ) - paths[ i ].lamp_blink_start_time > 1000 )
			{
			key_lamp->Set( !key_lamp->State( ) );
			paths[ i ].lamp_blink_start_time = MyGetMS( );
			}

		//	Если во время дренажа влкючается "В танк", то 
		if ( key_btn_Tank->State() == ON ) 
			{
			//	Если при этом не горит верхний уровень на танке, то включаем переходной
			if (( LH != NULL ) && ( LH->State( ) == LIS ))
				{
				//set_err_msg( "Горит верхний уровень", i, ERR_SIMPLE );
				}
			else
				{
				modes_manager->final( i );
				modes_manager->init( i, 2, this );		//дренаж+танк
				paths[ i ].next_step_filling = 3;		//с последующим включением "В танк"
				paths[ i ].DT_time = MyGetMS( );
				}
			}
		break;		

	case 2:	//	В линию вытеснения + В танк
		//	Мигание лампочки
		if ( MyGetMS( ) - paths[ i ].lamp_blink_start_time > 1000 )
			{
			key_lamp->Set( !key_lamp->State( ) );
			paths[ i ].lamp_blink_start_time = MyGetMS( );
			}

		//	Если вышло время переходного шага, то включаем наполнение в ...
		if ( ( MyGetMS() - paths[ i ].DT_time ) > 3000 )
			{
			//	Если это переходной после подачи в дренаж, то вкл. в танк
			//	Если это переходной после подачи в танк, то вкл. в дренаж	
			modes_manager->final( i );
			modes_manager->init( i, paths[ i ].next_step_filling, this );
			}	
		break;

	case 3:	//	В танк
		//	Включаем лампочку
		key_lamp->On();

		//	Если при подаче в танк влкючается "В дренаж", то включаем переходной
		if ( key_btn_Drain->State() == ON )
			{
			modes_manager->final( i );
			modes_manager->init( i, 2, this );		//дренаж+танк
			paths[ i ].next_step_filling = 1;					//с последующим включением "В дренаж"
			paths[ i ].DT_time = MyGetMS( );
			}

		//	Если идет наполнение в танк и сработал верхний уровень, то
		if (( LH != NULL ) && ( LH->State( ) == LIS ))
			{
			//set_err_msg( "Горит верхний уровень", i, ERR_SIMPLE );
			}
		break;	
		}	//	switch
	}
//-----------------------------------------------------------------------------

int TTank::SetModeTime( int mode, unsigned long modeTime )
    {
    modeEvaluationTime[mode] = modeTime;
    return 0;
    }
//-----------------------------------------------------------------------------
int TTank::set_default_state()
    {
    return 0;
    }
//-----------------------------------------------------------------------------
int TTank::get_saved_size()
    {
    return 0;
    }
//-----------------------------------------------------------------------------

#ifdef USE_SIMPLE_DEV_ERRORS
const char* TTank::get_mode_name( int mode ) const
    {
    if ( mode >= 0 && mode < 32 )
        {
        return mode_name[ mode ] ? mode_name[ mode ] : "?";
        }
    
    return "?";
    }
//-----------------------------------------------------------------------------
int TTank::set_err_msg( const char *err_msg, int mode, ERR_MSG_TYPES type )
    {  
	err_type = type;
	  
    switch ( type )
        {
        case ERR_CANT_ON:
            snprintf( err_str, sizeof( err_str ), 
                "Не включен режим %.1d \"%.40s\" танка %.1d - %.40s.", 
                mode + 1, get_mode_name( mode ), no, err_msg );
            break;

        case ERR_ON_WITH_ERRORS:
            snprintf( err_str, sizeof( err_str ), 
                "Включен с ошибкой режим %.1d \"%.40s\" танка %.1d - %.40s.", 
                mode + 1, get_mode_name( mode ), no, err_msg );
            break;

        case ERR_OFF:
            snprintf( err_str, sizeof( err_str ), 
                "Отключен режим %.1d \"%.40s\" танка %.1d - %.40s.", 
                mode + 1, get_mode_name( mode ), no, err_msg );
            break;

        case ERR_DURING_WORK:
            snprintf( err_str, sizeof( err_str ), 
                "Режим %.1d \"%.40s\" танка %.1d - %.40s.", 
                mode + 1, get_mode_name( mode ), no, err_msg );
            break;

        case ERR_SIMPLE:
            snprintf( err_str, sizeof( err_str ), 
                "Танк %.1d - %.60s.", no, err_msg );
            break;

        default:
#ifdef DEBUG
            Print( "Error TTank::set_err_msg(...) - unknown error type!\n" );
            Print( "Press any key to continue." );
            Getch();
            Print( "\n" );
#endif // DEBUG
            snprintf( err_str, sizeof( err_str ), 
                "Режим %.1d \"%.40s\" танка %.1d - %.40s!", 
                mode + 1, get_mode_name( mode ), no, err_msg );
            break;
        }

#ifdef DEBUG
    Print( "err_msg -> %s\n", err_msg );
    Print( "err_str -> %s\n", err_str );
#endif // DEBUG

    ++is_err;
    error_number = mode;

    return 0;
    }

int TTank::GetModeEx( int mode, ... )
    {
    int *p = &mode;            

    while ( *p != -1 )   
        {
        if ( GetMode( *p ) )
            {
            return *p;
            }
        p++;
        }

    return -1;
    }
//-----------------------------------------------------------------------------
int TTank::reset_error()
    {
    is_err       = 0;
    err_str[ 0 ] = 0;
    error_number = 0;
    return 0;
    }
//-----------------------------------------------------------------------------
int TTank::set_mode_name( int mode, char* name )
    {
    mode_name[ mode ] = name;
    return 0;
    }
//-----------------------------------------------------------------------------
#endif // USE_SIMPLE_DEV_ERRORS

#pragma  argsused
int TTank::save_active_state( char *buff )
    {
    return 0;
    }
//-----------------------------------------------------------------------------
#pragma  argsused
int TTank::load_active_state( char *buff )
    {
    return 0;
    }
//-----------------------------------------------------------------------------

#ifdef USE_SIMPLE_DEV_ERRORS
#pragma  argsused
int TTank::is_check_mode( int mode ) const
    {
    return 0;
    }
//-----------------------------------------------------------------------------
const char* TTank::get_type() const
	{
	switch ( err_type )
		{
	case ERR_CANT_ON:
	case ERR_ON_WITH_ERRORS:
		return "ответ";

	case ERR_OFF:    
	case ERR_DURING_WORK:
	case ERR_SIMPLE:
		return "сообщение";
		}

	return "?";
	}
//-----------------------------------------------------------------------------
int TTank::get_priority() const
	{
	switch ( err_type )
		{
	case ERR_CANT_ON:
	case ERR_ON_WITH_ERRORS:
		return error_tech_device::P_ANSWER;

	case ERR_OFF:    
	case ERR_DURING_WORK:
	case ERR_SIMPLE:
		return error_tech_device::P_MESSAGE;
		}

	return error_tech_device::P_ALARM;
	}
//-----------------------------------------------------------------------------
void TTank::init_levels( TVDI* LL, TVDI* LH )
	{
	this->LL = LL;
	this->LH = LH;
	}
#endif // USE_SIMPLE_DEV_ERRORS
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//Возвращаемые значения:
//  0    -  999 : ошибка включения режима;
//  1000 - 1999 : режим (0 - 999) включился;
//  2000 - 2999 : режим (0 - 999) выключился;

int TMyComb::SetMode( int mode, int newm ) 
    {
    int i, res;

#ifdef DEBUG
    Print ( "Start TMyComb::SetMode mode ( mode = %d, new state = %d ). \n", 
        mode, newm );
#endif

    if ( newm != 0 ) newm = 1; //newm может равняться только 0, или 1.
    if ( mode > statesCnt * 32 - 1 ) res = 3;
    else
        {
        if ( GetMode( mode ) == newm ) res = 1;
        else
            {
            if ( newm == 0 )
                {  //off mode
                state[ mode / 32 ] = state[ mode / 32 ] & ~( 1L << mode % 32 );
                FinalMode( mode );

#ifndef USE_NO_COMB
                g_greb->close_path( paths[ mode ].in_x, paths[ mode ].in_y, 
                    paths[ mode ].out_x, paths[ mode ].out_y );
#endif // USE_NO_COMB 

                res = mode + 2000;
                }
            else
                { 
/*
#ifdef USE_SIMPLE_DEV_ERRORS
                //Проверка на ошибки устройств, которые заданны вручную.
                const int MAX_LEN = 40;
                char err[ MAX_LEN ] = { 'о', 'б', 'р', '.', ' ', 'с', 'в', 'я', 'з', 'ь', ' ', 0 };
                int max_to_write = MAX_LEN - strlen( err );

                for ( i = 0; i < paths[ mode ].OVCnt; i++ )
                    {                        
                    if ( V( paths[ mode ].OV[ i ] )->State() == -1 )
                        {
                        int par = int ( V( paths[ mode ].OV[ i ] )->par->getParam( 0 ) );

                        if ( ( par & TVDO::DE_IS_SUPPRESS ) == 0 ) 
                            {                            
                            max_to_write -= snprintf( err + strlen( err ), 
                                max_to_write, "%s, ", 
                                V( paths[ mode ].OV[ i ] )->get_short_name() ); 

                            if ( max_to_write < 0 )
                                {
                                break;
                                }
                            }
                        }        
                    }

                for ( i = 0; i < paths[ mode ].CVCnt; i++ )
                    {                        
                    if ( V( paths[ mode ].CV[ i ] )->State() == -1 )
                        {
                        int par = int ( V( paths[ mode ].CV[ i ] )->par->getParam( 0 ) );

                        if ( ( par & TVDO::DE_IS_SUPPRESS ) == 0 ) 
                            {
                            if ( max_to_write < 0 )
                                {
                                break;
                                }

                            max_to_write -= snprintf( err + strlen( err ), 
                                max_to_write, "%s, ", 
                                V( paths[ mode ].CV[ i ] )->get_short_name() );   
                            }
                        }        
                    }    

                for ( i = 0; i < paths[ mode ].USCnt; i++ )
                    {                        
                    if ( V( paths[ mode ].US[ i ] )->State() == -1 )
                        {
                        int par = int ( V( paths[ mode ].US[ i ] )->par->getParam( 0 ) );

                        if ( ( par & TVDO::DE_IS_SUPPRESS ) == 0 ) 
                            {
                            if ( max_to_write < 0 )
                                {
                                break;
                                }

                            max_to_write -= snprintf( err + strlen( err ), 
                                max_to_write, "%s, ", 
                                V( paths[ mode ].US[ i ] )->get_short_name() );   
                            }
                        }        
                    }    
                for ( i = 0; i < paths[ mode ].LSCnt; i++ )
                    {                        
                    if ( V( paths[ mode ].LS[ i ] )->State() == -1 )
                        {
                        int par = int ( V( paths[ mode ].LS[ i ] )->par->getParam( 0 ) );

                        if ( ( par & TVDO::DE_IS_SUPPRESS ) == 0 ) 
                            {
                            if ( max_to_write < 0 )
                                {
                                break;
                                }

                            max_to_write -= snprintf( err + strlen( err ), 
                                max_to_write, "%s, ", 
                                V( paths[ mode ].LS[ i ] )->get_short_name() );   
                            }
                        }        
                    } 


                if ( err[ 11 ] ) //Есть ошибки.
                    {                    
                    if ( max_to_write < 0 )
                        {
                        //Если слишком длинная строка, добавляем многоточие.
                        err[ strlen( err ) - 1 ] = '.';
                        err[ strlen( err ) - 2 ] = '.';
                        err[ strlen( err ) - 3 ] = '.';
                        }
                    else
                        {
                        //Убираем последние символы - ", ".
                        err[ strlen( err ) - 1 ] = 0; 
                        err[ strlen( err ) - 1 ] = 0;
                        }

                    

                    if ( is_check_mode( mode ) )
                        {
                        set_err_msg( err, mode );
                        return res = mode + 1000;
                        }

					set_err_msg( err, mode, ERR_ON_WITH_ERRORS );
                    }                    
#endif // USE_SIMPLE_DEV_ERRORS
*/
                //check if possible
                if ( ( res = InitMode( mode ) ) == 0 )
                    {                    
                    state[ mode / 32 ] = state[ mode / 32 ] | 1L << mode % 32;

#ifndef USE_NO_COMB
                    g_greb->open_path( paths[ mode ].in_x, paths[ mode ].in_y, 
                        paths[ mode ].out_x, paths[ mode ].out_y, comb_path::OT_COMB,
                        this, mode ); 
#endif // USE_NO_COMB

                    res = mode + 1000;
                    }        
                }
            }
        }
#ifdef DEBUG
    Print ( "End TMyComb::SetMode mode = %d,[%d], res = %d. ", mode, newm, res );
    for ( i = 0; i < statesCnt; i++) Print ( " state[ %d ] = %lu \n", i, state[ i ] );
    Print( "\n" );
#endif 
    return res;
    }
//-----------------------------------------------------------------------------
int TMyComb::GetMode(int mode) const
    {
    return ( int )( ( unsigned long )( state [mode / 32] >> mode % 32 ) & 1 );
    }
//-----------------------------------------------------------------------------

int TMyComb::InitMode( int mode )
    { 
	int i;
#ifdef DEBUG
    Print ( "TMyComb::InitMode mode(...), mode = %d.\n", mode );
#endif

	//	Включение режима ------------------------------------------------------
    for ( i = 0; i < paths[ mode ].FB_need_Cnt; i++) 
        {	
		if ( DEV( paths[ mode ].FB_need[ i ] )->State() == 0 )
            {

#ifdef USE_SIMPLE_DEV_ERRORS
            char err_msg[ 50 ] = { 0 };
            snprintf( err_msg, sizeof( err_msg ), "Нет OC %d \"%.25s\"",
                int ( DEV( paths[ mode ].FB_need[ i ] )->get_n() ), 
                DEV( paths[ mode ].FB_need[ i ] )->get_name() );
            set_err_msg( err_msg, mode );
#endif // USE_SIMPLE_DEV_ERRORS

            return ( -paths[ mode ].FB_need[ i ].no );
            }
        }  

	//	Блокирующие устройства
	for ( i = 0; i < paths[ mode ].Block_dev_Cnt; i++ )
		{
		if( DEV( paths[ mode ].Block_dev[ i ] )->State() == 1 )
			{
//			Вывод сообщения работает не корректно - RESET
// 			char* str;
// 			sprintf( str, "Включено блокирующее устройство %d !", 
// 				paths[ mode ].Block_dev[ i ].no );
// 			set_err_msg( str, 0, ERR_SIMPLE );

			return 1;
			}
		}

	//	Блокирующие режимы гребенок
	for ( i = 0; i < paths[ mode ].Block_cm_cnt; i++ )
		{
		//	Преобразовываем код режима в номер объекта и номер режима
		int obj_idx;
		int obj_no = paths[ mode ].Block_cm[ i ] / 1000;
		int mode_no = paths[ mode ].Block_cm[ i ] % 1000;

		//	Ищем объект по его номеру
		for ( int j = 0; j < CombCnt; j++ )
			{
			if ( g_combs[ j ]->no == obj_no )
				{
				obj_idx = j;
				break;
				}
			}

		//	Проверяем, если найденный режим найденного объекта включен,
		//	то блокируем включение нашего режима и выдаем сообщение 
		if (	( obj_idx < CombCnt )
			&&	( mode_no < g_combs[ obj_idx ]->statesCnt )
			&&	g_combs[ obj_idx ]->GetMode( mode_no ) )
			{
			// 			sprintf( err_str, //sizeof( err_str ),
			// 				"Включен блокирующий режим %s танка %d",
			// 				g_combs[ obj_idx ]->get_mode_name( mode_no ),
			// 				g_combs[ obj_idx ]->no );
			// 
			// 			set_err_msg( err_str, mode, ERR_CANT_ON );
			return 1;
			}
		}

	//	Блокирующие режимы танков
	for ( i = 0; i < paths[ mode ].Block_tm_cnt; i++ )
		{
		//	Преобразовываем код режима в номер объекта и номер режима
		int obj_idx;
		int obj_no = paths[ mode ].Block_tm[ i ] / 1000;
		int mode_no = paths[ mode ].Block_tm[ i ] % 1000;

		//	Ищем объект по его номеру
		for ( int j = 0; j < TTank::TankCnt; j++ )
			{
			if ( g_tanks[ j ]->no == obj_no )
				{
				obj_idx = j;
				break;
				}
			}

		//	Проверяем, если найденный режим найденного объекта включен,
		//	то блокируем включение нашего режима и выдаем сообщение 
		if (	( obj_idx < TTank::TankCnt )
			&&	( mode_no < g_tanks[ obj_idx ]->mode_cnt )
			&&	g_tanks[ obj_idx ]->GetMode( mode_no ) )
			{
			// 			sprintf( err_str, //sizeof( err_str ),
			// 				"Включен блокирующий режим %s танка %d",
			// 				g_tanks[ obj_idx ]->get_mode_name( mode_no ),
			// 				g_tanks[ obj_idx ]->no );
			// 
			// 			set_err_msg( err_str, mode, ERR_CANT_ON );
			return 1;
			}
		}

	//	Параметры	--------------------------------------------------------------

	//	Проверяем какие режимы нужно ВКЛючить
	for ( i = 0; i < paths[ mode ].On_mode_Cnt; i++ )
		{
		//	Преобразовываем код режима в номер объекта и номер режима
		int obj_idx;
		int obj_no = paths[ mode ].On_mode[ i ] / 1000;
		int mode_no = paths[ mode ].On_mode[ i ] % 1000;

		//	Ищем объект по его номеру
		for ( int j = 0; j < TTank::TankCnt; j++ )
			{
			if ( g_tanks[ j ]->no == obj_no )
				{
				obj_idx = j;
				break;
				}
			}

		//	Включаем режимы
		if (	( obj_idx < TTank::TankCnt )
			&&	( mode_no < g_tanks[ obj_idx ]->mode_cnt )
			&&	!g_tanks[ obj_idx ]->GetMode( mode_no ) )
			{
			g_tanks[ obj_idx ]->SetMode( mode_no, 1 );
			}		  
		}

	//	Проверяем какие режимы нужно ВЫКЛючить
	for ( i = 0; i < paths[ mode ].Off_mode_Cnt; i++ )
		{
		//	Преобразовываем код режима в номер объекта и номер режима
		int obj_idx;
		int obj_no = paths[ mode ].Off_mode[ i ] / 1000;
		int mode_no = paths[ mode ].Off_mode[ i ] % 1000;

		//	Ищем объект по его номеру
		for ( int j = 0; j < TTank::TankCnt; j++ )
			{
			if ( g_tanks[ j ]->no == obj_no )
				{
				obj_idx = j;
				break;
				}
			}

		//	Включаем режимы
		if (	( obj_idx < TTank::TankCnt )
			&&	( mode_no < g_tanks[ obj_idx ]->mode_cnt )
			&&	g_tanks[ obj_idx ]->GetMode( mode_no ) )
			{
			g_tanks[ obj_idx ]->SetMode( mode_no, 0 );
			}		  
		}


	//	Фиксируем начало работы режима (для учета времени работы режима)
	paths[ mode ].start_time = MyGetMS();

    return 0;
    }
//-----------------------------------------------------------------------------

int TMyComb::Evaluate(void)
    {
    int i, j, k;
    int Flag_NorV;

    //eval mix timers
    if (TMR[TMR_USEATO]->Q()==1) 
        {        
        TMR[TMR_USEATO]->R();
        TMR[TMR_USEATO]->SZ(par->getParam(P_SEAT_PERIOD)*1000);
        TMR[TMR_USEATC]->S();
        UseatOn=1;
        };
    if (TMR[TMR_USEATC]->Q()==1) 
        {        
        TMR[TMR_USEATC]->R();
        TMR[TMR_USEATC]->SZ(par->getParam(P_SEAT_TIME));
        TMR[TMR_USEATO]->S();
        UseatOn=0;
        };
    if (TMR[TMR_LSEATO]->Q()==1) 
        {
        TMR[TMR_LSEATO]->R();
        TMR[TMR_LSEATO]->SZ(par->getParam(P_SEAT_PERIOD)*1000);
        TMR[TMR_LSEATC]->S();
        LseatOn=1;
        };
    if (TMR[TMR_LSEATC]->Q()==1) 
        {
        TMR[TMR_LSEATC]->R();
        TMR[TMR_LSEATC]->SZ(par->getParam(P_SEAT_TIME));
        TMR[TMR_LSEATO]->S();
        LseatOn=0;
        };

	//	Выключаем флипование во всех устройствах (см. по таймеру)
	for ( i = 0; i < statesCnt; i++ )
		{
		if ( GetMode( i ) && ( UseatOn == 0 ) && ( LseatOn == 0 ) )
			{
			for ( j = 0; j<paths[ i ].USCnt; j++) DEV(paths[ i ].US[ j ])->Off();
			for ( j = 0; j<paths[ i ].LSCnt; j++) DEV(paths[ i ].LS[ j ])->Off();
			}
		}

	//	По всем режимам
	for ( i = 0; i < statesCnt; i++ )
		{
		if ( GetMode( i ) )
			{
			//	Включаем заданные устройства
			for ( j = 0; j < paths[ i ].OVCnt; j++ )
				{
				DEV( paths[ i ].OV[ j ] )->On();
				}

			//	Выключаем заданные устройства
			for ( j = 0; j < paths[ i ].CVCnt; j++ )
				{
				DEV( paths[ i ].CV[ j ] )->Off();
				}

			//	Флипование верхними седлами микспруфов
			for ( j = 0; j < paths[ i ].USCnt; j++ )
				{
				if ( UseatOn == 1 ) DEV( paths[ i ].US[ j ] )->Set( 4 );
				}

			//	Флипование нижними седлами микспруфов
			for ( j = 0; j < paths[ i ].LSCnt; j++ )
				{
				if ( LseatOn == 1 ) DEV( paths[ i ].LS[ j ] )->Set( 5 );
				} 

			//	Подаем управляющие сигналы
			for ( j = 0; j < paths[ i ].UPRCnt; j++)
				{                    
				DEV( paths[ i ].UPR[ j ] )->On( );                    
				}

			//	Управляющие устройствами сигналы          
			for ( j = 0; j < paths[ i ].FB_control_Cnt; j++ )
				{							
				// В зависимости от сигнала обратной связи вкл/выкл устройство 
				if ( DEV( paths[ i ].FB_control[ j ] )->State( ) )
					{
					DEV( paths[ i ].FB_control_dev[ j ] )->On( );
					}
				else
					{
					DEV( paths[ i ].FB_control_dev[ j ] )->Off( );
					}		
				}
				
			//	Зависящие от устройств сигналы
			for ( j = 0; j < paths[ i ].UPR_control_Cnt; j++ )
				{
				if ( DEV( paths[ i ].UPR_control_dev[ j ] )->State() == -1 ) 
					{
					if ( MyGetMS() - paths[ i ].time_for_off_UPR[ j ] > 2000 )
						{
						//	Снимаем управляющие сигналы
						DEV( paths[ i ].UPR_control[ j ] )->Off();
						}	
					}
				else
					{
					paths[ i ].time_for_off_UPR[ j ] = MyGetMS();
					}
				}		     
			
			//		Выключающие режим устройства (сигналы)
			for ( j = 0; j < paths[ i ].Dev_off_Cnt; j++ )
				{
				if ( DEV( paths[ i ].Dev_off[ j ] )->State() == 1 ) 
					{
					if ( MyGetMS() - paths[ i ].time_for_off_mode[ j ] > 2000 )
						{
						SetMode( i, 0 );
						}	
					}
				else
					{
					paths[ i ].time_for_off_mode[ j ] = MyGetMS();
					}
				}

			////	Работа с ключом	(только для танков - т.к. работаем по шагам)
			//if ( paths[ i ].Key_signal_Cnt >= 2 )
			//	{
			//	Key_Work( i );
			//	}	   											 

			//	Время работы режима
			if (	paths[ i ].work_time_par < par->getParamCnt() 
				&&	par->getParam( paths[ i ].work_time_par ) > 0 )
				{
				if ( ( MyGetMS() - paths[ i ].start_time ) > 
					par->getParam( paths[ i ].work_time_par ) * 60 * 1000 )
					{
					SetMode( i, 0 );
					}
				}		
			} // if ( GetMode( i ) )   
		else
			{
			//	Проверяем, чтобы не было сигналов выключающих режим
			for ( j = 0; j < paths[ i ].Dev_off_Cnt; j++ )
				{
				if ( DEV( paths[ i ].Dev_off[ j ] )->State() == 1 )
					{
					break;
					}
				}

			//	Проверяем, чтобы были все необходимые для включения сигналы
			for ( k = 0; k < paths[ i ].FB_need_Cnt; k++ )
				{
				if ( DEV( paths[ i ].FB_need[ k ] )->State() == 0 )
					{
					break;
					}
				}

			//	Если их нет, то проверяем "Вкл. режим сигналы"
			if (( j == paths[ i ].Dev_off_Cnt ) && ( k == paths[ i ].FB_need_Cnt ))
				{
				//	Включающие режим устройства (сигналы)			
				for ( j = 0; j < paths[ i ].FB_on_mode_Cnt; j++ )
					{
					if ( DEV( paths[ i ].FB_on_mode[ j ] )->State() == 1 ) 
						{
						if ( MyGetMS() - paths[ i ].time_for_on_mode[ j ] > 2000 )
							{
							SetMode( i, 1 );
							}	
						}
					else
						{
						paths[ i ].time_for_on_mode[ j ] = MyGetMS();
						}	
					}
				}
			}	// if ( !GetMode( i ) )
		} // for ( i = 0; i < statesCnt; i++ )

    return 0;
    }
//-----------------------------------------------------------------------------

int TMyComb::FinalMode( int mode )
    {
    int i, j, k, tRes = 0;

#ifdef DEBUG
    Print ( "\nTMyComb::FinalMode mode = %d.", mode );
#endif

	//	Включаемые устройства
	for ( j = 0; j < paths[ mode ].OVCnt; j++ )
		{
		DEV( paths[ mode ].OV[ j ] )->Off();
		}

	//	Флипование верхними седлами микспруфов
	for ( j = 0; j < paths[ mode ].USCnt; j++ )
		{
		DEV( paths[ mode ].US[ j ] )->Off();
		}

	//	Флипование нижними седлами микспруфов
	for ( j = 0; j < paths[ mode ].LSCnt; j++ )
		{
		DEV( paths[ mode ].LS[ j ] )->Off();
		} 

	//	Подаем управляющие сигналы
	for ( j = 0; j < paths[ mode ].UPRCnt; j++)
		{                    
		DEV( paths[ mode ].UPR[ j ] )->Off();                    
		}

	//	Управляющие устройствами сигналы          
	for ( j = 0; j < paths[ mode ].FB_control_Cnt; j++ )
		{							
		DEV( paths[ mode ].FB_control_dev[ j ] )->Off( );
		}

	//	Включаемые по завершению устройства
	for ( j = 0; j < paths[ mode ].FVCnt; j++ ) 
		{
		DEV( paths[ mode ].FV[ j ])->On();
		}

	//	Включение следующего режима (автоматическое)
	if (	( paths[ mode ].next_mode > 0 )
		&&	( paths[ mode ].next_mode < statesCnt )
		&&	!GetMode( paths[ mode ].next_mode )
		)
		{
		SetMode( paths[ mode ].next_mode, 1 );
		}

	//	Проверяем какие режимы нужно было ВКЛючить - теперь мы их ВЫКЛючаем
	for ( i = 0; i < paths[ mode ].On_mode_Cnt; i++ )
		{
		//	Преобразовываем код режима в номер объекта и номер режима
		int obj_idx;
		int obj_no = paths[ mode ].On_mode[ i ] / 1000;
		int mode_no = paths[ mode ].On_mode[ i ] % 1000;

		//	Ищем объект по его номеру
		for ( int j = 0; j < TTank::TankCnt; j++ )
			{
			if ( g_tanks[ j ]->no == obj_no )
				{
				obj_idx = j;
				break;
				}
			}

		//	Выключаем режимы
		if (	( obj_idx < TTank::TankCnt )
			&&	( mode_no < g_tanks[ obj_idx ]->mode_cnt )
			&&	g_tanks[ obj_idx ]->GetMode( mode_no ) )
			{
			g_tanks[ obj_idx ]->SetMode( mode_no, 0 );
			}		  
		}

    return tRes;
    }
//-----------------------------------------------------------------------------

	void TMyComb::Key_Work( int i )
		{
/*
		key_btn_Drain	= DEV( paths[ i ].Key_signal[ 0 ] ); 
		key_btn_Tank	= DEV( paths[ i ].Key_signal[ 1 ] );

		if ( paths[ i ].Key_signal_Cnt > 2 )
			{
			key_lamp	= DEV( paths[ i ].Key_signal[ 2 ] );
			}

		//	Организуем переключения между шагами по ключу и работу светодиода (общий код для режимов)
		switch ( modes_manager->get_active_step( i ) )
			{
		case 0:		//	Ожидание сигнала с ключа
			if ( key_btn_Drain->State() == ON )
				{
				modes_manager->final( i );
				modes_manager->init( i, 1, this );
				}
																			 
			if ( key_btn_Tank->State() == ON ) 
				{
				modes_manager->final( i );
				modes_manager->init( i, 3, this );
				}

			//	Мигание лампочки
			if ( MyGetMS( ) - paths[ i ].lamp_blink_start_time > 1000 )
				{
				key_lamp->Set( !key_lamp->State( ) );
				paths[ i ].lamp_blink_start_time = MyGetMS( );
				}
			break;

		case 1:	//	В линию вытеснения
			//	Мигание лампочки
			if ( MyGetMS( ) - paths[ i ].lamp_blink_start_time > 1000 )
				{
				key_lamp->Set( !key_lamp->State( ) );
				paths[ i ].lamp_blink_start_time = MyGetMS( );
				}

			//	Если во время дренажа влкючается "В танк", то 
			if ( key_btn_Tank->State() == ON ) 
				{
				modes_manager->final( i );
				modes_manager->init( i, 2, this );		//дренаж+танк
				paths[ i ].next_step_filling = 3;					//с последующим включением "В танк"
				paths[ i ].DT_time = MyGetMS( );
				}
			break;		

		case 2:	//	В линию вытеснения + В танк
			//	Мигание лампочки
			if ( MyGetMS( ) - paths[ i ].lamp_blink_start_time > 1000 )
				{
				key_lamp->Set( !key_lamp->State( ) );
				paths[ i ].lamp_blink_start_time = MyGetMS( );
				}

			//	Если вышло время переходного шага, то включаем наполнение в ...
			if ( ( MyGetMS() - paths[ i ].DT_time ) > 3000 )
				{
				//	Если это переходной после подачи в дренаж, то вкл. в танк
				//	Если это переходной после подачи в танк, то вкл. в дренаж	
				modes_manager->final( i );
				modes_manager->init( i, paths[ i ].next_step_filling, this );
				}	
			break;

		case 3:	//	В танк
			//	Включаем лампочку
			key_lamp->On();

			//	Если при подаче в танк влкючается "В дренаж", то включаем переходной
			if ( key_btn_Drain->State() == ON )
				{
				modes_manager->final( i );
				modes_manager->init( i, 2, this );		//дренаж+танк
				paths[ i ].next_step_filling = 1;					//с последующим включением "В дренаж"
				paths[ i ].DT_time = MyGetMS( );
				}
			break;	
			}	//	switch
*/
		}
//-----------------------------------------------------------------------------

TMyComb::InitParams()
    {
    for ( int j = par->WORK_PARAM_CNT; j < par->getParamCnt(); j++ ) par->setParamM( j, 0 );    
    par->setParamM( P_SEAT_PERIOD, 60 );
    par->setParamM( P_SEAT_TIME, 1000 );

    return 0;
    }
//-----------------------------------------------------------------------------
TMyComb::InitWorkParams()
    {
    int j;
    for ( j = 0; j < par->WORK_PARAM_CNT; j++ ) par->setParamM( j, 0 );

    return 0;
    }
//-----------------------------------------------------------------------------
TMyComb::TMyComb( int no, int stCnt, int parCnt, int workParCnt,
    int tmrCnt ): 
	no( no ),
	statesCnt(stCnt), 
    tmrCnt( tmrCnt ),
    cmd( 0 )

#ifdef USE_SIMPLE_DEV_ERRORS
	,
    is_err( 0 )
#endif // USE_SIMPLE_DEV_ERRORS

    {
#if defined I7188_E || defined I7186_E
    active_dev = new active_device( this, active_device::AT_COMB );
#endif // defined I7188_E || defined I7186_E

    state = new unsigned long[ statesCnt ];
    int i;
    for ( i = 0; i < statesCnt; i++ ) state[ i ]=0;

    par = new TParams( parCnt );
    par->WORK_PARAM_CNT = workParCnt;
    par->LoadParams();

    TMR = new TTimer*[tmrCnt];
    for ( i = 0; i < tmrCnt; i++) TMR[ i ] = new TTimer;
    //run mix proof timers
    if ( tmrCnt < 4 )
        {
#ifdef DEBUG
        Print( "Error creating comb (tmrCnt[%d] < 4)! Change it to correct value via editor WPRG.EXE.\n",
            tmrCnt );    
#endif // DEBUG
        SetGlobalError( EC_DEVICE_DEFINITION, EC_COMB_TIMERS_COUNT, 0 );
        while  ( 1 )
            {
            ShowErrors();
            }
        }
    if ( parCnt < 17 )
        {
#ifdef DEBUG
        Print( "Error creating comb (parCnt[%d] < 17)! Change it to correct value via editor WPRG.EXE.\n",
            tmrCnt );    
#endif // DEBUG
        SetGlobalError( EC_DEVICE_DEFINITION, EC_COMB_PARAMS_COUNT, 0 );
        while  ( 1 )
            {
            ShowErrors();
            }
        }

    TMR[TMR_USEATO]->R();
    TMR[TMR_USEATO]->SZ(par->getParam(P_SEAT_PERIOD)*1000);
    TMR[TMR_USEATO]->S();

    TMR[TMR_USEATC]->R();
    TMR[TMR_USEATC]->SZ(par->getParam(P_SEAT_TIME));

    TMR[TMR_LSEATO]->R();
    TMR[TMR_LSEATO]->SZ(par->getParam(P_SEAT_PERIOD)*1000);
    TMR[TMR_LSEATO]->S();
    TMR[TMR_LSEATO]->ac=par->getParam(P_SEAT_PERIOD)*1000/2;

    TMR[TMR_LSEATC]->R();
    TMR[TMR_LSEATC]->SZ(par->getParam(P_SEAT_TIME));
    LseatOn=0;
    UseatOn=0;

    com_dev = new complex_device( 1, "COMB", 4, i_complex_device::COMPLEX_DEV );
    com_dev->sub_dev[ 0 ] = new single_state( "SINGLE_STATE", 1, state, 
        this, single_state::T_COMB, stCnt ); 
    com_dev->sub_dev[ 1 ] = new complex_state( "STATE", 1, state, 
        this, single_state::T_COMB, stCnt ); 
    com_dev->sub_dev[ 2 ] = new complex_state( "CMD", 1, &cmd, 
        this, single_state::T_COMB ); 
    com_dev->sub_dev[ 3 ] = new dev_param( "PARAM", 1, par );

    mode_name = new char *[ statesCnt * 32 - 1 ];
    for ( i = 0; i < statesCnt * 32; i++ )
        {
        mode_name[ i ] = 0;
        }

#ifdef USE_SIMPLE_DEV_ERRORS
    err_str[ 0 ] = 0;
#endif // USE_SIMPLE_DEV_ERRORS
    }
//-----------------------------------------------------------------------------
TMyComb::~TMyComb(void) 
    {
    int i;
    for ( i = 0; i<tmrCnt; i++) delete TMR[ i ];  
    delete TMR;
    }
//-----------------------------------------------------------------------------

#ifdef USE_SIMPLE_DEV_ERRORS
int TMyComb::reset_error()
    {
    is_err       = 0;
    err_str[ 0 ] = 0;
    error_number = 0;
    return 0;
    }
//-----------------------------------------------------------------------------
int TMyComb::set_err_msg( const char *err_msg, int mode, ERR_MSG_TYPES type )
    {
	err_type = type;

    switch ( type )
        {
    case ERR_CANT_ON:
        sprintf( err_str, "НЕ включен режим гребенки №%.1d \"%.40s\"- %.40s!", 
            mode + 1, get_mode_name( mode ), err_msg );
        break;

    case ERR_ON_WITH_ERRORS:
        sprintf( err_str, "Включен с ошибкой режим гребенки №%.1d \"%.40s\"- %.40s!", 
            mode + 1, get_mode_name( mode ), err_msg );
        break;

    case ERR_OFF:
        sprintf( err_str, "Отключен режим гребенки №%.1d \"%.40s\"- %.40s!", 
            mode + 1, get_mode_name( mode ), err_msg );
        break;

    case ERR_SIMPLE:
        snprintf( err_str, sizeof( err_str ), "%.80s", err_msg );
        break;

    default:
#ifdef DEBUG
        Print( "Error TMyComb::set_err_msg(...) - unknown error type!\n" );
        Print( "Press any key to continue." );
        Getch();
        Print( "\n" );
#endif // DEBUG
        snprintf( err_str, sizeof( err_str ), 
            "Режим гребенки %.1d \"%.40s\"- %.40s!", 
            mode + 1, get_mode_name( mode ), err_msg );
        break;
        }

#ifdef DEBUG
    Print( "err_msg = %s", err_msg );
#endif // DEBUG

    ++is_err;
    error_number = mode;

    return 0;
    }
//-----------------------------------------------------------------------------
const char* TMyComb::get_mode_name( int mode ) const
    {
    if ( mode >= 0 && mode <  32 * statesCnt )
        {
        return mode_name[ mode ] ? mode_name[ mode ] : "?";
        }

    return "?";    
    }
//-----------------------------------------------------------------------------
#pragma  argsused
int TMyComb::is_check_mode( int mode ) const
    {
    return 0;
    }
//-----------------------------------------------------------------------------
const char* TMyComb::get_type() const
	{
	switch ( err_type )
		{
	case ERR_CANT_ON:
	case ERR_ON_WITH_ERRORS:
		return "ответ";

	case ERR_OFF:    
	case ERR_DURING_WORK:
	case ERR_SIMPLE:
		return "сообщение";
		}

	return "?";
	}
//-----------------------------------------------------------------------------
int TMyComb::get_priority() const
	{
	switch ( err_type )
		{
	case ERR_CANT_ON:
	case ERR_ON_WITH_ERRORS:
		return error_tech_device::P_ANSWER;

	case ERR_OFF:    
	case ERR_DURING_WORK:
	case ERR_SIMPLE:
		return error_tech_device::P_MESSAGE;
		}

	return error_tech_device::P_ALARM;
	}
//-----------------------------------------------------------------------------
int TMyComb::is_any_mode() const
	{
	for ( int i = 0; i < statesCnt; i++ )
		{
		if ( state[ i ] > 0  ) return 1;
		}

	return 0;
	}
//-----------------------------------------------------------------------------
#endif // USE_SIMPLE_DEV_ERRORS
//-----------------------------------------------------------------------------