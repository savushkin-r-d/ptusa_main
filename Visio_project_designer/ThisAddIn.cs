using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;
using Visio = Microsoft.Office.Interop.Visio;
using Office = Microsoft.Office.Core;

using System.Windows.Forms;
using System.Text.RegularExpressions;

//-----------------------------------------------------------------------------
/// @brief Модуль ввода/вывода WAGO. 
/// 
///
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
        T_504     = 504,
        T_402     = 402,
        T_466     = 466
        };

    public enum CLAMPS_COUNT ///< Количество клемм. 
        {
        СLAMP_8 = 8,
        СLAMP_16 = 16
        };

    internal KINDS        kind;          ///< Вид модуля. 
    internal TYPES        type;          ///< Тип модуля. 
    public   CLAMPS_COUNT total_clamps;  ///< Общее количество клемм. 
                                         ///
    internal int order_number; ///< Порядковый номер в сборке.
    internal int node_number;  ///< Номер узла.

    public bool[] free_clamp_flags;      ///< Флаг свободности клеммы.

    /// @brief Конструктор.
    ///
    /// @param type  - тип модуля.
    /// @param shape - визуальная фигура Visio.
    public io_module( Visio.Shape shape )
        {
        this.clamp_names = new List<string>();
                                              
        this.shape = shape;

        type = ( TYPES ) Convert.ToUInt16( shape.Cells[ "Prop.type" ].ResultStr[ "" ] );
        order_number = Convert.ToUInt16( shape.Cells[ "Prop.order_number" ].Formula );
        node_number = Convert.ToUInt16( shape.Cells[ "Prop.node_number" ].Formula );
        
        //Определяем количество клемм.
        switch( type )
            {
            case TYPES.T_402:
            case TYPES.T_466:
            case TYPES.T_504:
                total_clamps = CLAMPS_COUNT.СLAMP_8;

                free_clamp_flags     = new bool[ ( int ) total_clamps ];
                suitable_clamp_flags = new bool[ ( int ) total_clamps ];

                for( int i = 0; i < ( int ) total_clamps; i++ )
                    {
                    free_clamp_flags[ i ]     = true;
                    suitable_clamp_flags[ i ] = false;

                    clamp_names.Add( "clamp_" + i + 1 );
                    }
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

    public string lua_save()
        {
        return "{ " + type + " }";
        }

    
    //Визуальное представление.
    Visio.Shape shape;           ///< Фигура Visio модуля.
    List < string > clamp_names; ///< Имена клемм.


    public bool[] suitable_clamp_flags;      ///< Флаг подходящих для привязки клемм.
                                             
    /// @brief Затемнение доступных клемм.
    public void mark_suitable()
        {
        for( int i = 0; i < ( int ) total_clamps; i++ )
            {
            if( free_clamp_flags[ i ] )
                {                
                shape.Shapes[ "type_skin" ].Shapes[ clamp_names[ i ] ].Cells[
                    "FillForegnd" ].Formula = "RGB( 127, 127, 127 )";

                suitable_clamp_flags[ i ] = true;
                }
            }
        }

    /// @brief 
    public void unmark()
        {
        for( int i = 0; i < ( int ) total_clamps; i++ )
            {
            suitable_clamp_flags[ i ] = false;
            }
        }

    private int selected_clamp = 0;

    public int get_selected()
        {
        return selected_clamp;
        }

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
                    if ( suitable_clamp_flags[ i ] == false )
                        {
                        //Клемма недоступна, выходим.
                        break;
                        }

                    if( selected_clamp != i + 1 )
                        {
                        unmagnify_clamp( selected_clamp );
                        selected_clamp = i + 1;
                        magnify_clamp( selected_clamp );
                        }
                    return;
                    }
                }

            if( selected_clamp > 0 )
                {
                unmagnify_clamp( selected_clamp  );
                selected_clamp = 0;
                }
            }
        else
            {
            if( selected_clamp > 0 )
                {
                unmagnify_clamp( selected_clamp );
                selected_clamp = 0;
                }
            }
        }

    internal void magnify_clamp( int clamp )
        {
        shape.Shapes[ "type_skin" ].Shapes[ clamp_names[ clamp ] ].Cells[
            "Width" ].Formula = string.Format( "=Sheet.{0}!Width*0.66",
            shape.Shapes[ "type_skin" ].ID );

        shape.Shapes[ "type_skin" ].Shapes[ clamp_names[ clamp ] ].Cells[
            "Height" ].Formula = string.Format( "=Sheet.{0}!Height*0.09",
            shape.Shapes[ "type_skin" ].ID );
        }

    void unmagnify_clamp( int clamp )
        {
        shape.Shapes[ "type_skin" ].Shapes[ clamp_names[ clamp ]
            ].Cells[ "Width" ].Formula = string.Format( "=Sheet.{0}!Width*0.33",
            shape.Shapes[ "type_skin" ].ID );

        shape.Shapes[ "type_skin" ].Shapes[ clamp_names[ clamp ]
            ].Cells[ "Height" ].Formula = string.Format( "=Sheet.{0}!Height*0.045",
            shape.Shapes[ "type_skin" ].ID );
        }

    public void activate( int clamp )
        {
        string color = "0";

        switch ( kind )
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

        shape.Shapes[ "red_boder" ].Cells[ "LineColor" ].Formula = color;            
        shape.Shapes[ "red_boder" ].Cells[ "LineColorTrans" ].Formula = "0%";

        shape.Shapes[ "type_skin" ].Shapes[ clamp_names[ clamp ] ].Cells[
            "FillForegnd" ].Formula = color;
        }

    public void deactivate()
        {
        foreach( string clamp in clamp_names )
            {
            shape.Shapes[ "type_skin" ].Shapes[ clamp ].Cells[
                "FillForegnd" ].Formula = "16";
            }

        shape.Shapes[ "red_boder" ].Cells[ "LineColorTrans" ].Formula = "100%";
        }
    }
//-----------------------------------------------------------------------------
public class PAC
    {
    public Visio.Shape shape;

    public string ip_addres;
    public string PAC_name;

    private List< io_module > io_modules;

    public List< io_module > get_io_modules()
        {
        return io_modules;
        }

    public PAC( string PAC_name, string ip_addres, Visio.Shape shape )
        {
        this.PAC_name = PAC_name;
        this.ip_addres = ip_addres;

        io_modules = new List< io_module >();

        this.shape = shape;
        }

    public void unmark()
        {
        foreach( io_module module in io_modules )
            {
            module.unmark();
            }        
        }

    public void mark_suitable( int kind )
        {
        foreach( io_module module in io_modules )
            {
            module.unmark();

            if( ( io_module.KINDS ) kind == module.kind )
                {
                module.mark_suitable();
                }
            }                
        }

    //Значение привязки имеет следующий формат:
    // -> узел 1 модуль 2 клемма 3;
    //на его основе получаем индекс подсвечиваемого объекта 
    //(номер узла и модуля) и номер клеммы:
    // -> 1, 2, 3.
        //node   = 0;
        //module = 0;
        //clamp  = 0;

        //Regex rex = new Regex( @"\w+\s(\d+)\s\w+\s(\d+)\s\w+\s(\d+)",
        //    RegexOptions.IgnoreCase );
        //if( rex.IsMatch( str ) )
        //    {
        //    Match mtc = rex.Match( str );

        //    node = Convert.ToInt16( mtc.Groups[ 1 ].ToString() ) - 1;
        //    module = Convert.ToInt16( mtc.Groups[ 2 ].ToString() ) - 1;
        //    clamp = Convert.ToInt16( mtc.Groups[ 3 ].ToString() );
        //    }

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
    internal int       clamp;

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

    }

public class wago_device
    {
    internal Dictionary< string, wago_channel > wago_channels;

    public wago_device()
        {
        wago_channels = new Dictionary< string, wago_channel >();
        }

    internal void add_wago_channel( string name, io_module module, int clamp )
        {
        wago_channels.Add( name, new wago_channel( module, clamp ) );        
        }

    internal void add_wago_channel( string name, io_module.KINDS kind )
        {
        wago_channels.Add( name, new wago_channel( kind ) );
        }
    }

public class device: wago_device
    {
    int n;

    public int get_n() 
        {
        return n;
        }
    
    public enum TYPES
        {
        T_V = 0,
        }

    public enum SUB_TYPES
        {
        V_1_CONTROL_CHANNEL = 0,
        V_2_CONTROL_CHANNEL,

        V_1_CONTROL_CHANNEL_1_FB,
        V_1_CONTROL_CHANNEL_2_FB,
        V_2_CONTROL_CHANNEL_2_FB,
        }

    TYPES       type;
    SUB_TYPES   sub_type;

    public Dictionary< string, string > to_str()
        {
        Dictionary< string, string > res = new Dictionary< string, string >();

        foreach( KeyValuePair<string, wago_channel> channel in wago_channels )
            {
            string description = "не привязан";
            if ( channel.Value.module != null )
                {
                description = string.Format( "узел {0}, модуль{1}, клемма{2}", 
                    channel.Value.module.node_number, 
                    channel.Value.module.order_number, channel.Value.clamp );
                }

            res.Add( channel.Key, description );            
            }

        return res;
        }

    public int get_type()
        {
        return ( int ) type;
        }

    Visio.Shape shape;

    string [] NAMES = 
        {
        "Клапан",
        };

    string [] SUB_NAMES = 
        {
        "1 КУ",
        "2 КУ",
        "1 КУ 1 ОС",
        "1 КУ 2 ОС",
        "2 КУ 2 ОС"
        };

    public device( Visio.Shape shape )
        {
        n    = Convert.ToUInt16( shape.Cells[ "Prop.n" ] );
        type = ( TYPES ) Convert.ToUInt16( shape.Cells[ "Prop.type" ].Formula );
        sub_type = ( SUB_TYPES ) Convert.ToUInt16( shape.Cells[ "Prop.sub_type" ] );

        switch( type )
            {
            case TYPES.T_V:

                switch( sub_type )
                    {
                    case SUB_TYPES.V_1_CONTROL_CHANNEL:
                        add_wago_channel( "DO1", io_module.KINDS.DO );
                        break;
                    }
                break;
            }
        }

    public void set_channel( string channel_name, io_module module, int clamp )
        {
        wago_channels[ channel_name ].module = module;
        wago_channels[ channel_name ].clamp = clamp;
        }

    public void unselect_channels()
        {
        foreach( KeyValuePair< string, wago_channel > channel in wago_channels )
            {
            channel.Value.module.deactivate();
            }
        }

    public void select_channels( string magnify_channel_name = null )
        {
        foreach( KeyValuePair<string, wago_channel> channel in wago_channels )
            {
            channel.Value.module.activate( channel.Value.clamp );
            }

        if( magnify_channel_name != null )
            {
            wago_channels[ magnify_channel_name ].module.magnify_clamp(
                wago_channels[ magnify_channel_name ].clamp );
            }
        }

    }

namespace Visio_project_designer
    {
    public partial class ThisAddIn
        {
        public bool is_selecting_clamp = false;


        public static main_ribbon vis_main_ribbon;

        public static List< device > g_devices; // Все устройства.

        public static PAC g_PAC = null;                 ///Объект контроллера.

        public static bool is_device_edit_mode = false; /// Режим привязки устройств к модулям.

        public static AnchorBarsUsage edit_io_frm; ///Форма редактирования привязки к каналам. 

        //Константы для обращения к окнам Visio.
        public enum VISIO_WNDOWS : short
            {
            MAIN    = 1,
            IO_EDIT = 2,
            };


        private Visio.Application visio_app;

        private void ThisAddIn_Startup( object sender, System.EventArgs e )
            {
            visio_app = this.Application;
            visio_app.DocumentCreated +=
                new Microsoft.Office.Interop.Visio.EApplication_DocumentCreatedEventHandler(
                    m_Application_DocumentCreated );

            visio_app.ShapeAdded +=
                new Microsoft.Office.Interop.Visio.EApplication_ShapeAddedEventHandler(
                    m_Application_shape_added );

            visio_app.BeforeShapeDelete +=
                new Microsoft.Office.Interop.Visio.EApplication_BeforeShapeDeleteEventHandler(
                    m_Application_shape_deleted );

            visio_app.ConnectionsAdded +=
                new Microsoft.Office.Interop.Visio.EApplication_ConnectionsAddedEventHandler(
                    m_Application_connections_added );

            visio_app.FormulaChanged += new Microsoft.Office.Interop.Visio.EApplication_FormulaChangedEventHandler(
                    m_Application_formula_changed );


            visio_app.SelectionChanged += new Microsoft.Office.Interop.Visio.EApplication_SelectionChangedEventHandler(
                    m_Application_selection_changed );

            visio_app.DocumentOpened += new Microsoft.Office.Interop.Visio.EApplication_DocumentOpenedEventHandler(
                 m_Application_document_opened );

            visio_app.MouseMove += new Visio.EApplication_MouseMoveEventHandler(
                visio_app_mouse_move );            

            visio_app.MouseDown +=new Visio.EApplication_MouseDownEventHandler(
                 visio_app_mouse_down );

            g_devices = new List< device > ();
            }

        void visio_app_mouse_down( int Button, int KeyButtonState, double x,
            double y, ref bool CancelDefault )
            {
            if( is_selecting_clamp )
                {
                foreach( io_module module in g_PAC.get_io_modules() )
                    {
                    int clamp = module.get_selected();
                    if( clamp > 0 )
                        {
                        m_Application_selection_changed( visio_app.Windows[ 2 ] );
                        //string value_DO1 = selected_shape.Cells[ "Prop.DO1" ].Formula;
                        //select_channel( value_DO1, 2 );

                        //string value_DO2 = selected_shape.Cells[ "Prop.DO2" ].Formula;
                        //select_channel( value_DO2, 2 );

                        //string value_DI1 = selected_shape.Cells[ "Prop.DI1" ].Formula;
                        //select_channel( value_DI1, 2 );

                        //string value_DI2 = selected_shape.Cells[ "Prop.DI2" ].Formula;
                        //select_channel( value_DI2, 2 );

                        //Обновление подсвечиваемых модулей Wago.
                        //g_PAC.deactivate();
                        //g_PAC.activate(  module.shape, );

                        //refresh_prop_wnd( null );

                        //MessageBox.Show( module.shape.Name );
                        is_selecting_clamp = false;

                        CancelDefault = true;
                        return;
                        }
                    }

                MessageBox.Show( "No clamp!" );
                is_selecting_clamp = false;
                }
            }

        void visio_app_mouse_move( int Button,
            int KeyButtonState, double x, double y, ref bool CancelDefault )
            {
            if( is_selecting_clamp )
                {
                foreach( io_module module in g_PAC.get_io_modules() )
                    {
                    module.magnify_on_mouse_move( x, y );
                    }
                }

            //if( Globals.ThisAddIn.Application.ActivePage.Shapes.get_ItemFromID(
            //    167 ).SpatialNeighbors( ) ( x, y, 0.1 ) > 0 )
            //    {
            //    MessageBox.Show( "sd1" );
            //    }

            //if( Globals.ThisAddIn.Application.ActivePage.Shapes[
            //    "750-xxx.27" ].HitTest( x, y, 0.1 ) > 0 )
            //    {
            //    MessageBox.Show( Convert.ToString( Globals.ThisAddIn.Application.ActivePage.Shapes[
            //            "750-xxx.27" ].Shapes[ "red_boder" ].HitTest( x - , 0.2, 0.01 ) ) );

                //if( Globals.ThisAddIn.Application.ActivePage.SpatialSearch[ x, y,
                //    ( short ) Visio.VisSpatialRelationCodes.visSpatialContainedIn |
                //    ( short ) Visio.VisSpatialRelationCodes.visSpatialContain, 0.02,
                //    ( short ) Visio.VisSpatialRelationFlags.visSpatialFrontToBack ].Count > 0 )

                //    {
                //    MessageBox.Show( Globals.ThisAddIn.Application.ActivePage.Shapes[
                //        "750-xxx.27" ].SpatialSearch[ 0.2, 0.2,
                //        ( short ) Visio.VisSpatialRelationCodes.visSpatialContain, 0.2, 0 ][ 1 ].Name );
                //    }
               // }

               //if( g_PAC.shape.HitTest( x, y, 0.1 ) > 0 )
            //    {
            //    CancelDefault = true;
            //    if( is_hit == 0 )
            //        {
            //        g_PAC.shape.Cells[ "Prop.ip_address" ].Formula = "2";
            //        is_hit = 1;
            //        }
            //    }
            //else
            //    {
            //    if( is_hit == 1 ) 
            //        {
            //        g_PAC.shape.Cells[ "Prop.ip_address" ].Formula = "1";                
            //        is_hit = 0;
            //        }
            //    }
            }

        void m_Application_document_opened( Visio.Document target )
            {
            if( target.Type == Visio.VisDocumentTypes.visTypeDrawing )
                {
                if( target.Template.Contains( "PTUSA project" ) )
                    {
                    vis_main_ribbon.show();

                    //Считывание устройств Wago.
                    foreach( Visio.Shape shape in target.Pages[ "Wago" ].Shapes )
                        {
                        if( shape.Data1 == "750" && shape.Data2 == "860" )
                            {
                            string ip_addr = shape.Cells[ "Prop.ip_address" ].Formula;
                            string name = shape.Cells[ "Prop.PAC_name" ].Formula;

                            g_PAC = new PAC( name.Substring( 1, name.Length - 2 ),
                                ip_addr.Substring( 1, ip_addr.Length - 2 ), shape );

                            }

                        if( shape.Data1 == "750" && shape.Data2 != "860" )
                            {
                            g_PAC.add_io_module( shape );
                            }
                        }

                    //Считывание устройств.
                    foreach( Visio.Shape shape in target.Pages[ "Устройства" ].Shapes )
                        {
                        if( shape.Data1 == "V" )
                            {
                            g_devices.Add( new device( shape ) );
                            }
                        }
                    }
                else
                    {
                    vis_main_ribbon.hide();
                    }
                }
            }

        void refresh_prop_wnd( Microsoft.Office.Interop.Visio.Shape shape )
            {
            Microsoft.Office.Interop.Visio.Window main_wnd = visio_app.Windows[
                ( short ) ThisAddIn.VISIO_WNDOWS.MAIN ];

            if( shape != null )
                {
                //Обновляем таблицу свойств.                
                edit_io_frm.listForm.enable_prop();
                edit_io_frm.listForm.change_type( 
                    Convert.ToInt16( shape.Cells[ "Prop.type" ].Formula ) );

                }
            else //if( shape != null )
                {
                //Сбрасываем таблицу свойств.
                edit_io_frm.listForm.clear_prop();
                edit_io_frm.listForm.disable_prop();
                }
            }

        device previous_selected_dev = null;
        internal device current_selected_dev = null;

        void m_Application_selection_changed( Microsoft.Office.Interop.Visio.Window Window )
            {

            // Проверка на режим привязки устройств к каналам ввода\вывода.
            if( ThisAddIn.is_device_edit_mode ) 
                {
                // Проверка на активность окна редактирования.
                if( Window.Index == ( short ) VISIO_WNDOWS.IO_EDIT )
                    {

                    // Проверка на активность страницы с устройствами (клапанами т.д.).
                    if( Window.Page.Name == "Устройства" )
                        {

                        const int IO_PROP_WINDOW_INDEX = 8;

                        Window.Windows[ IO_PROP_WINDOW_INDEX ].Caption =
                                "Каналы";

                        if( Window.Selection.Count > 0 )
                            {
                            Window.Windows[ IO_PROP_WINDOW_INDEX ].Caption =
                                Window.Windows[ IO_PROP_WINDOW_INDEX ].Caption +
                                " - " + Window.Selection[ 1 ].Name;

                            //Обновление окна со свойствами привязки.
                            Microsoft.Office.Interop.Visio.Shape selected_shape = Window.Selection[ 1 ];                            
                            refresh_prop_wnd( selected_shape );

                            //Снятие ранее подсвеченных модулей Wago.
                            if( previous_selected_dev != null )
                                {
                                previous_selected_dev.unselect_channels();
                                }
                            
                            //Поиск по shape объекта device.
                            int dev_n    = Convert.ToInt16( selected_shape.Cells[ "Prop.n" ].Formula );
                            int dev_type = Convert.ToInt16( selected_shape.Cells[ "Prop.type" ].Formula );

                            current_selected_dev = g_devices.Find( delegate( device dev )
                                {
                                return dev.get_n() == dev_n && dev.get_type() == dev_type; 
                                }
                                );

                            if( current_selected_dev != null )
                                {
                                current_selected_dev.select_channels();
                                }

                            } //if( Window.Selection.Count > 0 )
                        else
                            {
                            //Снятие ранее подсвеченных модулей Wago.
                            if( previous_selected_dev != null )
                                {
                                previous_selected_dev.unselect_channels();
                                previous_selected_dev = null;
                                }

                            refresh_prop_wnd( null );
                            }

                        } //if( Window.Page.Name == "Устройства" )
                    } //if( Window.Index == ( short ) VISIO_WNDOWS.IO_EDIT )
                } //if( ThisAddIn.is_device_edit_mode ) 
            }

        void m_Application_formula_changed( Microsoft.Office.Interop.Visio.Cell Cell )
            {
            if( Cell.Shape.Data1 == "750" )
                {
                if( Cell.Name == "Prop.order_number" )
                    {
                    Cell.Shape.Shapes[ "type_skin" ].Shapes[ "module_number" ].Text =
                        Cell.Shape.Cells[ "Prop.order_number" ].Formula;
                    return;
                    }

                if( Cell.Name == "Prop.type" )
                    {
                    int type = ( int ) Convert.ToDouble( Cell.Shape.Cells[ "Prop.type" ].ResultStr[ "" ] );

                    Cell.Shape.Data2 = Convert.ToString( type );
                    Cell.Shape.Shapes[ "type_str" ].Text = Convert.ToString( type );

                    string colour = "1";
                    switch( type )
                        {
                        case 461:
                        case 466:
                            colour = "RGB( 180, 228, 180 )";
                            break;

                        case 504:
                        case 512:
                            colour = "RGB( 255, 128, 128 )";
                            break;

                        case 402:
                            colour = "RGB( 255, 255, 128 )";
                            break;

                        case 602:
                        case 612:
                        case 638:
                            colour = "14";
                            break;

                        case 600:
                            colour = "15";
                            break;
                        }

                    Cell.Shape.Shapes[ "type_skin" ].Shapes[ "module_number" ].Cells[ 
                        "FillForegnd" ].Formula = colour;
                    Cell.Shape.Shapes[ "type_skin" ].Shapes[ "3" ].Cells[
                        "FillForegnd" ].Formula = colour;
                    }
                }

            if( Cell.Shape.Data1 == "V" )
                {
                switch( Cell.Name )
                    {
                    //case "Prop.type":
                    //    string type = Cell.Shape.Cells[ "Prop.type" ].get_ResultStr( 0 );

                    //    switch( type )
                    //        {
                    //        case "1 КУ":
                    //            //Cell.Shape.AddSection( Cell.Shape.get_CellsRowIndex );
                    //            int idx = Cell.Shape.get_CellsRowIndex( "Prop.type" );

                    //            Cell.Shape.AddNamedRow(
                    //                ( short ) Visio.VisSectionIndices.visSectionUser, "DO",
                    //                ( short ) Visio.VisRowTags.visTagDefault );

                    //            Cell.Shape.AddNamedRow(
                    //                ( short ) Visio.VisSectionIndices.visSectionProp, "hf",
                    //                ( short ) 0 );


                    //            break;
                    //        }

                    //    break;

                    case "Prop.name":
                        string str = Cell.Shape.Cells[ "Prop.name" ].Formula;

                        //Первый вариант маркировки клапана - 1V12.
                        Regex rex = new Regex( @"\b([0-9]{0,4})V([0-9]{1,2})\b",
                            RegexOptions.IgnoreCase );
                        if( rex.IsMatch( str ) )
                            {
                            Match mtc = rex.Match( str );

                            string n_part_1 = mtc.Groups[ 1 ].ToString();
                            string n_part_2 = mtc.Groups[ 2 ].ToString();
                            if( n_part_1 == "" ) n_part_1 = "0";

                            int n = Convert.ToUInt16( n_part_1 ) * 100 +
                                Convert.ToUInt16( n_part_2 );

                            Cell.Shape.Cells[ "Prop.number" ].Formula = n.ToString();

                            str = str.Replace( "\"", "" );
                            Cell.Shape.Name = str;
                            Cell.Shape.Shapes[ "name" ].Text = str;
                            break;
                            }

                        //Второй вариант маркировки клапана - S4V12.
                        Regex rex2 = new Regex( @"\b([a-z]{1})([0-9]{1})V([0-9]{1,2})\b",
                            RegexOptions.IgnoreCase );
                        if( rex2.IsMatch( str ) )
                            {
                            Match mtc = rex2.Match( str );

                            //Линия (A-W).
                            int n = 100 * ( 200 +
                                ( Convert.ToUInt16( mtc.Groups[ 1 ].ToString()[ 0 ] ) - 65 ) * 20 );
                            //Номер линии (0-19).
                            n += 100 * Convert.ToUInt16( mtc.Groups[ 2 ].ToString() );
                            //Клапан.
                            n += Convert.ToUInt16( mtc.Groups[ 3 ].ToString() );

                            Cell.Shape.Cells[ "Prop.number" ].Formula = n.ToString();

                            str = str.Replace( "\"", "" );
                            Cell.Shape.Name = str;                                                        
                            Cell.Shape.Shapes[ "name" ].Text = str;
                            break;
                            }

                        MessageBox.Show( "Неверная маркировка клапана - \"" +
                            str + "\"!" );
                        Cell.Shape.Cells[ "Prop.name" ].Formula = "\"V19\"";
                        break;
                    }
                }
            }

        void m_Application_connections_added( Microsoft.Office.Interop.Visio.Connects connect )
            {

            //Globals.ThisAddIn.Application.BuiltInToolbars. BuiltInToolbars 
            //--
            Microsoft.Office.Interop.Visio.Shape obj_2 =
                visio_app.ActivePage.Shapes[ connect.FromSheet.Name ];
            Microsoft.Office.Interop.Visio.Shape obj_1 =
                visio_app.ActivePage.Shapes[ connect.ToSheet.Name ];

            switch( obj_1.Data1 )
                {
                case "750":
                    if( obj_1.Data2 == "860" )
                        {
                        obj_2.Cells[ "Prop.order_number" ].Formula = "1";
                        }
                    else
                        {
                        int new_number = Convert.ToInt32( obj_1.Cells[ "Prop.order_number" ].Formula ) + 1;
                        obj_2.Cells[ "Prop.order_number" ].Formula = Convert.ToString( new_number );
                        }

                    break;
                }
            }

        void m_Application_shape_deleted( Microsoft.Office.Interop.Visio.Shape shape )
            {
            switch( shape.Data1 )
                {
                case "750":
                    if( shape.Data2 == "860" )
                        {
                        if( no_delete_g_pac_flag )
                            {
                            no_delete_g_pac_flag = false;
                            }
                        else
                            {
                            g_PAC = null;
                            }
                        }
                    break;
                }
            }

        private static bool is_duplicating = false;
        private static int duplicate_count = 0;

        static Microsoft.Office.Interop.Visio.Shape old_shape;
        static Microsoft.Office.Interop.Visio.Shape new_shape;

        private bool no_delete_g_pac_flag = false;

        void m_Application_shape_added( Microsoft.Office.Interop.Visio.Shape shape )
            {
           
            //shape.HitTest()

            //Проверяем, нужный ли нам объект мы добавляем на лист. Выводим окно
            //с запросом количества элементов. Помещаем на форму первый элемент,
            //остальные приклеиваем к нему путем дублирования вставляемого 
            //элемента.

            if( is_duplicating )
                {
                try
                    {
                    }

                catch( Exception e )
                    {
                    MessageBox.Show( e.Message );
                    }

                duplicate_count--;
                if( duplicate_count <= 0 )
                    {
                    is_duplicating = false;
                    }

                return;
                }

            if( shape.Data1 == "750" && shape.Data2 == "860" )
                {
                if( g_PAC == null )
                    {
                    string ip_addr = shape.Cells[ "Prop.ip_address" ].Formula;
                    string name = shape.Cells[ "Prop.PAC_name" ].Formula;

                    g_PAC = new PAC( name.Substring( 1, name.Length - 2 ),
                        ip_addr.Substring( 1, ip_addr.Length - 2 ), shape );
                    }
                else
                    {
                    no_delete_g_pac_flag = true;
                    MessageBox.Show( "Только один контроллер может быть в проекте!" );
                    shape.DeleteEx( 0 );
                    }
                return;
                }

            if( shape.Data1 == "750" && shape.Data2 != "860" )
                {
                modules_count_enter modules_count_enter_form = new modules_count_enter();
                modules_count_enter_form.ShowDialog();
                duplicate_count = modules_count_enter_form.modules_count - 1;


                g_PAC.add_io_module( shape );
                
                shape.Cells[ "Prop.type" ].Formula = 
                    modules_count_enter_form.modules_type;                                

                for( int i = 0; i < duplicate_count; i++ )
                    {
                    is_duplicating = true;
                    new_shape = shape.Duplicate();
                    old_shape = shape;
                                        
                    string str_x = string.Format( "PNTX(LOCTOPAR(PNT('{0}'!Connections.X2,'{0}'!Connections.Y2),'{0}'!EventXFMod,EventXFMod))+6 mm",
                        old_shape.Name );
                    string str_y = string.Format( "PNTY(LOCTOPAR(PNT('{0}'!Connections.X2,'{0}'!Connections.Y2),'{0}'!EventXFMod,EventXFMod))+-47 mm",
                        old_shape.Name );

                    new_shape.Cells[ "PinX" ].Formula = str_x;
                    new_shape.Cells[ "PinY" ].Formula = str_y;

                    shape = new_shape;

                    g_PAC.add_io_module( shape );
                    m_Application_formula_changed( shape.Cells[ "Prop.type" ] );
                    }
                }

            if( shape.Data1 == "V" )
                {
                g_devices.Add( new device( shape ) );
                }
            }

        void m_Application_DocumentCreated( Microsoft.Office.Interop.Visio.Document Doc )
            {
            if( Doc.Template.Contains( "PTUSA project" ) )
                {
                vis_main_ribbon.show();
                }
            else
                {
                vis_main_ribbon.hide();
                }
            }

        private void ThisAddIn_Shutdown( object sender, System.EventArgs e )
            {
            }

        #region VSTO generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InternalStartup()
            {
            this.Startup += new System.EventHandler( ThisAddIn_Startup );
            this.Shutdown += new System.EventHandler( ThisAddIn_Shutdown );
            }

        #endregion
        }
    }
