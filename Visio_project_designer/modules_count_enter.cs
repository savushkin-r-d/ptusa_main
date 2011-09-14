/// @file modules_count_enter.cs
/// @brief Форма с вводом параметров модулей ввода\вывода Wago.
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
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

using wago;

namespace visio_prj_designer
    {
    public partial class modules_count_enter : Form
        {
        public int modules_count    = 1;
        public string modules_type  = "504";
		public TreeNode sel_node;


        public modules_count_enter()
            {
            InitializeComponent();
            }

        private void modules_count_nupdown__ValueChanged( object sender, EventArgs e )
            {
            modules_count = ( int ) ( sender as NumericUpDown ).Value;
            }

		private void treeView1_DoubleClick( object sender, EventArgs e )
			{
			//	Добавление выбранного элемента в список модулей за текущим модулем
			//	Если модуль не выбран - добавляем в самое начало
//			TreeNode sel_node = TV_modules_list.SelectedNode;
						
			if ( sel_node.Level == 1 )
				{
				//sel_node.Text;

				}

			//	string j;
			//	wago.io_module.TYPES
				//TV_modules_list.SelectedNode.Name = ;  
				int i = Convert.ToInt32( TV_modules_list.SelectedNode.Name );
				
				//g_PAC.add_io_module( shape );
				//LB_modules.Items.Add( TV_modules_list.SelectedNode.Index );

			}

		private void modules_count_enter_Shown( object sender, EventArgs e )
			{
			//	Очищаем список узлов в дереве доступных модулей
			TV_modules_list.Nodes.Clear();

			//	Заполняем дерево доступных модулей элементами структуры
			TV_modules_list.Nodes.Add( "DI modules" );	//	0-ой узел
			foreach ( KeyValuePair<io_module.TYPES, string> module_type in Globals.visio_addin.service_config.DI_modules )
				{
				TV_modules_list.Nodes[ 0 ].Nodes.Add( module_type.Value );
				TV_modules_list.Nodes[ 0 ].LastNode.Tag = Convert.ToString( Convert.ToInt32( module_type.Key ) );
				}

			TV_modules_list.Nodes.Add( "DO modules" );	//	1-ой узел
			foreach ( KeyValuePair<io_module.TYPES, string> module_type in Globals.visio_addin.service_config.DO_modules )
				{
				TV_modules_list.Nodes[ 1 ].Nodes.Add( module_type.Value );
				TV_modules_list.Nodes[ 1 ].LastNode.Tag = Convert.ToString( Convert.ToInt32( module_type.Key ) );
				}

			TV_modules_list.Nodes.Add( "AI modules" );	//	2-ой узел
			foreach ( KeyValuePair<io_module.TYPES, string> module_type in Globals.visio_addin.service_config.AI_modules )
				{
				TV_modules_list.Nodes[ 2 ].Nodes.Add( module_type.Value );
				TV_modules_list.Nodes[ 2 ].LastNode.Tag = Convert.ToString( Convert.ToInt32( module_type.Key ) );
				}

			TV_modules_list.Nodes.Add( "AO modules" );	//	3-ой узел
			foreach ( KeyValuePair<io_module.TYPES, string> module_type in Globals.visio_addin.service_config.AO_modules )
				{
				TV_modules_list.Nodes[ 3 ].Nodes.Add( module_type.Value );
				TV_modules_list.Nodes[ 3 ].LastNode.Tag = Convert.ToString( Convert.ToInt32( module_type.Key ) );
				}

			TV_modules_list.Nodes.Add( "SYS modules" );	//	4-ой узел
			foreach ( KeyValuePair<io_module.TYPES, string> module_type in Globals.visio_addin.service_config.SYS_modules )
				{
				TV_modules_list.Nodes[ 4 ].Nodes.Add( module_type.Value );
				TV_modules_list.Nodes[ 4 ].LastNode.Tag = Convert.ToString( Convert.ToInt32( module_type.Key ) );
				}

			TV_modules_list.Nodes.Add( "Special modules" );	//	5-ой узел
			foreach ( KeyValuePair<io_module.TYPES, string> module_type in Globals.visio_addin.service_config.Special_modules )
				{
				TV_modules_list.Nodes[ 5 ].Nodes.Add( module_type.Value );
				TV_modules_list.Nodes[ 5 ].LastNode.Tag = Convert.ToString( Convert.ToInt32( module_type.Key ) );
				}
						
			}

		private void TV_modules_list_AfterSelect( object sender, TreeViewEventArgs e )
			{
			sel_node = TV_modules_list.SelectedNode;

			if ( sel_node.Level == 1 )
				{
				modules_type = sel_node.Tag.ToString();
				}
			}

        }
    }
