using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

using wago;
using tech_device;
using visio_prj_designer;
//using Globals;

namespace Visio_project_designer
	{
	public partial class Form_device : Form
		{
		public Form_device()
			{
			InitializeComponent();
			}

		private void LV_devices_SelectedIndexChanged( object sender, EventArgs e )
			{

			}

		private void Form_device_Shown( object sender, EventArgs e )
			{
			LV_devices.Items.Clear();

			//	Заполняем таблицу данными
			System.Windows.Forms.ListViewItem item;
			string[] str = new string[ 4 ];
				//str[ 0 ] = "";	//	number
				//str[ 1 ] = "";	//	name
				//str[ 2 ] = "";	//	description
				//str[ 3 ] = "";	//	sub_type

//			Globals.visio_addin.g_devices.Sort();

            foreach	( device dev in Globals.visio_addin.g_devices )
			    {
			    item = LV_devices.Items.Add( Convert.ToString( dev.get_type() ) );
			    str[ 0 ] = Convert.ToString( dev.get_n() );			//	number
			    str[ 1 ] = dev.get_name();							//	name
			    str[ 2 ] = dev.description;							//	description
			    str[ 3 ] = Convert.ToString( dev.get_sub_type() );	//	sub_type
			    item.SubItems.AddRange( str );
			    }

			//LV_devices.Sort(); 

			}

		private void LV_devices_DoubleClick( object sender, EventArgs e )
			{
            //  Если окно вызвано для добавления устройства в список устройств режима,
            //      то добавляем данное устройство в список, иначе выделяем его на схеме
            ListBox lbox_temp = Globals.visio_addin.vis_main_ribbon.Mode_List_Form.lbox;

            //  Находим выбранное устройство из общего списка
            //Globals.visio_addin.cur_sel_dev = Globals.visio_addin.g_devices.Find( delegate( device dev )
            //    {
            //    return dev.get_n() == temp_no && dev.get_name() == temp_name;
            //    }
            //    );

            if ( lbox_temp != null )
                {
                //  Добавляем найденное устройсто в текущий список
                device dev = Globals.visio_addin.g_devices[ LV_devices.SelectedItems[ 0 ].Index ];
                lbox_temp.Items.Add( dev.get_name() + ": " + dev.description );
                } 
            else
                {
			    //	Выделение соответствующего устройства на схеме
/*			
//  			Microsoft.Office.Interop.Visio.Windows visio_wnds = visio_app.Windows;

    			//	Выбираем окно
			    //visio_wnds[ ( short ) visio_addin.VISIO_WNDOWS.IO_EDIT ].Page =
			    //        visio_app.ActiveDocument.Pages[ "Устройства" ];

    			//visio_wnds[ ( short ) visio_addin.VISIO_WNDOWS.MAIN ].Activate();
					

	    		//Поиск по shape объекта device.
		    	device cur_dev = Globals.visio_addin.g_devices.Find( delegate( device dev )
		    	{
			    	return ( dev.get_name() == LV_devices.SelectedItems[ 0 ].SubItems[ "name" ].Text );
			    }
			    );

			    Microsoft.Office.Interop.Visio.Window window = visio_app.ActiveDocument.Pages[ "Устройства" ];

			    Microsoft.Office.Interop.Visio.Shape selected_shape =  window.Selection[ 1 ];

			    window.Selection[ 1 ] = cur_dev.get_shape();
 */ 
                }
			}

		}
	}
