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
using System.Text.RegularExpressions;
using Microsoft.Office.Tools.Ribbon;

using System.Windows.Forms;

using System.IO;

using System.Runtime.InteropServices; //

using wago;
using tech_device;

namespace visio_prj_designer
    {
    public partial class main_ribbon
        {
        private Microsoft.Office.Interop.Visio.Application visio_app;
        public Visio_project_designer.Form_modes Mode_List_Form;

        private void Ribbon1_Load( object sender, RibbonUIEventArgs e )
            {
            Globals.visio_addin.vis_main_ribbon = this;

            visio_app = Globals.visio_addin.Application;            
            }

        private void button1_Click( object sender, RibbonControlEventArgs e )
            {
            if( Globals.visio_addin.g_PAC_nodes[ 0 ] != null )
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
                            config_f.WriteLine( "PAC_name = \"" + Globals.visio_addin.g_PAC_nodes[ 0 ].PAC_name + "\"" );
                            config_f.WriteLine( "-- ----------------------------------------------------------------------------" );
                            config_f.WriteLine( "--Узлы WAGO" );

                            config_f.WriteLine( "nodes = " );
                            config_f.WriteLine( "\t{" );
                            string lua_str = Globals.visio_addin.g_PAC_nodes[ 0 ].lua_save( "\t\t" );
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

         private void save_as_icpcon_Click( object sender, RibbonControlEventArgs e )
             {
            BinaryWriter bw = new BinaryWriter( File.Create("wago.dsx") );

            try
                {

                Char[] kod = { 'W', 'G', ' ', '{', '4', 'B',
                    '7', '1', '4', 'C', '0', '8', '-', '9', '6', '0', '2', '-', '4', '1',
                    '3', '0', '-', '8', '5', '6', '3', '-', '4', 'B', '5', '1', 'E', '0',
                    '8', 'B', 'B', '9', 'D', '7', '}' };

                //  Записываем наш мега-код
                bw.Write( kod, 0, 41 );

                //  Версия редактора
                bw.Write( 11 );
                bw.Write( 0 );
                //  Версия файла
                bw.Write( 0 );
                bw.Write( 0 );

                //  Записываем данные по устройствам пректа
                //****************************************************************************
                byte b;
                int temp_i;

                //1.Write Header
                //port
                b = 0; //  cbPort.ItemIndex + 1;
                bw.Write( b );

                //node count
                b = ( ( byte ) Globals.visio_addin.g_PAC_nodes.Count );
                bw.Write( b );

                //  2   Проходим по узлам
                for ( int i = 0; i < Globals.visio_addin.g_PAC_nodes.Count; i++ )
                    {
                    //Type
                    temp_i = Convert.ToInt32( Globals.visio_addin.g_PAC_nodes[ i ].shape.Data2 );
                    bw.Write( temp_i );

                    //IP-adress
                    string str = Globals.visio_addin.g_PAC_nodes[ i ].ip_addres;
                    Regex rex = new Regex( @"(\d+)\.(\d+)\.(\d+)\.(\d+)", RegexOptions.IgnoreCase );
                    if ( rex.IsMatch( str ) )
                        {
                        Match mtc = rex.Match( str );

                        b = Convert.ToByte( mtc.Groups[ 1 ].ToString() );
                        bw.Write( b );
                        b = Convert.ToByte( mtc.Groups[ 2 ].ToString() );
                        bw.Write( b );
                        b = Convert.ToByte( mtc.Groups[ 3 ].ToString() );
                        bw.Write( b );
                        b = Convert.ToByte( mtc.Groups[ 4 ].ToString() );
                        bw.Write( b );
                        }
                    else
                        {
                        b = 0;
                        bw.Write( b );
                        bw.Write( b );
                        bw.Write( b );
                        bw.Write( b );
                        }

                    //  adress (номер узла)
                    bw.Write( Convert.ToByte( Globals.visio_addin.g_PAC_nodes[ i ].PAC_number ) );

                    //  module count (количество модулей)
                    bw.Write( Convert.ToByte( Globals.visio_addin.g_PAC_nodes[ i ].get_io_modules().Count ) );

                    //  3  Проходим по модулям
                    for ( int j = 0; j < Globals.visio_addin.g_PAC_nodes[ i ].get_io_modules().Count; j++ )
                        {
                        //  Записываем номер модуля
                        bw.Write( Convert.ToInt32(
                            Globals.visio_addin.g_PAC_nodes[ i ].get_io_modules()[ j ].type ) );

                        }   //  for j ... ( Проходим по модулям )
                    }   //  for i ... ( Проходим по узлам )


                
                //  Количество устройств
                bw.Write( Globals.visio_addin.g_devices.Count );

                //  Проходим по устройствам
                for ( int i = 0; i < Globals.visio_addin.g_devices.Count; i++ )
                    {
                    //  Type
                    // Типы устройств не совпадают с типами в старом редакторе WAGO
                    switch ( Globals.visio_addin.g_devices[ i ].get_type() )
                        {                
                        case ( int )tech_device.device.TYPES.T_V:	//	Клапан

                            //if ( Globals.visio_addin.g_devices[ i ].sub_type == 
                            //    tech_device.device.SUB_TYPES.V_1_CONTROL_CHANNEL ) 
                                b = ( byte ) tech_device.device.TDevType.dtV;
                            
                            if ( Globals.visio_addin.g_devices[ i ].sub_type == 
                                tech_device.device.SUB_TYPES.V_1_CONTROL_CHANNEL_1_FB ) 
                                b = ( byte ) tech_device.device.TDevType.dtV1DO1DI;

                            if ( Globals.visio_addin.g_devices[ i ].sub_type == 
                                tech_device.device.SUB_TYPES.V_1_CONTROL_CHANNEL_2_FB ) 
                                b = ( byte ) tech_device.device.TDevType.dtV1DO2DI;

                            if ( Globals.visio_addin.g_devices[ i ].sub_type == 
                                tech_device.device.SUB_TYPES.V_2_CONTROL_CHANNEL ) 
                                b = ( byte ) tech_device.device.TDevType.dtV2DO;

                            if ( Globals.visio_addin.g_devices[ i ].sub_type == 
                                tech_device.device.SUB_TYPES.V_2_CONTROL_CHANNEL_2_FB ) 
                                b = ( byte ) tech_device.device.TDevType.dtV2DO2DI;

                            if ( Globals.visio_addin.g_devices[ i ].sub_type == 
                                tech_device.device.SUB_TYPES.V_MIX_PROOF_3_UPR_2_FB ) 
                                b = ( byte ) tech_device.device.TDevType.dtMix;

                            if ( Globals.visio_addin.g_devices[ i ].sub_type == 
                                tech_device.device.SUB_TYPES.V_MIX_PROOF_AS_INTERFACE ) 
                                b = ( byte ) tech_device.device.TDevType.dtASMix;

                            break;

                        case ( int )tech_device.device.TYPES.T_N:	//	Насос
                            b = ( byte ) tech_device.device.TDevType.dtN1DO1DI;
                            break;

                        case ( int )tech_device.device.TYPES.T_MIX:	//	Мешалка
                            b = ( byte ) tech_device.device.TDevType.dtM1DO1DI;
                            break;

                        case ( int )tech_device.device.TYPES.T_CTR:	//	Расходомер
                            b = ( byte ) tech_device.device.TDevType.dtCTR;
                            break;

                        case ( int )tech_device.device.TYPES.T_TE:	//	Температура
                            b = ( byte ) tech_device.device.TDevType.dtTE;
                            break;

                        case ( int )tech_device.device.TYPES.T_QE:	//	Концентратомер
                            b = ( byte ) tech_device.device.TDevType.dtQE;
                            break;

                        case ( int )tech_device.device.TYPES.T_LS:	//	Гарничный уровень
                            b = ( byte ) tech_device.device.TDevType.dtLS_ex;
                            break;

                        case ( int )tech_device.device.TYPES.T_LE:	//	Текущий уровень
                            //  Уровни у нас 4-х типов
                            b = ( byte )
                                (
                                (( byte ) tech_device.device.TDevType.dtLE1 ) + 
                                (( byte ) Globals.visio_addin.g_devices[ i ].sub_type )
                                );
                            break;

                        case ( int )tech_device.device.TYPES.T_FS:	//	Расход (есть/нет)
                            b = ( byte ) tech_device.device.TDevType.dtFS;
                            break;

                        case ( int )tech_device.device.TYPES.T_FE:  //	Текущий расход
                            b = ( byte ) tech_device.device.TDevType.dtFE;
                            break;

                        case ( int )tech_device.device.TYPES.T_FB:	//	Обратнся связь
                            b = ( byte ) tech_device.device.TDevType.dtOS;
                            break;

                        case ( int )tech_device.device.TYPES.T_UPR:	//	Управляющий сигнал
                            b = ( byte ) tech_device.device.TDevType.dtUpr;
                            break;

                        case ( int )tech_device.device.TYPES.T_AI:	//	Аналоговый вход
                            b = ( byte ) tech_device.device.TDevType.dtAI;
                            break;

                        case ( int )tech_device.device.TYPES.T_AO:	//	Аналоговый выход
                            b = ( byte ) tech_device.device.TDevType.dtAO;
                            break;

//                        case ( int )tech_device.device.TYPES.T_FQT:	//	Расходомер и Концентратомер
//                            b = ( byte ) tech_device.device.TDevType.dt;
//                            break;

//                       case ( int )tech_device.device.TYPES.T_WTE:	//	Температура и Влажность
//                            b = ( byte ) tech_device.device.TDevType.dt;
//                            break;  
                        
                        default:
                            continue;   //  for i ...
                        }

                    bw.Write( b );

                    //  length description
                    String temp_descr = Globals.visio_addin.g_devices[ i ].description;
                    b = Convert.ToByte( temp_descr.Length );
                    if ( b > 42 )
                        {
                        temp_descr = temp_descr.Remove( 42 ) + "..";
                        b = Convert.ToByte( temp_descr.Length );
                        }

                    bw.Write( b );

                    //  description
                    for ( int j = 0; j < temp_descr.Length; j++ )
                        {
                        bw.Write( temp_descr[ j ] );
                        }
                    
                    //  no
                    bw.Write( Globals.visio_addin.g_devices[ i ].get_n() );

                    //  chennel count
                    bw.Write( Convert.ToByte(
                        Globals.visio_addin.g_devices[ i ].wago_channels.Count ) );

                    //  Проходим по каналам
                    foreach ( KeyValuePair<string, wago.wago_channel> chen in
                                Globals.visio_addin.g_devices[ i ].wago_channels )
                        {
                        //  chennel type
                        // Синхронизировать список типов со старой версией
                        bw.Write( ( byte ) chen.Value.kind );
                        
                        //  А вот отсюда начнется крутой пересчет адресов в таблице

                        //  tabel no
                        bw.Write( 1 );

                        //  offset
                        bw.Write( 1 );

                        //  value
                        bw.Write( 4 );
                        }

                    }

//             for j = 0 to Length( Devices[i].Chens ) - 1 do
//                 {
//                 //chen type
//                 b = integer( Devices[i].Chens[j].ctype );
//                 bw.Write( b, 1 );
//                 
//                 //tbl no
//                 b = Devices[i].Chens[j].TblNo;
//                 bw.Write( b, 1 );
// 
//                 //offset
//                 b = Devices[i].Chens[j].Offset;
//                 bw.Write( b, 1 );
// 
//                 //val
//                 fl = Devices[i].Chens[j].val;
//                 bw.Write( fl, 4 );
//                 }

                //  4   Write AS info
                //  port контроллера
                b = Convert.ToByte( Globals.visio_addin.g_PAC_nodes[ 0 ].
                            shape.Cells["Prop.port" ].Formula );
                bw.Write( b );

                //  gateways count
                b = Convert.ToByte( Globals.visio_addin.g_PAC_nodes[ 0 ].
                            shape.Cells["Prop.lock_cnt" ].Formula );
                bw.Write( b );

                //  write gateways - номера шлюзов начиная с 1
                for ( byte i = 1; i <= b; i++ )
                    {
                    //addrs (просто перечисление адресов)
                    bw.Write( i );
                    }

//****************************************************************************


                bw.Close();

                }
            catch ( System.Exception ex )
                {
                bw.Close();
                MessageBox.Show( ex.Message );
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

                //	Снятие подсветки с ранее выбранных модулей
                for ( int i = 0; i < Globals.visio_addin.g_PAC_nodes.Count; i++ )
                    {
                    foreach ( io_module mod in Globals.visio_addin.g_PAC_nodes[ i ].get_io_modules() )
                        {
                        mod.deactivate();
                        }
                    }
                }
            }

		private void Btn_devices_Click( object sender, RibbonControlEventArgs e )
			{
			Visio_project_designer.Form_device Dev_List_Form = new Visio_project_designer.Form_device();
			Dev_List_Form.Show();
			}

        private void button_modes_Click( object sender, RibbonControlEventArgs e )
            {
            if ( Globals.visio_addin.cur_sel_obj != null )
                {
                Mode_List_Form = new Visio_project_designer.Form_modes();
                Mode_List_Form.Show();
                }
            else
                {
                //  Не выбран сложный объект
                MessageBox.Show( "Объект не содержит режимов." );
                }
            }

        public void insert_dev_list()
            {
            Microsoft.Office.Interop.Visio.Windows visio_wnds = visio_app.Windows;

            visio_wnds[ ( short ) visio_addin.VISIO_WNDOWS.MAIN ].Page =
                        visio_app.ActiveDocument.Pages[ "Устройства" ];
  
            visio_wnds[ ( short ) visio_addin.VISIO_WNDOWS.MAIN ].Activate();
            visio_wnds.Arrange();
            }

        }
    }
