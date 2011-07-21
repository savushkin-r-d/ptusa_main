using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;
using Visio = Microsoft.Office.Interop.Visio;
using Office = Microsoft.Office.Core;

using System.Windows.Forms;
using System.Text.RegularExpressions;

public class io_module
    {
    private Visio.Shape shape;

    public int type;

    public io_module( int type, Visio.Shape shape )
        {
        this.type  = type;
        this.shape = shape;
        }

    public void activate( int clamp, int color = 0 )
        {
        shape.Shapes[ "red_boder" ].Cells[ "LineColor" ].Formula =
            Convert.ToString( color );

        shape.Shapes[ "red_boder" ].Cells[ "LineColorTrans" ].Formula = "0%";
        string clamp_name = "clamp_" + Convert.ToString( clamp );
        shape.Shapes[ "type_skin" ].Shapes[ clamp_name ].Cells[
            "FillForegnd" ].Formula = Convert.ToString( color );
        }

    public void deactivate()
        {
        for( int i = 1; i < 9; i++ )
            {
            string clamp_name = "clamp_" + Convert.ToString( i );

            shape.Shapes[ "type_skin" ].Shapes[ clamp_name ].Cells[
                "FillForegnd" ].Formula = "16";
            }        

        shape.Shapes[ "red_boder" ].Cells[ "LineColorTrans" ].Formula = "100%";
        }

    public string lua_save()
        {
        return "{ " + type + " }";
        }
    }

public class PAC
    {
    public Visio.Shape shape;

    public string ip_addres;
    public string PAC_name;

    private List<io_module> io_modules;

    public PAC( string PAC_name, string ip_addres, Visio.Shape shape )
        {
        this.PAC_name = PAC_name;
        this.ip_addres = ip_addres;

        io_modules = new List<io_module>();

        this.shape = shape;
        }

    public void activate( int module, int clamp, int color )
        {
        io_modules[ module ].activate( clamp, color );        
        }

    public void deactivate()
        {
        foreach( io_module module in io_modules )
            {
            module.deactivate();
            }        
        }

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

    public void add_io_module( int type, Visio.Shape shape )
        {
        io_modules.Add( new io_module( type, shape ) );
        }
    }

namespace Visio_project_designer
    {
    public partial class ThisAddIn
        {
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
            }

        //static int is_hit = 0;

        void visio_app_mouse_move( int Button,
            int KeyButtonState, double x, double y, ref bool CancelDefault )
            {            
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
                    g_PAC.add_io_module( Convert.ToUInt16( shape.Data2 ), shape );
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
                edit_io_frm.listForm.change_type( shape.Cells[ "Prop.type" ].get_ResultStr( "" ) );

                //string tmp = main_wnd.Page.Shapes[ "main_PAC.750-504.1" ].Shapes[
                //    "red_boder" ].Cells[ "LineColorTrans" ].Formula;

                //if( tmp == "0%" )
                //    {
                //    tmp = "100%";
                //    }
                //else
                //    {
                //    tmp = "0%";
                //    }

                //main_wnd.Page.Shapes[ "main_PAC.750-504.1" ].Shapes[
                //    "red_boder" ].Cells[ "LineColorTrans" ].Formula = tmp;

                }
            else //if( shape != null )
                {
                //Сбрасываем таблицу свойств.
                edit_io_frm.listForm.clear_prop();
                edit_io_frm.listForm.disable_prop();
                }
            }

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

                            //Обновление подсвечиваемых модулей Wago.
                            g_PAC.deactivate();
                                                        
                            //Значение привязки имеет следующий формат:
                            // -> узел 1 модуль 2 клемма 3;
                            //на его основе получаем индекс подсвечиваемого объекта 
                            //(номер узла и модуля) и номер клеммы:
                            // -> 1, 2, 3.
                            string value_DO1 = selected_shape.Cells[ "Prop.DO1" ].Formula;
                            select_channel( value_DO1, 2 );

                            string value_DO2 = selected_shape.Cells[ "Prop.DO2" ].Formula;
                            select_channel( value_DO2, 2 );

                            string value_DI1 = selected_shape.Cells[ "Prop.DI1" ].Formula;
                            select_channel( value_DI1, 2 );

                            string value_DI2 = selected_shape.Cells[ "Prop.DI2" ].Formula;
                            select_channel( value_DI2, 2 );


                            } //if( Window.Selection.Count > 0 )
                        else
                            {
                            //Обновление подсвечиваемых модулей Wago.
                            g_PAC.deactivate();

                            refresh_prop_wnd( null );
                            }

                        } //if( Window.Page.Name == "Устройства" )
                    } //if( Window.Index == ( short ) VISIO_WNDOWS.IO_EDIT )
                } //if( ThisAddIn.is_device_edit_mode ) 
            }

        public void select_channel( string channel, int color )
            {
            Regex rex = new Regex( @"\w+\s(\d+)\s\w+\s(\d+)\s\w+\s(\d+)",
                RegexOptions.IgnoreCase );
            if( rex.IsMatch( channel ) )
                {
                Match mtc = rex.Match( channel );

                int n_1 = Convert.ToInt16( mtc.Groups[ 1 ].ToString() ) - 1;
                int n_2 = Convert.ToInt16( mtc.Groups[ 2 ].ToString() ) - 1;
                int n_3 = Convert.ToInt16( mtc.Groups[ 3 ].ToString() );

                g_PAC.activate( n_2, n_3, color );
                }
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


                g_PAC.add_io_module( Convert.ToUInt16( shape.Data2 ), shape );
                
                shape.Cells[ "Prop.type" ].Formula = 
                    modules_count_enter_form.modules_type;                                

                for( int i = 0; i < duplicate_count; i++ )
                    {
                    is_duplicating = true;
                    new_shape = shape.Duplicate();
                    old_shape = shape;

                    g_PAC.add_io_module( Convert.ToUInt16( shape.Data2 ), shape );

                    string str_x = string.Format( "PNTX(LOCTOPAR(PNT('{0}'!Connections.X2,'{0}'!Connections.Y2),'{0}'!EventXFMod,EventXFMod))+6 mm",
                        old_shape.Name );
                    string str_y = string.Format( "PNTY(LOCTOPAR(PNT('{0}'!Connections.X2,'{0}'!Connections.Y2),'{0}'!EventXFMod,EventXFMod))+-47 mm",
                        old_shape.Name );

                    new_shape.Cells[ "PinX" ].Formula = str_x;
                    new_shape.Cells[ "PinY" ].Formula = str_y;

                    shape = new_shape;
                    m_Application_formula_changed( shape.Cells[ "Prop.type" ] );
                    }
                }
            }

        void m_Application_DocumentCreated( Microsoft.Office.Interop.Visio.Document Doc )
            {
            switch( Doc.Name )
                {
                case "MyDemoItem":
                    MessageBox.Show( "MyDemoItem was clicked" );
                    break;
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
