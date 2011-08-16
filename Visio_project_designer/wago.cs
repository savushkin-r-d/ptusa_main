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
using System.Collections.Generic;

using Visio = Microsoft.Office.Interop.Visio;

namespace wago
    {

    //-----------------------------------------------------------------------------
    /// <summary> Модуль ввода/вывода WAGO. </summary>
    ///
    /// <remarks> Id, 01.08.2011. </remarks>
    public class io_module
        {
        public enum KINDS ///< Виды модулей. 
            {
            SPECIAL,
            DO,
            DI,
            AO,
            AI
            };

        public enum TYPES ///< Типы модулей. 
            {
            T_UNKNOWN = 0,
            T_504 = 504,
            T_402 = 402,
            T_466 = 466
            };

        public enum CLAMPS_COUNT ///< Количество клемм. 
            {
            СLAMP_8 = 8,
            СLAMP_16 = 16
            };

        internal KINDS kind;          ///< Вид модуля. 
        internal TYPES type;          ///< Тип модуля. 
        public CLAMPS_COUNT total_clamps;  ///< Общее количество клемм. 

        internal int order_number; ///< Порядковый номер в сборке.
        internal int node_number;  ///< Номер узла.

        public bool[] free_clamp_flags;      ///< Флаг свободности клеммы.
        public bool[] available_clamp_flags; ///< Флаг доступности клеммы.

        /// <summary> Constructor. На основе свойств фигуры создается объект.</summary>
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
                case TYPES.T_402:
                case TYPES.T_466:
                case TYPES.T_504:
                    total_clamps = CLAMPS_COUNT.СLAMP_8;

                    free_clamp_flags = new bool[ ( int ) total_clamps ];
                    available_clamp_flags = new bool[ ( int ) total_clamps ];
                    suitable_clamp_flags = new bool[ ( int ) total_clamps ];

                    for( int i = 0; i < ( int ) total_clamps; i++ )
                        {
                        free_clamp_flags[ i ] = true;
                        available_clamp_flags[ i ] = false;
                        suitable_clamp_flags[ i ] = false;

                        clamp_names.Add( "clamp_" + ( i + 1 ) );
                        }

                    available_clamp_flags[ 1 - 1 ] = true;
                    available_clamp_flags[ 4 - 1 ] = true;
                    available_clamp_flags[ 5 - 1 ] = true;
                    available_clamp_flags[ 8 - 1 ] = true;
                    break;

                default:
                    Exception exc = new Exception(
                        "Неизвестный тип модуля - \"" + type + "\"!" );
                    throw exc;
                }

            //Определяем вид.
            switch( type )
                {
                case TYPES.T_402:
                    kind = KINDS.DI;
                    break;

                case TYPES.T_466:
                    kind = KINDS.AI;
                    break;

                case TYPES.T_504:
                    kind = KINDS.DO;
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

        Visio.Shape shape;           ///< Фигура Visio модуля.
        List<string> clamp_names; ///< Имена клемм.

        public bool[] suitable_clamp_flags;      ///< Флаг подходящих для привязки клемм.

        /// <summary> Пометка доступных клемм (для режима привязки устройств). </summary>
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
            shape.Shapes[ "type_skin" ].Shapes[ clamp_names[ clamp ] ].Cells[
                "Width" ].FormulaU = string.Format( "=Sheet.{0}!Width*0.66",
                shape.Shapes[ "type_skin" ].ID );

            shape.Shapes[ "type_skin" ].Shapes[ clamp_names[ clamp ] ].Cells[
                "Height" ].FormulaU = string.Format( "=Sheet.{0}!Height*0.09",
                shape.Shapes[ "type_skin" ].ID );
            }

        /// <summary> Вернуть нормальный размер клеммы. </summary>
        ///
        /// <remarks> Id, 01.08.2011. </remarks>
        ///
        /// <param name="clamp"> Номер помечаемой клеммы ( >= 0 ). </param>
        void unmagnify_clamp( int clamp )
            {
            shape.Shapes[ "type_skin" ].Shapes[ clamp_names[ clamp ]
                ].Cells[ "Width" ].FormulaU = string.Format( "=Sheet.{0}!Width*0.33",
                shape.Shapes[ "type_skin" ].ID );

            shape.Shapes[ "type_skin" ].Shapes[ clamp_names[ clamp ]
                ].Cells[ "Height" ].FormulaU = string.Format( "=Sheet.{0}!Height*0.045",
                shape.Shapes[ "type_skin" ].ID );
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

    //-----------------------------------------------------------------------------
    /// <summary> Промышленный контроллер. </summary>
    ///
    /// <remarks> Id, 01.08.2011. </remarks>
    public class PAC
        {
        public Visio.Shape shape; ///< Фигура Visio контроллера.

        public string ip_addres;  ///< IP-адрес контроллера.  
        public string PAC_name;   ///< Имя контроллера.  

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

        public void add_io_module( Visio.Shape shape )
            {
            io_modules.Add( new io_module( shape ) );
            }
        }

    public class wago_channel
        {
        internal io_module.KINDS kind;

        internal io_module module;
        internal int clamp;

        public wago_channel( io_module module, int clamp )
            {
            this.module = module;
            this.clamp = clamp;
            }

        public wago_channel( io_module.KINDS kind )
            {
            this.module = null;
            this.clamp = 0;

            this.kind = kind;
            }

        public void set( PAC pac, int module, int clamp )
            {
            if( module > 0 )
                {
                this.module = pac.get_io_modules()[ module ];
                this.clamp = clamp;
                this.module.use( clamp );
                }
            }

        }

    public class wago_device
        {
        internal Dictionary<string, wago_channel> wago_channels;

        public wago_device()
            {
            wago_channels = new Dictionary<string, wago_channel>();
            }

        public void add_wago_channel( string name, io_module.KINDS kind )
            {
            wago_channels.Add( name, new wago_channel( kind ) );
            }
        }  
    }