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
        public visio_addin addin = Globals.visio_addin;
        public ListBox lbox;                //  Удалить
        public TreeNode[] Prop_struct;      //  Стандартная структура характеристик режима
        public TreeNode cur_node;           //  Текущий узел дерева режимов
        
        public const int first_lavel = 2;   // Первый уровень хар-к для которого можно добавлять устр.

        public Form_modes()
            {
            InitializeComponent();

            addin.is_selecting_dev = true;

            //   Заполнение структуры характеристик на основе визуального компонента
            Prop_struct = new TreeNode[ treeView_prop.Nodes.Count ];

            for ( int i = 0; i < treeView_prop.Nodes.Count; i++ )
                {
                Prop_struct[ i ] = treeView_prop.Nodes[ i ];
                }
            //-----------------------------------------------------------------------

            //   Заполнение дерева объектов и их режимов (для Ограничений)
            treeView2.Nodes.Clear();

            for ( int i = 0; i < addin.g_objects.Count; i++ )
                {
                treeView2.Nodes.Add( addin.g_objects[ i ].get_name() );

                for ( int j = 0; j < addin.g_objects[ i ].mode_mas.Count; j++ )
                    {
                    treeView2.Nodes[ i ].Nodes.Add( addin.g_objects[ i ].mode_mas[ j ].name );
                    }
                }
            //-----------------------------------------------------------------------
            }

        private void Form_modes_Shown( object sender, EventArgs e )
            {
            this.Text = "Данные объекта " + addin.cur_sel_obj.get_name();

            //  Заполнение списка устройств
            dev_list.Items.Clear();
            foreach	( device dev in Globals.visio_addin.g_devices )
			    {
                dev_list.Items.Add( dev.get_name() + ": " + dev.description );
                }
            //-----------------------------------------------------------------------

            //  Создание временной структуры для хранения исходной конфигурации
            //T_Object OLD_obj_config = new T_Object( addin.cur_sel_obj.shape, null );
            
            //addin.cur_sel_obj.copy_obj_config_to( ref OLD_obj_config );
            
            //addin.cur_sel_obj.mode_mas[ 0 ].name = "heeeeeeeeelen";
            //addin.cur_sel_obj.mode_mas[ 1 ].name = "billibob";


            //  Заполнение списка режимов и шагов для выбранного объекта
            Refresh_mode_tree();

            //  Установка количества таймеров
            NumUpDown_Timers.Value = addin.cur_sel_obj.timers;

            //  Заполнение таблицы с параметрами
            numericUpDown1.Value = addin.cur_sel_obj.param_list_temp.Count;

            for ( int i = 0; i < addin.cur_sel_obj.param_list_temp.Count; i++ )
                {
                dataGridView1.Rows[ i ].Cells[ 0 ].Value = Convert.ToString( i );
                dataGridView1.Rows[ i ].Cells[ 1 ].Value = addin.cur_sel_obj.param_list_temp[ i ][ 0 ];
                dataGridView1.Rows[ i ].Cells[ 2 ].Value = addin.cur_sel_obj.param_list_temp[ i ][ 1 ];
                }

            numericUpDown2.Value = addin.cur_sel_obj.param_list_save.Count;

            for ( int i = 0; i < addin.cur_sel_obj.param_list_save.Count; i++ )
                {
                dataGridView2.Rows[ i ].Cells[ 0 ].Value = Convert.ToString( i );
                dataGridView2.Rows[ i ].Cells[ 1 ].Value = addin.cur_sel_obj.param_list_save[ i ][ 0 ];
                dataGridView2.Rows[ i ].Cells[ 2 ].Value = addin.cur_sel_obj.param_list_save[ i ][ 1 ];
                }
            //-----------------------------------------------------------------------

            }
        //---------------------------------------------------------------------

        private void Form_modes_FormClosing( object sender, FormClosingEventArgs e )
            {
            //  Обработка закрытия формы (Сохранять изменения или нет)

            switch ( MessageBox.Show( "Сохранить изменения?", "", MessageBoxButtons.YesNoCancel ) )
                {
                case DialogResult.Yes:
/*
                //  Сохранение списка режимов и их характеристик
                    for ( int i = 0; i < treeView_modes.Nodes.Count; i++ )
                        {
                        Save_ModeData( addin.cur_sel_obj.mode_mas[ i ] );

                             
                        //******************
                        //public T_Object.mode Save_ModeData( T_Object.mode cur_mode )

                        cur_mode.no = treeView_modes.Nodes[ i ].Index;
                        cur_mode.name = treeView_modes.Nodes[ i ].Text;

                        for ( int k = 0; k < cur_mode.Nodes.Count; k++ )
                            {                        

                            }
                        
                        for ( int j = 0; j < treeView_modes.Nodes[ i ].Nodes.Count; j++ )
                            {

                            }
                        //******************


                        }
*/
                    //  Сохраняем количество таймеров объекта
                    addin.cur_sel_obj.timers = (int) NumUpDown_Timers.Value;

                    //  Сохранение временных параметров
                    addin.cur_sel_obj.param_list_temp.Clear();
                    
                    for ( int i = 0; i < numericUpDown1.Value; i++ )
                        {
                        addin.cur_sel_obj.param_list_temp.Add( new string[ 2 ] );

                        if ( dataGridView1.Rows[ i ].Cells[ 1 ].Value == null )
                            {
                            dataGridView1.Rows[ i ].Cells[ 1 ].Value = "";
                            }
                        if ( dataGridView1.Rows[ i ].Cells[ 2 ].Value == null )
                            {
                            dataGridView1.Rows[ i ].Cells[ 2 ].Value = "";
                            }   
                        
                        addin.cur_sel_obj.param_list_temp[ i ][ 0 ] =
                            dataGridView1.Rows[ i ].Cells[ 1 ].Value.ToString();
                        addin.cur_sel_obj.param_list_temp[ i ][ 1 ] =
                            dataGridView1.Rows[ i ].Cells[ 2 ].Value.ToString();
                        }

                    //  Сохранение сохраняемых параметров
                    addin.cur_sel_obj.param_list_save.Clear();
                    
                    for ( int i = 0; i < numericUpDown2.Value; i++ )
                        {
                        addin.cur_sel_obj.param_list_save.Add( new string[ 2 ] );

                        if ( dataGridView2.Rows[ i ].Cells[ 1 ].Value == null )
                            {
                            dataGridView2.Rows[ i ].Cells[ 1 ].Value = "";
                            }
                        if ( dataGridView2.Rows[ i ].Cells[ 2 ].Value == null )
                            {
                            dataGridView2.Rows[ i ].Cells[ 2 ].Value = "";
                            }   
                        
                        addin.cur_sel_obj.param_list_save[ i ][ 0 ] =
                            dataGridView2.Rows[ i ].Cells[ 1 ].Value.ToString();
                        addin.cur_sel_obj.param_list_save[ i ][ 1 ] =
                            dataGridView2.Rows[ i ].Cells[ 2 ].Value.ToString();
                        }

                    addin.is_selecting_dev = false;
                    break;

                case DialogResult.No:
                    addin.is_selecting_dev = false;
                    break;

                case DialogResult.Cancel:
                    e.Cancel = true;
                    break;

                }
            }

        //---------------------------------------------------------------------

        private void treeView_modes_MouseClick( object sender, MouseEventArgs e )
            {
            MouseButtons key = e.Button;
            switch ( key )
                {
                case MouseButtons.Left:
                    
                    break;

                case MouseButtons.Right:
                    //contextMenuStrip1.Show( MousePosition.X, MousePosition.Y );

                    break;
                }
            }

        private void treeView_modes_BeforeSelect( object sender, TreeViewCancelEventArgs e )
            {
            //  Работа с предыдущим режимом - Сохранение даннах дерева
            if ( cur_node != null )
                {
                if ( cur_node.Level == 0 )
                    {
                    Save_PropTree(
                        addin.cur_sel_obj.mode_mas[ addin.cur_mode ] );
                    }
                else
                    {
                    Save_PropTree(
                        addin.cur_sel_obj.mode_mas[ addin.cur_mode ].step[ addin.cur_step ] );
                    }
                }    
            }
        //---------------------------------------------------------------------

        private void treeView_modes_BeforeLabelEdit( object sender, NodeLabelEditEventArgs e )
            {

            //  Преобразовать надпись - оставить имя без номера
            if ( ( ( TreeView ) sender ).SelectedNode.Level == 0 )
                {   //  Режим
                if ( e.Node.Text != addin.cur_sel_obj.mode_mas[ e.Node.Index ].name )
                    {
                    e.Node.EndEdit( false );
                    e.Node.Text = addin.cur_sel_obj.mode_mas[ e.Node.Index ].name;
                    }
                }
            else
                {   //  Шаг
                if ( e.Node.Text != 
                        addin.cur_sel_obj.mode_mas[ e.Node.Parent.Index ].step[ e.Node.Index ].name )
                    {
                    e.Node.EndEdit( false );
                    e.Node.Text = 
                        addin.cur_sel_obj.mode_mas[ e.Node.Parent.Index ].step[ e.Node.Index ].name;
                    }                
                
                }            
            }
        //---------------------------------------------------------------------

        private void treeView_modes_AfterLabelEdit( object sender, NodeLabelEditEventArgs e )
            {

            if ( e.Label != null )
                {
                //e.Node.BeginEdit();

                //  Переименовываем режим в структуре
                if ( ( ( TreeView ) sender ).SelectedNode.Level == 0 )
                    {   //  Режим
                    addin.cur_sel_obj.
                        mode_mas[ e.Node.Index ].
                        set_attribute( e.Node.Index, e.Label );
                    }
                else
                    {   //  Шаг
                    addin.cur_sel_obj.
                        mode_mas[ ( ( TreeView ) sender ).SelectedNode.Parent.Index ].
                        step[ e.Node.Index ].
                        set_attribute( e.Node.Index, e.Label );
                    }

                //e.Node.EndEdit( true );

                //  Преобразовываем название режима в дереве
                //Refresh_mode_tree();
                e.Node.Text = e.Node.Index + ". " + e.Label;
                e.Node.EndEdit( false );

                //e.Node.Text.Substring( 5 );
                //string.Format( {0, 3} - {1}, e.index, e.Node.Text );
                }
            }
                                                                       
        //---------------------------------------------------------------------

        private void treeView_modes_AfterSelect( object sender, TreeViewEventArgs e )
            {

            //e.Node.Text = cur_mode.name;

            //  Преобразовать надпись - оставить имя без номера
            //if ( ( ( TreeView ) sender ).SelectedNode.Level == 0 )
            //    {   //  Режим
            //    e.Node.Text = addin.cur_sel_obj.mode_mas[ e.Node.Index ].name;
            //    }
            //else
            //    {   //  Шаг
            //    e.Node.Text = addin.cur_sel_obj.
            //        mode_mas[ ( ( TreeView ) sender ).SelectedNode.Parent.Index ].
            //        step[ e.Node.Index ].name;
            //    }
            //-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

            //  Работа с выбранным режимом
            cur_node = treeView_modes.SelectedNode;

            addin.cur_mode = -1;    //0
            addin.cur_step = -1;    //0

            //  Определяем режим и шаг
            if ( cur_node.Level == 0 )
                {
                addin.cur_mode = cur_node.Index;
                addin.cur_step = -1;
                //cur_oper = addin.cur_sel_obj.mode_mas[ addin.cur_mode ];

                //  Заполнение списков с устройствами
                if ( addin.cur_mode < addin.cur_sel_obj.mode_mas.Count )
                    {
                    //filling_dev_list( addin.cur_sel_obj.mode_mas[ addin.cur_mode ] );
                    addin.cur_sel_obj.mode_mas[ addin.cur_mode ] =
                    Refresh_prop_tree( addin.cur_sel_obj.mode_mas[ addin.cur_mode ] );
                    }
                }
            else
                {
                addin.cur_mode = cur_node.Parent.Index;
                addin.cur_step = cur_node.Index;
                //cur_oper = addin.cur_sel_obj.mode_mas[ addin.cur_mode ].step[ addin.cur_step ];

                //  Заполнение списков с устройствами
                if ( ( addin.cur_mode < addin.cur_sel_obj.mode_mas.Count )
                    && ( addin.cur_step < addin.cur_sel_obj.mode_mas[ addin.cur_mode ].step.Count )
                   )
                    {
                    //filling_dev_list( addin.cur_sel_obj.mode_mas[ addin.cur_mode ].step[ addin.cur_step ] );
                    addin.cur_sel_obj.mode_mas[ addin.cur_mode ].step[ addin.cur_step ] =
                    Refresh_prop_tree( addin.cur_sel_obj.mode_mas[ addin.cur_mode ].step[ addin.cur_step ] );
                    }
                }


            //  Выполнить проверку необходимости показывать все виды характеристик
            //      если ни одна хар-ка в группе не задана - снять галочку (в соотв. меню)
            //  *****
            /*            
                        bool flag_no_prop;      // флаг необходимости отображения данного типа хар-к 
                        //  Проходим по типам характеристик
                        for ( int i = 0; i < treeView_prop.Nodes.Count; i++ )
                            {
                            //  Выделяем текущий узел
                            treeView_prop.SelectedNode = treeView_prop.Nodes[ i ];
                
                            flag_no_prop = false;
                
                            //  Проходим по характеристикам
                            for ( int j = 0; j < treeView_prop.Nodes[ i ].Nodes.Count; j++ )
                                {
                                //  Если в характеристике задан хотябы один элемент
                                if ( treeView_prop.Nodes[ i ].Nodes[ j ].Nodes.Count > 0 )
                                    {
                                    flag_no_prop = true;    //  То устанавливаем флаг
                                    break;
                                    }
                                }

                            //  Находим соответствующий узлу дерева пункт меню  
                            ToolStripItem qwer = contextMenuStrip2.Items.Find( 
                                                            treeView_prop.Nodes[ i ].Name, true )[ 0 ];
                
                            if ( flag_no_prop == true )
                                {
                                ((ToolStripMenuItem ) qwer).Checked = true;
                                }
                            else
                                {
                                ((ToolStripMenuItem ) qwer).Checked = false;
                                }

                            }
            */

            }
        //---------------------------------------------------------------------

        private void treeView_prop_MouseClick( object sender, MouseEventArgs e )
            {
            MouseButtons key = e.Button;
            switch ( key )
                {
                case MouseButtons.Left:
                    break;

                case MouseButtons.Right:
                    if ( ( ( TreeView ) sender ).SelectedNode.Level < first_lavel )
                        {
                        contextMenuStrip2.Show( MousePosition.X, MousePosition.Y );
                        }
                    else
                        {
                        contextMenuStrip4.Show( MousePosition.X, MousePosition.Y );
                        }
                    break;
                }
            }

        private void treeView_prop_AfterSelect( object sender, TreeViewEventArgs e )
            {
            if ( ( ( ( TreeView ) sender ).SelectedNode.Level == first_lavel )
              && ( ( ( TreeView ) sender ).SelectedNode.Parent.Text == "Ограничения" )
               )
                {
                treeView2.Visible = true;
                }
            else
                {
                treeView2.Visible = false;
                }
            }

        private void treeView_prop_KeyDown( object sender, KeyEventArgs e )
            {
            switch ( e.KeyCode )
                {
                case Keys.Delete:
                    if ( ( ( ( TreeView ) sender ).SelectedNode != null )
                        && ( ( ( TreeView ) sender ).SelectedNode.Level > first_lavel )
                        )
                        {
                        ( ( TreeView ) sender ).SelectedNode.Remove();
                        }
                    break;

                case Keys.Insert:
                    break;

                case Keys.Enter:
                    break;
                }
            }

        //---------------------------------------------------------------------

        private void dev_list_DoubleClick( object sender, EventArgs e )
            {
            device cur_dev = addin.g_devices.Find( delegate( device dev )
                {
                    return ( ( dev.get_name() + ": " + dev.description ) ==
                             ( dev_list.SelectedItem.ToString() ) );
                }
            );

            if ( cur_dev != null )
                add_dev_in_tree( cur_dev );
            }
        //---------------------------------------------------------------------

        private void ContextMenuStrip2_set_def_prop_Click( object sender, EventArgs e )
            {
            tech_device.mode cur_mode = new tech_device.mode();

            if ( addin.cur_step >= 0 )
                {
                cur_mode = addin.cur_sel_obj.mode_mas[ addin.cur_mode ].step[ addin.cur_step ];
                }
            else
                {
                cur_mode = addin.cur_sel_obj.mode_mas[ addin.cur_mode ];
                }

            //  Если нет, то задаем стандартную структуру
            cur_mode.TreeView_params.Nodes.Clear();

            //  Если структура пустая, то забиваем ее стандартными пунктами
            for ( int i = 0; i < Prop_struct.Length; i++ )
                {
                cur_mode.TreeView_params.Nodes.Add( Prop_struct[ i ] );
                }

            cur_mode = Refresh_prop_tree( cur_mode );
            }

        //---------------------------------------------------------------------

        private void ContextMenuStrip3_insert_mode_Click( object sender, EventArgs e )
            {
            int index = 0;

            //  Создаем режим
            mode new_mode = new mode();

            //  Добавляем режим в структуру
            if ( treeView_modes.SelectedNode != null ) 
                {
                if ( treeView_modes.SelectedNode.Level == 0 )
                    {
                    index = treeView_modes.SelectedNode.Index + 1;
                    //  Задаем значения
                    new_mode.set_attribute( index, "Новый режим" );
                    //  Добавляем в структуру
                    addin.cur_sel_obj.mode_mas.Insert( index, new_mode );
                    }
                }
            else
                {
                //  Задаем значения
                new_mode.set_attribute( index, "Новый режим" );
                //  Добавляем в структуру
                addin.cur_sel_obj.mode_mas.Insert( index, new_mode );
                }

            new_mode = null;

            //  Обновляем дерево
            Refresh_mode_tree();
            }
        //---------------------------------------------------------------------

        private void ContextMenuStrip3_insert_step_Click( object sender, EventArgs e )
            {
            //  Создаем режим
            mode new_mode = new mode();

            //  Добавляем режим в структуру
            if ( treeView_modes.SelectedNode != null )
                {
                if ( treeView_modes.SelectedNode.Level == 0 )
                    {
                    //  0-ой шаг, т.к. при обновлении будет присвоен нужный номер
                    new_mode.set_attribute( 0, "Новый шаг" );

                    //  Добавляем в структуру
                    addin.cur_sel_obj.mode_mas[ treeView_modes.SelectedNode.Index ].step.Add( new_mode );
                    }
                else
                    {
                    //  0-ой шаг, т.к. при обновлении будет присвоен нужный номер
                    new_mode.set_attribute( 0, "Новый шаг" );

                    //  Добавляем в структуру
                    addin.cur_sel_obj.mode_mas[ treeView_modes.SelectedNode.Parent.Index ].step.Insert(
                                                treeView_modes.SelectedNode.Index + 1, new_mode );
                    }
                }
            else
                {
                MessageBox.Show( "Выделенный объект не соответствует операции!" );
                }    

            new_mode = null;

            //  Обновляем дерево
            Refresh_mode_tree();
            }
        //---------------------------------------------------------------------

        private void ContextMenuStrip3_delete_mode_Click( object sender, EventArgs e )
            {
            if ( treeView_modes.SelectedNode != null )
                {
                if ( treeView_modes.SelectedNode.Level == 0 )
                    {
                    addin.cur_sel_obj.mode_mas.Remove( addin.cur_sel_obj.mode_mas[ addin.cur_mode ] );
                    }
                else
                    {
                    addin.cur_sel_obj.mode_mas[ addin.cur_mode ].step.Remove(
                                    addin.cur_sel_obj.mode_mas[ addin.cur_mode ].step[ addin.cur_step ] );
                    }

                //  Обновляем дерево
                treeView_modes.SelectedNode.Remove();
                }
            else
                {
                MessageBox.Show( "Выделенный объект не соответствует операции!" );
                }    
            }
        //---------------------------------------------------------------------

        private void ContextMenuStrip3_update_Click( object sender, EventArgs e )
            {
            Refresh_mode_tree();
            }
        //---------------------------------------------------------------------

        private void ContextMenuStrip4_select_on_map_Click( object sender, EventArgs e )
            {
            //  Переход на схему и последующий выбор
            Globals.visio_addin.vis_main_ribbon.insert_dev_list();
            }
        //---------------------------------------------------------------------

        public void add_dev_in_tree( device dev )
            {
            //  В текущую выделенную характеристику добавляем выбранное устройство
            if ( ( treeView_prop.SelectedNode != null )
              && ( treeView_prop.SelectedNode.Level >= first_lavel ) )
                {
                treeView_prop.SelectedNode.Nodes.Add( new TreeNode( dev.get_name() ) );
                treeView_prop.SelectedNode.Expand();
                }
            else
                {
                MessageBox.Show( "Не выбран список для заполнения устройствами!" );
                //Globals.visio_addin.vis_main_ribbon.insert_dev_list();
                //window.Selection[ 1 ] = cur_dev.get_shape();
                }
            }
        //---------------------------------------------------------------------

        private void numericUpDown1_ValueChanged( object sender, EventArgs e )
            {
            dataGridView1.RowCount = Convert.ToInt32( numericUpDown1.Value ) + 1;

            for ( int i = 0; i < dataGridView1.RowCount; i++ )
                {
                dataGridView1.Rows[ i ].Cells[ 0 ].Value = Convert.ToString( i );
                }
            }
        //---------------------------------------------------------------------

        private void numericUpDown2_ValueChanged( object sender, EventArgs e )
            {
            dataGridView2.RowCount = Convert.ToInt32( numericUpDown2.Value ) + 1;

            for ( int i = 0; i < dataGridView2.RowCount; i++ )
                {
                dataGridView2.Rows[ i ].Cells[ 0 ].Value = Convert.ToString( i );
                }
            }

        //---------------------------------------------------------------------

        private void dataGridView1_RowsAdded( object sender, DataGridViewRowsAddedEventArgs e )
            {
            numericUpDown1.Value = dataGridView1.RowCount - 1;
            }

        private void dataGridView1_KeyDown( object sender, KeyEventArgs e )
            {
            switch ( e.KeyCode )
                {
                case Keys.Delete:
                    //  Удаляет автоматически текущую строку
                    break;

                case Keys.Insert:
                    if ( dataGridView1.CurrentRow != null )
                        {
                        //  Увеличиваем количество параметров
                        numericUpDown1.Value = dataGridView1.RowCount;
                        //  Вставляем новый параметр
                        dataGridView1.Rows.Insert( dataGridView1.CurrentRow.Index, 1 );
                        }
                    break;
                }
            }

        private void dataGridView2_RowsAdded( object sender, DataGridViewRowsAddedEventArgs e )
            {
            numericUpDown2.Value = dataGridView2.RowCount - 1;
            }

        private void dataGridView2_KeyDown( object sender, KeyEventArgs e )
            {
            switch ( e.KeyCode )
                {
                case Keys.Delete:
                    //  Удаляет автоматически текущую строку
                    break;

                case Keys.Insert:
                    if ( dataGridView2.CurrentRow != null )
                        {
                        //  Увеличиваем количество параметров
                        numericUpDown2.Value = dataGridView2.RowCount;
                        //  Вставляем новый параметр
                        dataGridView2.Rows.Insert( dataGridView2.CurrentRow.Index, 1 );
                        }
                    break;
                }
            }

        //---------------------------------------------------------------------

        //  Заполнение списков с устройствами
        private void filling_dev_list( mode cur_mode )
            {
            //  Для работы с деревьями

/*          //  Для работы со списками
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

*/
            }
        //---------------------------------------------------------------------

        private void Refresh_mode_tree()
            {
            TreeNode new_node;

            treeView_modes.BeginUpdate();
            treeView_modes.Nodes.Clear();

            //  Добавляем режимы
            for ( int i = 0; i < addin.cur_sel_obj.mode_mas.Count; i++ )
                {
                addin.cur_sel_obj.mode_mas[ i ].set_attribute( i, null );

                new_node = treeView_modes.Nodes.Add( string.Format( "{0,3} - {1}",
                    addin.cur_sel_obj.mode_mas[ i ].no,
                    addin.cur_sel_obj.mode_mas[ i ].name ) );

                //  Добавляем шаги
                for ( int j = 0; j < addin.cur_sel_obj.mode_mas[ i ].step.Count; j++ )
                    {
                    addin.cur_sel_obj.mode_mas[ i ].step[ j ].set_attribute( j, null );

                    new_node.Nodes.Add( string.Format( "{0,3} - {1}",
                        addin.cur_sel_obj.mode_mas[ i ].step[ j ].no,
                        addin.cur_sel_obj.mode_mas[ i ].step[ j ].name ) );
                    }
                
                treeView_modes.Nodes[ i ].Expand();
                
                new_node = null;
                }

            treeView_modes.EndUpdate();

            //new_node = null;
            }
        //---------------------------------------------------------------------

        private mode Refresh_prop_tree( mode cur_mode )
            {
            treeView_prop.BeginUpdate();
            treeView_prop.Nodes.Clear();

            //  Перед заполнением дерева проверяем есть ли структура дерева характ-к в режиме
            //  Если нет, то задаем стандартную структуру
            if ( cur_mode.TreeView_params == null )
                {
                cur_mode.TreeView_params = new TreeView();
                }

            //  Если структура пустая, то забиваем ее стандартными пунктами
            if ( cur_mode.TreeView_params.Nodes.Count == 0 )
                {
                for ( int i = 0; i < Prop_struct.Length; i++ )
                    {
                    cur_mode.TreeView_params.Nodes.Add( Prop_struct[ i ] );
                    }
                }

            //  Заполнение дерева характеристик
            for ( int i = 0; i < cur_mode.TreeView_params.Nodes.Count; i++ )
                {
                treeView_prop.Nodes.Add(
                    ( ( TreeNode ) cur_mode.TreeView_params.Nodes[ i ].Clone() ) );
                treeView_prop.Nodes[ i ].Expand();                    

                for ( int j = 0; j < cur_mode.TreeView_params.Nodes[ i ].Nodes.Count; j++ )
                    {
                    treeView_prop.Nodes[ i ].Nodes[ j ].Expand();
                    }                
                }

            treeView_prop.EndUpdate();

            return cur_mode;
            }
        //---------------------------------------------------------------------

        private void Save_PropTree( mode cur_mode )
            {
            if ( cur_mode.TreeView_params != null )
                {
                cur_mode.TreeView_params.Nodes.Clear();

                for ( int i = 0; i < treeView_prop.Nodes.Count; i++ )
                    {
                    cur_mode.TreeView_params.Nodes.Add(
                        ( ( TreeNode ) treeView_prop.Nodes[ i ].Clone() ) );
                    }
                }
            }

        //---------------------------------------------------------------------

        private void ToolStripMenuItem_CheckStateChanged( object sender, EventArgs e )
            {
/*
            int i;

            //  Определяем какой узел выбран
            if ( treeView_prop.SelectedNode != null )
                {
                i = treeView_prop.SelectedNode.Index;

                //  В зависимости от выбора добавляем или удаляем характеристики в дерево
                if ( ( ( ToolStripMenuItem ) sender ).Checked == true )
                    {
                    treeView_prop.Nodes.Insert( i, Prop_struct[ i ] );
                    }
                else
                    {
                    treeView_prop.Nodes.Remove( Prop_struct[ i ] );
                    }
                }
*/
            }
        //---------------------------------------------------------------------
        
        private void listView_modes_SelectedIndexChanged( object sender, EventArgs e )
            {
            //  Заполнение списков устройств

            }

        private void label5_Click( object sender, EventArgs e )
            {

            }

        //---------------------------------------------------------------------

        }
    }
