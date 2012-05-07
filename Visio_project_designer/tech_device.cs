/// @file tech_device.cs
/// @brief Классы, которые реализуют функциональность для работы простыми 
/// устройствами (клапана, насосы и т.д.).
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

using System.Text.RegularExpressions;
using System.Windows.Forms;

using Visio = Microsoft.Office.Interop.Visio;

using wago;
using visio_prj_designer;

/// <summary> Устройства проекта (клапан, насос, танк...).</summary>
namespace tech_device
    {

    public class mode
        {
        public int no;				//	Номер режима (шага)
        public string name;		    //	Название режима (шага)
        public TreeView TreeView_params;
        public List<mode> step;		//	Шаги режима
                   
     
        public mode( int no_ = 0, string name_ = "New mode" )
            {
            TreeView_params = new TreeView();
            step = new List<mode>();

            set_attribute( no_, name_ );
            }
        //-----------------------------------------------------------------------

        public void set_attribute( int no_, string name_ )
            {
            no = no_;
            if ( name_ != null )
                name = name_;

            if ( step == null )
                step = new List<mode>();
            }
        //-----------------------------------------------------------------------

        public mode Copy()
            {
            mode new_mode = new mode();

            new_mode.set_attribute( this.no, this.name );

            new_mode.TreeView_params.Nodes.Clear();

            //  Проходим по характеристикам режима
            for ( int j = 0; j < this.TreeView_params.Nodes.Count; j++ )
                {
                new_mode.TreeView_params.Nodes.Add(
                    ( ( TreeNode ) this.TreeView_params.Nodes[ j ].Clone() ) );
                }

            return new_mode;
            }
        //-----------------------------------------------------------------------

        /// <summary> Lua save. </summary>
        ///
        /// <remarks> ASV, 03.05.2012. </remarks>
        ///
        /// <param name="prefix"> (optional) the prefix. </param>
        ///
        /// <returns> . </returns>
        public string lua_save_mode( string prefix = "" )
            {
            string res = "";
            res += prefix + "[ " + Convert.ToString( no ) + " ] =\n";
            
            res += prefix + "\t" + "{\n";
            res += prefix + "\t" + "name = \'" + name + "\',\n";
            
            res += prefix + "\t" + "opened_devices = \n";
            res += prefix + "\t\t" + "{\n";
            
            //  Проходим по списку устройств
            res += prefix + "\t\t" + "V = {";
//             for (  )
//                 {
// 
//                 }
            res += " }\n";

            res += prefix + "\t\t" + "},\n";
                 
            //  Проходим по шагам
            //foreach ( mode temp_mode in step )
            //    {
            //    res += temp_mode.lua_save_mode( "\t\t" );
            //    }
            
            res += prefix + "\t" + "},\n";

            return res;
            }
        //---------------------------------------------------------------------

        }

	/// <summary> Сложный объект (Гребенка, Танк). </summary>
	///
	/// <remarks> ASV, 07.09.2011. </remarks>
	public class T_Object : device
		{
        //	Режимы объекта
		public List<mode> mode_mas;		//	Список режимов

		//	Параметры объекта
		public List<string[]> param_list_temp;	//	Список параметров param_list< Название, Значение >
        public List<string[]> param_list_save;	//	Список параметров param_list< Название, Значение >
		
        //  Количество таймеров объекта
        public int timers = 0;

        //	Список ошибок 
//		public enum ERROR_LIST
//		public List<string> ERROR_LIST
//		public Dictionary<int, string> ERROR_LIST 


		//	Список устройств объекта (Ссылки на элементы g_devices)
		public List<device> list_object_device;

        /// <summary> Constructor. </summary>
        /// <remarks> asvovik, 16.09.2011. </remarks>
        ///
        /// <param name="shape"> Фигура Visio. </param>
        /// <param name="pac">	Узел Wago. </param>
        public T_Object( Visio.Shape shape, List<PAC> pac ):base( shape, pac )
            {
            mode_mas = new List<mode>();

            param_list_temp = new List<string[]>(
                new string[][] 
                { 
                    new string[] { "Пар_0", "0" }, 
                    new string[] { "Пар_1", "0" },
                    new string[] { "Пар_2", "0" },
                    new string[] { "Пар_3", "0" },
                    new string[] { "Пар_4", "0" },
                    new string[] { "Пар_5", "0" },
                    new string[] { "Пар_6", "0" },
                    new string[] { "Пар_7", "0" },
                    new string[] { "Пар_8", "0" },
                    new string[] { "Пар_9", "0" },
                } );

            param_list_save = new List<string[]>( 
                new string[][] 
                { 
                    new string[] { "Пар_10", "0" }, 
                    new string[] { "Пар_11", "0" },
                    new string[] { "Пар_12", "0" },
                    new string[] { "Пар_13", "0" },
                    new string[] { "Пар_14", "0" },
                    new string[] { "Пар_15", "0" },
                    new string[] { "Пар_16", "0" },
                } );

            }
        //---------------------------------------------------------------------------

        //  Копирование объекта
        public void copy_obj_config_to( ref T_Object obj_to )
            {
            //  Копируем общие характеристики
            obj_to.n = this.n;
            obj_to.name = this.name;
            obj_to.timers = this.timers;
            
            //  Копируем параметры
            obj_to.param_list_temp.Clear();
            for ( int i = 0; i < param_list_temp.Count; i++ )
                {
                obj_to.param_list_temp.Add( new string[ 2 ] );
                obj_to.param_list_temp[ i ][ 0 ] = param_list_temp[ i ][ 0 ];
                obj_to.param_list_temp[ i ][ 1 ] = param_list_temp[ i ][ 1 ];
                }

            obj_to.param_list_save.Clear();
            for ( int i = 0; i < param_list_save.Count; i++ )
                {
                obj_to.param_list_save.Add( new string[ 2 ] );
                obj_to.param_list_save[ i ][ 0 ] = param_list_save[ i ][ 0 ];
                obj_to.param_list_save[ i ][ 1 ] = param_list_save[ i ][ 1 ];
                }

            //  Проходим по режимам
            copy_modes( mode_mas, obj_to.mode_mas );
            }
        //---------------------------------------------------------------------

        private void copy_modes( List<mode> from_modes, List<mode> to_modes )
            {
            for ( int i = 0; i < from_modes.Count; i++ )
                {
                mode tmode = from_modes[ i ].Copy();
            
                //  Проходим по шагам режима         
                copy_modes( from_modes[ i ].step, tmode.step );

                to_modes.Add( tmode );
                }
            }
        //---------------------------------------------------------------------

        /// <summary> Lua save. </summary>
        ///
        /// <remarks> ASV, 03.05.2012. </remarks>
        ///
        /// <param name="prefix"> (optional) the prefix. </param>
        ///
        /// <returns> . </returns>
        public string lua_save_obj( string prefix = "" )
            {
            string res = prefix + "{\n";
            res += prefix + "n = " + Convert.ToString( get_n() ) + ",\n";
            res += prefix + "name = \'" + get_name() + "\',\n";
            res += prefix + "modes = \n";
            res += prefix + "\t" + "{\n";

            //  Проходим по режимам
            foreach ( mode temp_mode in mode_mas )
                {
                res += temp_mode.lua_save_mode( "\t\t\t" );
                }
            
            res += prefix + "\t" + "},\n";
            res += prefix + "},\n";

            return res;
            }
        //---------------------------------------------------------------------

		}
    
//-----------------------------------------------------------------------------	   
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


    /// <summary> Простое устройство (клапан, насос, ...). </summary>
    ///
    /// <remarks> Id, 17.08.2011. </remarks>
    public class device : wago_device
        {
        /// <summary> Типы устройств. </summary>
        ///
        /// <remarks> Id, 17.08.2011. </remarks>
        public enum TYPES
            {
            /// <summary> Клапан.  </summary>
			T_V		= 0,	//	Клапан
			T_N		= 1,	//	Насос
			T_MIX	= 2,	//	Мешалка
			T_CTR	= 3,	//	Расходомер
			T_TE	= 4,	//	Температура
			T_QE	= 5,	//	Концентратомер

			T_LS	= 6,	//	Гарничный уровень
			T_LE	= 7,	//	Текущий уровень

			T_FS	= 8,	//	Расход (есть/нет)
			T_FE	= 9,	//	Текущий расход	 
						
			T_FB	= 10,	//	Обратнся связь
			T_UPR	= 11,	//	Управляющий сигнал

			T_AI	= 12,	//	Аналоговый вход
			T_AO	= 13,	//	Аналоговый выход
			
			T_FQT	= 14,	//	Расходомер и Концентратомер
			T_WTE	= 15,	//	Температура и Влажность

			T_TANK	= 100,	//	Танк (сложный объект)
			T_GREB  = 200,	//	Гребенка
            }

    //  Структура перечисления допустимых типов устройств (для экспорта в PLC)
    public enum TDevType
        {
        dtV         = 0,    //0 клапан без ОС
        dtV2DO      = 1,    //1 клапан 2 КУ без ОС
        dtV1DO1DI   = 2,    //2 клапан 1 КУ 1 ОС
        dtV1DO2DI   = 3,    //3 клапан 1 КУ 2 ОС
        dtV2DO2DI   = 4,    //4 клапан 2 КУ 2 ОС
        dtN1DO1DI   = 5,    //5 насос
        dtM1DO1DI   = 6,    //6 эл двигатель
        dtLS        = 7,    //7 уровень (есть/нет)
        dtTE        = 8,    //8 температура
        dtFE        = 9,    //9 текущий расход
        dtFS        = 10,   //10 расход (есть/нет)
        dtCTR       = 11,   //11 счетчик
        dtAO        = 12,   //12 AO
        dtLE1       = 13,   //13 текущий уровень тип1 конус внутрь
        dtLE2       = 14,   //14 текущий уровень тип2 конус вниз
        dtLE3       = 15,   //15 текущий уровень тип3 усеченный цилиндр
        dtLE4       = 16,   //16 текущий уровень тип4 без конуса
        dtMix       = 17,   //17 mixprove
        dtOS        = 18,   //18 feed back
        dtUpr       = 19,   //19 upr
        dtQE        = 20,   //20 концентрация
        dtMF        = 21,   //21 memmory flag

        dtV1DO3DI   = 22,   //22 клапан 1 КУ 3 ОС
        dtV1DO2DI_S = 23,   //23 клапан 1 КУ 2 ОС  на одно из состояний
        dtASMix     = 24,   //24 Клапан с AS интерфейсом
        dtAI        = 25,   //25 AI

        dtLS_ex     = 26,   //26 Настраиваемый уровень (есть/нет)
        }

        /// <summary> Подтип клапана. </summary>
        ///
        /// <remarks> Id, 17.08.2011. </remarks>
        public enum SUB_TYPES
            {
            /// <summary> 1 канал управления.  </summary>
            V_1_CONTROL_CHANNEL = 0,

            /// <summary> 2 канала управления.  </summary>
            V_2_CONTROL_CHANNEL,

            /// <summary> 1 канал управления и 1 обратная связь.  </summary>
            V_1_CONTROL_CHANNEL_1_FB,

            /// <summary> 1 канал управления и 2 обратных связи.  </summary>
            V_1_CONTROL_CHANNEL_2_FB,

            /// <summary> 2 канала управления и 2 обратных связи.  </summary>
            V_2_CONTROL_CHANNEL_2_FB,

			/// <summary> MixProof 3 КУ и 2 ОС.  </summary>
			V_MIX_PROOF_3_UPR_2_FB,

			/// <summary> MixProof AS-Interface.  </summary>
			V_MIX_PROOF_AS_INTERFACE,
            }

        /// <summary> Имена типов устройств.  </summary>
        private string[] NAMES = 
        {
        "Клапан",
        };

        /// <summary> Имена подтипов устройств.  </summary>
//         private string[] SUB_NAMES = 
//         {
//         "1 КУ",
//         "2 КУ",
//         "1 КУ 1 ОС",
//         "1 КУ 2 ОС",
//         "2 КУ 2 ОС",
//         "MixProof 3 КУ 2 ОС",
//         "MixProof AS interface",
//         };


    	/// <summary> Имя устройства.  </summary>
		public string name;

		/// <summary> Получение названия устройства. </summary>
		///
		/// <remarks> Id, 17.08.2011. </remarks>
		///
		/// <returns> Название устройства без кавычек. </returns>
		public string get_name()
		    {
		    return name.Replace( "\"", "" );
		    }


		/// <summary> Номер устройства.  </summary>
		public int n;

		/// <summary> Получение номера устройства. </summary>
		///
		/// <remarks> Id, 17.08.2011. </remarks>
		///
		/// <returns> Номер устройства. </returns>
		public int get_n()
		    {
		    return n;
		    }


        /// <summary> Тип устройства. </summary>
		public TYPES type;

		/// <summary> Получение типа устройства. </summary>
		///
		/// <remarks> Id, 17.08.2011. </remarks>
		///
		/// <returns> The type. </returns>
		public int get_type()
			{
			return ( int ) type;
			}

        public byte Get_old_type()
            {
            byte b = 0;

            // Типы устройств не совпадают с типами в старом редакторе WAGO
            switch ( type )
                {
                case TYPES.T_V:	//	Клапан

                    //if ( Globals.visio_addin.g_devices[ i ].sub_type == 
                    //    tech_device.device.SUB_TYPES.V_1_CONTROL_CHANNEL ) 
                    b = ( byte ) tech_device.device.TDevType.dtV;

                    if ( sub_type == SUB_TYPES.V_1_CONTROL_CHANNEL_1_FB )
                        b = ( byte ) tech_device.device.TDevType.dtV1DO1DI;

                    if ( sub_type == SUB_TYPES.V_1_CONTROL_CHANNEL_2_FB )
                        b = ( byte ) tech_device.device.TDevType.dtV1DO2DI;

                    if ( sub_type == SUB_TYPES.V_2_CONTROL_CHANNEL )
                        b = ( byte ) tech_device.device.TDevType.dtV2DO;

                    if ( sub_type == SUB_TYPES.V_2_CONTROL_CHANNEL_2_FB )
                        b = ( byte ) tech_device.device.TDevType.dtV2DO2DI;

                    if ( sub_type == SUB_TYPES.V_MIX_PROOF_3_UPR_2_FB )
                        b = ( byte ) tech_device.device.TDevType.dtMix;

                    if ( sub_type == SUB_TYPES.V_MIX_PROOF_AS_INTERFACE )
                        b = ( byte ) tech_device.device.TDevType.dtASMix;

                    break;

                case TYPES.T_N:	//	Насос
                    b = ( byte ) tech_device.device.TDevType.dtN1DO1DI;
                    break;

                case TYPES.T_MIX:	//	Мешалка
                    b = ( byte ) tech_device.device.TDevType.dtM1DO1DI;
                    break;

                case TYPES.T_CTR:	//	Расходомер
                    b = ( byte ) tech_device.device.TDevType.dtCTR;
                    break;

                case TYPES.T_TE:	//	Температура
                    b = ( byte ) tech_device.device.TDevType.dtTE;
                    break;

                case TYPES.T_QE:	//	Концентратомер
                    b = ( byte ) tech_device.device.TDevType.dtQE;
                    break;

                case TYPES.T_LS:	//	Гарничный уровень
                    b = ( byte ) tech_device.device.TDevType.dtLS_ex;
                    break;

                case TYPES.T_LE:	//	Текущий уровень
                    //  Уровни у нас 4-х типов
                    b = ( byte )
                        (
                        ( ( byte ) tech_device.device.TDevType.dtLE1 ) +
                        ( ( byte ) sub_type )
                        );
                    break;

                case TYPES.T_FS:	//	Расход (есть/нет)
                    b = ( byte ) tech_device.device.TDevType.dtFS;
                    break;

                case TYPES.T_FE:  //	Текущий расход
                    b = ( byte ) tech_device.device.TDevType.dtFE;
                    break;

                case TYPES.T_FB:	//	Обратнся связь
                    b = ( byte ) tech_device.device.TDevType.dtOS;
                    break;

                case TYPES.T_UPR:	//	Управляющий сигнал
                    b = ( byte ) tech_device.device.TDevType.dtUpr;
                    break;

                case TYPES.T_AI:	//	Аналоговый вход
                    b = ( byte ) tech_device.device.TDevType.dtAI;
                    break;

                case TYPES.T_AO:	//	Аналоговый выход
                    b = ( byte ) tech_device.device.TDevType.dtAO;
                    break;

                //                        case ( int )tech_device.device.TYPES.T_FQT:	//	Расходомер и Концентратомер
                //                            b = ( byte ) tech_device.device.TDevType.dt;
                //                            break;

                //                       case ( int )tech_device.device.TYPES.T_WTE:	//	Температура и Влажность
                //                            b = ( byte ) tech_device.device.TDevType.dt;
                //                            break;  
                }

            return ( byte ) b;
            }


        /// <summary> Подтип устройства. </summary>
		public SUB_TYPES sub_type;

		/// <summary> Получение подтипа устройства. </summary>
		///
		/// <remarks> ASV, 24.08.2011. </remarks>
		///
		/// <returns> The sub type. </returns>
		public int get_sub_type()
			{
			return ( int ) sub_type;
			}


		/// <summary> Подтип устройства. </summary>
		public string description;
        
        /// <summary> Получение описания устройства. </summary>
        ///
        /// <remarks> Id, 17.08.2011. </remarks>
        ///
        /// <returns> Описание устройства без кавычек. </returns>
        public string get_descr()
            {
            return description.Replace( "\"", "" );
            }

		/// <summary> Активный (выбранный) в данный момент канал устройства (для визуальной
        ///  работы с устройством). </summary>
        private string active_channel_name = "";

        /// <summary> Получение активного канала. </summary>
        ///
        /// <remarks> Id, 17.08.2011. </remarks>
        ///
        /// <returns> Активный канал. </returns>
        public string get_active_channel()
            {
            return active_channel_name;
            }

        /// <summary> Обновление окна со свойствами устройства. </summary>
        ///
        /// <remarks> Id, 17.08.2011. </remarks>
        ///
        /// <param name="cbox">           The cbox. </param>
        /// <param name="lview">          The lview control. </param>
        /// <param name="only_list_view"> (optional) the only list view. </param>
        public void refresh_edit_window( ComboBox cbox, ListView lview, 
            bool only_list_view = false )
            {
            if( only_list_view == false )
                {
                cbox.Items.Clear();

                int cnt = Globals.visio_addin.DDTypes.Descr_Dev_Types[ type ].Count;
                for ( int i = 0; i < cnt; i++ )
                    {
                    cbox.Items.Add( Globals.visio_addin.DDTypes.Descr_Dev_Types[ type ][ i ].name );
                    }

                cbox.SelectedIndex = ( int ) sub_type;
                }

            lview.Items.Clear();

            Dictionary<string, string> items_str = to_str();
            foreach( KeyValuePair<string, string> item_str in items_str )
                {
                System.Windows.Forms.ListViewItem item;
                item = lview.Items.Add( item_str.Key );
                item.SubItems.Add( item_str.Value );

                if( active_channel_name == item_str.Key )
                    {
                    item.Selected = true;
                    lview.Focus();
                    }
                }
            }

        /// <summary> Представление в виде списка строк, которые описывают каналы
        /// устройства. </summary>
        ///
        /// <remarks> Id, 17.08.2011. </remarks>
        ///
        /// <returns> A string representation of this object. </returns>
        private Dictionary<string, string> to_str()
            {
            Dictionary<string, string> res = new Dictionary<string, string>();

            foreach( KeyValuePair<string, wago_channel> channel in wago_channels )
                {
                string description = "не привязан";
                if( channel.Value.module != null )
                    {
                    description = string.Format( "узел {0} модуль {1} клемма {2}",
                        channel.Value.module.node_number,
                        channel.Value.module.order_number, channel.Value.clamp + 1 );
                    }
                
                if ( channel.Value.kind == io_module.KINDS.PARAM )
                    {
                    description = Convert.ToString( channel.Value.clamp );
                    }

                res.Add( channel.Key, description );
                }

            return res;
            }


        /// <summary> Фигура Visio устройства. </summary>
        public Visio.Shape shape;

		/// <summary> Получение подтипа устройства. </summary>
		///
		/// <remarks> Id, 17.08.2011. </remarks>
		///
		/// <returns> The sub type. </returns>
		public Visio.Shape get_shape()
			{
			return ( Visio.Shape ) shape;
			}

        /// <summary> Получение параметров канала из строкового описания канала
        /// (для записи\чтения из файла). </summary>
        ///
        /// <remarks> Id, 17.08.2011. </remarks>
        ///
        /// <param name="str">    The string. </param>
        /// <param name="node">   [out] The node. </param>
        /// <param name="module"> [out] The module. </param>
        /// <param name="clamp">  [out] The clamp. </param>
        ///
        /// <returns> true if it succeeds, false if it fails. </returns>
        private bool get_n_from_str( string str, out int node, out int module,
            out int clamp )
            {
            //Значение привязки имеет следующий формат:
            // -> узел 1 модуль 2 клемма 3;
            //на его основе получаем индекс подсвечиваемого объекта 
            //(номер узла и модуля) и номер клеммы:
            // -> 1, 2, 3.
            node = 0;
            module = 0;
            clamp = 0;

            Regex rex = new Regex( @"\w+\s(\d+)\s\w+\s(\d+)\s\w+\s(\d+)",
                RegexOptions.IgnoreCase );
            if( rex.IsMatch( str ) )
                {
                Match mtc = rex.Match( str );

                if (  ( Convert.ToInt16( mtc.Groups[ 1 ].ToString() ) >= 0 )
                  &&  ( Convert.ToInt16( mtc.Groups[ 2 ].ToString() ) >= 0 )
                  &&  ( Convert.ToInt16( mtc.Groups[ 3 ].ToString() ) >= 0 ) )
                    {
                    node = Convert.ToInt16( mtc.Groups[ 1 ].ToString() );
                    module = Convert.ToInt16( mtc.Groups[ 2 ].ToString() );
                    clamp = Convert.ToInt16( mtc.Groups[ 3 ].ToString() );
                    }
                
                return true;
                }

            //  Если мы здесь, значит это параметр -> нас интересует олько "clamp"
            str = str.Replace( "\"", "" );
            
            if ( ( str == "MIN" ) || ( str == "NC" ) /*временно->*/ || ( str == "НЗ" ) )
                {
                clamp = 0;
                return true;
                }

            if ( ( str == "MAX" ) || ( str == "NO" ) /*временно->*/ || ( str == "НО" ) )
                {
                clamp = 1;
                return true;
                }

            //  "AS_adres", "AS_gateway"
            int temp;
            bool result = Int32.TryParse( str, out temp );
            if ( result )
                {
                clamp = temp;
                return true;
                }

            return false;
            }

        /// <summary> Изменение подтипа устройства. </summary>
        ///
        /// <remarks> Id, 17.08.2011. </remarks>
        ///
        /// <param name="sub_type"> Подтип устройства. </param>
        /// <param name="pac">      Узел Wago. </param>
        public void change_sub_type( SUB_TYPES sub_type, List<PAC> pac )
            {
            this.sub_type = sub_type;
			    
			shape.Cells["Prop.sub_type"].FormulaU = Convert.ToString((int)sub_type);

            set_sub_type( sub_type, pac );
            }

        /// <summary> Установка подтипа устройства. </summary>
        ///
        /// <remarks> Id, 17.08.2011. </remarks>
        ///
        /// <param name="sub_type"> Подтип устройства. </param>
        /// <param name="pac">      Узел Wago. </param>
        private void set_sub_type( SUB_TYPES sub_type, List<PAC> pac )
            {
			int node;
			int module;
			int clamp;

            wago_channels.Clear();

            if (    ( ( int ) type < Globals.visio_addin.DDTypes.Descr_Dev_Types.Count )
                &&  ( ( int ) sub_type < Globals.visio_addin.DDTypes.Descr_Dev_Types[ type ].Count ) 
                )
                {
                DDT.Dev_Struct temp_dev = Globals.visio_addin.DDTypes.Descr_Dev_Types[ type ][ ( int ) sub_type ];
                int cnt = temp_dev.chens.Count;

                for ( int i = 0; i < cnt; i++ )
                    {
                    // string str_DO1 = shape.Cells[ "Prop.DO1" ].Formula;
                    // add_wago_channel( "DO1", io_module.KINDS.DO );
                    // get_n_from_str( str_DO1, out node, out module, out clamp );
                    // wago_channels[ "DO1" ].set( pac, node, module, clamp );

                    try
                        {
                        string str_chen = shape.Cells[ "Prop." + temp_dev.chens[ i ].name ].Formula;
                        add_wago_channel( temp_dev.chens[ i ].name, temp_dev.chens[ i ].type );
                        get_n_from_str( str_chen, out node, out module, out clamp );
                        wago_channels[ temp_dev.chens[ i ].name ].set( pac, node, module, clamp );
                        }
                    catch ( System.Exception )
                        {
                        MessageBox.Show( "Ошибка при задании канала \"" + temp_dev.chens[ i ].name + 
                            "\" устройства: " + temp_dev.name + " " + 
                            shape.Cells[ "Prop.name" ].Formula );
                        }

                    }
                }

//*********************************************************************************************
/*           
            switch( type )
                {
                case TYPES.T_V:
					string str_DO1 = shape.Cells[ "Prop.DO1" ].Formula;
					string str_DO2 = shape.Cells[ "Prop.DO2" ].Formula;
					string str_DI1 = shape.Cells[ "Prop.DI1" ].Formula;
					string str_DI2 = shape.Cells[ "Prop.DI2" ].Formula;

                    switch( sub_type )
                        {
                        case SUB_TYPES.V_1_CONTROL_CHANNEL:
                            add_wago_channel( "DO1", io_module.KINDS.DO );
                            get_n_from_str( str_DO1, out node, out module, out clamp );
                            wago_channels[ "DO1" ].set( pac, node, module, clamp );
                            break;

                        case SUB_TYPES.V_2_CONTROL_CHANNEL:
                            add_wago_channel( "DO1", io_module.KINDS.DO );
                            get_n_from_str( str_DO1, out node, out module, out clamp );
                            wago_channels[ "DO1" ].set( pac, node, module, clamp );

                            add_wago_channel( "DO2", io_module.KINDS.DO );
                            get_n_from_str( str_DO2, out node, out module, out clamp );
                            wago_channels[ "DO2" ].set( pac, node, module, clamp );

                            break;

                        case SUB_TYPES.V_1_CONTROL_CHANNEL_1_FB:
                            add_wago_channel( "DO1", io_module.KINDS.DO );
                            get_n_from_str( str_DO1, out node, out module, out clamp );
                            wago_channels[ "DO1" ].set( pac, node, module, clamp );

							add_wago_channel( "DI1", io_module.KINDS.DI );
                            get_n_from_str( str_DI1, out node, out module, out clamp );
                            wago_channels[ "DI1" ].set( pac, node, module, clamp );
                            break;

                        case SUB_TYPES.V_1_CONTROL_CHANNEL_2_FB:
                            add_wago_channel( "DO1", io_module.KINDS.DO );
                            add_wago_channel( "DI1", io_module.KINDS.DI );
                            add_wago_channel( "DI2", io_module.KINDS.DI );

                            get_n_from_str( str_DO1, out node, out module, out clamp );
                            wago_channels[ "DO1" ].set( pac, node, module, clamp );

                            get_n_from_str( str_DI1, out node, out module, out clamp );
                            wago_channels[ "DI1" ].set( pac, node, module, clamp );

                            get_n_from_str( str_DI2, out node, out module, out clamp );
                            wago_channels[ "DI2" ].set( pac, node, module, clamp );

                            break;

                        case SUB_TYPES.V_2_CONTROL_CHANNEL_2_FB:
                            add_wago_channel( "DO1", io_module.KINDS.DO );
                            add_wago_channel( "DO2", io_module.KINDS.DO );
                            add_wago_channel( "DI1", io_module.KINDS.DI );
                            add_wago_channel( "DI2", io_module.KINDS.DI );

                            get_n_from_str( str_DO1, out node, out module, out clamp );
                            wago_channels[ "DO1" ].set( pac, node, module, clamp );

                            get_n_from_str( str_DO2, out node, out module, out clamp );
                            wago_channels[ "DO2" ].set( pac, node, module, clamp );

                            get_n_from_str( str_DI1, out node, out module, out clamp );
                            wago_channels[ "DI1" ].set( pac, node, module, clamp );

                            get_n_from_str( str_DI2, out node, out module, out clamp );
                            wago_channels[ "DI2" ].set( pac, node, module, clamp );

                            break;

						case SUB_TYPES.V_MIX_PROOF_3_UPR_2_FB:
							add_wago_channel( "DO1", io_module.KINDS.DO );
							add_wago_channel( "DOH", io_module.KINDS.DO );
							add_wago_channel( "DOL", io_module.KINDS.DO );

							add_wago_channel( "DI1", io_module.KINDS.DI );
							add_wago_channel( "DI2", io_module.KINDS.DI );

							get_n_from_str( str_DO1, out node, out module, out clamp );
							wago_channels[ "DO1" ].set( pac, node, module, clamp );

							get_n_from_str( str_DO2, out node, out module, out clamp );
							wago_channels[ "DOH" ].set( pac, node, module, clamp );

							get_n_from_str( str_DO2, out node, out module, out clamp );
							wago_channels[ "DOL" ].set( pac, node, module, clamp );

							get_n_from_str( str_DI1, out node, out module, out clamp );
							wago_channels[ "DI1" ].set( pac, node, module, clamp );

							get_n_from_str( str_DI2, out node, out module, out clamp );
							wago_channels[ "DI2" ].set( pac, node, module, clamp );

							break;

						case SUB_TYPES.V_MIX_PROOF_AS_INTERFACE:
							//	AS_adres
							//	AS_gateway
                            
                            add_wago_channel( "AS_adres", io_module.KINDS.PARAM );
                            //wago_channels[ "AS_adres" ].set_par( , "0" );

                            add_wago_channel( "AS_gateway", io_module.KINDS.PARAM );
                            //wago_channels[ "AS_gateway" ].set_par( , "0" );
							break;

                        }
                    break;

                case TYPES.T_TANK:
                case TYPES.T_GREB:

                    break;
                }


				if (	( type == TYPES.T_UPR )
					||	( type == TYPES.T_MIX )	||	( type == TYPES.T_N )	)
				 	{
					string str_DO1 = shape.Cells[ "Prop.DO1" ].Formula;

					add_wago_channel( "DO1", io_module.KINDS.DO );
					get_n_from_str( str_DO1, out node, out module, out clamp );
					wago_channels[ "DO1" ].set( pac, node, module, clamp );
					}

				if  (	( type == TYPES.T_FB )
					||	( type == TYPES.T_LS )
					||	( type == TYPES.T_FS )
					|| ( type == TYPES.T_MIX )	|| ( type == TYPES.T_N )	)
					{
					string str_DI1 = shape.Cells[ "Prop.DI1" ].Formula;

					add_wago_channel( "DI1", io_module.KINDS.DI );
					get_n_from_str( str_DI1, out node, out module, out clamp );
					wago_channels[ "DI1" ].set( pac, node, module, clamp );
					}

				if ( type == TYPES.T_AO )
				 	{
					string str_AO1 = shape.Cells[ "Prop.AO1" ].Formula;

					add_wago_channel( "AO1", io_module.KINDS.AO );
					get_n_from_str( str_AO1, out node, out module, out clamp );
					wago_channels[ "AO1" ].set( pac, node, module, clamp );
					}

				if (   ( type == TYPES.T_AI )
					|| ( type == TYPES.T_LE )
					|| ( type == TYPES.T_TE )
					|| ( type == TYPES.T_FE )
					|| ( type == TYPES.T_QE )
					|| ( type == TYPES.T_CTR )	)
				 	{
					string str_AI1 = shape.Cells[ "Prop.AI1" ].Formula;

					add_wago_channel( "AI1", io_module.KINDS.AI );
                    get_n_from_str( str_AI1, out node, out module, out clamp );
                    wago_channels[ "AI1" ].set( pac, node, module, clamp );
					}
*/
//*********************************************************************************************
            }

        /// <summary> Constructor. </summary>
        ///
        /// <remarks> Id, 17.08.2011. </remarks>
        ///
        /// <param name="shape"> Фигура Visio. </param>
        /// <param name="pac">   Узел Wago. </param>
        public device( Visio.Shape shape, List<PAC> pac )
            {
            try
                {
                n = Convert.ToUInt16( shape.Cells[ "Prop.number" ].FormulaU );
                name = shape.Cells[ "Prop.name" ].FormulaU;
                description = shape.Cells[ "Prop.description" ].FormulaU;
                type = ( TYPES ) Convert.ToUInt16( shape.Cells[ "Prop.type" ].FormulaU );
                sub_type = ( SUB_TYPES ) Convert.ToUInt16( shape.Cells[ "Prop.sub_type" ].FormulaU );
                }
            catch ( System.Exception )
                {
                MessageBox.Show( "Одно из свойств фигуры не задано в шаблоне!" );
                }

            this.shape = shape;

            set_sub_type( sub_type, pac );
            }

        /// <summary> Привязка канала к клемме. </summary>
        ///
        /// <remarks> Id, 17.08.2011. </remarks>
        ///
        /// <param name="channel_name"> Name of the channel. </param>
        /// <param name="module">       module. </param>
        /// <param name="clamp">        The clamp. </param>
        public void set_channel( string channel_name, io_module module, int clamp )
            {
            //Освобождаем при наличии ранее привязанную клемму.
            if( wago_channels[ channel_name ].module != null )
                {
                wago_channels[ channel_name ].module.free(
                    wago_channels[ channel_name ].clamp );
                }

            //  Задаем новые модуль и клему
            wago_channels[ channel_name ].module = module;
            wago_channels[ channel_name ].clamp = clamp;

            //  Если выбран модуль для привязки, то привязываем
            //      иначе сбрасываем привязку
            if ( module != null )
                {
                string prop = "Prop." + channel_name;
                string value = string.Format(
                    "\"узел {0} модуль {1} клемма {2}\"",
                    module.node_number, module.order_number, clamp + 1 );

                shape.Cells[ prop ].FormulaU = value;
                                                     
                module.use( clamp );
                }
//             else
//                 {
//                 string prop = "Prop." + channel_name;
//                 shape.Cells[ prop ].FormulaU = "\"" + Convert.ToString( clamp ) + "\"";
//                 }
            }

        /// <summary> Установка активной ("подсвечиваемой") клеммы в данный 
        /// момент времени. </summary>
        ///
        /// <remarks> Id, 17.08.2011. </remarks>
        ///
        /// <param name="channel_name"> Name of the channel. </param>
        public void set_active_channel( string channel_name )
            {
            active_channel_name = channel_name;
            }

        /// <summary> Отмена "подсветки" клемм, занятых устройством. </summary>
        ///
        /// <remarks> Id, 17.08.2011. </remarks>
        public void unselect_channels()
            {
            foreach( KeyValuePair<string, wago_channel> channel in wago_channels )
                {
                if( channel.Value.module != null )
                    {
                    channel.Value.module.deactivate();
                    }
                }
            }

        /// <summary> "Подсветка" клемм, занятых устройством. </summary>
        ///
        /// <remarks> Id, 17.08.2011. </remarks>
        public void select_channels()
            {
            foreach( KeyValuePair<string, wago_channel> channel in wago_channels )
                {
                if( channel.Value.module != null )
                    {
                    channel.Value.module.activate( channel.Value.clamp );
                    }
                }

            if( active_channel_name != "" &&
                wago_channels.ContainsKey( active_channel_name ) &&
                wago_channels[ active_channel_name ].module != null )
                {
                wago_channels[ active_channel_name ].module.magnify_clamp(
                    wago_channels[ active_channel_name ].clamp );
                }
            }


        /// <summary> Установка параметра устройства </summary>
        ///
        /// <remarks> ASV, 04.01.2012. </remarks>
        ///
        /// <param name="par_name"> Название параметра </param>
        /// <param name="value">    Значение параметра </param>
        public void set_par( string par_name, string value )
            {
            //  Если выбран модуль для привязки, то привязываем
            //      иначе сбрасываем привязку
            if ( par_name != "" )
                {
                string prop = "Prop." + par_name;
                shape.Cells[ prop ].FormulaU = value;
                }
            }


        /// <summary> Lua save. </summary>
        ///
        /// <remarks> ASV, 03.05.2012. </remarks>
        ///
        /// <param name="prefix"> (optional) the prefix. </param>
        ///
        /// <returns> . </returns>
        public string lua_save_dev( string prefix = "" )
            {
            string res = prefix + "{\n";
            res += prefix + "descr    = " + description + ",\n";
            res += prefix + "dtype = " + ( int )type + ",\n";
            res += prefix + "subtype = " + ( int ) sub_type + ",\n";
            res += prefix + "number = " + get_n() + ",\n";

            //  Проходим по каналам
            foreach ( KeyValuePair<string, wago.wago_channel> chen in wago_channels )
                {
                res += chen.Value.lua_save_chen("\t\t");
                }
            res += prefix + "}\n";
               
            return res;
            }
        //---------------------------------------------------------------------
        }



    /// <summary> Описание типов устройств </summary>
    ///
    /// <remarks> ASV, 09.01.2012. </remarks>
    class DDT
        {
        internal Dictionary<device.TYPES, List<Dev_Struct>> Descr_Dev_Types = 
            new Dictionary<device.TYPES, List<Dev_Struct>>();

        public struct Chen_Struct
            {
            public io_module.KINDS type;    //  Тип канала
            public string name;             //  Название

            public Chen_Struct( io_module.KINDS t, string n )
                {
                type = t;
                name = n;
                }

            //            byte table_no;    //  Номер узла
            //            byte offset;      //  Номер клеммы в общем списке рабочих клемм узла  
            //            int value;        //  Значение параметра
            }

        public struct Dev_Struct
            {
            public device.TYPES type;            //  Тип устройства
            public device.SUB_TYPES sub_type;        //  Подтип устройства
            public string name;            //  Полное название устройства
            public string sname;           //  Сокращенное имя устройства

            public List<Chen_Struct> chens;   //  Список каналов устройства

            public Dev_Struct( device.TYPES t, device.SUB_TYPES st, string n, string sn )
                {
                type = t;
                sub_type = st;
                name = n;
                sname = sn;

                chens = new List<Chen_Struct>();

                //  Заполнение структуры каждого из устройств в зависимости от его типа и подтипа
                switch ( type )
                    {
                    case device.TYPES.T_V:
                        switch ( sub_type )
                            {
                            case device.SUB_TYPES.V_1_CONTROL_CHANNEL:
                                chens.Add( new Chen_Struct( io_module.KINDS.DO, "DO1" ) );
                                chens.Add( new Chen_Struct( io_module.KINDS.PARAM, "septum" ) );
                                break;

                            case device.SUB_TYPES.V_2_CONTROL_CHANNEL:
                                chens.Add( new Chen_Struct( io_module.KINDS.DO, "DO1" ) );
                                chens.Add( new Chen_Struct( io_module.KINDS.DO, "DO2" ) );
                                break;

                            case device.SUB_TYPES.V_1_CONTROL_CHANNEL_1_FB:
                                chens.Add( new Chen_Struct( io_module.KINDS.DO, "DO1" ) );
                                chens.Add( new Chen_Struct( io_module.KINDS.DI, "DI1" ) );
                                chens.Add( new Chen_Struct( io_module.KINDS.PARAM, "septum" ) );
                                break;

                            case device.SUB_TYPES.V_1_CONTROL_CHANNEL_2_FB:
                                chens.Add( new Chen_Struct( io_module.KINDS.DO, "DO1" ) );
                                chens.Add( new Chen_Struct( io_module.KINDS.DI, "DI1" ) );
                                chens.Add( new Chen_Struct( io_module.KINDS.DI, "DI2" ) );
                                break;

                            case device.SUB_TYPES.V_2_CONTROL_CHANNEL_2_FB:
                                chens.Add( new Chen_Struct( io_module.KINDS.DO, "DO1" ) );
                                chens.Add( new Chen_Struct( io_module.KINDS.DO, "DO2" ) );
                                chens.Add( new Chen_Struct( io_module.KINDS.DI, "DI1" ) );
                                chens.Add( new Chen_Struct( io_module.KINDS.DI, "DI2" ) );
                                break;

                            case device.SUB_TYPES.V_MIX_PROOF_3_UPR_2_FB:
                                chens.Add( new Chen_Struct( io_module.KINDS.DO, "DO1" ) );
                                chens.Add( new Chen_Struct( io_module.KINDS.DO, "DOH" ) );
                                chens.Add( new Chen_Struct( io_module.KINDS.DO, "DOL" ) );
                                chens.Add( new Chen_Struct( io_module.KINDS.DI, "DI1" ) );
                                chens.Add( new Chen_Struct( io_module.KINDS.DI, "DI2" ) );
                                break;

                            case device.SUB_TYPES.V_MIX_PROOF_AS_INTERFACE:
                                chens.Add( new Chen_Struct( io_module.KINDS.PARAM, "AS_adres" ) );
                                chens.Add( new Chen_Struct( io_module.KINDS.PARAM, "AS_gateway" ) );
                                break;

                            }   //  sub_type
                        break;


                    case device.TYPES.T_N:
                        chens.Add( new Chen_Struct( io_module.KINDS.DO, "DO1" ) );
                        chens.Add( new Chen_Struct( io_module.KINDS.DI, "DI1" ) );
                        break;

                    case device.TYPES.T_MIX:
                        chens.Add( new Chen_Struct( io_module.KINDS.DO, "DO1" ) );
                        chens.Add( new Chen_Struct( io_module.KINDS.DI, "DI1" ) );
                        break;

                    case device.TYPES.T_LS:
                        chens.Add( new Chen_Struct( io_module.KINDS.DI, "DI1" ) );
                        chens.Add( new Chen_Struct( io_module.KINDS.PARAM, "scheme" ) );
                        break;

                    case device.TYPES.T_FS:
                        chens.Add( new Chen_Struct( io_module.KINDS.DI, "DI1" ) );
                        break;

                    case device.TYPES.T_FB:
                        chens.Add( new Chen_Struct( io_module.KINDS.DI, "DI1" ) );
                        break;

                    case device.TYPES.T_UPR:
                        chens.Add( new Chen_Struct( io_module.KINDS.DO, "DO1" ) );
                        break;

                    case device.TYPES.T_TE:
                        chens.Add( new Chen_Struct( io_module.KINDS.AI, "AI1" ) );
                        break;

                    case device.TYPES.T_FE:
                        chens.Add( new Chen_Struct( io_module.KINDS.AI, "AI1" ) );
                        chens.Add( new Chen_Struct( io_module.KINDS.PARAM, "min" ) );
                        chens.Add( new Chen_Struct( io_module.KINDS.PARAM, "max" ) );
                        break;

                    case device.TYPES.T_LE:
                        switch ( sub_type )
                            {
                            case device.SUB_TYPES.V_1_CONTROL_CHANNEL:         //  Конус внутрь
                            case device.SUB_TYPES.V_2_CONTROL_CHANNEL:    //  Конус вниз
                            case device.SUB_TYPES.V_1_CONTROL_CHANNEL_1_FB:    //  Конус усеченный целиндр
                                chens.Add( new Chen_Struct( io_module.KINDS.AI, "AI1" ) );
                                chens.Add( new Chen_Struct( io_module.KINDS.PARAM, "radius" ) );
                                chens.Add( new Chen_Struct( io_module.KINDS.PARAM, "H1" ) );
                                chens.Add( new Chen_Struct( io_module.KINDS.PARAM, "H2" ) );
                                break;

                            case device.SUB_TYPES.V_1_CONTROL_CHANNEL_2_FB:         //  Без конуса
                                chens.Add( new Chen_Struct( io_module.KINDS.AI, "AI1" ) );
                                chens.Add( new Chen_Struct( io_module.KINDS.PARAM, "radius" ) );
                                chens.Add( new Chen_Struct( io_module.KINDS.PARAM, "H1" ) );
                                break;

//                             case device.SUB_TYPES.V_2_CONTROL_CHANNEL_2_FB:    //  Настраиваемый уровень
//                                 chens.Add( new Chen_Struct( io_module.KINDS.AI, "AI1" ) );
//                                 chens.Add( new Chen_Struct( io_module.KINDS.PARAM, "sheme" ) );
//                                 break;
                            }
                        break;

                    case device.TYPES.T_QE:
                        chens.Add( new Chen_Struct( io_module.KINDS.AI, "AI1" ) );
                        chens.Add( new Chen_Struct( io_module.KINDS.PARAM, "min" ) );
                        chens.Add( new Chen_Struct( io_module.KINDS.PARAM, "max" ) );
                        break;

                    case device.TYPES.T_CTR:
                        chens.Add( new Chen_Struct( io_module.KINDS.AI, "AI1" ) );
                        break;



                    case device.TYPES.T_AO:
                        chens.Add( new Chen_Struct( io_module.KINDS.AO, "AO1" ) );
                        break;

                    case device.TYPES.T_AI:
                        chens.Add( new Chen_Struct( io_module.KINDS.AI, "AI1" ) );
                        break;

                    }   //  type

                }   //  public Dev_Struct

            }

        public DDT()
            {
            //  Заполняем список устройств
            List<Dev_Struct> temp_list = new List<Dev_Struct>();

            temp_list.Add( new Dev_Struct( device.TYPES.T_V, device.SUB_TYPES.V_1_CONTROL_CHANNEL, "Клапан 1КУ без ОС", "V" ) );
            temp_list.Add( new Dev_Struct( device.TYPES.T_V, device.SUB_TYPES.V_2_CONTROL_CHANNEL, "Клапан 2КУ без ОС", "V" ) );
            temp_list.Add( new Dev_Struct( device.TYPES.T_V, device.SUB_TYPES.V_1_CONTROL_CHANNEL_1_FB, "Клапан 1КУ 1ОС", "V" ) );
            temp_list.Add( new Dev_Struct( device.TYPES.T_V, device.SUB_TYPES.V_1_CONTROL_CHANNEL_2_FB, "Клапан 1КУ 2ОС", "V" ) );
            temp_list.Add( new Dev_Struct( device.TYPES.T_V, device.SUB_TYPES.V_2_CONTROL_CHANNEL_2_FB, "Клапан 2КУ 2ОС", "V" ) );
            temp_list.Add( new Dev_Struct( device.TYPES.T_V, device.SUB_TYPES.V_MIX_PROOF_3_UPR_2_FB, "Клапан MixProof 3КУ 2ОС", "V" ) );
            temp_list.Add( new Dev_Struct( device.TYPES.T_V, device.SUB_TYPES.V_MIX_PROOF_AS_INTERFACE, "Клапан MixProof ASinterface", "V" ) );
            Descr_Dev_Types.Add( device.TYPES.T_V, temp_list );
            
            temp_list = new List<Dev_Struct>();
            temp_list.Add( new Dev_Struct( device.TYPES.T_N, device.SUB_TYPES.V_1_CONTROL_CHANNEL, "Насос", "N" ) );
            Descr_Dev_Types.Add( device.TYPES.T_N, temp_list );

            temp_list = new List<Dev_Struct>();
            temp_list.Add( new Dev_Struct( device.TYPES.T_MIX, device.SUB_TYPES.V_1_CONTROL_CHANNEL, "Электродвигатель", "M" ) );
            Descr_Dev_Types.Add( device.TYPES.T_MIX, temp_list );

            temp_list = new List<Dev_Struct>();
            temp_list.Add( new Dev_Struct( device.TYPES.T_CTR, device.SUB_TYPES.V_1_CONTROL_CHANNEL, "Счетчик", "CTR" ) );
            Descr_Dev_Types.Add( device.TYPES.T_CTR, temp_list );

            temp_list = new List<Dev_Struct>();
            temp_list.Add( new Dev_Struct( device.TYPES.T_TE, device.SUB_TYPES.V_1_CONTROL_CHANNEL, "Текущая температура", "TE" ) );
            Descr_Dev_Types.Add( device.TYPES.T_TE, temp_list );

            temp_list = new List<Dev_Struct>();
            temp_list.Add( new Dev_Struct( device.TYPES.T_QE, device.SUB_TYPES.V_1_CONTROL_CHANNEL, "Текущая концентрация", "QE" ) );
            Descr_Dev_Types.Add( device.TYPES.T_QE, temp_list );

            temp_list = new List<Dev_Struct>();
            temp_list.Add( new Dev_Struct( device.TYPES.T_LS, device.SUB_TYPES.V_1_CONTROL_CHANNEL, "Уровень", "LS" ) );
            Descr_Dev_Types.Add( device.TYPES.T_LS, temp_list );

            temp_list = new List<Dev_Struct>();
            temp_list.Add( new Dev_Struct( device.TYPES.T_LE, device.SUB_TYPES.V_1_CONTROL_CHANNEL, "Текущий уровень (конус вниз)", "LE" ) );
            temp_list.Add( new Dev_Struct( device.TYPES.T_LE, device.SUB_TYPES.V_1_CONTROL_CHANNEL_1_FB, "Текущий уровень (конус внутрь)", "LE" ) );
            temp_list.Add( new Dev_Struct( device.TYPES.T_LE, device.SUB_TYPES.V_1_CONTROL_CHANNEL_2_FB, "Текущий уровень (Усеченный цилиндр)", "LE" ) );
            temp_list.Add( new Dev_Struct( device.TYPES.T_LE, device.SUB_TYPES.V_2_CONTROL_CHANNEL, "Текущий уровень (без конуса)", "LE" ) );
            temp_list.Add( new Dev_Struct( device.TYPES.T_LE, device.SUB_TYPES.V_2_CONTROL_CHANNEL_2_FB, "Текущий уровень (настраиваемый)", "LE" ) );
            Descr_Dev_Types.Add( device.TYPES.T_LE, temp_list );

            temp_list = new List<Dev_Struct>();
            temp_list.Add( new Dev_Struct( device.TYPES.T_FS, device.SUB_TYPES.V_1_CONTROL_CHANNEL, "Расход (есть/нет)", "FS" ) );
            Descr_Dev_Types.Add( device.TYPES.T_FS, temp_list );

            temp_list = new List<Dev_Struct>();
            temp_list.Add( new Dev_Struct( device.TYPES.T_FE, device.SUB_TYPES.V_1_CONTROL_CHANNEL, "Текущий расход", "FE" ) );
            Descr_Dev_Types.Add( device.TYPES.T_FE, temp_list );

            temp_list = new List<Dev_Struct>();
            temp_list.Add( new Dev_Struct( device.TYPES.T_FB, device.SUB_TYPES.V_1_CONTROL_CHANNEL, "Обратная связь", "FB" ) );
            Descr_Dev_Types.Add( device.TYPES.T_FB, temp_list );

            temp_list = new List<Dev_Struct>();
            temp_list.Add( new Dev_Struct( device.TYPES.T_UPR, device.SUB_TYPES.V_1_CONTROL_CHANNEL, "Управляющий сигнал", "UPR" ) );
            Descr_Dev_Types.Add( device.TYPES.T_UPR, temp_list );

            temp_list = new List<Dev_Struct>();
            temp_list.Add( new Dev_Struct( device.TYPES.T_AI, device.SUB_TYPES.V_1_CONTROL_CHANNEL, "Аналоговый вход", "AI" ) );
            Descr_Dev_Types.Add( device.TYPES.T_AI, temp_list );

            temp_list = new List<Dev_Struct>();
            temp_list.Add( new Dev_Struct( device.TYPES.T_AO, device.SUB_TYPES.V_1_CONTROL_CHANNEL, "Аналоговый выход", "AO" ) );
            Descr_Dev_Types.Add( device.TYPES.T_AO, temp_list );


/*
            Descr_Dev_Types.Add( new Dev_Struct( device.TYPES.T_V, device.SUB_TYPES.V_1_CONTROL_CHANNEL, "Клапан без ОС", "V" ) );
            Descr_Dev_Types.Add( new Dev_Struct( device.TYPES.T_V, device.SUB_TYPES.V_1_CONTROL_CHANNEL_1_FB, "Клапан 1 КУ - 1 ОС", "V" ) );
            Descr_Dev_Types.Add( new Dev_Struct( device.TYPES.T_V, device.SUB_TYPES.V_1_CONTROL_CHANNEL_2_FB, "Клапан 1 КУ - 2 ОС", "V" ) );
            Descr_Dev_Types.Add( new Dev_Struct( device.TYPES.T_V, device.SUB_TYPES.V_2_CONTROL_CHANNEL, "Клапан 2 КУ без ОС", "V" ) );
            Descr_Dev_Types.Add( new Dev_Struct( device.TYPES.T_V, device.SUB_TYPES.V_2_CONTROL_CHANNEL_2_FB, "Клапан 2 КУ - 2 ОС", "V" ) );
            Descr_Dev_Types.Add( new Dev_Struct( device.TYPES.T_V, device.SUB_TYPES.V_MIX_PROOF_3_UPR_2_FB, "Клапан 3 КУ - 2 ОС", "V" ) );
            Descr_Dev_Types.Add( new Dev_Struct( device.TYPES.T_V, device.SUB_TYPES.V_MIX_PROOF_AS_INTERFACE, "Клапан ASinterface", "V" ) );

            Descr_Dev_Types.Add( new Dev_Struct( device.TYPES.T_N, device.SUB_TYPES.V_1_CONTROL_CHANNEL, "Насос", "N" ) );
            Descr_Dev_Types.Add( new Dev_Struct( device.TYPES.T_MIX, device.SUB_TYPES.V_1_CONTROL_CHANNEL, "Электродвигатель", "M" ) );
            Descr_Dev_Types.Add( new Dev_Struct( device.TYPES.T_CTR, device.SUB_TYPES.V_1_CONTROL_CHANNEL, "Счетчик", "CTR" ) );
            Descr_Dev_Types.Add( new Dev_Struct( device.TYPES.T_TE, device.SUB_TYPES.V_1_CONTROL_CHANNEL, "Текущая температура", "TE" ) );
            Descr_Dev_Types.Add( new Dev_Struct( device.TYPES.T_QE, device.SUB_TYPES.V_1_CONTROL_CHANNEL, "Текущая концентрация", "QE" ) );

            Descr_Dev_Types.Add( new Dev_Struct( device.TYPES.T_LS, device.SUB_TYPES.V_1_CONTROL_CHANNEL, "Уровень", "LS" ) );
            Descr_Dev_Types.Add( new Dev_Struct( device.TYPES.T_LE, device.SUB_TYPES.V_1_CONTROL_CHANNEL, "Текущий уровень (конус вниз)", "LE" ) );
            Descr_Dev_Types.Add( new Dev_Struct( device.TYPES.T_LE, device.SUB_TYPES.V_1_CONTROL_CHANNEL_1_FB, "Текущий уровень (конус внутрь)", "LE" ) );
            Descr_Dev_Types.Add( new Dev_Struct( device.TYPES.T_LE, device.SUB_TYPES.V_1_CONTROL_CHANNEL_2_FB, "Текущий уровень (Усеченный цилиндр)", "LE" ) );
            Descr_Dev_Types.Add( new Dev_Struct( device.TYPES.T_LE, device.SUB_TYPES.V_2_CONTROL_CHANNEL, "Текущий уровень (без конуса)", "LE" ) );
            Descr_Dev_Types.Add( new Dev_Struct( device.TYPES.T_LE, device.SUB_TYPES.V_2_CONTROL_CHANNEL_2_FB, "Текущий уровень (настраиваемый)", "LE" ) );

            Descr_Dev_Types.Add( new Dev_Struct( device.TYPES.T_FS, device.SUB_TYPES.V_1_CONTROL_CHANNEL, "Расход (есть/нет)", "FS" ) );
            Descr_Dev_Types.Add( new Dev_Struct( device.TYPES.T_FE, device.SUB_TYPES.V_1_CONTROL_CHANNEL, "Текущий расход", "FE" ) );

            Descr_Dev_Types.Add( new Dev_Struct( device.TYPES.T_FB, device.SUB_TYPES.V_1_CONTROL_CHANNEL, "Обратная связь", "FB" ) );
            Descr_Dev_Types.Add( new Dev_Struct( device.TYPES.T_UPR, device.SUB_TYPES.V_1_CONTROL_CHANNEL, "Управляющий сигнал", "UPR" ) );
            Descr_Dev_Types.Add( new Dev_Struct( device.TYPES.T_AI, device.SUB_TYPES.V_1_CONTROL_CHANNEL, "Аналоговый вход", "AI" ) );
            Descr_Dev_Types.Add( new Dev_Struct( device.TYPES.T_AO, device.SUB_TYPES.V_1_CONTROL_CHANNEL, "Аналоговый выход", "AO" ) );
      
 */
            }

        }

    }