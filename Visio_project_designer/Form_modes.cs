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
        public T_Object OLD_obj;
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
            }

        private void Form_modes_Shown( object sender, EventArgs e )
            {
            this.Text = "Данные объекта " + addin.cur_sel_obj.get_name();

            //  Заполнение списка устройств
            dev_list.Items.Clear();

            Globals.visio_addin.g_devices.Sort( main_ribbon.sort_devices );
            
            foreach	( device dev in Globals.visio_addin.g_devices )
                {
                dev_list.Items.Add( dev.get_name() + ": " + dev.description );
                }
            //-----------------------------------------------------------------------

                        
            //  Создание временной структуры для хранения исходной конфигурации
            OLD_obj = new T_Object( addin.cur_sel_obj.shape, null );
            addin.cur_sel_obj.copy_obj_config_to( ref OLD_obj );

            //  Заполнение списка режимов и шагов для выбранного объекта
            Refresh_mode_tree();

            //  Установка количества таймеров
            Num_timers.Value = addin.cur_sel_obj.timers;

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


            //  Работа с меню (создаем структуру меню на основе структуры характеристик)
            //contextMenuStrip2.Items[ "add_prop" ]

            //-----------------------------------------------------------------------
            
            }
        //---------------------------------------------------------------------

        private void Form_modes_FormClosing( object sender, FormClosingEventArgs e )
            {
            //  Обработка закрытия формы (Сохранять изменения или нет)

            switch ( MessageBox.Show( "Сохранить изменения?", "", MessageBoxButtons.YesNoCancel ) )
                {
                case DialogResult.Yes:
                    //  Сохраняем количество таймеров объекта
                    addin.cur_sel_obj.timers = (int) Num_timers.Value;

                    //  Сохранение временных параметров
                    addin.cur_sel_obj.param_list_temp.Clear();
                    
                    for ( int i = 0; i < numericUpDown1.Value; i++ )
                        {
                        addin.cur_sel_obj.param_list_temp.Add( new string[ 2 ] );

                        if (    dataGridView1.Rows[ i ].Cells[ 1 ].Value == null )
                            {
                            dataGridView1.Rows[ i ].Cells[ 1 ].Value = 
                                "Параметр_" + Convert.ToString( i );
                            }

                        if (    dataGridView1.Rows[ i ].Cells[ 2 ].Value == null )
                            {
                            dataGridView1.Rows[ i ].Cells[ 2 ].Value = "0";
                            }   
                        
                        addin.cur_sel_obj.param_list_temp[ i ][ 0 ] =
                            dataGridView1.Rows[ i ].Cells[ 1 ].Value.ToString().Replace( " ", "_" );
                        addin.cur_sel_obj.param_list_temp[ i ][ 1 ] =
                            dataGridView1.Rows[ i ].Cells[ 2 ].Value.ToString().Replace( " ", "_" );
                        }

                    //  Сохранение сохраняемых параметров
                    addin.cur_sel_obj.param_list_save.Clear();
                    
                    for ( int i = 0; i < numericUpDown2.Value; i++ )
                        {
                        addin.cur_sel_obj.param_list_save.Add( new string[ 2 ] );

                        if (    dataGridView2.Rows[ i ].Cells[ 1 ].Value == null )
                            {
                            dataGridView2.Rows[ i ].Cells[ 1 ].Value =
                                "Параметр_" + Convert.ToString( i );
                            }

                        if (    dataGridView2.Rows[ i ].Cells[ 2 ].Value == null )
                            {
                            dataGridView2.Rows[ i ].Cells[ 2 ].Value = "0";
                            }    
                        
                        addin.cur_sel_obj.param_list_save[ i ][ 0 ] =
                            dataGridView2.Rows[ i ].Cells[ 1 ].Value.ToString().Replace( " ", "_" );
                        addin.cur_sel_obj.param_list_save[ i ][ 1 ] =
                            dataGridView2.Rows[ i ].Cells[ 2 ].Value.ToString().Replace( " ", "_" );
                        }

                    addin.is_selecting_dev = false;
                    break;

                case DialogResult.No:
                    //  Удаляем измененный вариант объекта, меняем его на первоначальный вариант                    
                    addin.g_objects.Insert( addin.g_objects.IndexOf( addin.cur_sel_obj ), OLD_obj );
                    addin.g_objects.Remove( addin.cur_sel_obj );
                    addin.cur_sel_obj = OLD_obj;

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
        //-----------------------------------------------------------------------------------------

        private void treeView_prop_AfterSelect( object sender, TreeViewEventArgs e )
            {
            if ( ((TreeView)sender).SelectedNode.Level == first_lavel )
                {
                if ( //( ((TreeView)sender).SelectedNode.Parent.Text == "Параметры" )
                    ( ((TreeView)sender).SelectedNode.Text == "Блокирующие_режимы_гребенок" )
                ||  ( ((TreeView)sender).SelectedNode.Text == "Блокирующие_режимы_танков" )
                ||  ( ( ( TreeView ) sender ).SelectedNode.Text == "Включить_режимы_танка" )
                ||  ( ( ( TreeView ) sender ).SelectedNode.Text == "Выключить_режимы_танка" )
                ||  ( ( ( TreeView ) sender ).SelectedNode.Text == "Настройка_ожидания" )
                    )
                    {
                    dev_list.Visible = false;
                    panel_prop.Visible = false;
                    treeView_block.Visible = true;

                    //   Заполнение дерева объектов и их режимов (для Ограничений)
                    treeView_block.Nodes.Clear();

                    for ( int i = 0; i < addin.g_objects.Count; i++ )
                        {
                        if ( ( ( ( TreeView ) sender ).SelectedNode.Text == "Блокирующие_режимы_гребенок"
                                && addin.g_objects[ i ].type == device.TYPES.T_GREB )
                        ||
                                ( ( ( ( TreeView ) sender ).SelectedNode.Text == "Блокирующие_режимы_танков"
                                    || ( ( TreeView ) sender ).SelectedNode.Text == "Включить_режимы_танка"
                                    || ( ( TreeView ) sender ).SelectedNode.Text == "Выключить_режимы_танка"
                                    || ( ( TreeView ) sender ).SelectedNode.Text == "Настройка_ожидания"
                                   )
                                && addin.g_objects[ i ].type == device.TYPES.T_TANK )
                           )
                            {
                            //  Добавляем объект (Гребенка или Танк)
                            TreeNode p_node = new TreeNode(
                                Convert.ToString( addin.g_objects[ i ].n ) + "." +
                                addin.g_objects[ i ].get_name() );

                            treeView_block.Nodes.Add( p_node );

                            //  Добавляем режимы объекта
                            for ( int j = 0; j < addin.g_objects[ i ].mode_mas.Count; j++ )
                                {
                                TreeNode t_node = new TreeNode(
                                    Convert.ToString( addin.g_objects[ i ].mode_mas[ j ].no ) + "." +
                                    addin.g_objects[ i ].mode_mas[ j ].name );

                                t_node.ImageIndex = addin.g_objects[ i ].get_n() * 1000 +
                                    addin.g_objects[ i ].mode_mas[ j ].no;

                                //  Проставляем галочки в соответствии со списком режимов
                                for ( int k = 0; k < ( ( TreeView ) sender ).SelectedNode.Nodes.Count; k++ )
                                    {
                                    if ( ( ( TreeView ) sender ).SelectedNode.Nodes[ k ].Text ==
                                        Convert.ToString( t_node.ImageIndex ) )
                                        {
                                        t_node.Checked = true;
                                        p_node.Expand();
                                        break;
                                        }
                                    }   //  for k
                                //-----------------------------------------------------------------------

                                p_node.Nodes.Add( t_node );
                                }   //  for j
                            }
                        }   //  for i
                    //-----------------------------------------------------------------------

                    return;
                    }
/*                
                if (    ( ((TreeView)sender).SelectedNode.Text == "Включить_режимы_танка" )
                    ||  ( ((TreeView)sender).SelectedNode.Text == "Выключить_режимы_танка" )
                    )
                    {
                    dev_list.Visible = false;
                    panel_prop.Visible = false;
                    treeView_block.Visible = true;

                    //   Заполнение дерева объектов и их режимов (для Ограничений)
                    treeView_block.Nodes.Clear();

                    TreeNode p_node = new TreeNode( addin.cur_sel_obj.get_name() );
                    treeView_block.Nodes.Add( p_node );

                    for ( int j = 0; j < addin.cur_sel_obj.mode_mas.Count; j++ )
                        {
                        TreeNode t_node = new TreeNode(
                            Convert.ToString( addin.cur_sel_obj.mode_mas[ j ].no ) + "." +
                            addin.cur_sel_obj.mode_mas[ j ].name );

                        t_node.ImageIndex = addin.cur_sel_obj.get_n() * 1000 +
                            addin.cur_sel_obj.mode_mas[ j ].no;

                        //  Проставляем галочки в соответствии со списком режимов
                        for ( int k = 0; k < ( ( TreeView ) sender ).SelectedNode.Nodes.Count; k++ )
                            {
                            if ( ( ( TreeView ) sender ).SelectedNode.Nodes[ k ].Text ==
                                Convert.ToString( t_node.ImageIndex ) )
                                {
                                t_node.Checked = true;
                                p_node.Expand();
                                break;
                                }
                            }   //  for k

                        treeView_block.Nodes[ 0 ].Nodes.Add( t_node );
                        }   //  for j

                    return;
                    }
*/

                if (    ( ((TreeView)sender).SelectedNode.Text == "Время_работы_режима" )
                    ||  ( ((TreeView)sender).SelectedNode.Text == "Номер_следующего_режима" )
                   )
                    {
                    dev_list.Visible = false;
                    panel_prop.Visible = true;
                    treeView_block.Visible = false;

                    //  Заполнение полей на панели данными
                    if ( ((TreeView)sender).SelectedNode.FirstNode != null )
                        {
                        Num_field.Value = Convert.ToInt32( ((TreeView)sender).SelectedNode.FirstNode.Text );
                        }
                    else
                        {
                        Num_field.Value = 0;
                        }
                                            
                    return;
                    }

                dev_list.Visible = true;
                panel_prop.Visible = false;
                treeView_block.Visible = false;
                return;
                }
            
            //  Для списков, где каждое устройства содержит список зависящих от него устройств
            //      например, клапана, включаемые по сигналу
            if (   ((TreeView)sender).SelectedNode.Level == ( first_lavel + 1 )
                && (    ((TreeView)sender).SelectedNode.Parent.Text == "Управляющие_устройствами_сигналы"
                    ||  ((TreeView)sender).SelectedNode.Parent.Text == "Зависящие_от_устройств_сигналы" )
               )
                {
                dev_list.Visible = true;
                panel_prop.Visible = false;
                treeView_block.Visible = false;
                return;
                }

            dev_list.Visible = false;
            panel_prop.Visible = false;
            treeView_block.Visible = false;
            }
        //-----------------------------------------------------------------------------------------
        
        private void Num_field_ValueChanged( object sender, EventArgs e )
            {
            if ( treeView_prop.SelectedNode.Level == first_lavel )
                {
                if (    ( treeView_prop.SelectedNode.Text == "Время_работы_режима" )
                    ||  ( treeView_prop.SelectedNode.Text == "Номер_следующего_режима" )
                   )
                    {
                    if ( treeView_prop.SelectedNode.FirstNode == null )
                        {
                        treeView_prop.SelectedNode.Nodes.Add( new TreeNode( Convert.ToString( Num_field.Value ) ) );
                        }
                    else
                        {
                        treeView_prop.SelectedNode.FirstNode.Text = Convert.ToString( Num_field.Value );
                        }
                    }
                }
            }
        //-----------------------------------------------------------------------------------------

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

                addin.cur_mode = addin.cur_mode - 1;

                //  Обновляем дерево свойств 
                //      (если этого не сделать перед удаление шага, то унаследуются свойства удаляемого режима)
                Refresh_prop_tree( addin.cur_sel_obj.mode_mas[ addin.cur_mode ] );
                
                treeView_modes.SelectedNode.Remove();
                
                Refresh_mode_tree();
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
            else
                {
                TreeView temp_tree = new TreeView();
                TreeNode temp_node;

                temp_tree.Nodes.Add( "Proporties" );

                //  Синхронизация дерева характеристик
                for ( int i = 0; i < Prop_struct[ 0 ].Nodes.Count; i++ )
                    {
                    Synchronize_nodes(
                        Prop_struct[ 0 ].Nodes[ i ], cur_mode.TreeView_params.Nodes[ 0 ], out temp_node );

                    //  Добавляем получившийся набор характеристик
                    temp_tree.Nodes[ 0 ].Nodes.Add( temp_node );
                    }

                //  Задаем новое дерево характеристик для режима
                cur_mode.TreeView_params.Nodes.Clear();
                for ( int i = 0; i < temp_tree.Nodes.Count; i++ )
                    {
                    cur_mode.TreeView_params.Nodes.Add( (TreeNode) temp_tree.Nodes[ i ].Clone() );
                    }
                }

            //  Заполнение дерева характеристик
            for ( int i = 0; i < cur_mode.TreeView_params.Nodes.Count; i++ )
                {
                //  Корневой элемент
                treeView_prop.Nodes.Add(
                    ( ( TreeNode ) cur_mode.TreeView_params.Nodes[ i ].Clone() ) );
                treeView_prop.Nodes[ i ].Expand();                    

                //  Наборы списков устройств
                //for ( int j = 0; j < cur_mode.TreeView_params.Nodes[ i ].Nodes.Count; j++ )
                for ( int j = cur_mode.TreeView_params.Nodes[ i ].Nodes.Count - 1; j >= 0 ; j-- )
                    {
                    treeView_prop.Nodes[ i ].Nodes[ j ].Expand();
                    
                     //  Списки устройств
                     //for ( int k = 0; k < cur_mode.TreeView_params.Nodes[ i ].Nodes[ j ].Nodes.Count; k++ )
                     for ( int k = cur_mode.TreeView_params.Nodes[ i ].Nodes[ j ].Nodes.Count - 1; k >= 0; k-- )
                         {
                         treeView_prop.Nodes[ i ].Nodes[ j ].Nodes[ k ].Expand();
                         } 
                    }                
                }

            treeView_prop.EndUpdate();

            return cur_mode;
            }
        //---------------------------------------------------------------------

        public void Synchronize_nodes( TreeNode pattern, TreeNode cur_node, out TreeNode res )
            {
            int j;
            TreeNode temp = null;
            
            res = new TreeNode( pattern.Text );

            for ( j = 0; j < pattern.Nodes.Count; j++ )
                {
                temp = Find_Node( cur_node, pattern.Nodes[ j ].Text );
                
                if ( temp != null )
                    {
                    res.Nodes.Add( ( TreeNode ) temp.Clone() );
                    }
                else
                    {
                    res.Nodes.Add( pattern.Nodes[ j ].Text );
                    }
                }

            }
        //---------------------------------------------------------------------

        public TreeNode Find_Node( TreeNode n, string s )
            {
            TreeNode node;

            for ( int i = 0; i < n.Nodes.Count; i++ )
                {
                if ( n.Nodes[ i ].Text == s )
                    {
                    return n.Nodes[ i ];
                    }

                if ( (node = Find_Node( n.Nodes[ i ], s )) != null )
                    {
                    return node;
                    }
                }

            return null;
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

        private void treeView_block_AfterCheck( object sender, TreeViewEventArgs e )
            {
            if (    ( treeView_prop.SelectedNode.Level == first_lavel )
                &&  (      ( treeView_prop.SelectedNode.Text == "Блокирующие_режимы_гребенок" )
                        || ( treeView_prop.SelectedNode.Text == "Блокирующие_режимы_танков" )
                        || ( treeView_prop.SelectedNode.Text == "Включить_режимы_танка" )
                        || ( treeView_prop.SelectedNode.Text == "Выключить_режимы_танка" )
                        || ( treeView_prop.SelectedNode.Text == "Настройка_ожидания" )
                    )
                )
                {
                if ( e.Node.Level == 0 )
                    {
                    //  Значит выбран объект
                    }
                else
                    {
                    //  Значит выбран режим
                    if ( e.Node.Checked == true )
                        {
                        //  Если поставили галочку - добавляем узел
                        //TreeNode temp = ( TreeNode ) e.Node.Clone(); 
                        //temp.Text = Convert.ToString( temp.ImageIndex );
                        treeView_prop.SelectedNode.Nodes.Add( Convert.ToString( e.Node.ImageIndex ) );
                        }
                    else
                        {
                        //  Если сняли галочку
                        for( int i = 0; i < treeView_prop.SelectedNode.Nodes.Count; i++ )
                            {
                            if ( treeView_prop.SelectedNode.Nodes[ i ].Text ==
                                 Convert.ToString( e.Node.ImageIndex ) )
                                {
                                treeView_prop.SelectedNode.Nodes[ i ].Remove();
                                }
                            }
                        }

                    }   //  -выбран режим
                
                }   //  Проверка на соответствие списка
            }

        private void contextMenuStrip2_Opening( object sender, CancelEventArgs e )
            {

            }
        //---------------------------------------------------------------------

        }
    }
