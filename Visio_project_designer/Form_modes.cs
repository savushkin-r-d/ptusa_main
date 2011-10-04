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
        visio_addin addin = Globals.visio_addin;

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

            //  Заполнение списка режимов и шагов для выбранного объекта
            treeView_modes.Nodes.Clear();

            for ( int i = 0; i < addin.cur_sel_obj.mode_mas.Count; i++ )
                {
                TreeNode temp_node = treeView_modes.Nodes.Add( 
                    Convert.ToString( addin.cur_sel_obj.mode_mas[ i ].no ) + 
                    ". " + addin.cur_sel_obj.mode_mas[ i ].name );
                
                for ( int j = 0; j < addin.cur_sel_obj.mode_mas[ i ].step.Count; j++ )
                    {
                    temp_node.Nodes.Add(
//                    treeView_modes.Nodes.Add( 
                        Convert.ToString( addin.cur_sel_obj.mode_mas[ i ].step[ j ].no ) + 
                        ". " + addin.cur_sel_obj.mode_mas[ i ].step[ j ].name );
                    }
                }


            //  Заполнение таблицы с параметрами
            numericUpDown1.Value = addin.cur_sel_obj.param_list.Count;
            //dataGridView1.RowCount = addin.cur_sel_obj.param_list.Count;

            for ( int i = 0; i < addin.cur_sel_obj.param_list.Count; i++ )
                {
                dataGridView1.Rows[ i ].Cells[ 0 ].Value = Convert.ToString( i );
                dataGridView1.Rows[ i ].Cells[ 1 ].Value = addin.cur_sel_obj.param_list[ i ][ 0 ];
                dataGridView1.Rows[ i ].Cells[ 2 ].Value = addin.cur_sel_obj.param_list[ i ][ 1 ];
                }
            }

        private void Form_modes_FormClosing( object sender, FormClosingEventArgs e )
            {
            //  Обработка закрытия формы (Сохранять изменения или нет)

            switch ( MessageBox.Show( "Сохранить изменения?", "", MessageBoxButtons.YesNoCancel ) )
                {
                case DialogResult.Yes:
                    addin.cur_sel_obj.param_list.Clear();

                    for ( int i = 0; i < numericUpDown1.Value; i++ )
                        {
                        addin.cur_sel_obj.param_list.Add( new string[ 2 ] );

                        addin.cur_sel_obj.param_list[ i ][ 0 ] =
                            dataGridView1.Rows[ i ].Cells[ 1 ].Value.ToString();
                        addin.cur_sel_obj.param_list[ i ][ 1 ] =
                            dataGridView1.Rows[ i ].Cells[ 2 ].Value.ToString();
                        }
                    break;

                case DialogResult.No:
                    break;

                case DialogResult.Cancel:
                    e.Cancel = true;
                    break;

                }
            }

        //  Заполнение списков с устройствами
        private void filling_dev_list( T_Object.mode cur_mode )
            {
            listBox_open.Items.Clear();

            if ( cur_mode.on_device != null )
                {
                foreach ( tech_device.device dev in cur_mode.on_device )
                    {
                    if ( addin.g_devices.Contains( dev ) ) 
                        {
                        listBox_open.Items.Add( //"V" + Convert.ToString( dev.get_n() ) + 
                                            dev.get_name() + ": " +
                                            dev.description );
                        }
                    }
                }
            
            listBox_close.Items.Clear();

            if ( cur_mode.off_device != null )
                {
                foreach ( tech_device.device dev in cur_mode.off_device )
                    {
                    if ( addin.g_devices.Contains( dev ) ) 
                        {
                        listBox_close.Items.Add( //"V" + Convert.ToString( dev.get_n() ) + 
                                             dev.get_name() + ": " +
                                             dev.description );
                        }
                    }
                }


            }

        private void dataGridView1_CellEnter( object sender, DataGridViewCellEventArgs e )
            {
            dataGridView1.Rows[ e.RowIndex ].Cells[ 0 ].Value = Convert.ToString( e.RowIndex );
            }

        private void numericUpDown1_ValueChanged( object sender, EventArgs e )
            {
            dataGridView1.RowCount = Convert.ToInt32( numericUpDown1.Value ) + 1;

            for ( int i = 0; i < dataGridView1.RowCount; i++ )
                {
                dataGridView1.Rows[ i ].Cells[ 0 ].Value = Convert.ToString( i );
                }
            }

        private void treeView_modes_AfterSelect( object sender, TreeViewEventArgs e )
            {
            TreeNode cur_node = treeView_modes.SelectedNode;

            addin.cur_mode = 0;
            addin.cur_step = 0;

            //  Определяем режим и шаг
            if ( cur_node.Level == 0 )
                {
                addin.cur_mode = cur_node.Index;
                addin.cur_step = 0;

                //  Заполнение списков с устройствами
                if ( addin.cur_mode < addin.cur_sel_obj.mode_mas.Count )
                    {
                    filling_dev_list( addin.cur_sel_obj.mode_mas[ addin.cur_mode ] );
                    }
                }
            else
                {
                addin.cur_mode = cur_node.Parent.Index;
                addin.cur_step = cur_node.Index;

                //  Заполнение списков с устройствами
                if ( ( addin.cur_mode < addin.cur_sel_obj.mode_mas.Count )
                    && ( addin.cur_step < addin.cur_sel_obj.mode_mas[ addin.cur_mode ].step.Count )
                   )
                    {
                    filling_dev_list( addin.cur_sel_obj.mode_mas[ addin.cur_mode ].step[ addin.cur_step ] );
                    }
                }
            }

        //----------------------------------------------------------------------

        }
    }
