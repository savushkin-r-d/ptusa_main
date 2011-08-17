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

namespace visio_prj_designer
    {
    public partial class modules_count_enter : Form
        {
        public int modules_count    = 1;
        public string modules_type  = "504";

        public modules_count_enter()
            {
            InitializeComponent();
            }

        private void modules_count_nupdown__ValueChanged( object sender, EventArgs e )
            {
            modules_count = ( int ) ( sender as NumericUpDown ).Value;
            }

        private void modules_type_cbox__TextChanged( object sender, EventArgs e )
            {
            modules_type = ( sender as ComboBox ).Text;
            }

        private void modules_type_cbox_SelectedIndexChanged( object sender, EventArgs e )
            {

            }
        }
    }
