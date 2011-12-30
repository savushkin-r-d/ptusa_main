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

/// <summary> Устройства проекта (клапан, насос, танк...).</summary>
namespace tech_device
    {

    public class mode
        {
        public int no;				//	Номер режима (шага)
        public string name;		//	Название режима (шага)

        public TreeView TreeView_params;

        public List<device> on_device;	//	Устройства для включения
        public List<device> off_device;	//	Устройства для выключения
        //	...

        //	Устройства флипования

        //	Сигналы без которых нельзя вкл. режим
        //	Сигналы автоматического вкл/выкл режима
        //	Сигналы UPR (когда режим включен)
        //	Сигналы ОС управляющие устройствами
        //		Списки устройств (на вкл. и на выкл.)
        //	Пары сигналов (дублирование)
        //	...

        public List<mode> step;		//	Шаги режима

        public mode( int no_ = 0, string name_ = "New mode" )
            {
            TreeView_params = new TreeView();
            step = new List<mode>();

            set_attribute( no_, name_ );
            }

        public void set_attribute( int no_, string name_ )
            {
            no = no_;
            if ( name_ != null )
                name = name_;

            if ( step == null )
                step = new List<mode>();
            }
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
            param_list_temp = new List<string[]>();
            param_list_save = new List<string[]>();

            }

/*      //  Копирование объекта
        public void copy_obj_config_to( ref T_Object obj_to )
            {
            this.name = obj_to.name;

            for ( int i = 0; i < mode_mas.Count; i++ )
                {
                mode temp = new mode();

                temp.no = this.mode_mas[ i ].no;
                temp.name = this.mode_mas[ i ].name;


                obj_to.mode_mas[ i ].TreeView_params.Nodes.Clear();
                for ( int j = 0; j < this.mode_mas[ i ].TreeView_params.Nodes.Count; j++ )
                    {
                    obj_to.mode_mas[ i ].TreeView_params.Nodes.Add(
                        ( ( TreeNode ) cur_mode.TreeView_params.Nodes[ i ].Clone() ) );
                        temp.TreeView_params.;
                    }
                                         
                for (  )
                    {
                    temp.step;
                    }
                
                obj_to.mode_mas.Add( temp );
                }

            }
*/
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

		/// <summary> Имя устройства.  </summary>
		public string name;

		/// <summary> Получение номера устройства. </summary>
		///
		/// <remarks> Id, 17.08.2011. </remarks>
		///
		/// <returns> Номер устройства. </returns>
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
                cbox.Items.AddRange( SUB_NAMES );

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

        /// <summary> Имена типов устройств.  </summary>
        private string[] NAMES = 
        {
        "Клапан",
        };
        
        /// <summary> Имена подтипов устройств.  </summary>
        private string[] SUB_NAMES = 
        {
        "1 КУ",
        "2 КУ",
        "1 КУ 1 ОС",
        "1 КУ 2 ОС",
        "2 КУ 2 ОС"
        };

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
            else
                {
                string prop = "Prop." + channel_name;
                shape.Cells[ prop ].FormulaU = "\"none\"";
                }
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

        }
    }
