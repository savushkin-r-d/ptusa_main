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
    public partial class Form_modes : Form
        {
        public Form_modes()
            {
            InitializeComponent();
            }

        private void listView_modes_SelectedIndexChanged( object sender, EventArgs e )
            {
            //  Заполнение списков устройств

            }
                 
        private void Form_modes_Shown( object sender, EventArgs e )
            {
            //System.Windows.Forms.ListViewItem item;

            //  Заполнение списка режимов для выбранного объекта
            treeView_modes.Nodes.Clear();

            //for ( int i = 0; i < Globals.visio_addin.current_selected_object.mode_mas.Count; i++ )
            //    {
            //    treeView_modes.Nodes.Add( 
            //        Convert.ToString( Globals.visio_addin.current_selected_object.mode_mas[ i ].no ) + 
            //        ". " + Globals.visio_addin.current_selected_object.mode_mas[ i ].name );
                
            //    for ( int j = 0; j < Globals.visio_addin.current_selected_object.mode_mas[ i ].step.Count; j++ )
            //        {
            //        treeView_modes.Nodes.Add( 
            //            Convert.ToString( Globals.visio_addin.current_selected_object.mode_mas[ i ].step[ j ].no ) + 
            //            ". " + Globals.visio_addin.current_selected_object.mode_mas[ i ].step[ j ].name );
            //        }
            //    }


            //  Заполнение таблицы с параметрами

            
            numericUpDown1.Value = Globals.visio_addin.current_selected_object.param_list.Count;
            //dataGridView1.RowCount = Globals.visio_addin.current_selected_object.param_list.Count;

            for ( int i = 0; i < Globals.visio_addin.current_selected_object.param_list.Count; i++ )
                {
                dataGridView1.Rows[ i ].Cells[ 0 ].Value = Convert.ToString( i );
                dataGridView1.Rows[ i ].Cells[ 1 ].Value = Globals.visio_addin.current_selected_object.param_list[ 0 ][ 0 ];
                dataGridView1.Rows[ i ].Cells[ 2 ].Value = Globals.visio_addin.current_selected_object.param_list[ 0 ][ 1 ];
                }
            }


        //  Заполнение списков с устройствами
        private void filling_dev_list( T_Object.mode cur_mode )
            {

            if ( cur_mode.on_device != null )
                {
                foreach ( tech_device.device dev in cur_mode.on_device )
                    {
                    if ( Globals.visio_addin.g_devices.Contains( dev ) ) 
                        {
                        listBox_open.Items.Add( //"V" + Convert.ToString( dev.get_n() ) + 
                                            dev.get_name() + ": " +
                                            dev.description );
                        }
                    }
                }
            
            if ( cur_mode.off_device != null )
                {
                foreach ( tech_device.device dev in cur_mode.off_device )
                    {
                    if ( Globals.visio_addin.g_devices.Contains( dev ) ) 
                        {
                        listBox_close.Items.Add( //"V" + Convert.ToString( dev.get_n() ) + 
                                             dev.get_name() + ": " +
                                             dev.description );
                        }
                    }
                }


            }

        private void treeView_modes_Click( object sender, EventArgs e )
            {
            TreeNode cur_node = treeView_modes.SelectedNode;
            Globals.visio_addin.cur_mode = 0;
            Globals.visio_addin.cur_step = 0;

            //  Определяем режим и шаг
            if ( cur_node.Level == 0 )
                {
                Globals.visio_addin.cur_mode = cur_node.Index;
                Globals.visio_addin.cur_step = 0;
                
                //  Заполнение списков с устройствами
                filling_dev_list( Globals.visio_addin.current_selected_object.mode_mas[ 
                    Globals.visio_addin.cur_mode ] );
                }
            else
                {
                Globals.visio_addin.cur_mode = cur_node.Parent.Index;
                Globals.visio_addin.cur_step = cur_node.Index;

                //  Заполнение списков с устройствами
                filling_dev_list( Globals.visio_addin.current_selected_object.mode_mas[ 
                    Globals.visio_addin.cur_mode ].step[ Globals.visio_addin.cur_step ] );
                }
            }

        private void dataGridView1_CellEnter( object sender, DataGridViewCellEventArgs e )
            {
            dataGridView1.Rows[ e.RowIndex ].Cells[ 0 ].Value = Convert.ToString( e.RowIndex );
            }

        private void Form_modes_FormClosing( object sender, FormClosingEventArgs e )
            {
            //  Обработка закрытия формы (Сохранять изменения или нет)

            }

        private void numericUpDown1_ValueChanged( object sender, EventArgs e )
            {
            dataGridView1.RowCount = Convert.ToInt32( numericUpDown1.Value );

            for ( int i = 0; i < dataGridView1.RowCount; i++ )
                {
                dataGridView1.Rows[ i ].Cells[ 0 ].Value = Convert.ToString( i );
                }
            }

        //----------------------------------------------------------------------

        }
    }
