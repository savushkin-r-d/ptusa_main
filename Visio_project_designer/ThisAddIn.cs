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
    public int type;

    public io_module( int type )
        {
        this.type = type;
        }

    public string lua_save()
        {
        return "{ " + type + " }";
        }
    }

public class PAC
    {
    public string ip_addres;
    public string PAC_name;

    private List<io_module> io_modules;

    public PAC( string PAC_name, string ip_addres )
        {
        this.PAC_name = PAC_name;
        this.ip_addres = ip_addres;

        io_modules = new List<io_module>();
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

    public void add_io_module( int type )
        {
        io_modules.Add( new io_module( type ) );
        }
    }




namespace Visio_project_designer
    {
    public partial class ThisAddIn
        {
        public static PAC g_PAC;

        private Visio.Application m_Application;

        private void ThisAddIn_Startup( object sender, System.EventArgs e )
            {
            m_Application = this.Application;
            m_Application.DocumentCreated +=
                new Microsoft.Office.Interop.Visio.EApplication_DocumentCreatedEventHandler(
                    m_Application_DocumentCreated );

            m_Application.ShapeAdded +=
                new Microsoft.Office.Interop.Visio.EApplication_ShapeAddedEventHandler(
                    m_Application_shape_added );

            m_Application.BeforeShapeDelete +=
                new Microsoft.Office.Interop.Visio.EApplication_BeforeShapeDeleteEventHandler(
                    m_Application_shape_deleted );

            m_Application.ConnectionsAdded +=
                new Microsoft.Office.Interop.Visio.EApplication_ConnectionsAddedEventHandler(
                    m_Application_connections_added );

            m_Application.FormulaChanged += new Microsoft.Office.Interop.Visio.EApplication_FormulaChangedEventHandler(
                    m_Application_formula_changed );
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
                    int type = Convert.ToInt16( Cell.Shape.Cells[ "Prop.type" ].ResultStr[ "" ] );

                    Cell.Shape.Data2 = Convert.ToString( type );
                    Cell.Shape.Shapes[ "type_str" ].Text = Convert.ToString( type );

                    string colour = "1";
                    switch( type )
                        {
                        case 461:
                        case 466:
                            colour = "9";
                            break;

                        case 504:
                        case 512:
                            colour = "2";
                            break;

                        case 402:
                            colour = "5";
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
                    case "Prop.type":
                        string type = Cell.Shape.Cells[ "Prop.type" ].get_ResultStr( 0 );

                        switch( type )
                            {
                            case "1 КУ":
                                short tmp = 1;
                                //Cell.Shape.AddSection( Cell.Shape.get_CellsRowIndex );
                                int idx = Cell.Shape.get_CellsRowIndex( "Prop.type" );

                                Cell.Shape.AddNamedRow(
                                    ( short ) Visio.VisSectionIndices.visSectionUser, "DO",
                                    ( short ) Visio.VisRowTags.visTagDefault );

                                Cell.Shape.AddNamedRow(
                                    ( short ) Visio.VisSectionIndices.visSectionProp, "hf",
                                    ( short ) 0 );


                                break;
                            }

                        break;

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
            Microsoft.Office.Interop.Visio.Shape obj_2 =
                Globals.ThisAddIn.Application.ActivePage.Shapes[ connect.FromSheet.Name ];
            Microsoft.Office.Interop.Visio.Shape obj_1 =
                Globals.ThisAddIn.Application.ActivePage.Shapes[ connect.ToSheet.Name ];

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
                        ip_addr.Substring( 1, ip_addr.Length - 2 ) );
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

                g_PAC.add_io_module( Convert.ToUInt16( shape.Data2 ) );

                for( int i = 0; i < duplicate_count; i++ )
                    {
                    is_duplicating = true;
                    new_shape = shape.Duplicate();
                    old_shape = shape;

                    g_PAC.add_io_module( Convert.ToUInt16( shape.Data2 ) );

                    string str_x = string.Format( "PNTX(LOCTOPAR(PNT('{0}'!Connections.X2,'{0}'!Connections.Y2),'{0}'!EventXFMod,EventXFMod))+6 mm",
                        old_shape.Name );
                    string str_y = string.Format( "PNTY(LOCTOPAR(PNT('{0}'!Connections.X2,'{0}'!Connections.Y2),'{0}'!EventXFMod,EventXFMod))+-41.8142 mm",
                        old_shape.Name );

                    new_shape.Cells[ "PinX" ].Formula = str_x;
                    new_shape.Cells[ "PinY" ].Formula = str_y;

                    shape = new_shape;
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
