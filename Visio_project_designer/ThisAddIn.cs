using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;
using Visio = Microsoft.Office.Interop.Visio;
using Office = Microsoft.Office.Core;

using System.Windows.Forms;
using System.Text.RegularExpressions;

public class PAC
    {
    public string ip_addres;
    public string PAC_name;

    public PAC( string PAC_name, string ip_addres )
        {
        this.PAC_name = PAC_name;
        this.ip_addres = ip_addres;
        }

    public string lua_save()
        {
        return "ip_addres = \"" + ip_addres + "\"";
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
            switch( Cell.Shape.Data1 )
                {
                case "V":
                    switch( Cell.Name )
                        {
                        case "Prop.name":
                            string str = Cell.Shape.Cells[ "Prop.name" ].Formula;

                            //Первый вариант маркировки клапана - 1V12.
                            Regex rex = new Regex( @"\b([0-9]{0,4})V([0-9]{1,2})\b", RegexOptions.IgnoreCase );
                            if( rex.IsMatch( str ) )
                                {
                                Match mtc = rex.Match( str );
                                MessageBox.Show( mtc.Groups[ 1 ].ToString() + "; " +
                                    mtc.Groups[ 2 ].ToString() );

                                string n_part_1 = mtc.Groups[ 1 ].ToString();
                                string n_part_2 = mtc.Groups[ 2 ].ToString();
                                if( n_part_1 == "" ) n_part_1 = "0";

                                int n = Convert.ToUInt16( n_part_1 ) * 100 +
                                    Convert.ToUInt16( n_part_2 );
                                MessageBox.Show( n.ToString() );

                                Cell.Shape.Cells[ "Prop.number" ].Formula = n.ToString();
                                }

                            //Второй вариант маркировки клапана - S4V12.
                            Regex rex2 = new Regex( @"\b([a-z]{1})([0-9]{1})V([0-9]{1,2})\b", RegexOptions.IgnoreCase );
                            if( rex2.IsMatch( str ) )
                                {
                                Match mtc = rex2.Match( str );
                                //MessageBox.Show( mtc.Groups[ 1 ].ToString() + "; " +
                                //    mtc.Groups[ 2 ].ToString() + "; " +
                                //   mtc.Groups[ 3 ].ToString() );

                                //devices_veiw dev_veiw = new devices_veiw();
                                //dev_veiw.ShowDialog();

                                //Линия (A-W).
                                int n = 100 * ( 200 +
                                    ( Convert.ToUInt16( mtc.Groups[ 1 ].ToString()[ 0 ] ) - 65 ) * 20 );
                                //Номер линии (0-19).
                                n += 100 * Convert.ToUInt16( mtc.Groups[ 2 ].ToString() );
                                //Клапан.
                                n += Convert.ToUInt16( mtc.Groups[ 3 ].ToString() );

                                Cell.Shape.Cells[ "Prop.number" ].Formula = n.ToString();
                                }

                            break;
                        }

                    break;
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
                case "750-860":
                    obj_2.Cells[ "Prop.Number" ].Formula = "1";
                    break;

                default:
                    int new_number = Convert.ToInt32( obj_1.Cells[ "Prop.Number" ].Formula ) + 1;
                    obj_2.Cells[ "Prop.Number" ].Formula = Convert.ToString( new_number );
                    break;
                }
            }

        void m_Application_shape_deleted( Microsoft.Office.Interop.Visio.Shape shape )
            {
            switch( shape.Data1 )
                {
                case "750-860":
                    g_PAC = null;
                    break;
                }
            }

        void m_Application_shape_added( Microsoft.Office.Interop.Visio.Shape shape )
            {
            switch( shape.Data1 )
                {
                case "750-860":
                    if( g_PAC == null )
                        {
                        string ip_addr = shape.Cells[ "Prop.ip_address" ].Formula;
                        string name = shape.Cells[ "Prop.PAC_name" ].Formula;

                        g_PAC = new PAC( name.Substring( 1, name.Length - 2 ),
                            ip_addr.Substring( 1, ip_addr.Length - 2 ) );
                        }
                    else
                        {
                        MessageBox.Show( "Только один контроллер может быть в проекте!" );
                        shape.DeleteEx( 0 );
                        }
                    break;
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
