using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Office.Tools.Ribbon;

using System.Windows.Forms;

using System.IO;


namespace Visio_project_designer
    {
    public partial class Ribbon1
        {
        private void Ribbon1_Load( object sender, RibbonUIEventArgs e )
            {
            }

        private void button1_Click( object sender, RibbonControlEventArgs e )
            {
            if( ThisAddIn.g_PAC != null )
                {
                try
                    {
                    using( SaveFileDialog sfd = new SaveFileDialog
                    {
                        InitialDirectory = ".",
                        FileName = "main.wago.plua",

                        Filter = "Файлы описания устройств Wago|*.plua"
                    } )
                        {
                        if( sfd.ShowDialog() == DialogResult.OK )
                            {
                            StreamWriter config_f = new StreamWriter( sfd.FileName,
                                false, Encoding.GetEncoding( 1251 ) );

                            config_f.WriteLine( "require 'sys_wago' --Системные функции." );
                            config_f.WriteLine( "-- ----------------------------------------------------------------------------" );
                            config_f.WriteLine( "editor_version = 12" );
                            config_f.WriteLine( "file_version   = 21" );
                            config_f.WriteLine( "" );
                            config_f.WriteLine( "PAC_name = \"" + ThisAddIn.g_PAC.PAC_name + "\"" );
                            config_f.WriteLine( "-- ----------------------------------------------------------------------------" );
                            config_f.WriteLine( "--Узлы WAGO" );

                            string lua_str = ThisAddIn.g_PAC.lua_save();

                            config_f.WriteLine( lua_str );
                            config_f.Flush();
                            config_f.Close();
                            }
                        }


                    }
                catch( System.Exception ex )
                    {
                    MessageBox.Show( ex.Message );
                    }


                }
            }
        }
    }
