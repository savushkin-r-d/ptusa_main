/// @file wago.cs
/// @brief Классы, которые реализуют функциональность для работы с Wago.
/// 
/// @author  Иванюк Дмитрий Сергеевич.
/// 
/// @par Текущая версия:
/// @$Rev$.\n
/// @$Author$.\n
/// @$Date::                     $.
/// 
using System;
using System.Collections;
using System.Collections.Generic;

using Visio = Microsoft.Office.Interop.Visio;

/// <summary> Устройства Wago проекта (модули, контроллеры...).</summary>
namespace wago
    {

	public class service_data
		{
								
		internal Dictionary<io_module.TYPES, string> DI_modules;
		internal Dictionary<io_module.TYPES, string> DO_modules;
		internal Dictionary<io_module.TYPES, string> AI_modules;
		internal Dictionary<io_module.TYPES, string> AO_modules;
		internal Dictionary<io_module.TYPES, string> SYS_modules;
		internal Dictionary<io_module.TYPES, string> Special_modules;

        /// <summary> Default constructor. </summary>
        ///
        /// <remarks> ASV, 01.09.2011. </remarks>
		public service_data()
            {
			DI_modules = new Dictionary<io_module.TYPES, string>();
			DI_modules.Add( io_module.TYPES.T_402, "750-402 ( 4 DI )" );
			DI_modules.Add( io_module.TYPES.T_430, "750-430 ( 8 DI )" );
			DI_modules.Add( io_module.TYPES.T_1405, "750-1405 ( 16 DI 24 VDC )" );
			DI_modules.Add( io_module.TYPES.T_1415, "750-1415 ( 8 DI 24 VDC )" );
			DI_modules.Add( io_module.TYPES.T_1420, "750-1420 ( 4 DI 24 VDC )" );

			DO_modules = new Dictionary<io_module.TYPES, string>();
			DO_modules.Add( io_module.TYPES.T_504, "750-504 ( 4 DO )" );
			DO_modules.Add( io_module.TYPES.T_512, "750-512 ( 2 DO )" );
			DO_modules.Add( io_module.TYPES.T_530, "750-530 ( 8 DO )" );
			DO_modules.Add( io_module.TYPES.T_1504, "750-1504 ( 16 DO 24 VDC )" );
			DO_modules.Add( io_module.TYPES.T_1515, "750-1515 ( 8 DO 24 VDC )" );

			AI_modules = new Dictionary<io_module.TYPES, string>();
			AI_modules.Add( io_module.TYPES.T_455, "750-455 ( 4 AI 4-20 mA )" );
			AI_modules.Add( io_module.TYPES.T_460, "750-460 ( 4 AI RTD )" );
			AI_modules.Add( io_module.TYPES.T_461, "750-461 ( 2 AI RTD )" );
			AI_modules.Add( io_module.TYPES.T_461_002, "750-461/002 ( 2 AI 10R-1k2 )" );
			AI_modules.Add( io_module.TYPES.T_466, "750-466 ( 2 AI 4-20 mA )" );
			AI_modules.Add( io_module.TYPES.T_493, "750-493 ( 3-Phase Power Measurement )" );

			AO_modules = new Dictionary<io_module.TYPES, string>();
			AO_modules.Add( io_module.TYPES.T_554, "750-554 ( 2 AO 4-20 mA )" );

			SYS_modules = new Dictionary<io_module.TYPES, string>();
			SYS_modules.Add( io_module.TYPES.T_600, "750-600 ( End Module )" );
			SYS_modules.Add( io_module.TYPES.T_602, "750-602 ( Power Supply Module DC 24 V )" );
			SYS_modules.Add( io_module.TYPES.T_612, "750-612 ( Power Supply Module AC/DC 230 V )" );
			SYS_modules.Add( io_module.TYPES.T_613, "750-613 ( Bus Power Supply DC 24 V )" );
			SYS_modules.Add( io_module.TYPES.T_627, "750-627 ( End Module )" );
			SYS_modules.Add( io_module.TYPES.T_628, "750-628 ( Coupler Module )" );

			Special_modules = new Dictionary<io_module.TYPES, string>();
			Special_modules.Add( io_module.TYPES.T_638, "750-638 ( 2 CTR )" );
			Special_modules.Add( io_module.TYPES.T_655, "750-655 ( AS-Interface Master Module )" );
			}

	}
  
    //--------------------------------------------------------------------------
    /// <summary> Модуль ввода/вывода WAGO. </summary>
    ///
    /// <remarks> Id, 01.08.2011. </remarks>
    public class io_module
        {
        /// <summary> Виды модулей. </summary>
        public enum KINDS
            {
            SPECIAL,
            DO,
            DI,
            AO,
            AI,
			SYSTEM,
            };

        /// <summary> Типы модулей. </summary>
        public enum TYPES 
            {
            T_UNKNOWN = 0,

			//	DI modules
			T_402 = 402,
			T_430 = 430,
			T_1405 = 1405,
			T_1415 = 1415,
			T_1420 = 1420,
			
			//	DO modules
			T_504 = 504,
			T_512 = 512,
			T_530 = 530,
			T_1504 = 1504,
			T_1515 = 1515,

			//	AI modules
            T_455 = 455,
			T_460 = 460,
			T_461 = 461,
			T_461_002 = 4612,
            T_466 = 466,
			T_638 = 638,

			//	AO modules
			T_554 = 554,

			//	System modules
			T_600 = 600,
			T_602 = 602,
			T_612 = 612,
			T_613 = 613,
			T_627 = 627,
			T_628 = 628,

			//	Special modules
			T_493 = 493,	//	3-Phase	counter
			T_655 = 655,	//	AS-interface
			};

        /// <summary> Количество клемм. </summary>
        public enum CLAMPS_COUNT
            {
			CLAMP_0		=	0,
            СLAMP_8		=	8,
            СLAMP_16	=	16,
            };

        /// <summary> Вид модуля. </summary>
        internal KINDS kind;          

        /// <summary> Тип модуля. </summary>
        internal TYPES type;          

        /// <summary> Общее количество клемм. </summary>
        public CLAMPS_COUNT total_clamps;  

        /// <summary> Порядковый номер в сборке. </summary>
        internal int order_number; 

        /// <summary> Номер узла. </summary>
        internal int node_number;  

        /// <summary> Флаг свободности клеммы. </summary>
        public bool[] free_clamp_flags; 

        /// <summary> Флаг доступности клеммы. </summary>
        public bool[] available_clamp_flags;

        /// <summary> Constructor. На основе свойств фигуры создается объект.
        /// </summary>
        ///
        /// <remarks> Id, 01.08.2011. </remarks>
        ///
        /// <exception cref="Exception"> Thrown when exception. </exception>
        ///
        /// <param name="shape"> Visio фигура модуля. </param>
        public io_module( Visio.Shape shape )
            {
            this.clamp_names = new List<string>();

            this.shape = shape;

            type = ( TYPES ) Convert.ToUInt16( shape.Cells[ "Prop.type" ].FormulaU );
            order_number = Convert.ToUInt16( shape.Cells[ "Prop.order_number" ].FormulaU );
            node_number = Convert.ToUInt16( shape.Cells[ "Prop.node_number" ].FormulaU );

            //Определяем количество клемм.
            switch( type )
                {
				//	System modules
				case TYPES.T_600:
				case TYPES.T_602:
				case TYPES.T_612:
				case TYPES.T_613:
				case TYPES.T_627:
				case TYPES.T_628:
					total_clamps = CLAMPS_COUNT.CLAMP_0;
					break;

				//	DI modules
				case TYPES.T_402:
				case TYPES.T_430:
				//	DO modules
				case TYPES.T_504:
				case TYPES.T_512:
				case TYPES.T_530:
				//	AI modules
				case TYPES.T_455:
				case TYPES.T_460:
				case TYPES.T_461:
				case TYPES.T_461_002:
				case TYPES.T_466:
				case TYPES.T_638:
				//	AO modules
				case TYPES.T_554:
                    total_clamps = CLAMPS_COUNT.СLAMP_8;
                    break;
			  
				//	DI modules	
				case TYPES.T_1405:
				case TYPES.T_1415:
				case TYPES.T_1420:
				//	DO modules
				case TYPES.T_1504:
				case TYPES.T_1515:
					total_clamps = CLAMPS_COUNT.СLAMP_16;
					break;

                default:
                    Exception exc = new Exception(
                        "Неизвестный тип модуля - \"" + type + "\"!" );
                    throw exc;
                }
		//---------------------------------------------------------------------

		//	Создаем заданное количество клемм
		if ( total_clamps != CLAMPS_COUNT.CLAMP_0 )
			{
			free_clamp_flags = new bool[ ( int ) total_clamps ];
			available_clamp_flags = new bool[ ( int ) total_clamps ];
			suitable_clamp_flags = new bool[ ( int ) total_clamps ];

			for ( int i = 0; i < ( int ) total_clamps; i++ )
				{
				free_clamp_flags[ i ] = true;
				available_clamp_flags[ i ] = false;
				suitable_clamp_flags[ i ] = false;

				clamp_names.Add( "clamp_" + ( i + 1 ) );
				}
			}
		//---------------------------------------------------------------------

			//Определяем расположение клемм на модуле
			switch ( type )
				{
				//	Только для 16-и канальных модулей расположение клем отличается от заданного
				case TYPES.T_1405:
				case TYPES.T_1504:
					for ( int i = 0; i < ( int ) total_clamps; i++ )
						{
						//	Создаем новое имя (чтобы не было конфликта имен)
						shape.Shapes[ "type_skin" ].Shapes[ clamp_names[ i ] ].Name = 
							shape.Shapes[ "type_skin" ].Shapes[ clamp_names[ i ] ].Name + "_";
						}

					for ( int i = 0; i < ( int ) total_clamps; i++ )
						{	
						//	Фиксируем новое имя (для наглядности)
						string temp_name = clamp_names[ i ] +  "_";
																									  
						string new_name = "clamp_" + 
							Convert.ToString( shape.Shapes[ "type_skin" ].Shapes[ temp_name ].Data1 );

						//	Переименовываем клемы
					    shape.Shapes[ "type_skin" ].Shapes[	temp_name ].Name = new_name;
						}


					//	Изменяем текст (номер клемы на фигуре)
					for ( int i = 0; i < ( int ) total_clamps; i++ )
						{
						shape.Shapes[ "type_skin" ].Shapes[ clamp_names[ i ] ].Text = Convert.ToString( i + 1 );
						}

					break;					
				}	
		//---------------------------------------------------------------------

			//Определяем доступные клеммы
			switch ( type )
				{
				//	Возможна отдельное определение по всем этим разделам
				//		при другой нумерации клем (гориз или вертикальная)

				//	С 2-мя доступными клеммами
				case TYPES.T_461:
				case TYPES.T_461_002:
				case TYPES.T_466:
				case TYPES.T_512:
				case TYPES.T_554:
				case TYPES.T_638:
				    available_clamp_flags[ 1 - 1 ] = true;
					available_clamp_flags[ 5 - 1 ] = true;
					break;


				//	С 4-мя доступными клеммами
				case TYPES.T_402:
				case TYPES.T_455:
				case TYPES.T_460:
				case TYPES.T_504:
					available_clamp_flags[ 1 - 1 ] = true;
					available_clamp_flags[ 4 - 1 ] = true;
					available_clamp_flags[ 5 - 1 ] = true;
					available_clamp_flags[ 8 - 1 ] = true;
					break;

				case TYPES.T_1420:
					available_clamp_flags[ 1 - 1 ] = true;
					available_clamp_flags[ 6 - 1 ] = true;
					available_clamp_flags[ 9 - 1 ] = true;
					available_clamp_flags[ 14 - 1 ] = true;
					break;


				//	С 8-ю доступными клеммами
				case TYPES.T_430:
				case TYPES.T_530:
				case TYPES.T_1415:
				case TYPES.T_1515:
					for ( int i = 0; i < 8; i++ )
						{
						available_clamp_flags[ i ] = true;
						}
					break;


				//	С 16-мя доступными клемами
				case TYPES.T_1405:
				case TYPES.T_1504:
					for ( int  i = 0; i < 16; i++ )
						{
						available_clamp_flags[ i ] = true;
						}
					break;
				}
		//---------------------------------------------------------------------

            //Определяем вид модуля
            switch( type )
                {
				//	DO modules
				case TYPES.T_504:
				case TYPES.T_512:
				case TYPES.T_530:
				case TYPES.T_1504:
				case TYPES.T_1515:
					kind = KINDS.DO;
					break;

				//	DI modules
				case TYPES.T_402:
				case TYPES.T_430:
				case TYPES.T_1405:
				case TYPES.T_1415:
				case TYPES.T_1420:
					kind = KINDS.DI;
					break;

				//	AO modules
				case TYPES.T_554:
					kind = KINDS.AO;
					break;

				//	AI modules
				case TYPES.T_455:
				case TYPES.T_460:
				case TYPES.T_461:
				case TYPES.T_461_002:
				case TYPES.T_466:
				case TYPES.T_638:
					kind = KINDS.AI;
					break;

				//	Special modules
				case TYPES.T_493:
				case TYPES.T_655:
					kind = KINDS.SPECIAL;
					break;

				//	System modules
				case TYPES.T_602:
				case TYPES.T_612:
				case TYPES.T_613:
				case TYPES.T_600:
				case TYPES.T_627:
				case TYPES.T_628:
					kind = KINDS.SYSTEM;
					break;
                }
            }

        /// <summary> Сохранение в виде скрипта Lua. </summary>
        ///
        /// <remarks> Id, 01.08.2011. </remarks>
        ///
        /// <returns> Lua-скрипт, представляющий объект. </returns>
        public string lua_save()
            {
            return "{ " + type + " }";
            }

        /// <summary> Фигура Visio модуля. </summary>
        internal Visio.Shape shape;        

        /// <summary> Имена клемм. </summary>
        List<string> clamp_names;

        /// <summary> Флаг подходящих для привязки клемм. </summary>
        public bool[] suitable_clamp_flags;    

        /// <summary> Пометка доступных клемм (для режима привязки устройств).
        /// </summary>
        ///
        /// <remarks> Id, 01.08.2011. </remarks>
        public void mark_suitable()
            {
            for( int i = 0; i < ( int ) total_clamps; i++ )
                {
                if( available_clamp_flags[ i ] && free_clamp_flags[ i ] )
                    {
                    shape.Shapes[ "type_skin" ].Shapes[ clamp_names[ i ] ].Cells[
                        "FillForegnd" ].FormulaU = "RGB( 127, 127, 127 )";

                    suitable_clamp_flags[ i ] = true;
                    }
                }
            }

        /// <summary> Снятие пометки о доступности клемм. </summary>
        ///
        /// <remarks> Id, 01.08.2011. </remarks>
        public void unmark_suitable()
            {
            if( magnified_clamp > -1 )
                {
                unmagnify_clamp( magnified_clamp );
                magnified_clamp = -1;
                }

            for( int i = 0; i < ( int ) total_clamps; i++ )
                {
                shape.Shapes[ "type_skin" ].Shapes[ clamp_names[ i ] ].Cells[
                       "FillForegnd" ].FormulaU = "RGB( 200, 200, 200 )";

                suitable_clamp_flags[ i ] = false;
                }
            }

        /// <summary> Пометка клеммы как свободной. </summary>
        ///
        /// <remarks> Id, 01.08.2011. </remarks>
        ///
        /// <param name="clamp"> Номер помечаемой клеммы ( >= 0 ). </param>
        public void free( int clamp )
            {
            free_clamp_flags[ clamp ] = true;
            }

        /// <summary> Пометка клеммы как занятой. </summary>
        ///
        /// <remarks> Id, 01.08.2011. </remarks>
        ///
        /// <param name="clamp"> Номер помечаемой клеммы ( >= 0 ). </param>
        public void use( int clamp )
            {
            free_clamp_flags[ clamp ] = false;
            }

        /// <summary> Получение клеммы, выделенной мышкой. </summary>
        ///
        /// <remarks> Id, 01.08.2011. </remarks>
        ///
        /// <returns> Номер клеммы, выделенной мышкой ( >= 0
        /// 		   ). </returns>
        public int get_mouse_selection()
            {
            return magnified_clamp;
            }

        /// В текущий момент выделенная мышкой клемма ( >= 0 ).
        private int magnified_clamp = -1;

        /// <summary> Выделение клеммы под курсором мыши. </summary>
        ///
        /// <remarks> Id, 01.08.2011. </remarks>
        ///
        /// <param name="x"> The x coordinate. </param>
        /// <param name="y"> The y coordinate. </param>
        public void magnify_on_mouse_move( double x, double y )
            {
            if( shape.HitTest( x, y, 0.01 ) > 0 )
                {
                for( int i = 0; i < ( int ) total_clamps; i++ )
                    {
                    double new_x = x;
                    double new_y = y;

                    new_x -= shape.Cells[ "PinX" ].Result[ 0 ] -
                        shape.Cells[ "Width" ].Result[ 0 ] / 2;
                    new_x -= shape.Shapes[ "type_skin" ].Cells[ "PinX" ].Result[ 0 ] -
                        shape.Shapes[ "type_skin" ].Cells[ "Width" ].Result[ 0 ] / 2;

                    new_y -= shape.Cells[ "PinY" ].Result[ 0 ] -
                        shape.Cells[ "Height" ].Result[ 0 ] / 2; ;
                    new_y -= shape.Shapes[ "type_skin" ].Cells[ "PinY" ].Result[ 0 ] -
                        shape.Shapes[ "type_skin" ].Cells[ "Height" ].Result[ 0 ] / 2;

                    if( shape.Shapes[ "type_skin" ].Shapes[
                        clamp_names[ i ] ].HitTest( new_x, new_y, 0.01 ) > 0 )
                        {
                        if( suitable_clamp_flags[ i ] == false )
                            {
                            //Клемма недоступна, выходим.
                            break;
                            }

                        if( magnified_clamp != i )
                            {
                            if( magnified_clamp > -1 ) unmagnify_clamp( magnified_clamp );
                            magnified_clamp = i;
                            magnify_clamp( magnified_clamp );
                            }
                        return;
                        }
                    }

                if( magnified_clamp > -1 )
                    {
                    unmagnify_clamp( magnified_clamp );
                    magnified_clamp = -1;
                    }
                }
            else
                {
                if( magnified_clamp > -1 )
                    {
                    unmagnify_clamp( magnified_clamp );
                    magnified_clamp = -1;
                    }
                }
            }

        /// <summary> Увеличить размер клеммы. </summary>
        ///
        /// <remarks> Id, 01.08.2011. </remarks>
        ///
        /// <param name="clamp"> Номер клеммы ( >= 0 ). </param>
        public void magnify_clamp( int clamp )
            {
			//	module_number	-	Просто образец размера
            shape.Shapes[ "type_skin" ].Shapes[ clamp_names[ clamp ] ].Cells[
                "Width" ].FormulaU = string.Format( "=Sheet.{0}!Width",
                shape.Shapes[ "module_number" ].ID );

            shape.Shapes[ "type_skin" ].Shapes[ clamp_names[ clamp ] ].Cells[
                "Height" ].FormulaU = string.Format( "=Sheet.{0}!Height",
                shape.Shapes[ "module_number" ].ID );
            }

        /// <summary> Вернуть нормальный размер клеммы. </summary>
        ///
        /// <remarks> Id, 01.08.2011. </remarks>
        ///
        /// <param name="clamp"> Номер помечаемой клеммы ( >= 0 ). </param>
        void unmagnify_clamp( int clamp )
            {
			//	module_number	-	Просто образец размера
            shape.Shapes[ "type_skin" ].Shapes[ clamp_names[ clamp ]
                ].Cells[ "Width" ].FormulaU = string.Format( "=Sheet.{0}!Width*0.5",
                shape.Shapes[ "module_number" ].ID );

            shape.Shapes[ "type_skin" ].Shapes[ clamp_names[ clamp ]
                ].Cells[ "Height" ].FormulaU = string.Format( "=Sheet.{0}!Height*0.5",
                shape.Shapes[ "module_number" ].ID );
            }

        /// <summary> "Подсветить" модуль и заданную клемму. При этом подсветка
        /// зависит от вида модуля. </summary>
        ///
        /// <remarks> Id, 01.08.2011. </remarks>
        ///
        /// <param name="clamp"> Номер подсвечиваемой клеммы ( >= 0 ). </param>
        public void activate( int clamp )
            {
            string color = "0";

            switch( kind )
                {
                case io_module.KINDS.AI:        //Зеленый.
                    color = "3";
                    break;

                case io_module.KINDS.AO:        //Синий.
                    color = "4";
                    break;

                case io_module.KINDS.DI:        //Желтый.
                    color = "5";
                    break;

                case io_module.KINDS.DO:        //Красный.
                    color = "2";
                    break;

                case io_module.KINDS.SPECIAL:   //Серый.
                    color = "14";
                    break;
                }

            shape.Shapes[ "red_boder" ].Cells[ "LineColor" ].FormulaU = color;
            shape.Shapes[ "red_boder" ].Cells[ "LineColorTrans" ].FormulaU = "0%";

            shape.Shapes[ "type_skin" ].Shapes[ clamp_names[ clamp ] ].Cells[
                "FillForegnd" ].FormulaU = color;

            unmagnify_clamp( clamp );
            }

        /// <summary> Убрать подсветку с модуля. </summary>
        ///
        /// <remarks> Id, 01.08.2011. </remarks>
        public void deactivate()
            {
            int i = 0;
            foreach( string clamp in clamp_names )
                {
                shape.Shapes[ "type_skin" ].Shapes[ clamp ].Cells[
                    "FillForegnd" ].FormulaU = "16";

                unmagnify_clamp( i++ );
                }

			shape.Shapes[ "red_boder" ].Cells[ "LineColorTrans" ].FormulaU = "100%";
            }
        }

    //--------------------------------------------------------------------------
    /// <summary> Промышленный контроллер. </summary>
    ///
    /// <remarks> Id, 01.08.2011. </remarks>
    public class PAC
        {
        /// <summary> Фигура Visio контроллера. </summary>
        public Visio.Shape shape; 

        /// <summary> IP-адрес контроллера. </summary>
        public string ip_addres;  

        /// <summary> Имя контроллера. </summary>
        public string PAC_name;

		/// <summary> Выбранный в данный момент модуль </summary>
		internal io_module current_module = null; 

        private List<io_module> io_modules; ///< Модули ввода\вывода.  

        /// <summary> Получение модулей ввода\вывода контроллера. </summary>
        ///
        /// <remarks> Id, 15.08.2011. </remarks>
        ///
        /// <returns> Список модулей ввода\вывода. </returns>
        public List<io_module> get_io_modules()
            {
            return io_modules;
            }

        /// <summary> Constructor. </summary>
        ///
        /// <remarks> Id, 15.08.2011. </remarks>
        ///
        /// <param name="PAC_name">  Имя контроллера. </param>
        /// <param name="ip_addres"> IP-адрес контроллера. </param>
        /// <param name="shape">     Фигура Visio контроллера. </param>
        public PAC( string PAC_name, string ip_addres, Visio.Shape shape )
            {
            this.PAC_name = PAC_name;
            this.ip_addres = ip_addres;

            io_modules = new List<io_module>();

            this.shape = shape;
            }

        /// <summary> Снятие пометки о доступности клемм. </summary>
        ///
        /// <remarks> Id, 15.08.2011. </remarks>
        public void unmark()
            {
            foreach( io_module module in io_modules )
                {
                module.unmark_suitable();
                }
            }

        /// <summary> Установка пометки о доступности клемм. </summary>
        ///
        /// <remarks> Id, 15.08.2011. </remarks>
        ///
        /// <param name="kind"> Вид клемм, среди которых ищутся доступные. </param>
        public void mark_suitable( string kind )
            {
            io_module.KINDS io_module_kind = 0;
            switch( kind )
                {
                case "AI":
                    io_module_kind = io_module.KINDS.AI;
                    break;

                case "AO":
                    io_module_kind = io_module.KINDS.AO;
                    break;

                case "DI":
                    io_module_kind = io_module.KINDS.DI;
                    break;

                case "DO":
                    io_module_kind = io_module.KINDS.DO;
                    break;

                case "SPECIAL":
                    io_module_kind = io_module.KINDS.SPECIAL;
                    break;
                }

            foreach( io_module module in io_modules )
                {
                module.unmark_suitable();

                if( io_module_kind == module.kind )
                    {
                    module.mark_suitable();
                    }
                }
            }

        /// <summary> Lua save. </summary>
        ///
        /// <remarks> Id, 15.08.2011. </remarks>
        ///
        /// <param name="prefix"> (optional) the prefix. </param>
        ///
        /// <returns> . </returns>
        public string lua_save( string prefix = "\t" )
            {
            string res = prefix + "{\n";
            res += prefix + "n_type    = 0,\n";
            res += prefix + "ip_addres = \"" + ip_addres + "\",\n";

            res += prefix + "modules   = \n";
            res += prefix + "\t{\n";
            foreach( io_module module in io_modules )
                {
                res += prefix + "\t" + module.lua_save() + ",\n";
                }
            res += prefix + "\t}\n";
            res += prefix + "}";


            return res;
            }

        /// <summary> Добавление i/o модуля. </summary>
        ///
        /// <remarks> Id, 17.08.2011. </remarks>
        ///
        /// <param name="shape"> Фигура Visio модуля. </param>
        public void add_io_module( Visio.Shape shape )
            {
            io_modules.Add( new io_module( shape ) );
            }

		/// <summary> Вставка i/o модуля. </summary>
		///
		/// <remarks> ASV, 12.09.2011. </remarks>
		///
		/// <param name="shape"> Фигура Visio модуля. </param>
		public void insert_io_module( int index, Visio.Shape shape )
            {
            io_modules.Insert( index, new io_module( shape ) );
            }
        }

    //--------------------------------------------------------------------------
    /// <summary> Канал ввода\вывода Wago. </summary>
    ///
    /// <remarks> Id, 17.08.2011. </remarks>
    public class wago_channel
        {

        /// <summary> Вид канала. </summary>
        internal io_module.KINDS kind;

        /// <summary> Модуль, к которому привязан канал. </summary>
        internal io_module module;

        /// <summary> Клемма, к которой привязан канал. </summary>
        internal int clamp;

        /// <summary> Constructor. </summary>
        ///
        /// <remarks> Id, 17.08.2011. </remarks>
        ///
        /// <param name="module"> The module. </param>
        /// <param name="clamp">  The clamp. </param>
        public wago_channel( io_module module, int clamp )
            {
            this.module = module;
            this.clamp = clamp;
            }

        /// <summary> Constructor. </summary>
        ///
        /// <remarks> Id, 17.08.2011. </remarks>
        ///
        /// <param name="kind"> The kind. </param>
        public wago_channel( io_module.KINDS kind )
            {
            this.module = null;
            this.clamp = 0;

            this.kind = kind;
            }

        /// <summary> Привязка к модулю. </summary>
        ///
        /// <remarks> Id, 17.08.2011. </remarks>
        ///
        /// <param name="pac">    The pac. </param>
        /// <param name="module"> The module. </param>
        /// <param name="clamp">  The clamp. </param>
        public void set( PAC pac, int module, int clamp )
            {
            if( module > 0 )
                {
				//Поиск по shape объекта device.
				this.module = pac.get_io_modules().Find( delegate( io_module mod )
				{
					return mod.order_number == module;
				}
				);

                this.clamp = clamp - 1;
                this.module.use( clamp );
                }
            }

        }

    //--------------------------------------------------------------------------
    /// <summary> Устройство с каналами ввода\вывода Wago. </summary>
    ///
    /// <remarks> Id, 17.08.2011. </remarks>
    public class wago_device
        {

        /// <summary> Каналы ввода\вывода. </summary>
        internal Dictionary<string, wago_channel> wago_channels;

        /// <summary> Default constructor. </summary>
        ///
        /// <remarks> Id, 17.08.2011. </remarks>
        public wago_device()
            {
            wago_channels = new Dictionary<string, wago_channel>();
            }

        /// <summary> Добавление ввода\вывода. </summary>
        ///
        /// <remarks> Id, 17.08.2011. </remarks>
        ///
        /// <param name="name"> Имя канала. </param>
        /// <param name="kind"> Вид канала. </param>
        public void add_wago_channel( string name, io_module.KINDS kind )
            {
            wago_channels.Add( name, new wago_channel( kind ) );
            }
        }  
    }
