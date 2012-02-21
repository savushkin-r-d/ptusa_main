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

using Excel = Microsoft.Office.Interop.Excel;

using wago;
using tech_device;

namespace visio_prj_designer
    {
    public partial class main_ribbon
        {
        //  Для работы с Excel
        private Excel.Application excel_app;
        //private Excel.Window excelWindow;
        private Excel.Workbook excel_wb;
        private Excel.Worksheet excel_ws;
        private Excel.Range excel_cells;

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
        //-------------------------------------------------------------------------------

        private static int sort_devices( device dev1, device dev2 )
            {
            if ( ( dev1 != null ) && ( dev2 != null ) )
                {
                //  Сравнение по типам
                if ( dev1.get_type() == dev2.get_type() )
                    {
                    //  Сравнение по номеру устройства
                    if ( dev1.get_n() == dev2.get_n() )
                        {
                        return 0;   //  equal
                        }
                    else
                        {
                        if ( dev1.get_n() > dev2.get_n() )
                            {
                            return 1;   //  dev1 is greater
                            }
                        else
                            {
                            return -1;  //  dev2 is greater
                            }
                        }
                    }
                else
                    {
                    if ( dev1.get_type() > dev2.get_type() )
                        {
                        return 1;   //  dev1 is greater
                        }
                    else
                        {
                        return -1;  //  dev2 is greater
                        }
                    }
                }
                                
            return 0;
            }
        //-------------------------------------------------------------------------------

        public int sort_modules( io_module mod1, io_module mod2 )
            {
            if ( mod1.order_number == mod2.order_number )
                {
                return 0;   //  equal
                }
            else
                {
                if ( mod1.order_number > mod2.order_number )
                    {
                    return 1;   //  mod1 is greater
                    }
                else
                    {
                    return -1;   //  mod2 is greater
                    }
                }
            }
         //-------------------------------------------------------------------------------

         private void save_as_icpcon_Click( object sender, RibbonControlEventArgs e )
             {
             // Проверяем есть ли в описании контроллер ICPCON
             if ( Globals.visio_addin.g_PAC_nodes[ 0 ].shape.Data2 != "7186" )
                 {
                 MessageBox.Show( "В описании нет контроллера ICP CON !" );
                 return;
                 }
                 

            //  Открываем файл для записи описания
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
                bw.Write( (byte) 11 );
                bw.Write( (byte) 0 );
                //  Версия файла
                bw.Write( (byte) 0 );
                bw.Write( (byte) 0 );

                //  Записываем данные по устройствам проекта
                //****************************************************************************
                byte b, b1;
                short sh;
                int temp_i;

                //1.Write Header
                //port
                b = Convert.ToByte( Globals.visio_addin.g_PAC_nodes[ 0 ].shape.Cells[ "Prop.port" ].Formula );
                bw.Write( b );
                
                //node count
                b1 = ( byte ) ( Globals.visio_addin.g_PAC_nodes.Count - 1 );
                bw.Write( b1 );    

                //  2   Проходим по узлам
                for ( int i = 1; i < Globals.visio_addin.g_PAC_nodes.Count; i++ )
                    {
                    //Type
                    b = ( byte ) ( Globals.visio_addin.Node_Types.IndexOf( 
                                Globals.visio_addin.g_PAC_nodes[ i ].shape.Data2 ) );
                    bw.Write( b );

                    //IP-adress
                    string str = Globals.visio_addin.g_PAC_nodes[ i ].ip_addres;
                    Regex rex = new Regex( @"(\d+)\.(\d+)\.(\d+)\.(\d+)", RegexOptions.IgnoreCase );
                    if (    rex.IsMatch( str ) 
                        &&  Globals.visio_addin.g_PAC_nodes[ i ].shape.Data2 != Globals.visio_addin.Node_Types[ 0 ]
                        &&  Globals.visio_addin.g_PAC_nodes[ i ].shape.Data2 != Globals.visio_addin.Node_Types[ 1 ] )
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
//                     else
//                         {
//                         b = 0;
//                         bw.Write( b );
//                         bw.Write( b );
//                         bw.Write( b );
//                         bw.Write( b );
//                         }

                    //  adress (номер узла)
                    b = Convert.ToByte( Globals.visio_addin.g_PAC_nodes[ i ].PAC_number );
                    bw.Write( b );

                    //  module count (количество модулей)
                    b = Convert.ToByte( Globals.visio_addin.g_PAC_nodes[ i ].get_io_modules().Count );
                    bw.Write( b );

                    //  Сортируем модули перед тем, как записывать данные о них
                    Globals.visio_addin.g_PAC_nodes[ i ].get_io_modules().Sort( sort_modules );

                    //  3  Проходим по модулям
                    for ( int j = 0; j < Globals.visio_addin.g_PAC_nodes[ i ].get_io_modules().Count; j++ )
                        {
                        //  Записываем номер модуля
                        temp_i = Convert.ToInt32( Globals.visio_addin.g_PAC_nodes[ i ].get_io_modules()[ j ].type );
                        bw.Write( ( short )temp_i );

                        }   //  for j ... ( Проходим по модулям )
                    }   //  for i ... ( Проходим по узлам )


                
                //  Количество устройств
                bw.Write( ( short ) Globals.visio_addin.g_devices.Count );

                Globals.visio_addin.g_devices.Sort( sort_devices );

                //  Проходим по устройствам
                for ( int i = 0; i < Globals.visio_addin.g_devices.Count; i++ )
                    {
                    try
                        {
                        //  Type
                        // Типы устройств не совпадают с типами в старом редакторе WAGO
                        switch ( Globals.visio_addin.g_devices[ i ].get_type() )
                            {
                            case ( int ) tech_device.device.TYPES.T_V:	//	Клапан

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

                            case ( int ) tech_device.device.TYPES.T_N:	//	Насос
                                b = ( byte ) tech_device.device.TDevType.dtN1DO1DI;
                                break;

                            case ( int ) tech_device.device.TYPES.T_MIX:	//	Мешалка
                                b = ( byte ) tech_device.device.TDevType.dtM1DO1DI;
                                break;

                            case ( int ) tech_device.device.TYPES.T_CTR:	//	Расходомер
                                b = ( byte ) tech_device.device.TDevType.dtCTR;
                                break;

                            case ( int ) tech_device.device.TYPES.T_TE:	//	Температура
                                b = ( byte ) tech_device.device.TDevType.dtTE;
                                break;

                            case ( int ) tech_device.device.TYPES.T_QE:	//	Концентратомер
                                b = ( byte ) tech_device.device.TDevType.dtQE;
                                break;

                            case ( int ) tech_device.device.TYPES.T_LS:	//	Гарничный уровень
                                b = ( byte ) tech_device.device.TDevType.dtLS_ex;
                                break;

                            case ( int ) tech_device.device.TYPES.T_LE:	//	Текущий уровень
                                //  Уровни у нас 4-х типов
                                b = ( byte )
                                    (
                                    ( ( byte ) tech_device.device.TDevType.dtLE1 ) +
                                    ( ( byte ) Globals.visio_addin.g_devices[ i ].sub_type )
                                    );
                                break;

                            case ( int ) tech_device.device.TYPES.T_FS:	//	Расход (есть/нет)
                                b = ( byte ) tech_device.device.TDevType.dtFS;
                                break;

                            case ( int ) tech_device.device.TYPES.T_FE:  //	Текущий расход
                                b = ( byte ) tech_device.device.TDevType.dtFE;
                                break;

                            case ( int ) tech_device.device.TYPES.T_FB:	//	Обратнся связь
                                b = ( byte ) tech_device.device.TDevType.dtOS;
                                break;

                            case ( int ) tech_device.device.TYPES.T_UPR:	//	Управляющий сигнал
                                b = ( byte ) tech_device.device.TDevType.dtUpr;
                                break;

                            case ( int ) tech_device.device.TYPES.T_AI:	//	Аналоговый вход
                                b = ( byte ) tech_device.device.TDevType.dtAI;
                                break;

                            case ( int ) tech_device.device.TYPES.T_AO:	//	Аналоговый выход
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

                        //Проверить что записываем и сравнить с тем что записывает старый редактор

                        bw.Write( b );  //  type dev

                        //  length description
                        String temp_descr = Globals.visio_addin.g_devices[ i ].description;
                        b = Convert.ToByte( temp_descr.Length );
                        if ( b > 40 )
                            {
                            temp_descr = temp_descr.Remove( 37 ) + "...";
                            b = Convert.ToByte( temp_descr.Length );
                            }

                        bw.Write( b );

                        //  description
                        for ( int j = 0; j < temp_descr.Length; j++ )
                            {
                            bw.Write( ( byte ) temp_descr[ j ] );
                            }

                        //  no
                        sh = ( short ) Globals.visio_addin.g_devices[ i ].get_n();
                        bw.Write( sh );

                        //  chennel count
                        bw.Write( Convert.ToByte(
                            Globals.visio_addin.g_devices[ i ].wago_channels.Count ) );

                        //  Проходим по каналам
                        foreach ( KeyValuePair<string, wago.wago_channel> chen in
                                    Globals.visio_addin.g_devices[ i ].wago_channels )
                            {
                            bw.Write( ( byte ) chen.Value.kind );   // 1  channel type

                            //  Параметр это или канал устройства
                            if ( chen.Value.kind == io_module.KINDS.PARAM )
                                {
                                bw.Write( ( byte ) 0 );         // 2  table no (node no) - порядковый номер в списке == просто номер узла
                                bw.Write( ( byte ) 0 );         // 3  offset
                                bw.Write( chen.Value.clamp );   // 4  value (Значение параметра - на случай, если это параметр)
                                }
                            else
                                {
                                //  Если устройство не привязано
                                if ( chen.Value.module == null )
                                    {
                                    bw.Write( ( byte ) 0 );     // 2  table no
                                    bw.Write( ( byte ) 0 );     // 3  offset
                                    }
                                else
                                    {
                                    bw.Write( ( byte )( chen.Value.module.node_number - 1 ) );  // 2  tabel no

                                    int index = Globals.visio_addin.get_index_PAC(
                                        Globals.visio_addin.g_PAC_nodes, chen.Value.module.node_number );
                                    b = chen.Value.get_clamp_offset( Globals.visio_addin.g_PAC_nodes[ index ] );

                                    bw.Write( ( byte ) b );          // 3  offset
                                    }

                                bw.Write( ( int ) 0 );     // 4  value
                                }
                            }

                        }
                    catch ( System.Exception )
                        {
                        MessageBox.Show( "Ошибка записи данных устройства " + 
                            Globals.visio_addin.g_devices[ i ].name );
                        }
                    }   //  for i...


                try
                    {
                    //  4   Write AS info
                    //  port контроллера
                    b = Convert.ToByte( Globals.visio_addin.g_PAC_nodes[ 0 ].
                                shape.Cells[ "Prop.port" ].Formula );
                    bw.Write( b );

                    //  gateways count
                    b = Convert.ToByte( Globals.visio_addin.g_PAC_nodes[ 0 ].
                                shape.Cells[ "Prop.lock_cnt" ].Formula );
                    bw.Write( b );

                    //  write gateways - номера шлюзов начиная с 1
                    for ( byte i = 1; i <= b; i++ )
                        {
                        //addrs (просто перечисление адресов)
                        bw.Write( ( byte ) i );
                        }
                    }
                catch ( System.Exception )
                    {
                    MessageBox.Show( "Ошибка записи данных ASinterface!" );
                    }


//****************************************************************************
                //  Объекты типа ГРЕБЕНКА
                T_Object temp_obj = Globals.visio_addin.g_objects.Find(
                    delegate( T_Object temp )
                    {
                    return temp.type == device.TYPES.T_GREB;
                    }
                    );

                b1 = 0;
                if ( temp_obj != null )
                    {
                    b1 = 1;
                    }
                    
                bw.Write( b1 );     //  Количество гребенок

                b1 = (byte) Globals.visio_addin.g_objects.Count;    //  Количество танков 

                //  Проходим по сложным устройствам
                foreach ( T_Object obj in Globals.visio_addin.g_objects )
                    {
                    try
                        {
                        if ( obj.get_type() == ( int ) device.TYPES.T_GREB )
                            {
                            bw.Write( ( byte ) obj.sub_type );                      //  подтип гребенки
                            bw.Write( ( byte ) ( obj.mode_mas.Count / 32 + 1 ) );   //  states

                            bw.Write( ( short ) obj.n );                                      // Номер гребенки
                            bw.Write( ( short )
                                ( obj.param_list_save.Count + obj.param_list_temp.Count ) );  // Кол-во параметров
                            bw.Write( ( short ) obj.param_list_temp.Count );                  // Кол-во врем. пар-ов
                            bw.Write( ( short ) obj.timers );                                 // Кол-во таймеров

                            //  Проходим по режимам
                            for ( int i = 0; i < ( obj.mode_mas.Count / 32 + 1 )*32; i++ )
                                {
                                /*
                                "Параметры"
                                "Устройства"
                                    "Включать"
                                    "Выключать"
                                "Сигналы"
                                "Ограничения"
                                */
                                if ( i < obj.mode_mas.Count )
                                    {
                                    get_write_data( bw, obj.mode_mas[ i ], "Устройства", "Включать" );
                                    get_write_data( bw, obj.mode_mas[ i ], "Устройства", "Выключать" );
                                    get_write_data( bw, obj.mode_mas[ i ], "Устройства", "Включать_по_завершению" );
                                    get_write_data( bw, obj.mode_mas[ i ], "Устройства", "Верхний_флип" );
                                    get_write_data( bw, obj.mode_mas[ i ], "Устройства", "Нижний_флип" );

                                    bw.Write( ( byte ) 0 ); //  FB
                                    bw.Write( ( byte ) 0 ); //  N
                                    bw.Write( ( byte ) 0 ); //  V
                                    bw.Write( ( short ) 0 ); //  washFB
                                    bw.Write( ( byte ) 0 ); //  washUPR
                                    bw.Write( ( byte ) 0 ); //  in_FB
                                    bw.Write( ( byte ) 0 ); //  out_UPR

                                    bw.Write( ( byte ) 0 ); //  Маршрут
                                    }
                                else
                                    {
                                    bw.Write( ( byte ) 0 ); //  ON
                                    bw.Write( ( byte ) 0 ); //  OFF
                                    bw.Write( ( byte ) 0 ); //  OFF_after
                                    bw.Write( ( byte ) 0 ); //  U_FLIP
                                    bw.Write( ( byte ) 0 ); //  L_FLIP

                                    bw.Write( ( byte ) 0 ); //  FB
                                    bw.Write( ( byte ) 0 ); //  N
                                    bw.Write( ( byte ) 0 ); //  V
                                    bw.Write( ( short ) 0 ); //  washFB
                                    bw.Write( ( byte ) 0 ); //  washUPR
                                    bw.Write( ( byte ) 0 ); //  in_FB
                                    bw.Write( ( byte ) 0 ); //  out_UPR

                                    bw.Write( ( byte ) 0 ); //  Маршрут
                                    }

                                //get_write_data( bw, obj.mode_mas[ i ], "Сигналы", "Включающие_режим" );
                                //get_write_data( bw, obj.mode_mas[ i ], "Сигналы", "Включающие_режим" );
                                //get_write_data( bw, obj.mode_mas[ i ], "Сигналы", "Включающие_режим" );
                                }

                            //  Определяем сколько Танков
                            b1 = ( byte ) ( Globals.visio_addin.g_objects.Count - 1 );

                            break;
                            }
                        }
                    catch ( System.Exception )
                        {
                        MessageBox.Show( "Ошибка записи данных объекта " + obj.name );
                        }
                    }


                //  Объекты типа ТАНК
                bw.Write( b1 );

                foreach ( T_Object obj in Globals.visio_addin.g_objects )
                    {
                    try
                        {
                        if ( obj.get_type() == ( int ) device.TYPES.T_TANK )
                            {
                            bw.Write( ( byte ) obj.sub_type );                      // Подтип танка
                            bw.Write( ( byte ) 0 );                                // Номер гребенки для танка
                            bw.Write( ( byte ) ( obj.mode_mas.Count / 32 + 1 ) );  // Количество режимов

                            bw.Write( ( short ) obj.n );                                      // Номер танка
                            bw.Write( ( short )
                                ( obj.param_list_save.Count + obj.param_list_temp.Count ) );  // Кол-во параметров
                            bw.Write( ( short ) obj.param_list_temp.Count );                  // Кол-во врем. пар-ов
                            bw.Write( ( short ) obj.timers );                                 // Кол-во таймеров

                            //  Проходим по режимам
                            for ( int i = 0; i < ( obj.mode_mas.Count / 32 + 1 ) * 32; i++ )
                                {
                                if ( i < obj.mode_mas.Count )
                                    {
                                    //  "Параметры"      "Сигналы"          "Ограничения"
                                    bw.Write( ( byte ) obj.mode_mas[ i ].step.Count ); //  Количество шагов

                                    for ( int j = 0; j < obj.mode_mas[ i ].step.Count; j++ )
                                        {
                                        //get_write_data( bw, obj.mode_mas[ i ], "Устройства", "Включать" );
                                        //get_write_data( bw, obj.mode_mas[ i ], "Устройства", "Выключать" );

                                        get_write_data( bw, obj.mode_mas[ i ].step[ j ], "Устройства", "Включать" );
                                        get_write_data( bw, obj.mode_mas[ i ].step[ j ], "Устройства", "Выключать" );

                                        bw.Write( ( byte ) 0 );    //Следующий шаг при завершении времени текущего шага
                                        bw.Write( ( byte ) 0 );    //Номер параметра, содержащий время шага, мин.                    
                                        bw.Write( ( byte ) 0 );    //  Маршрут
                                        }   //  for j
                                    }
                                else
                                    {
                                    bw.Write( ( byte ) 0 ); //  Количество шагов
                                    }
                                }   //  for i
                            }   

                        }
                    catch ( System.Exception )
                        {
                        MessageBox.Show( "Ошибка записи данных объекта " + obj.name );
                        }
                    }

                //  Структура гребенки
                bw.Write( ( byte ) 0 ); //  Y
                bw.Write( ( byte ) 0 ); //  X

                bw.Write( ( byte ) 0 ); //  гребенок
                bw.Write( ( byte ) 0 ); //  танков

/*
                //  Имена режимов Гребенки
                //[ 0 ] - количество гребенок
                //[ 1 ] - индекс гребенки
                //[ 2 ] - количество режимов гребенки
                //[ 3 ] - номер режима
                //[ 4 ] - длина названия
                //[ 5 ] - название режима
                bw.Write( ( byte ) 1 );

                foreach ( T_Object obj in Globals.visio_addin.g_objects )
                    {
                    try
                        {
                        if ( obj.get_type() == ( int ) device.TYPES.T_GREB )
                            {
                            bw.Write( ( byte ) obj.n );
                            bw.Write( ( byte ) obj.mode_mas.Count );

                            for ( byte j = 0; j < obj.mode_mas.Count; j++ )
                                {
                                bw.Write( j );
                                bw.Write( ( short ) obj.mode_mas[ j ].name.Length );
                                bw.Write( obj.mode_mas[ j ].name );
                                //for ( int k = 0; k < obj.mode_mas[ j ].name.Length; k++ )
                                //    {
                                //    bw.Write( ( byte ) obj.mode_mas[ j ].name[ k ] );
                                //    }
                                }
                            }
                        }
                    catch ( System.Exception )
                        {
                        MessageBox.Show( "Ошибка записи названия режимов объекта " + obj.name );
                        }
                    }


                //  Имена режимов Танка
                //[ 0 ] - количество танков
                //[ 1 ] - индекс танка
                //[ 2 ] - количество режимов танка
                //[ 3 ] - номер режима
                //[ 4 ] - длина названия
                //[ 5 ] - название режима
                bw.Write( ( byte ) ( Globals.visio_addin.g_objects.Count - 1 ) );

                foreach ( T_Object obj in Globals.visio_addin.g_objects )
                    {
                    try
                        {
                        if ( obj.get_type() == ( int ) device.TYPES.T_TANK )
                            {
                            bw.Write( ( byte ) obj.n );
                            bw.Write( ( byte ) obj.mode_mas.Count );

                            for ( byte j = 0; j < obj.mode_mas.Count; j++ )
                                {
                                bw.Write( j );
                                bw.Write( ( short ) obj.mode_mas[ j ].name.Length * 2 );
                                bw.Write( obj.mode_mas[ j ].name );
                                //for ( int k = 0; k < obj.mode_mas[ j ].name.Length; k++ )
                                //    {
                                //    bw.Write( ( byte ) obj.mode_mas[ j ].name[ k ] );
                                //    }
                                }
                            }
                        }
                    catch ( System.Exception )
                        {
                        MessageBox.Show( "Ошибка записи названия режимов объекта " + obj.name );
                        }
                     }
*/
//****************************************************************************
                bw.Close();

                }
            catch ( System.Exception ex )
                {
                bw.Close();
                MessageBox.Show( ex.Message );
                }

             }
         //-----------------------------------------------------------------------------------------

         private string get_chen_string( io_module mod, int chen )
             {
             List<string> res = new List<string>();
             string str = "";

             // Проходим по устройствам
             foreach ( device dev in Globals.visio_addin.g_devices )
                 {
                 // Проходим по каналам устройства и ищем привязанные на данную клемму
                 foreach ( KeyValuePair<string, wago_channel> channel in dev.wago_channels )
                     {
                     // (1) Если тип модуля и тип канала устройства совпадают
                     // (2) Если канал имеет привязку
                     // (3) Если в привязке указан текущий узел
                     // (4) Если в привязке указан текущий модуль
                     // (5) Если в привязке указан текущая клемма
                     if ( ( channel.Value.kind == mod.kind )
                       && ( channel.Value.module != null )
                       && ( channel.Value.module.node_number == mod.node_number )
                       && ( channel.Value.module.order_number == mod.order_number )
                       && ( channel.Value.clamp == chen ) )
                         {
                         res.Add( channel.Key + ": " + dev.get_name() + " (" +
                                  Convert.ToString( dev.n ) + ") " + dev.get_descr() );
                         }
                     }      //  foreach channel
                 }      //  foreach dev

             // Записываем все полученные записи в одну строку
             for ( int j = 0; j < res.Count; j++ )
                 {
                 str = str + res[ j ] + ( ( j != res.Count - 1 ) ? "\n" : "" );
                 }

             return str;
             }
         //-----------------------------------------------------------------------------------------

         private int Drow_Module_Grid( io_module mod )
             {
             Excel.Range temp_cell = excel_cells;
             int cur_row = excel_cells.Row;
             int cur_col = excel_cells.Column;

             // mod.node_number             // Номер узла
             // mod.order_number            // Порядковый номер модуля
             // mod.type                    // Номер модуля (тип)
             // mod.work_clamps_cnt         // Количество рабочих клемм
             // ( byte ) mod.total_clamps   // Количество клемм в модуле
             int clamp_cnt = ( ( ( byte ) mod.total_clamps > 0 ) ? ( byte ) mod.total_clamps - 1 : 1 );

             // Цвет модуля
             int color = Convert.ToInt32( mod.get_color() ) + 1;

             // Рисуем тонкую сетку на всю таблицу для модуля
             temp_cell = excel_ws.get_Range(
                 ( Excel.Range ) excel_ws.Cells[ cur_row, cur_col ],
                 ( Excel.Range ) excel_ws.Cells[ cur_row + clamp_cnt, cur_col + 3 ]
                 );
             temp_cell.Borders.LineStyle = Excel.XlLineStyle.xlContinuous;
             temp_cell.Borders[ Excel.XlBordersIndex.xlEdgeTop ].Weight = Excel.XlBorderWeight.xlMedium;
             temp_cell.Borders[ Excel.XlBordersIndex.xlEdgeBottom ].Weight = Excel.XlBorderWeight.xlMedium;
             temp_cell.Borders[ Excel.XlBordersIndex.xlEdgeLeft ].Weight = Excel.XlBorderWeight.xlMedium;
             temp_cell.Borders[ Excel.XlBordersIndex.xlEdgeRight ].Weight = Excel.XlBorderWeight.xlMedium;

             // Выводим порядковый номер модуля
             temp_cell = excel_ws.get_Range(
                 ( Excel.Range ) excel_ws.Cells[ cur_row, cur_col ],
                 ( Excel.Range ) excel_ws.Cells[ cur_row + clamp_cnt, cur_col ]
                 );
             // Объединяем ячейки  и задаем их характеристики
             temp_cell.Merge( Type.Missing );
             temp_cell.Borders.Weight = Excel.XlBorderWeight.xlMedium;
             // Задаем выравнивание по центру
             temp_cell.HorizontalAlignment = Excel.Constants.xlCenter;
             temp_cell.VerticalAlignment = Excel.Constants.xlCenter;
             temp_cell.Value2 = mod.order_number;   // Выводим номер
             temp_cell.ColumnWidth = 4;

             // Выводим номер модуля
             temp_cell = excel_ws.get_Range(
                 ( Excel.Range ) excel_ws.Cells[ cur_row, cur_col + 1 ],
                 ( Excel.Range ) excel_ws.Cells[ cur_row + clamp_cnt, cur_col + 1 ]
                 );
             // Объединяем ячейки  и задаем их характеристики
             temp_cell.Merge( Type.Missing );
             temp_cell.Borders.Weight = Excel.XlBorderWeight.xlMedium;
             temp_cell.Interior.ColorIndex = color; //  Заливаем нужным цветом
             // Задаем выравнивание по центру
             temp_cell.HorizontalAlignment = Excel.Constants.xlCenter;
             temp_cell.VerticalAlignment = Excel.Constants.xlCenter;
             temp_cell.Value2 = ( int ) ( mod.type );   // Выводим номер
             temp_cell.ColumnWidth = 4;

             // Выводим таблицу даннх по модулю
             for ( int i = 0; i <= clamp_cnt; i++ )
                 {
                 // Номер клеммы
                 temp_cell = ( Excel.Range ) excel_ws.Cells[ cur_row + i, cur_col + 2 ];
                 temp_cell.Value2 = i + 1;
                 temp_cell.ColumnWidth = 4;

                 // Данные о привязке клемм
                 temp_cell.get_Offset( 0, 1 ).Value2 = get_chen_string( mod, i );

                 // Если клемма не предназначена для привязки подсвечиваем ее серым цветом
                 if ( mod.available_clamp_flags != null
                   && mod.available_clamp_flags[ i ] == false )
                     {
                     temp_cell.get_Offset( 0, 1 ).Interior.ColorIndex = 15;
                     }
                 }


             // Переходим на следующую строку (Изменяем текущую ячейку)
             excel_cells =
                ( Excel.Range ) excel_ws.Cells[ cur_row + clamp_cnt + 1, cur_col ];

             return 0;
             }
         //-----------------------------------------------------------------------------------------

         private void export_to_excel_Click( object sender, RibbonControlEventArgs e )
             {
             Dictionary<int, int> resume = new Dictionary<int, int>();   //  Для подсчета модулей

             excel_app = new Excel.Application();           //  
             excel_app.SheetsInNewWorkbook = 3;               //  Количество листов
             excel_app.Workbooks.Add( Type.Missing );       //  Создаем рабочую книгу
             excel_wb = excel_app.Workbooks[ 1 ];           //  Выбираем одну (если их несколько)
             excel_ws = excel_wb.Worksheets.get_Item( 1 );  //  Выбираем страницу

             // Выводим название проекта и дату формирования отчета
             excel_cells = excel_ws.get_Range( "D1", "D1" );
             excel_cells.Value2 = visio_app.ActiveDocument.Name + " " + Convert.ToString( DateTime.Now );
             excel_cells = excel_ws.get_Range( "A2", "A2" );

             try
                 {
                 //  Проходим по узлам проекта
                 for ( int i = 0; i < Globals.visio_addin.g_PAC_nodes.Count; i++ )
                     {
                     // Создаем временную переменную для удобства
                     PAC temp_node = Globals.visio_addin.g_PAC_nodes[ i ];

                     excel_cells.Value2 = "Узел " + Convert.ToString( temp_node.PAC_number );
                     excel_cells = excel_cells.get_Offset( 1, 0 );

                     // Упорядочиваем модули
                     temp_node.get_io_modules().Sort( sort_modules );

                     // Проходим по всем модулям узла
                     foreach ( io_module mod in temp_node.get_io_modules() )
                         {
                         // Заполнение данных по модулям
                         Drow_Module_Grid( mod );

                         //  Подсчет имеющихся модулей
                         if ( resume.Keys.Contains( ( int ) mod.type ) )
                             {
                             resume[ ( int ) mod.type ]++;
                             }
                         else
                             {
                             resume.Add( ( int ) mod.type, 1 );
                             }
                         }   //  foreach mod
                     }   //  for i

                 excel_cells = excel_cells.get_Offset( 1, 0 );


                 //  Вывод количества модулей
                 foreach ( KeyValuePair<int, int> temp in resume )
                     {
                     excel_cells.Value2 = temp.Key;
                     //excel_cells.Interior.ColorIndex = Convert.ToInt32( mod.get_color() );
                     excel_cells.get_Offset( 0, 1 ).Value2 = temp.Value;
                     excel_cells = excel_cells.get_Offset( 1, 0 );
                     }

                 excel_cells = excel_cells.get_Offset( 1, 0 );


                 //  Вывод информации по устройствам с AS interface
                 if ( Globals.visio_addin.g_PAC_nodes[ 0 ].shape.Data2 == "7186" )
                     {
                     int cnt = Convert.ToInt32( Globals.visio_addin.g_PAC_nodes[ 0 ].shape.Cells[ "Prop.lock_cnt" ].Formula );
 
                     //  Проходим по шлюзам
                     for ( int i = 1; i <= cnt; i++ )
                         {
                         excel_cells.get_Offset( 0, 3 ).Interior.ColorIndex = 15;
                         excel_cells.get_Offset( 0, 3 ).Value2 =
                             "AS шлюз №" + Convert.ToString( i ) + 
                             " Адрес " + Convert.ToString( i );
                         
                         excel_cells = excel_cells.get_Offset( 1, 0 );

                         //  Проверяем устройства на соотв. текущему значению адреса
                         int adr;
                         string str = "";
                         List<string> res = new List<string>();
                         
                         foreach ( device dev in Globals.visio_addin.g_devices )
                             {
                             if ( ( dev.type == device.TYPES.T_V )
                               && ( dev.sub_type == device.SUB_TYPES.V_MIX_PROOF_AS_INTERFACE )
                               && ( dev.wago_channels[ "AS_gateway" ].clamp == i ) )
                                 {
                                 adr = dev.wago_channels[ "AS_adres" ].clamp;
                                 str = dev.get_name() + " (" + Convert.ToString( dev.n ) + ") " + dev.get_descr();
                                 
                                 // Выводим в Excel
                                 excel_cells.get_Offset( 0, 2 ).Value2 = adr;
                                 excel_cells.get_Offset( 0, 3 ).Value2 = str;
                                 excel_cells = excel_cells.get_Offset( 1, 0 );
                                 }
                             } 

                         excel_cells = excel_cells.get_Offset( 1, 0 );
                         }  //  for i
                     }  //    if

                 //  Автоподбор ширины столбца с описанием
                 excel_cells = ( Excel.Range ) excel_ws.Columns[ "A:E", Type.Missing ];
                 excel_cells.EntireColumn.AutoFit();
                 excel_cells.EntireRow.AutoFit();

                 }
             catch ( System.Exception )
                 {
                 MessageBox.Show( "Ошибка при выводе данных по модулям!!!" );
                 }


             excel_app.Visible = true;
             //excel_app.Quit();
             }
         //-----------------------------------------------------------------------------------------

         public void get_write_data( BinaryWriter bw, mode temp_mode, string part1, string part2 )
             {
             byte b;
             TreeNode temp_node;
             device temp_dev;

             temp_node = FindNode( temp_mode.TreeView_params.Nodes[ 0 ], part2 );

             b = ( byte ) temp_node.Nodes.Count;
             bw.Write( b );  //  count

             for ( int j = 0; j < b; j++ )
                 {
                 //  Находим устройство по имени
                 temp_dev = Globals.visio_addin.g_devices.Find( delegate( device dev )
                     {
                         return dev.get_name() == temp_node.Nodes[ j ].Text;
                     }
                     );

                 if ( temp_dev != null )
                     {
                     bw.Write( ( short ) temp_dev.n );  //  no
                     }
                 else
                     {
                     bw.Write( ( short ) 0 );  //  no

                     MessageBox.Show( "Устройство \""
                         + temp_node.Nodes[ j ].Text + "\" не было найдено!"
                         + "( " + temp_mode.name + " )" );
                     }
                 }
             }


         // Поиск узла в дереве по названию.
         private TreeNode FindNode( TreeNode tn, string name )
             {
             // Ищем в узлах первого уровня.
             foreach ( TreeNode temp in tn.Nodes )
                 {
                 // Если нашли,
                 if ( temp.Text == name )
                     {
                     // то возвращаем.
                     return temp;
                     }
                 }

             // Ищем в подузлах.
             TreeNode node;
             foreach ( TreeNode temp in tn.Nodes )
                 {
                 // Делаем поиск в узлах.
                 node = FindNode( temp, name );
                 // Если нашли,
                 if ( node != null )
                     {
                     // то возвращаем.
                     return node;
                     }
                 }

             // Ничего не нашли.
             return null;
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

                }
            else
                {
                //	Снятие подсветки с ранее выбранных модулей
                for ( int i = 0; i < Globals.visio_addin.g_PAC_nodes.Count; i++ )
                    {
                    foreach ( io_module mod in Globals.visio_addin.g_PAC_nodes[ i ].get_io_modules() )
                        {
                        mod.deactivate();
                        }
                    }

                //  Закрываем окно Устройства
                visio_wnds[ ( short ) visio_addin.VISIO_WNDOWS.IO_EDIT ].Close();
                //  Активизируем окно WAGO
                visio_wnds[ ( short ) visio_addin.VISIO_WNDOWS.MAIN ].Activate();
                //  Активизируем страничку с устройствами
                visio_wnds[ ( short ) visio_addin.VISIO_WNDOWS.MAIN ].Page =
                    visio_app.ActiveDocument.Pages[ "Устройства" ];
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
