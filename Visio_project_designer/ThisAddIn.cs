using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;
using Visio=Microsoft.Office.Interop.Visio;
using Office=Microsoft.Office.Core;

using System.Windows.Forms;


public class PAC
    {
    public string ip_addres;
    public string PAC_name;

    public PAC ( string PAC_name, string ip_addres )
        {
        this.PAC_name  = PAC_name;
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
                        MessageBox.Show( "Только один контроллер может быть в проекте!");
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
            this.Startup+=new System.EventHandler( ThisAddIn_Startup );
            this.Shutdown+=new System.EventHandler( ThisAddIn_Shutdown );
            }

        #endregion
        }
    }
