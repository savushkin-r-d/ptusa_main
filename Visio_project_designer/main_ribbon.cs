/// @file main_ribbon.cs
/// @brief Реализация ленты меню с командами по описанию техпроцесса.
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
using System.Linq;
using System.Text;
using Microsoft.Office.Tools.Ribbon;

using System.Windows.Forms;

using System.IO;

using System.Runtime.InteropServices; //

namespace visio_prj_designer
    {
    public partial class main_ribbon
        {
        private Microsoft.Office.Interop.Visio.Application visio_app;

        private void Ribbon1_Load( object sender, RibbonUIEventArgs e )
            {
            Globals.visio_addin.vis_main_ribbon = this;

            visio_app = Globals.visio_addin.Application;            
            }

        private void button1_Click( object sender, RibbonControlEventArgs e )
            {
            if( Globals.visio_addin.g_PAC != null )
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
                            config_f.WriteLine( "PAC_name = \"" + Globals.visio_addin.g_PAC.PAC_name + "\"" );
                            config_f.WriteLine( "-- ----------------------------------------------------------------------------" );
                            config_f.WriteLine( "--Узлы WAGO" );

                            config_f.WriteLine( "nodes = " );
                            config_f.WriteLine( "\t{" );
                            string lua_str = Globals.visio_addin.g_PAC.lua_save( "\t\t" );
                            config_f.WriteLine( lua_str );
                            config_f.WriteLine( "\t}" );

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

        private void toggleButton_edit_mode_Click( object sender, RibbonControlEventArgs e )
            {
            Globals.visio_addin.is_device_edit_mode = this.edit_mode_tbutton.Checked;

            Microsoft.Office.Interop.Visio.Windows visio_wnds = visio_app.Windows;

            // Переход в режим привязки устройств к каналам ввода\вывода.
            if( Globals.visio_addin.is_device_edit_mode )
                {
                //Закрываем другие окна при их наличии.
                for( short i = 2; i < visio_app.Windows.Count; i++ )
                    {
                    visio_wnds[ i ].Close();
                    }

                visio_wnds[ ( short ) visio_addin.VISIO_WNDOWS.MAIN ].NewWindow();
                visio_wnds[ ( short ) visio_addin.VISIO_WNDOWS.MAIN ].Activate();
                visio_wnds.Arrange();
                //visio_wnds[ ( short ) visio_addin.VISIO_WNDOWS.IO_EDIT ].Windows.ItemEx[ 
                //    "Фигуры" ].Close();


                visio_wnds[ ( short ) visio_addin.VISIO_WNDOWS.MAIN ].Page =
                    visio_app.ActiveDocument.Pages[ "Wago" ];
                visio_wnds[ ( short ) visio_addin.VISIO_WNDOWS.IO_EDIT ].Page =
                    visio_app.ActiveDocument.Pages[ "Устройства" ];

                visio_wnds[ ( short ) visio_addin.VISIO_WNDOWS.IO_EDIT ].Activate();

                edit_valve_type_form_usage tmp = new edit_valve_type_form_usage();
                tmp.create_tool_form( visio_app, true );
                Globals.visio_addin.edit_io_frm = tmp;

                //visio_wnds[ ( short ) visio_addin.VISIO_WNDOWS.MAIN ].MouseMove -=
                //    new Microsoft.Office.Interop.Visio.EWindow_MouseMoveEventHandler(
                //    Globals.visio_addin.visio_addin__MouseMove );  

                }
            else
                {
                visio_wnds[ ( short ) visio_addin.VISIO_WNDOWS.IO_EDIT ].Close();
                visio_wnds[ ( short ) visio_addin.VISIO_WNDOWS.MAIN ].Activate();
                //1 КУ;2 КУ;1 КУ 1 ОС;1 КУ 2 ОС;2 КУ 2 ОС
                }
            }

		private void Btn_devices_Click( object sender, RibbonControlEventArgs e )
			{
			Visio_project_designer.Form_device Dev_List_Form = new Visio_project_designer.Form_device();
			Dev_List_Form.Show();
			}

        private void button_modes_Click( object sender, RibbonControlEventArgs e )
            {
            if ( Globals.visio_addin.current_selected_object != null )
                {
                Visio_project_designer.Form_modes Mode_List_Form = new Visio_project_designer.Form_modes();
                Mode_List_Form.Show();
                }
            else
                {
                //  Не выбран сложный объект
                MessageBox.Show( "Объект не содержит режимов." );
                }
            }

        }
    }
