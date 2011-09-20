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

			foreach	( device dev in Globals.visio_addin.g_devices )
			    {
			    item = LV_devices.Items.Add(  Convert.ToString( dev.get_type() ) );
			    str[ 0 ] = Convert.ToString( dev.get_n() );			//	number
			    str[ 1 ] = dev.get_name();							//	name
			    str[ 2 ] = dev.description;							//	description
			    str[ 3 ] = Convert.ToString( dev.get_sub_type() );	//	sub_type
			    item.SubItems.AddRange( str );
			    }

			LV_devices.Sort(); 

			}

		private void LV_devices_DoubleClick( object sender, EventArgs e )
			{
			//	Выделение соответствующего устройства на схеме
/*			
//			Microsoft.Office.Interop.Visio.Windows visio_wnds = visio_app.Windows;

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
