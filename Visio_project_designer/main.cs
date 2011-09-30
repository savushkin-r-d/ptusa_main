/// @file main.cs
/// @brief Системный класс, который реализует дополнение к Visio.
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
using System.Xml.Linq;
using System.Xml;

using Visio  = Microsoft.Office.Interop.Visio;
using Office = Microsoft.Office.Core;

using System.Windows.Forms;
using System.Text.RegularExpressions;

using wago;
using tech_device;

/// Пространство имен данного дополнения к Visio.
namespace visio_prj_designer
    {
    /// <summary> Класс дополнения. </summary>
    ///
    /// <remarks> Id, 16.08.2011. </remarks>
    public partial class visio_addin
        {
        /// <summary> Флаг режима привязки канала к клемме модуля. </summary>
        internal bool is_selecting_clamp = false;

        /// <summary> Лента дополнения. </summary>
        internal main_ribbon vis_main_ribbon;

		/// <summary> Список доступных модулей </summary>
        internal service_data service_config;

		/// <summary> Все устройства. </summary>
        internal List<device> g_devices;

		/// <summary> Объекты проекта (гребенки и танки) </summary>
		internal List<T_Object> g_objects;

        /// <summary> Объект контроллера. </summary>
        internal PAC g_PAC = null;       

        /// <summary> Флаг режима привязки устройств к модулям. </summary>
        internal bool is_device_edit_mode = false; 

        /// <summary> Форма редактирования привязки к каналам. </summary>
        internal edit_valve_type_form_usage edit_io_frm;

        /// <summary> Константы для обращения к окнам Visio. </summary>
        public enum VISIO_WNDOWS : short
            {
            MAIN    = 1, ///< Главное окно.
            IO_EDIT = 2, ///< Дополнительное окно.
            };

        /// <summary> Приложение Visio. </summary>
        private Visio.Application visio_app;

        /// <summary> Event handler. Called by <c>visio_addin</c> for startup 
        /// events. Вызывается при загрузке дополнения. Здесь мы регистрируем 
        /// свои обработчики событий, создаем нужные объекты. </summary>
        ///
        /// <remarks> Id, 16.08.2011. </remarks>
        ///
        /// <param name="sender"> Source of the event. </param>
        /// <param name="e">      Event information. </param>
        private void visio_addin__startup( object sender, System.EventArgs e )
            {

            visio_app = this.Application;
            visio_app.DocumentCreated +=
                new Microsoft.Office.Interop.Visio.EApplication_DocumentCreatedEventHandler(
                    visio_addin__DocumentCreated );

            visio_app.ShapeAdded +=
                new Microsoft.Office.Interop.Visio.EApplication_ShapeAddedEventHandler(
                    visio_addin__ShapeAdded );

            visio_app.ShapeExitedTextEdit +=
                new Microsoft.Office.Interop.Visio.EApplication_ShapeExitedTextEditEventHandler(
                    visio_addin__ShapeExitedTextEdit);

            visio_app.BeforeShapeDelete +=
                new Microsoft.Office.Interop.Visio.EApplication_BeforeShapeDeleteEventHandler(
                    visio_addin__BeforeShapeDelete );

            visio_app.ConnectionsAdded +=
                new Microsoft.Office.Interop.Visio.EApplication_ConnectionsAddedEventHandler(
                    visio_addin__ConnectionsAdded );

            visio_app.FormulaChanged += 
                new Microsoft.Office.Interop.Visio.EApplication_FormulaChangedEventHandler(
                    visio_addin__FormulaChanged );

            visio_app.SelectionChanged += 
                new Microsoft.Office.Interop.Visio.EApplication_SelectionChangedEventHandler(
                    visio_addin__SelectionChanged );

            visio_app.DocumentOpened += 
                new Microsoft.Office.Interop.Visio.EApplication_DocumentOpenedEventHandler(
                 visio_addin__DocumentOpened );

            visio_app.MouseDown +=new 
                Visio.EApplication_MouseDownEventHandler(
                 visio_addin___MouseDown );

            //visio_app.BeforeDocumentSave += 
            //    new Microsoft.Office.Interop.Visio.EApplication_BeforeDocumentSaveEventHandler(
            //    visio_addin__BeforeDocumentSave );

            visio_app.DocumentSaved +=
                new Microsoft.Office.Interop.Visio.EApplication_DocumentSavedEventHandler(
                visio_addin__DocumentSaved );  

            g_devices = new List< device > ();
            g_objects = new List< T_Object > ();
            }

        /// <summary> Event handler. Called by visio_addin for shutdown events. </summary>
        ///
        /// <remarks> Id, 16.08.2011. </remarks>
        ///
        /// <param name="sender"> Source of the event. </param>
        /// <param name="e">      Event information. </param>
        private void visio_addin__shutdown( object sender, System.EventArgs e )
            {
            }

        //private void visio_addin__BeforeDocumentSave( Visio.Document target )
        //    {
        //    }

        private void visio_addin__DocumentSaved( Visio.Document target )
            {
            if( target.Type == Visio.VisDocumentTypes.visTypeDrawing )
                {
                Write_XML_description( target.Name );
                }
            }           

        /// <summary> Event handler. Реализована обработка клика мыши
        /// в режиме выбора клеммы для привязки. </summary>
        ///
        /// <remarks> Id, 16.08.2011. </remarks>
        ///
        /// <param name="Button">         The button. </param>
        /// <param name="KeyButtonState"> State of the key button. </param>
        /// <param name="x">              The x coordinate. </param>
        /// <param name="y">              The y coordinate. </param>
        /// <param name="CancelDefault">  [in,out] The cancel default. </param>
        private void visio_addin___MouseDown( int Button, int KeyButtonState, double x,
            double y, ref bool CancelDefault )
            {
            if( is_selecting_clamp )
                {
                foreach( io_module module in g_PAC.get_io_modules() )
                    {
                    int clamp = module.get_mouse_selection();
                    if( clamp > -1 )
                        {
                        g_PAC.unmark();

                        current_selected_dev.set_channel( 
                            current_selected_dev.get_active_channel(), module, clamp );

                        visio_addin__SelectionChanged( visio_app.Windows[ 
                            ( short ) visio_addin.VISIO_WNDOWS.IO_EDIT ] );

                        visio_app.Windows[ ( short ) visio_addin.VISIO_WNDOWS.MAIN ].MouseMove -=
                            new Microsoft.Office.Interop.Visio.EWindow_MouseMoveEventHandler(
                            Globals.visio_addin.visio_addin__MouseMove );
                        is_selecting_clamp = false;

                        CancelDefault = true;

                        return;
                        }
                    }

                g_PAC.unmark();
                visio_addin__SelectionChanged( visio_app.Windows[
                    ( short ) visio_addin.VISIO_WNDOWS.IO_EDIT ] );

                visio_app.Windows[ ( short ) visio_addin.VISIO_WNDOWS.MAIN ].MouseMove -=
                    new Microsoft.Office.Interop.Visio.EWindow_MouseMoveEventHandler(
                    Globals.visio_addin.visio_addin__MouseMove );
                is_selecting_clamp = false;
                }
            }

        /// <summary> Event handler. Реализована обработка перемещения мыши
        /// в режиме выбора клеммы для привязки. </summary>
        ///
        /// <remarks> Id, 16.08.2011. </remarks>
        ///
        /// <param name="Button">         The button. </param>
        /// <param name="KeyButtonState"> State of the key button. </param>
        /// <param name="x">              The x coordinate. </param>
        /// <param name="y">              The y coordinate. </param>
        /// <param name="CancelDefault">  [in,out] The cancel default. </param>
        internal void visio_addin__MouseMove( int Button,
            int KeyButtonState, double x, double y, ref bool CancelDefault )
            {

            try
                {
                if( is_selecting_clamp )
                    {
                    foreach( io_module module in g_PAC.get_io_modules() )
                        {
                        module.magnify_on_mouse_move( x, y );
                        }
                    }

                }
            catch( System.Exception ex )
                {
                MessageBox.Show( ex.Message );
                }
            }

        /// <summary> Event handler. Создание необходимых объектов при открытии
        /// файла. </summary>
        ///
        /// <remarks> Id, 16.08.2011. </remarks>
        ///
        /// <param name="target"> Target for the. </param>
        private void visio_addin__DocumentOpened( Visio.Document target )
            {
            if( target.Type == Visio.VisDocumentTypes.visTypeDrawing )
                {
//                if( target.Template.Contains( "PTUSA project" ) )
                    {
                    vis_main_ribbon.show();

					//	Создание списка доступных элементов
					service_config = new service_data();
					//for ( int i = 0; i < service_config.DI_modules.Count; i++ )
					//    {
					//    string ss = service_config.DI_modules[ 402 ];
					//    }
					
try	
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
                            g_PAC.add_io_module( shape );
                            }
                        }
	}
catch ( Exception err )
	{
	MessageBox.Show( "Ошибка считывания модулей WAGO" + err );
	//throw;
	}
//-----------------------------------------------------------------------------
					
try
	{
					g_devices.Clear();

                    //Считывание устройств.
                    foreach( Visio.Shape shape in target.Pages[ "Устройства" ].Shapes )
                        {
                        switch ( shape.Data1 )
							{
							case "V":
							case "N":
							case "MIX":
							case "CTR":
							case "TE":
							case "QE":
							case "LS":
							case "LE":
							case "FS":
							case "FE":
							case "FB":
							case "UPR":
							case "AI":
							case "AO":
							case "FQT":
							case "WTE":							
							    g_devices.Add( new device( shape, g_PAC ) );
								break;

                            case "TANK":
                                g_objects.Add( new T_Object( shape, g_PAC ) );
                                break;
                            }		//	switch ( shape.Data1 )	
                        }		//	foreach
	}
catch ( Exception err )
	{
	System.Diagnostics.Debug.WriteLine( err.Message );
	MessageBox.Show( "Ошибка считывания устройств проекта" );
	//throw;
	}
//-----------------------------------------------------------------------------

try
	{
					foreach ( Visio.Shape shape in target.Pages[ "Обмен сигналами" ].Shapes )
						{
						switch ( shape.Data1 )
							{
							case "FB":
							case "UPR":
							case "AI":
							case "AO":
								g_devices.Add( new device( shape, g_PAC ) );
								break;
							}		//	switch ( shape.Data1 )	
						}		//	foreach
	}
catch ( Exception err )
	{
	System.Diagnostics.Debug.WriteLine( err.Message );
	MessageBox.Show( "Ошибка считывания сигналов проекта" );
	//throw;
	}


                    //  Открытие файла описания сложных объектов (гребенка, танк)
                    Read_XML_description( 0 );

                    }
				//else
				//    {
				//    vis_main_ribbon.hide();
				//    }
                }
            }

        /// <summary> Предыдущее активное устройство (выделенное мышкой). </summary>
        private device previous_selected_dev = null;
        
        /// <summary> Текущее активное устройство (выделенное мышкой). </summary>
        internal device current_selected_dev = null;


        /// <summary> Текущий активный объект (Гребенка или Танк). </summary>
        internal T_Object current_selected_object = null;

        /// <summary> Текущий режим. </summary>
        internal int cur_mode;
        
        /// <summary> Текущий шаг. </summary>
        internal int cur_step;
       

        /// <summary> Event handler. Обработка изменения активного устройства 
        /// (подсветка клемм устройства и т.д.). </summary>
        ///
        /// <remarks> Id, 16.08.2011. </remarks>
        ///
        /// <param name="window"> Активное окно. </param>
        private void visio_addin__SelectionChanged( Microsoft.Office.Interop.Visio.Window window )
            {
            // Проверка на режим привязки устройств к каналам ввода\вывода.
            if( Globals.visio_addin.is_device_edit_mode ) 
                {
                // Проверка на активность окна редактирования.
                if( window.Index == ( short ) VISIO_WNDOWS.IO_EDIT )
                    {

                    // Проверка на активность страницы с устройствами (клапанами т.д.).
                    if( window.Page.Name == "Устройства" )
                        {
						const int IO_PROP_WINDOW_INDEX = 8;
						window.Windows[ IO_PROP_WINDOW_INDEX ].Caption =
                                "Каналы";

                        if( window.Selection.Count > 0 )
                            {
                            window.Windows[ IO_PROP_WINDOW_INDEX ].Caption =
                                window.Windows[ IO_PROP_WINDOW_INDEX ].Caption +
                                " - " + window.Selection[ 1 ].Name;

                            //Снятие ранее подсвеченных модулей Wago.
                            if( previous_selected_dev != null )
                                {
                                previous_selected_dev.unselect_channels();
                                }

							Microsoft.Office.Interop.Visio.Shape selected_shape =
							    window.Selection[ 1 ];

							current_selected_dev = g_devices.Find( delegate( device dev )
							    {
								return dev.get_shape() == selected_shape;
								}
							    );

                            if( current_selected_dev != null )
                                {
                                current_selected_dev.select_channels();

                                //Обновление окна со свойствами привязки.
                                edit_io_frm.listForm.enable_prop();

                                current_selected_dev.refresh_edit_window(
                                    edit_io_frm.listForm.type_cbox,
                                    edit_io_frm.listForm.type_lview );                                                               
                                }

                            previous_selected_dev = current_selected_dev;

                            } //if( window.Selection.Count > 0 )
                        else
                            {
                            //	Снятие ранее подсвеченных модулей Wago.
							if( previous_selected_dev != null )
							    {
							    previous_selected_dev.unselect_channels();
							    previous_selected_dev = null;
							    }

							//	Снятие подсветки с ранее выбранных модулей
							foreach ( io_module mod in g_PAC.get_io_modules() )
								{
								mod.deactivate();
								}
							
                            edit_io_frm.listForm.disable_prop();
                            }

                        } //if( window.Page.Name == "Устройства" )
                    } //if( window.Index == ( short ) VISIO_WNDOWS.IO_EDIT )
                } //if( visio_addin.is_device_edit_mode ) 
            else
                {
                //  Обычный режим работы
                current_selected_object = null;
                
                Microsoft.Office.Interop.Visio.Shape selected_shape =
				        window.Selection[ 1 ];

                if (    ( selected_shape != null ) 
                    &&  (   ( Convert.ToInt32( selected_shape.Cells[ "Prop.type" ].Formula ) ==
                                                Convert.ToInt32( device.TYPES.T_TANK ) )
                        ||  ( Convert.ToInt32( selected_shape.Cells[ "Prop.type" ].Formula ) ==
                                                Convert.ToInt32( device.TYPES.T_GREB ) )
                        )
                   )
                    {
                    current_selected_object = g_objects.Find( delegate( T_Object obj )
                        {
                        return obj.get_shape() == selected_shape;
                        }
                        );
                    }
                }
            }

        /// <summary> Event handler. Обработчик изменения формул (задание 
        /// вычисляемых свойств объекта (порядковый номер, ...) при его 
        /// добавлении и т.д.). </summary>
        ///
        /// <remarks> Id, 16.08.2011. </remarks>
        ///
        /// <param name="cell"> Ячейка, где произошло изменение. </param>
        private void visio_addin__FormulaChanged( Microsoft.Office.Interop.Visio.Cell cell )
            {
			switch ( cell.Shape.Data1 )
				{
				//	Изменения свойств модуля WAGO
				case "750":

					if ( cell.Name == "Prop.order_number" )
						{
						cell.Shape.Shapes[ "module_number" ].Text =
							cell.Shape.Cells[ "Prop.order_number" ].Formula;
						return;
						}

					if ( cell.Name == "Prop.type" )
						{
						int type = ( int ) Convert.ToDouble( cell.Shape.Cells[ "Prop.type" ].ResultStr[ "" ] );

						cell.Shape.Data2 = Convert.ToString( type );
						cell.Shape.Shapes[ "type_str" ].Text = Convert.ToString( type );

						string colour = "1";
						switch ( type )
							{
							//	DO modules
							case 504:
							case 512:
							case 530:
							case 1504:
							case 1515:
								colour = "2";	//"RGB( 255, 128, 128 )";	//	red
								break;

							//	DI modules
							case 402:
							case 430:
							case 1405:
							case 1415:
							case 1420:
								colour = "5";	//"RGB( 255, 255, 128 )";	//	yellow
								break;

							//	AO modules
							case 554:
								colour = "7"; //"RGB( 180, 228, 180 )";		//	blue	
								break;

							//	AI modules
							case 455:
							case 460:
							case 461:
							case 4612:
							case 466:
							case 638:
								colour = "3";	//"RGB( 180, 228, 180 )";	//	green
								break;

							//	Special modules
							case 493:	//	3-Phase	counter
							case 655:	//	AS-interface
								colour = "6";
								break;

							//	System modules
							case 602:
							case 612:
							case 613:
							case 628:
								colour = "14";
								break;

							case 600:
							case 627:
								colour = "15";
								break;
							}

						cell.Shape.Shapes[ "module_number" ].Cells[
							"FillForegnd" ].FormulaU = colour;
						cell.Shape.Shapes[ "3" ].Cells[
							"FillForegnd" ].FormulaU = colour;

						}
					break;

				//	Изменение свойств простых устройств
				case "N":
				case "MIX":
				case "CTR":
				case "TE":
				case "QE":
				case "LS":
				case "LE":
				case "FS":
				case "FE":
				case "FB":
				case "UPR":
				case "AI":
				case "AO":
				case "FQT":
				case "WTE":
                    //Поиск по shape объекта device.
                    current_selected_dev = g_devices.Find( delegate( device dev )
                    {
                        return dev.get_shape() == cell.Shape;
                    }
                    );

					switch ( cell.Name )
						{
						case "Prop.number":
							int number = Convert.ToInt16( cell.Shape.Cells[ "Prop.number" ].Formula );
							current_selected_dev.n = number;
							break;

						case "Prop.name":
							string str = cell.Shape.Cells[ "Prop.name" ].Formula;
							str = str.Replace( "\"", "" );
                            cell.Shape.Shapes[ "name" ].Text = str.ToUpper();

                            current_selected_dev.name = str.ToUpper();
							break;

						case "Prop.description":
							current_selected_dev.description = cell.Shape.Cells[ "Prop.description" ].Formula;
							break;

						case "Prop.type":	//	Тип устройства менять нельзя
							//int type = Convert.ToUInt16( cell.Shape.Cells[ "Prop.type" ].Formula );
							//current_selected_dev.type = ( tech_device.device.TYPES ) type;
							break;

						case "Prop.sub_type":
							int sub_type = Convert.ToInt16( cell.Shape.Cells[ "Prop.sub_type" ].Formula );

							if ( current_selected_dev != null )
								{
								if ( current_selected_dev.get_sub_type() != sub_type )
									{
									edit_io_frm.listForm.type_cbox.SelectedIndex = sub_type;
									current_selected_dev.change_sub_type((device.SUB_TYPES)sub_type, g_PAC);
									}
								}

							//current_selected_dev.sub_type = cell.Shape.Cells[ "Prop.sub_type" ].Formula;
							break;
						}
					break;


                case "TANK":
                case "GREB":
                    //Поиск по shape объекта
                    current_selected_object = g_objects.Find( delegate( T_Object obj )
                    {
                        return obj.get_shape() == cell.Shape;
                    }
                    );

					switch ( cell.Name )
						{
                        case "Prop.name":
                        case "Prop.number":
                        case "Prop.tonnage":
                        case "Prop.description":
							string str_name = cell.Shape.Cells[ "Prop.name" ].Formula;
							str_name = str_name.Replace( "\"", "" );
                            current_selected_object.name = str_name.ToUpper();

                            string str_no = cell.Shape.Cells[ "Prop.number" ].Formula;
                            current_selected_object.n = Convert.ToInt32( str_no );

                            string str_discr = cell.Shape.Cells[ "Prop.description" ].Formula;
                            str_discr = str_discr.Replace( "\"", "" );
                            current_selected_object.description = str_discr;
                            
                            if ( cell.Shape.Data1 == "GREB" )
                                {
                                //  Формируем текст на гребенке
                                cell.Shape.Shapes[ "name" ].Text = str_name + " " + str_no;
                                }
                            else    //  cell.Shape.Data1 = "TANK"
                                {
                                //  Считываем поле, характерное только для танка
                                string str_t = cell.Shape.Cells[ "Prop.tonnage" ].Formula;
                                //  Нет поля для храниния этого параметра (только в фигуре)

                                //  Формируем текст на танке
                                cell.Shape.Shapes[ "name" ].Text =  "№" + str_no + "\n" + str_name + "\n" + str_t + " т";
                                }
							break;
                        }
                    break;


                case "V":
                    //Поиск по shape объекта device.
                    current_selected_dev = g_devices.Find( delegate( device dev )
                    {
                        return dev.get_shape() == cell.Shape;
                    }
                    );

					switch ( cell.Name )
						{
						case "Prop.number":	//	Получаем из имени устройства
							break;


						case "Prop.name":
							string str = cell.Shape.Cells[ "Prop.name" ].Formula;

							//Первый вариант маркировки клапана - 1V12.
							Regex rex = new Regex( @"\b([0-9]{0,4})V([0-9]{1,2})\b",
								RegexOptions.IgnoreCase );
							if ( rex.IsMatch( str ) )
								{
								Match mtc = rex.Match( str );

								string n_part_1 = mtc.Groups[ 1 ].ToString();
								string n_part_2 = mtc.Groups[ 2 ].ToString();
								if ( n_part_1 == "" )
									n_part_1 = "0";

								int n = Convert.ToUInt16( n_part_1 ) * 100 +
										Convert.ToUInt16( n_part_2 );

								cell.Shape.Cells[ "Prop.number" ].FormulaU = n.ToString();

								str = str.Replace( "\"", "" );
								cell.Shape.Name = str.ToUpper();
								cell.Shape.Shapes[ "name" ].Text = str.ToUpper();
								break;
								}
								
							//Второй вариант маркировки клапана - S4V12.
							Regex rex2 = new Regex( @"\b([a-z]{1})([0-9]{1})V([0-9]{1,2})\b",
								RegexOptions.IgnoreCase );
							if ( rex2.IsMatch( str ) )
								{
								Match mtc = rex2.Match( str );

								//	Принцип нумерации клапанов в зависимости от имени
								//	( А - 200, В - 220, С - 240, ..., W - 640 )
								//
								//	B 3 V 17    ===> ( 220 * 100 ) + ( 3 * 100 ) + ( 17 )  ===>  V( 22317 )
								//	| |	| |____	17 = ( 17 )				 от 0 до 99
								//	| |	|______  V =  ---				 ---
								//	| |________	 3 = ( 3   * 100 )		 от 0 до 19
								//	|__________	 B = ( 220 * 100 )		 от А до W

								//	Линия (A-W). Выделяем букву и переводим её в верхний регистр (А - 65 ..., а - 97 ...).
								//	Получаем адрес буквы в таблице символов (А - 65 ...) 
								char letter = mtc.Groups[ 1 ].ToString().ToUpper()[ 0 ];
								int n = Convert.ToUInt16( letter );

								//	Преобразуем букву в номер в соответствии с принятым принципом нумерации
								n = ( ( ( n - 65 ) * 20 ) + 200 ) * 100;

								//Номер линии (0-19).
								n += 100 * Convert.ToUInt16( mtc.Groups[ 2 ].ToString() );
								//Клапан.
								n += Convert.ToUInt16( mtc.Groups[ 3 ].ToString() );

								cell.Shape.Cells[ "Prop.name" ].FormulaU = str.ToUpper();
								cell.Shape.Cells[ "Prop.number" ].FormulaU = n.ToString();

								//	Именуем элемент на схеме
								str = str.Replace( "\"", "" );			//	Удаляем лишние символы
								cell.Shape.Name = str.ToUpper();		//	Переводим в верхний регистр
								cell.Shape.Shapes[ "name" ].Text = str.ToUpper();

								current_selected_dev.name = str.ToUpper();
								break;
								}

							MessageBox.Show( "Неверная маркировка клапана - \"" +
								str + "\"!" );
							cell.Shape.Cells[ "Prop.name" ].FormulaU = "\"V19\"";
							break;

						case "Prop.description":
							current_selected_dev.description = cell.Shape.Cells[ "Prop.description" ].Formula;
							break;

						case "Prop.type":	//	Тип устройства менять нельзя
							  //int type = Convert.ToInt16( cell.Shape.Cells[ "Prop.type" ].Formula );
							  //current_selected_dev.type = ( tech_device.device.TYPES ) type;

						//    string type = cell.Shape.Cells[ "Prop.type" ].get_ResultStr( 0 );

						//    switch ( type )
						//        {
						//        case "1 КУ":
						//            //cell.Shape.AddSection( cell.Shape.get_CellsRowIndex );
						//            int idx = cell.Shape.get_CellsRowIndex( "Prop.type" );

						//            cell.Shape.AddNamedRow(
						//                ( short ) Visio.VisSectionIndices.visSectionUser, "DO",
						//                ( short ) Visio.VisRowTags.visTagDefault );

						//            cell.Shape.AddNamedRow(
						//                ( short ) Visio.VisSectionIndices.visSectionProp, "hf",
						//                ( short ) 0 );
						//            break;
						//        }
						    break;


						case "Prop.sub_type":
							int sub_type = Convert.ToInt16( cell.Shape.Cells[ "Prop.sub_type" ].Formula );

							if ( current_selected_dev != null )
								{
								if ( current_selected_dev.get_sub_type() != sub_type )
									{
									edit_io_frm.listForm.type_cbox.SelectedIndex = sub_type;
									current_selected_dev.change_sub_type((device.SUB_TYPES)sub_type, g_PAC);
									}
								}

							//current_selected_dev.sub_type = cell.Shape.Cells[ "Prop.sub_type" ].Formula;
							break;


						case "Prop.septum":
							str = cell.Shape.Cells[ "Prop.septum" ].Formula;
							str = str.Replace( "\"", "" );
							str = str.ToUpper();

							if ( str == "НО" )
								{
								cell.Shape.Shapes[ "septum" ].SendToBack();
								}
							else  //	"НЗ"
								{
								cell.Shape.Shapes[ "septum" ].BringToFront();
								}
							break;
						}	//	switch ( cell.Name )
					break;

				}	//	switch ( cell.Shape.Data1 )

			}

        /// <summary> Event handler. Обработчик события "привязки" фигуры. Здесь
        /// заполняем порядковый номер модуля в наборе. </summary>
        ///
        /// <remarks> Id, 16.08.2011. </remarks>
        ///
        /// <param name="connect"> Объект "склейки". </param>
        private void visio_addin__ConnectionsAdded( Microsoft.Office.Interop.Visio.Connects connect )
            {
            Microsoft.Office.Interop.Visio.Shape obj_2 =
                visio_app.ActivePage.Shapes[ connect.FromSheet.Name ];
            Microsoft.Office.Interop.Visio.Shape obj_1 =
                visio_app.ActivePage.Shapes[ connect.ToSheet.Name ];

            switch( obj_1.Data1 )
                {
                case "750":
                    if( obj_1.Data2 == "860" )
                        {
                        obj_2.Cells[ "Prop.order_number" ].FormulaU = "1";
                        }
                    else
                        {
                        int new_number = Convert.ToInt32( obj_1.Cells[ "Prop.order_number" ].FormulaU ) + 1;
                        obj_2.Cells[ "Prop.order_number" ].FormulaU = Convert.ToString( new_number );
                        }

                    break;
                }
				
			//	Необходимо реализовать авто нумерацию присоединенной пачки модулей
									  
			//Array V_S;

			//V_S = obj_1.ConnectedShapes( 
			//    Microsoft.Office.Interop.Visio.VisConnectedShapesFlags.visConnectedShapesAllNodes, "" );

			//V_S = obj_2.ConnectedShapes( 
			//    Microsoft.Office.Interop.Visio.VisConnectedShapesFlags.visConnectedShapesAllNodes, "" );

            }


        private void visio_addin__ShapeExitedTextEdit( Microsoft.Office.Interop.Visio.Shape shape )
            {
            if ( shape.Text != shape.RootShape.Cells[ "Prop.name" ].Formula.Replace( "\"", "" ) )
                {
                shape.RootShape.Cells[ "Prop.name" ].FormulaU = "\"" + shape.Text + "\"";
                }
            }


        /// <summary> Event handler. Удаляем соответствующий объект при удалении
        /// связанной фигуры. </summary>
        ///
        /// <remarks> Id, 16.08.2011. </remarks>
        ///
        /// <param name="shape"> Удаляемая фигура. </param>
        private void visio_addin__BeforeShapeDelete( Microsoft.Office.Interop.Visio.Shape shape )
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
					else  //	( shape.Data2 != "860" )
						{
						//	Удаление выбранного модуля из списка модулей
						g_PAC.current_module = g_PAC.get_io_modules().Find
							( 
							delegate( io_module module )
								{
								return module.shape == shape;
								}
							);
							g_PAC.get_io_modules().Remove( g_PAC.current_module );
						}
                    break;

				case "V":
				case "N":
				case "MIX":
				case "CTR":
				case "TE":
				case "QE":
				case "LS":
				case "LE":
				case "FS":
				case "FE":
				case "FB":
				case "UPR":
				case "AI":
				case "AO":
				case "FQT":
				case "WTE":	
					 current_selected_dev = g_devices.Find( delegate( device dev )
                         {
						 return dev.get_shape() == shape;
						 }
						 );

					g_devices.Remove( current_selected_dev );

					break;

                case "TANK":
                case "GREB":
					 current_selected_object = g_objects.Find( delegate( T_Object obj )
                         {
						 return obj.get_shape() == shape;
						 }
						 );

					g_objects.Remove( current_selected_object );	
                    break;
                }
            }

        #region Поля для реализации функциональности вставки сразу нескольких модулей Wago.

        private bool is_duplicating = false;
        private int duplicate_count = 0;

        private Microsoft.Office.Interop.Visio.Shape old_shape;
        private Microsoft.Office.Interop.Visio.Shape new_shape;

        private bool no_delete_g_pac_flag = false;
        
        #endregion

        /// <summary> Event handler. Обработка события добавления фигуры. Здесь
        /// реализована часть функциональности вставки сразу нескольких модулей 
        /// Wago. </summary>
        ///
        /// <remarks> Id, 16.08.2011. </remarks>
        ///
        /// <param name="shape"> Добавляемая фигура. </param>
        private void visio_addin__ShapeAdded( Microsoft.Office.Interop.Visio.Shape shape )
            {
            //Проверяем, нужный ли нам объект мы добавляем на лист. Выводим окно
            //с запросом количества элементов. Помещаем на форму первый элемент,
            //остальные приклеиваем к нему путем дублирования вставляемого 
            //элемента.

            if( is_duplicating )
                {

                duplicate_count--;
                if( duplicate_count <= 0 )
                    {
                    is_duplicating = false;
                    }

                return;
                }

			switch ( shape.Data1 )
				{
				case "750":
					if ( shape.Data2 == "860" )
						{
						if ( g_PAC == null )
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
					else	 
						{
						//	if ( shape.Data2 != "860" )

						modules_count_enter modules_count_enter_form = new modules_count_enter();
						modules_count_enter_form.ShowDialog();
						duplicate_count = modules_count_enter_form.modules_count - 1;

						////////////////////////////

						//	Нужно будет доработать
						//	Проверка составленого списка модулей (LB_modules) 
						//		с имеющимся списком объектов g_PAC.io_modules
						//	g_PAC.insert_io_module( index, shape ); для недостоющих

						////////////////////////////
			
						shape.Cells[ "Prop.type" ].FormulaU	= modules_count_enter_form.modules_type;
						shape.Shapes[ "type_str" ].Text		= modules_count_enter_form.modules_type;

						g_PAC.add_io_module( shape );

						for ( int i = 0; i < duplicate_count; i++ )
							{
							is_duplicating = true;
							new_shape = shape.Duplicate();
							old_shape = shape;

							string str_x = string.Format( 
								"PNTX(LOCTOPAR(PNT('{0}'!Connections.X2,'{0}'!Connections.Y2),'{0}'!EventXFMod,EventXFMod))+6 mm",
							    old_shape.Name );

							string str_y = string.Format( 
								"PNTY(LOCTOPAR(PNT('{0}'!Connections.X2,'{0}'!Connections.Y2),'{0}'!EventXFMod,EventXFMod))+-47 mm",
							    old_shape.Name );

							new_shape.Cells[ "PinX" ].Formula = str_x;
							new_shape.Cells[ "PinY" ].Formula = str_y;

							shape = new_shape;

							g_PAC.add_io_module( shape );

							shape.Cells[ "Prop.type" ].FormulaU = modules_count_enter_form.modules_type;
							shape.Shapes[ "type_str" ].Text		= modules_count_enter_form.modules_type;

							visio_addin__FormulaChanged( shape.Cells[ "Prop.type" ] );
							}
						}
					break;

				case "V":
				case "N":
				case "MIX":
				case "CTR":
				case "TE":
				case "QE":
				case "LS":
				case "LE":
				case "FS":
				case "FE":
				case "FB":
				case "UPR":
				case "AI":
				case "AO":
				case "FQT":
				case "WTE":
					g_devices.Add( new device( shape, g_PAC ) );
					break;

				case "TANK":
                case "GREB":
				    g_objects.Add( new T_Object( shape, g_PAC ) );
				    break;
				}	//	switch ( shape.Data1 ) 

            }

		/// <summary> Event handler.  </summary>
		///
		/// <remarks> ASV, 23.08.2011. </remarks>
		///
		/// <param name="shape"> Редактируемая фигура. </param>
		private void visio_addin__ShapeChanged(Microsoft.Office.Interop.Visio.Shape shape)
		{
			switch (shape.Data1)
			{
				//case "750":
				//    if (shape.Data2 == "860")
				//    {
				//        if (no_delete_g_pac_flag)
				//        {
				//            no_delete_g_pac_flag = false;
				//        }
				//        else
				//        {
				//            g_PAC = null;
				//        }
				//    }
				//    break;

				case "V":
				case "N":
				case "MIX":
				case "CTR":
				case "TE":
				case "QE":
				case "LS":
				case "LE":
				case "FS":
				case "FE":
				case "FB":
				case "UPR":
				case "AI":
				case "AO":
				case "FQT":
				case "WTE":	

					//Поиск по shape объекта device.
					//int dev_n = Convert.ToInt16(shape.Cells["Prop.number"].FormulaU);
					//int dev_type = Convert.ToInt16(shape.Cells["Prop.type"].FormulaU);

					current_selected_dev = g_devices.Find(delegate(device dev)
						{
						//return dev.get_n() == dev_n && dev.get_type() == dev_type;
						return dev.get_shape() == shape;
						}
						);

					//g_devices[ current_selected_dev ];

					break;
			}
		}

        /// <summary> Event handler. При создании документа показываем полосу 
        /// работы с техпроцессом. </summary>
        ///
        /// <remarks> Id, 16.08.2011. </remarks>
        ///
        /// <param name="doc"> Created document. </param>
        private void visio_addin__DocumentCreated( Microsoft.Office.Interop.Visio.Document doc )
            {
            if( doc.Template.Contains( "PTUSA project" ) )
                {
                vis_main_ribbon.show();
                }
            else
                {
                vis_main_ribbon.hide();
                }
            }


        /// <summary> Загрузка данных класса из файла описания </summary>
        /// <remarks> asvovik, 21.09.2011. </remarks>
        ///
        /// <param name="XML"> I don't now </param>
        public void Read_XML_description( int XML )
            {
            string fileName="..\\Visio docs\\description_device.xml";
            XmlTextReader tr = new XmlTextReader( fileName );

            while (     !tr.EOF
                    &&  tr.Read()    
                  )
                {

                string temp_name;
                int temp_no;

                switch ( tr.Name )
                    {
                    case "object":
                        temp_name = tr.GetAttribute( "object-name" );
                        temp_no = Convert.ToInt32( tr.GetAttribute( "object-number" ) );

                        //  Объекты уже были прочитаны, нужно только найти среди них текущий
                        current_selected_object = g_objects.Find( delegate( T_Object obj )
                            {
                                return obj.get_n() == temp_no && obj.get_name() == temp_name; 
                            }
                            );
                    	break;

                    case "mode":
                        temp_name = tr.GetAttribute( "mode-name" );
                        temp_no = Convert.ToInt32( tr.GetAttribute( "mode-number" ) );
                        
                        if ( current_selected_object != null )
                            {
                            //  Создаем новый режим
                            T_Object.mode temp_mode = new T_Object.mode();
                            //  Устанавлеиваем его атрибуты
                            temp_mode.set_attribute( temp_no, temp_name );

                            //  Добавляем в список режимамов
                            current_selected_object.mode_mas.Add( temp_mode );

                            //  Фиксируем номер текущего режима для дальнейших обращений к нему
                            cur_mode = temp_no;
                            }
                        break;

                    case "step":
                        temp_name = tr.GetAttribute( "step-name" );
                        temp_no = Convert.ToInt32( tr.GetAttribute( "step-number" ) );

                        if ( current_selected_object != null )
                            {
                            //  Создаем новый режим
                            T_Object.mode temp_step = new T_Object.mode();
                            //  Устанавлеиваем его атрибуты
                            temp_step.set_attribute( temp_no, temp_name );

                            //  Добавляем в список режимамов
                            current_selected_object.mode_mas[ cur_mode ].step.Add( temp_step );

                            //  Фиксируем номер текущего режима для дальнейших обращений к нему
                            cur_step = temp_no;
                            }
                    	break;

                    case "on_device":
                        try
                            {
                            do
                                {
                                tr.Read();

                                if (    tr.NodeType != XmlNodeType.EndElement
                                    &&  tr.NodeType != XmlNodeType.Whitespace
                                    &&  tr.NodeType != XmlNodeType.Text  )
                                    {
                                    //  Остается только список имен устройств
                                    temp_name = tr.Name;

                                    //  Ищем устройство с прочитанным именем
                                    current_selected_dev = g_devices.Find( delegate( device dev )
                                        {
                                            return dev.get_name() == temp_name;
                                        }
                                        );

                                    //  Если есть такое устройство, то добавляем указатель на него в соотв. список
                                    if (    current_selected_object != null 
                                        &&  current_selected_dev != null )
                                        {
                                        current_selected_object.mode_mas[ cur_mode ].on_device.Add( current_selected_dev );
                                        }    
                                    }
                                }
                            while ( tr.Name != "on_device" );   //  EndElement

                            }
                        catch ( Exception err )
                            {
                            MessageBox.Show( "Ошибка сохраниния описания режимов объектов \n" + err );
                            }
                        break;


                    case "off_device":
                        
                        //tr.ReadElementContentAs();
                        
                        break;

                    case "Parameters":
                        if ( current_selected_object != null )
                            {

                            string[] temp_str = { "0", "0" };

                            do {
                                tr.Read();

                                switch ( tr.NodeType )
                                    {
                                    case XmlNodeType.Element:
                                        temp_str[ 0 ] = tr.Name;
                                        break;

                                    case XmlNodeType.Text:
                                        temp_str[ 1 ] = tr.Value;
                                        current_selected_object.param_list.Add( temp_str );

                                        temp_str = new string[2];
                                        temp_str[ 0 ] = "0";
                                        temp_str[ 1 ] = "0";
                                        break;
                                    }

                                }
                            while ( tr.Name != "Parameters" );   //  EndElement
                            }

                        break;

                    }   //  switch
                }   //  while
            }

        /// <summary> Запись данных класса в файл описания </summary>
        /// <remarks> asvovik, 21.09.2011. </remarks>
        ///
        /// <param name="XML"> I don't now </param>
        public void Write_XML_description( string doc_name )
            {
            string fileName="..\\Visio docs\\Export.xml";

            XmlTextWriter tw = new XmlTextWriter( fileName, null );

            // задаём форматирование с отступом
            tw.Formatting = Formatting.Indented;
            tw.WriteStartDocument();
try
    {
            //Начать создание элементов и атрибутов
            tw.WriteStartElement( "application" );
            tw.WriteAttributeString( "application-name", "wago description" );
                tw.WriteStartElement( "project" );
                tw.WriteAttributeString( "project-name", Application.ActiveDocument.Name );
                    
                for ( int i = 0; i < g_objects.Count; i++ )
                    {
                    tw.WriteStartElement( "object" );
                    tw.WriteAttributeString( "object-name", g_objects[ i ].name );
                    tw.WriteAttributeString( "object-number", Convert.ToString( g_objects[ i ].n ) );

                    foreach ( T_Object.mode temp_mode in g_objects[ i ].mode_mas )
                        {                                                
                        tw.WriteStartElement( "mode" );
                        tw.WriteAttributeString( "mode-name", temp_mode.name );
                        tw.WriteAttributeString( "mode-number", Convert.ToString( temp_mode.no ) );

                        write_device_list( tw, temp_mode );
////  В отдельную функцию...
                        
//                            tw.WriteStartElement( "on_device" );
//                            for ( int k = 1; k <= temp_mode.on_device.Count; k++ )
//                                {
//                                //  Проверяем есть ли это устройство еще на схеме
//                                if ( g_devices.Contains( temp_mode.on_device[ k ] ) ) 
//                                    {
//                                    //  Если есть, то записываем данные в файл
//                                    tw.WriteElementString( "V", temp_mode.on_device[ k ].get_name() );
//                                    }
//                                else 
//                                    {
//                                    //  Если устройства уже нет, то удаляем его из списка
//                                    temp_mode.on_device.Remove( temp_mode.on_device[ k ] );
//                                    }
//                                }
//                            tw.WriteEndElement();

//                            tw.WriteStartElement( "off_device" );
//                            for ( int k = 1; k <= temp_mode.off_device.Count; k++ )
//                                {
//                                //  Проверяем есть ли это устройство еще на схеме
//                                if ( g_devices.Contains( temp_mode.off_device[ k ] ) )
//                                    {
//                                    //  Если есть, то записываем данные в файл
//                                    tw.WriteElementString( "V", temp_mode.off_device[ k ].get_name() );
//                                    }
//                                else
//                                    {
//                                    //  Если устройства уже нет, то удаляем его из списка
//                                    temp_mode.off_device.Remove( temp_mode.off_device[ k ] );
//                                    }
//                                }
//                            tw.WriteEndElement();

//                        //  Проходим по шагам
//                        for ( int k = 0; k < temp_mode.step.Count; k++ )
//                            {
////                            if ( temp_mode.step[ k ] != null )
//                                {
//                                tw.WriteStartElement( "step" );
//                                tw.WriteAttributeString( "step-name", temp_mode.step[ k ].name );
//                                tw.WriteAttributeString( "step-number",Convert.ToString( temp_mode.step[ k ].no ) );
                                
//                                // Вызов отдельной функции для шага ()
//                                //write_device_list( temp_mode.step[ k ] );

//                                tw.WriteEndElement();
//                                }
//                            }
////  ...В отдельную функцию
                        
                        tw.WriteEndElement();                   //  mode
                        }                                   //  for j ...

                    tw.WriteEndElement();                   //  object
                    }
                tw.WriteEndElement();                   //  project
            tw.WriteEndElement();                   //  application
            //-------------------------------------

    }
catch ( Exception err )
    {
    MessageBox.Show( "Ошибка сохраниния описания объектов \n" + err );
    }

            tw.WriteEndDocument();

            // очистить
            tw.Flush();
            tw.Close();

            }


        /// <summary> Запись данных режима в файл описания XML </summary>
        /// <remarks> asvovik, 29.09.2011. </remarks>
        ///
        /// <param name="XML">  </param>
        public void write_device_list( XmlTextWriter tw, T_Object.mode temp_mode )
            {
            tw.WriteStartElement( "on_device" );
            for ( int k = 1; k <= temp_mode.on_device.Count; k++ )
                {
                //  Проверяем есть ли это устройство еще на схеме
                if ( g_devices.Contains( temp_mode.on_device[ k ] ) )
                    {
                    //  Если есть, то записываем данные в файл
                    tw.WriteElementString( "V", temp_mode.on_device[ k ].get_name() );
                    }
                else
                    {
                    //  Если устройства уже нет, то удаляем его из списка
                    temp_mode.on_device.Remove( temp_mode.on_device[ k ] );
                    }
                }
            tw.WriteEndElement();

            tw.WriteStartElement( "off_device" );
            for ( int k = 1; k <= temp_mode.off_device.Count; k++ )
                {
                //  Проверяем есть ли это устройство еще на схеме
                if ( g_devices.Contains( temp_mode.off_device[ k ] ) )
                    {
                    //  Если есть, то записываем данные в файл
                    tw.WriteElementString( "V", temp_mode.off_device[ k ].get_name() );
                    }
                else
                    {
                    //  Если устройства уже нет, то удаляем его из списка
                    temp_mode.off_device.Remove( temp_mode.off_device[ k ] );
                    }
                }
            tw.WriteEndElement();

            //  Проходим по шагам
            if ( temp_mode.step != null )
                {
                for ( int k = 0; k < temp_mode.step.Count; k++ )
                    {
                    tw.WriteStartElement( "step" );
                    tw.WriteAttributeString( "step-name", temp_mode.step[ k ].name );
                    tw.WriteAttributeString( "step-number", Convert.ToString( temp_mode.step[ k ].no ) );

                    // Вызов отдельной функции для шага ()
                    write_device_list( tw, temp_mode.step[ k ] );

                    tw.WriteEndElement();
                    }
                }

            }



        #region VSTO generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InternalStartup()
            {
            this.Startup += new System.EventHandler( visio_addin__startup );
            this.Shutdown += new System.EventHandler( visio_addin__shutdown );
            }

        #endregion
        }
    }
