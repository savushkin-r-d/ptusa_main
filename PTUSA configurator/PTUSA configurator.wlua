-- ----------------------------------------------------------------------------
require( "wx" )
-- ----------------------------------------------------------------------------
-- Тестовая конфигурация.
g_project =
	{
	name 	= "Выдача сыворотки",

	editor_version = 12,
	file_version   = 13,

	--Узлы WAGO
	nodes =
		{
			{
			ntype   = 0,
			address = 1,
			modules =
				{
				{ 466, '', '', '', '', '' },
				{ 466, '', '', '', '', '' },
				{ 402, '', '', '', '', '' },
				{ 402, '', '', '', '', '' },
				{ 402, '', '', '', '', '' },
				{ 402, '', '', '', '', '' },
				{ 504, '', '', '', '', '' },
				{ 504, '', '', '', '', '' },
				{ 504, '', '', '', '', '' },
				{ 504, '', '', '', '', '' },
				{ 504, '', '', '', '', '' },
				{ 504, '', '', '', '', '' },
				{ 504, '', '', '', '', '' },
				{ 504, '', '', '', '', '' },
				{ 504, '', '', '', '', '' },
				{ 638, '', '', '', '', '' },
				{ 461, '', '', '', '', '' },
				{ 461, '', '', '', '', '' },
				{ 461, '', '', '', '', '' },
				{ 612, '', '', '', '', '' },
				{ 512, '', '', '', '', '' },
				{ 512, '', '', '', '', '' },
				{ 602, '', '', '', '', '' },
				{ 402, '', '', '', '', '' },
				{ 600, '', '', '', '', '' },
				}
			},
		},

	--Устройства
	devices =
		{
			{
			descr   = 'Счетчик №1',
			dtype   = 7,
			subtype = 0,
			number  = 1,
			AI  =
				{
					sys_ = {
					node   = 0,
					offset = 4
					},

					{
					node   = 0,
					offset = 4,
					klemma = 2
					}
				},
			},

			{
			descr   = 'Счетчик №2',
			dtype   = 7,
			subtype = 0,
			number  = 2,
			AI  =
				{
					{
					node   = 0,
					offset = 5
					}
				},
			},

			{
			descr   = 'Текущий уровень 1',
			dtype   = 9,
			subtype = 0,
			number  = 1,
			AI  =
				{
					{
					node   = 0,
					offset = 0
					}
				},
			},

			{
			descr   = 'Текущий уровень 2',
			dtype   = 9,
			subtype = 0,
			number  = 2,
			AI  =
				{
					{
					node   = 0,
					offset = 1
					}
				},
			},

			{
			descr   = 'Текущий уровень 3',
			dtype   = 9,
			subtype = 0,
			number  = 3,
			AI  =
				{
					{
					node   = 0,
					offset = 2
					}
				},
			},

			{
			descr   = 'Текущий уровень 4',
			dtype   = 9,
			subtype = 0,
			number  = 4,
			AI  =
				{
					{
					node   = 0,
					offset = 3
					}
				},
			},

			{
			descr   = '1LS1-Н.У. Танк № 1',
			dtype   = 3,
			subtype = 0,
			number  = 101,
			DI  =
				{
					{
					node   = 0,
					offset = 1
					}
				},
			},

			{
			descr   = '1LS2-В.У. Танк № 1',
			dtype   = 3,
			subtype = 0,
			number  = 102,
			DI  =
				{
					{
					node   = 0,
					offset = 0
					}
				},
			},

			{
			descr   = '2LS1-Н.У. Танк № 2',
			dtype   = 3,
			subtype = 0,
			number  = 201,
			DI  =
				{
					{
					node   = 0,
					offset = 3
					}
				},
			},

			{
			descr   = '2LS2-В.У. Танк № 2',
			dtype   = 3,
			subtype = 0,
			number  = 202,
			DI  =
				{
					{
					node   = 0,
					offset = 2
					}
				},
			},

			{
			descr   = '3LS1-Н.У. Танк № 3',
			dtype   = 3,
			subtype = 0,
			number  = 301,
			DI  =
				{
					{
					node   = 0,
					offset = 5
					}
				},
			},

			{
			descr   = '3LS2-В.У. Танк № 3',
			dtype   = 3,
			subtype = 0,
			number  = 302,
			DI  =
				{
					{
					node   = 0,
					offset = 4
					}
				},
			},

			{
			descr   = '4LS1-Н.У. Танк № 4',
			dtype   = 3,
			subtype = 0,
			number  = 401,
			DI  =
				{
					{
					node   = 0,
					offset = 7
					}
				},
			},

			{
			descr   = '4LS2-2.У. Танк № 4',
			dtype   = 3,
			subtype = 0,
			number  = 402,
			DI  =
				{
					{
					node   = 0,
					offset = 6
					}
				},
			},

			{
			descr   = 'N1-Насос выдачи №1',
			dtype   = 1,
			subtype = 0,
			number  = 1,
			DO  =
				{
					{
					node   = 0,
					offset = 36
					}
				},
			DI  =
				{
					{
					node   = 0,
					offset = 8
					}
				},
			},

			{
			descr   = 'N-Насос выдачи №2',
			dtype   = 1,
			subtype = 0,
			number  = 2,
			DO  =
				{
					{
					node   = 0,
					offset = 37
					}
				},
			DI  =
				{
					{
					node   = 0,
					offset = 10
					}
				},
			},

			{
			descr   = '22N2 -Возврат мойки',
			dtype   = 1,
			subtype = 0,
			number  = 2202,
			DO  =
				{
					{
					node   = 0,
					offset = 38
					}
				},
			DI  =
				{
					{
					node   = 0,
					offset = 9
					}
				},
			},

			{
			descr   = 'SB1 старт выдача 1',
			dtype   = 10,
			subtype = 0,
			number  = 1,
			DI  =
				{
					{
					node   = 0,
					offset = 12
					}
				},
			},

			{
			descr   = 'SB2 стоп выдача 11',
			dtype   = 10,
			subtype = 0,
			number  = 2,
			DI  =
				{
					{
					node   = 0,
					offset = 14
					}
				},
			},

			{
			descr   = 'SB3 старт выдачи 2',
			dtype   = 10,
			subtype = 0,
			number  = 3,
			DI  =
				{
					{
					node   = 0,
					offset = 13
					}
				},
			},

			{
			descr   = 'SB4 стоп выдачи 2',
			dtype   = 10,
			subtype = 0,
			number  = 4,
			DI  =
				{
					{
					node   = 0,
					offset = 15
					}
				},
			},

			{
			descr   = 'Есть поток выдача 1',
			dtype   = 10,
			subtype = 0,
			number  = 5,
			DI  =
				{
					{
					node   = 0,
					offset = 16
					}
				},
			},

			{
			descr   = 'Есть поток выдача 2',
			dtype   = 10,
			subtype = 0,
			number  = 6,
			DI  =
				{
					{
					node   = 0,
					offset = 18
					}
				},
			},

			{
			descr   = 'Температура Т. №1',
			dtype   = 4,
			subtype = 0,
			number  = 1,
			AI  =
				{
					{
					node   = 0,
					offset = 6
					}
				},
			},

			{
			descr   = 'Температура Т. №2',
			dtype   = 4,
			subtype = 0,
			number  = 2,
			AI  =
				{
					{
					node   = 0,
					offset = 7
					}
				},
			},

			{
			descr   = 'Температура Т. №3',
			dtype   = 4,
			subtype = 0,
			number  = 3,
			AI  =
				{
					{
					node   = 0,
					offset = 8
					}
				},
			},

			{
			descr   = 'Температура Т. №4',
			dtype   = 4,
			subtype = 0,
			number  = 4,
			AI  =
				{
					{
					node   = 0,
					offset = 9
					}
				},
			},

			{
			descr   = 'темп. выдачи 1даче 1',
			dtype   = 4,
			subtype = 0,
			number  = 5,
			AI  =
				{
					{
					node   = 0,
					offset = 10
					}
				},
			},

			{
			descr   = 'темп. выдачи 2даче 1',
			dtype   = 4,
			subtype = 0,
			number  = 6,
			AI  =
				{
					{
					node   = 0,
					offset = 11
					}
				},
			},

			{
			descr   = 'HL1 готов к выдаче 1',
			dtype   = 11,
			subtype = 0,
			number  = 1,
			DO  =
				{
					{
					node   = 0,
					offset = 32
					}
				},
			},

			{
			descr   = 'HL2 выдача идет 1е 2',
			dtype   = 11,
			subtype = 0,
			number  = 2,
			DO  =
				{
					{
					node   = 0,
					offset = 34
					}
				},
			},

			{
			descr   = 'HL3 готов к выдаче 2',
			dtype   = 11,
			subtype = 0,
			number  = 3,
			DO  =
				{
					{
					node   = 0,
					offset = 33
					}
				},
			},

			{
			descr   = 'HL4 идет выдача 2',
			dtype   = 11,
			subtype = 0,
			number  = 4,
			DO  =
				{
					{
					node   = 0,
					offset = 35
					}
				},
			},

			{
			descr   = 'V1- Запорный пар',
			dtype   = 0,
			subtype = 1,
			number  = 1,
			DO  =
				{
					{
					node   = 0,
					offset = 28
					}
				},
			par = { 0 }
			},

			{
			descr   = 'V2- Запорный пар',
			dtype   = 0,
			subtype = 1,
			number  = 2,
			DO  =
				{
					{
					node   = 0,
					offset = 30
					}
				},
			par = { 0 }
			},

			{
			descr   = '1V1-Донный Т. №1',
			dtype   = 0,
			subtype = 1,
			number  = 101,
			DO  =
				{
					{
					node   = 0,
					offset = 0
					}
				},
			par = { 0 }
			},

			{
			descr   = '1V2-Наполнение Т.№1',
			dtype   = 0,
			subtype = 1,
			number  = 102,
			DO  =
				{
					{
					node   = 0,
					offset = 2
					}
				},
			par = { 0 }
			},

			{
			descr   = '1V3-Выдача  Т.№1',
			dtype   = 0,
			subtype = 1,
			number  = 103,
			DO  =
				{
					{
					node   = 0,
					offset = 1
					}
				},
			par = { 0 }
			},

			{
			descr   = '1V4- Выдача  Т.№1',
			dtype   = 0,
			subtype = 1,
			number  = 104,
			DO  =
				{
					{
					node   = 0,
					offset = 3
					}
				},
			par = { 0 }
			},

			{
			descr   = '1V5-Мойка  Т.№1',
			dtype   = 0,
			subtype = 1,
			number  = 105,
			DO  =
				{
					{
					node   = 0,
					offset = 4
					}
				},
			par = { 0 }
			},

			{
			descr   = '1V6-Запорный Л.сыво',
			dtype   = 0,
			subtype = 1,
			number  = 106,
			DO  =
				{
					{
					node   = 0,
					offset = 6
					}
				},
			par = { 0 }
			},

			{
			descr   = '1V7-Зпорный Л.Выдачи',
			dtype   = 0,
			subtype = 1,
			number  = 107,
			DO  =
				{
					{
					node   = 0,
					offset = 5
					}
				},
			par = { 0 }
			},

			{
			descr   = '1V8-Вид. разр. Л.сыв',
			dtype   = 0,
			subtype = 1,
			number  = 108,
			DO  =
				{
					{
					node   = 0,
					offset = 7
					}
				},
			par = { 0 }
			},

			{
			descr   = '1V9-Запорный Л.мойки',
			dtype   = 0,
			subtype = 1,
			number  = 109,
			DO  =
				{
					{
					node   = 0,
					offset = 29
					}
				},
			par = { 0 }
			},

			{
			descr   = '2V1-Донный Т.№2',
			dtype   = 0,
			subtype = 1,
			number  = 201,
			DO  =
				{
					{
					node   = 0,
					offset = 8
					}
				},
			par = { 0 }
			},

			{
			descr   = '2V2-Наполнение  Т.№2',
			dtype   = 0,
			subtype = 1,
			number  = 202,
			DO  =
				{
					{
					node   = 0,
					offset = 10
					}
				},
			par = { 0 }
			},

			{
			descr   = '2V3-  Выдача  Т.№2',
			dtype   = 0,
			subtype = 1,
			number  = 203,
			DO  =
				{
					{
					node   = 0,
					offset = 9
					}
				},
			par = { 0 }
			},

			{
			descr   = '2V4- Выдача  Т.№2',
			dtype   = 0,
			subtype = 1,
			number  = 204,
			DO  =
				{
					{
					node   = 0,
					offset = 11
					}
				},
			par = { 0 }
			},

			{
			descr   = '2V5-Мойка  Т.№2',
			dtype   = 0,
			subtype = 1,
			number  = 205,
			DO  =
				{
					{
					node   = 0,
					offset = 12
					}
				},
			par = { 0 }
			},

			{
			descr   = '2V6-Выход мойки танк',
			dtype   = 0,
			subtype = 1,
			number  = 206,
			DO  =
				{
					{
					node   = 0,
					offset = 14
					}
				},
			par = { 0 }
			},

			{
			descr   = '2V7-Зпорный Л.Выдачи',
			dtype   = 0,
			subtype = 1,
			number  = 207,
			DO  =
				{
					{
					node   = 0,
					offset = 13
					}
				},
			par = { 0 }
			},

			{
			descr   = '2V8- Вид.разрыв мойк',
			dtype   = 0,
			subtype = 1,
			number  = 208,
			DO  =
				{
					{
					node   = 0,
					offset = 15
					}
				},
			par = { 0 }
			},

			{
			descr   = '3V1-Донный Т.№3',
			dtype   = 0,
			subtype = 1,
			number  = 301,
			DO  =
				{
					{
					node   = 0,
					offset = 16
					}
				},
			par = { 0 }
			},

			{
			descr   = '3V2-Наполнение  Т.№3',
			dtype   = 0,
			subtype = 1,
			number  = 302,
			DO  =
				{
					{
					node   = 0,
					offset = 18
					}
				},
			par = { 0 }
			},

			{
			descr   = '3V3-  Выдача  Т.№3',
			dtype   = 0,
			subtype = 1,
			number  = 303,
			DO  =
				{
					{
					node   = 0,
					offset = 17
					}
				},
			par = { 0 }
			},

			{
			descr   = '3V4- Выдача  Т.№3',
			dtype   = 0,
			subtype = 1,
			number  = 304,
			DO  =
				{
					{
					node   = 0,
					offset = 19
					}
				},
			par = { 0 }
			},

			{
			descr   = '3V5-Мойка  Т.№3',
			dtype   = 0,
			subtype = 1,
			number  = 305,
			DO  =
				{
					{
					node   = 0,
					offset = 20
					}
				},
			par = { 0 }
			},

			{
			descr   = '3V7-Зпорный Л.Выдачи',
			dtype   = 0,
			subtype = 1,
			number  = 307,
			DO  =
				{
					{
					node   = 0,
					offset = 22
					}
				},
			par = { 0 }
			},

			{
			descr   = '4V1-Донный Т.№4',
			dtype   = 0,
			subtype = 1,
			number  = 401,
			DO  =
				{
					{
					node   = 0,
					offset = 21
					}
				},
			par = { 0 }
			},

			{
			descr   = '4V2-Наполнение  Т.№7',
			dtype   = 0,
			subtype = 1,
			number  = 402,
			DO  =
				{
					{
					node   = 0,
					offset = 23
					}
				},
			par = { 0 }
			},

			{
			descr   = '4V3-  Выдача  Т.№4',
			dtype   = 0,
			subtype = 1,
			number  = 403,
			DO  =
				{
					{
					node   = 0,
					offset = 24
					}
				},
			par = { 0 }
			},

			{
			descr   = '4V4- Выдача  Т.№4',
			dtype   = 0,
			subtype = 1,
			number  = 404,
			DO  =
				{
					{
					node   = 0,
					offset = 26
					}
				},
			par = { 0 }
			},

			{
			descr   = '4V5-Мойка  Т.№4',
			dtype   = 0,
			subtype = 1,
			number  = 405,
			DO  =
				{
					{
					node   = 0,
					offset = 25
					}
				},
			par = { 0 }
			},

			{
			descr   = '5V7-Зпорный Л.Выдачи',
			dtype   = 0,
			subtype = 1,
			number  = 507,
			DO  =
				{
					{
					node   = 0,
					offset = 27
					}
				},
			par = { 0 }
			},

		},

	grebenka =
		{
		get_name = function( self )
			return "Гребенка"
		end,

		number 			= 1,
		param_cnt 		= 20,
		work_param_cnt	= 10,
		tmr_cnt 		= 5,

		modes =
			{
			get_name = function( self )
				return "Режимы"
			end,

				{
				name = 'Мойка линии подачи',

				get_name = function( self )
					return self.name
				end,


				V_open =
				{
				get_name = function( self )
					return "V_open"
				end,
				 { 106 }, {108},{109}
				},

				V_close =
				{
				get_name = function( self )
					return "V_close"
				end,

				106, 108, 109
				}
				}
			}
		}
	}

-- ----------------------------------------------------------------------------
g_prev_activ_item  	 = nil
g_data			  	 = {}
g_selected_tree_item = nil
-- ----------------------------------------------------------------------------
-- Сохранение таблицы.
local function save_table_to_file( tbl, table_name, filename )

	local function save_table( tbl, str, table_name, tab_str )
		--Проверка на массив.
		local is_array = false
		for fields, value in pairs( tbl ) do
			if type( value ) == "table" and
			string.sub( fields, 1, 4 ) ~= "sys_" then
				is_array = true
				break
			end
		end
		if is_array == false then
			str = str..tab_str..'{ '

			for fields, value in pairs( tbl ) do
				if not ( type( value ) == "function" or
					type( value ) == "userdata" or
					string.sub( fields, 1, 4 ) == "sys_" ) then

					local field_value = value
					if type( value ) == "string" then
						field_value = "'"..field_value.."'"
					end

					local field_name = ''
					if type( fields ) == "string" then
						field_name = fields.." = "
					end

					str = str..field_name..field_value..', '

				end
			end

			str = string.sub( str, 1, string.len( str ) - 2 )
			return str..' },\n'
		end

		str = str..tab_str..table_name..' = \n'..tab_str..'\t{\n'

		for fields, value in pairs( tbl ) do
			if not ( type( value ) == "function" or
				type( value ) == "userdata" or
				string.sub( fields, 1, 4 ) == "sys_" ) then

				local field_name = fields
				if type( field_name ) == "number" then
					field_name = "["..field_name.."]"
				end

				local field_value = value
				if type( value ) == "string" then
					field_value = "'"..field_value.."'"
				end

				if type( fields ) == "string" and type( value ) ~= "table" then
					str = str..tab_str..'\t'..field_name..' = '..field_value..',\n'
				end

				if type( fields ) == "number" and type( value ) ~= "table" then
					str = str..tab_str..'\t'..field_value..',\n '
				end

				if type( value ) == "table" then
					str = save_table( value, str, field_name, tab_str..'\t' )
				end
			end
		end

		str = string.sub( str, 1, string.len( str ) - 2 )


		str = str..'\n'..tab_str..'\t},\n\n'

		return str
	end

	local str = ''
	str = save_table( tbl, str, table_name, '' )
	--Удаляем лишнюю запятую.
	str = string.sub( str, 1, string.len( str ) - 3 )

	local file = io.open( filename, "w+" )
	file:write( str )
	io.close( file )
end
-- ----------------------------------------------------------------------------
-- Отображение таблицы в виде дерева.
local function update_tree_veiw( tbl, tree, level, parent_property_name )

	local root_id
	if level == nil then
		root_id = tree:AddRoot( tbl:get_name() )

	else
		local node_name

		if tbl.get_name ~= nil then
			node_name = tbl:get_name()
		else
			node_name = "?"

			if type( tbl[ 1 ] ) == "number" then
				node_name = string.format( "%d", tbl[ 1 ] )
			end

			if type( tbl[ 1 ] ) == "string" then
				node_name = tbl[ 1 ]
			end

		end

		root_id = tree:AppendItem( level, node_name )

	end

	g_data[ root_id:GetValue() ] = tbl
	g_data[ root_id:GetValue() ].sys_parent_property_name =
		parent_property_name

	if tbl.sys_selected ~= nil then
		g_selected_tree_item = root_id
		tbl.sys_selected = nil
	end

    for fields, value in pairs( tbl ) do
		if type( value ) == "table" and
		not ( type( fields ) == "string" and
		string.sub( fields, 1, 4 ) == "sys_" ) then
			update_tree_veiw( value, tree, root_id, fields )
		end
    end

end
-- ----------------------------------------------------------------------------
-- ----------------------------------------------------------------------------
local function add_visual_behavior( project )
	--Отображение проекта.
	project.get_name = function( self )
		return self.name
	end

	--Отображение всех узлов Wago.
	project.nodes.get_name = function( self )
		return "Узлы Wago ("..#self..")"
	end

	--Отображение каждого узла Wago.
	for fields, value in pairs( project.nodes ) do
		if type( value ) == "table" then

			value.sys_parent_property_name = fields
			value.get_name = function( self )
				return self.sys_parent_property_name..". ".."Узел Wago"
			end

			--Отображение каждого модуля Wago.
			for fields, value in pairs( value ) do
				if fields == "modules" then

					value.get_name = function( self )
						return "Модули ("..#self..")"
					end

					for fields, value in pairs( value ) do
						if type( value ) == "table" then

							value.get_name = function( self )
								return string.format( "%d", self[ 1 ] )
							end
						end

					end

				end

			end

		end
	end

	--Отображение всех устройств.
	project.devices.get_name = function( self )
		return "Устройства ("..#self..")"
	end

	--Отображение каждого устройства.
	for fields, value in pairs( project.devices ) do
		if type( value ) == "table" then

			value.sys_parent_property_name = fields
			value.get_name = function( self )
				return value.sys_parent_property_name..'. '..self.descr
			end

			for fields, value in pairs( value ) do
				if type( value ) == "table" then
					value.get_name = function( self )
						return fields..' ('..#value..')'
						end

					for fields, value in pairs( value ) do
						if type( value ) == "table" then
							value.get_name = function( self )
								return self.node..'.'..self.offset
								end
						end
					end
				end
			end
		end

	end
end
-- ----------------------------------------------------------------------------
-- ----------------------------------------------------------------------------
-- Create the main menu and attach it to the frame.
local function open_main_nodes( tree )

	local root = tree:GetRootItem()
	tree:Expand( root )

	local cookie = 0
	local child = g_tree:GetFirstChild( root, cookie )
	while child:IsOk() == true do
		tree:Expand( child )

--[[		local sub_cookie = 0
		local sub_child = g_tree:GetFirstChild( child, sub_cookie )

		while sub_child:IsOk() == true do
			tree:Expand( sub_child )
			sub_child = tree:GetNextSibling( sub_child )
		end]]

		child = tree:GetNextSibling( child )
	end
end
-- ----------------------------------------------------------------------------
-- Create the main menu and attach it to the frame.
local function create_main_menu( frame )

	local function MItem( menu, id, text, help, bmp )
        local m = wx.wxMenuItem( menu, id, text, help )
        m:SetBitmap( bmp )
        bmp:delete()
        return m
    end

    local fileMenu = wx.wxMenu()
	fileMenu:Append( wx.wxID_NEW, "N&ew\tCtrl+N", "New description" )
	fileMenu:Append( MItem( fileMenu, wx.wxID_OPEN,
		"O&pen\tCtrl+O", "Open description",
		wx.wxArtProvider.GetBitmap( wx.wxART_FILE_OPEN, wx.wxART_TOOLBAR ) ) )
	fileMenu:Append( wx.wxID_SAVE, "S&ave\tCtrl+S", "Save description" )
	fileMenu:Append( wx.wxID_CLOSE, "C&lose\tCtrl+X", "Close description" )

	fileMenu:AppendSeparator()
	fileMenu:Append( wx.wxID_EXIT, "E&xit\tAlt+F4", "Quit the program" )
    local helpMenu = wx.wxMenu()
    helpMenu:Append(wx.wxID_ABOUT, "&About",
		"About the wxLua wxTreeCtrl Sample")

    local menuBar = wx.wxMenuBar()
    menuBar:Append( fileMenu, "&File" )
    menuBar:Append( helpMenu, "&Help" )

    frame:SetMenuBar( menuBar )

	local main_icon_data =
		{
		--/* columns rows colors chars-per-pixel */
		"32 32 6 1",
		"  c black",
		". c navy",
		"X c red",
		"o c yellow",
		"O c gray100",
		"+ c None",
		--/* pixels */
		"++++++++++++++++++++++++++++++++",
		"++++++++++++++++++++++++++++++++",
		"++++++++++++++++++++++++++++++++",
		"++++++++++++++++++++++++++++++++",
		"++++++++++++++++++++++++++++++++",
		"++++++++              ++++++++++",
		"++++++++ ............ ++++++++++",
		"++++++++ ............ ++++++++++",
		"++++++++ .OO......... ++++++++++",
		"++++++++ .OO......... ++++++++++",
		"++++++++ .OO......... ++++++++++",
		"++++++++ .OO......              ",
		"++++++++ .OO...... oooooooooooo ",
		"         .OO...... oooooooooooo ",
		" XXXXXXX .OO...... oOOooooooooo ",
		"++++++++++++++++++++++++++++++++",
		" XOOXXXX ......... oOOooooooooo ",
		" XOOXXXX ......... oOOooooooooo ",
		" XOOXXXX           oOOooooooooo ",
		" XOOXXXXXXXXX ++++ oOOooooooooo ",
		" XOOXXXXXXXXX ++++ oOOooooooooo ",
		" XOOXXXXXXXXX ++++ oOOooooooooo ",
		" XOOXXXXXXXXX ++++ oooooooooooo ",
		" XOOXXXXXXXXX ++++ oooooooooooo ",
		" XXXXXXXXXXXX ++++              ",
		" XXXXXXXXXXXX ++++++++++++++++++",
		"              ++++++++++++++++++",
		"++++++++++++++++++++++++++++++++",
		"++++++++++++++++++++++++++++++++",
		"++++++++++++++++++++++++++++++++",
		"++++++++++++++++++++++++++++++++",
		"++++++++++++++++++++++++++++++++"
		}

	local icon = wx.wxIcon()
	icon:CopyFromBitmap( wx.wxBitmap( main_icon_data ) )

	frame:SetIcon( icon )
	icon:delete()

    -- Connect the selection event of the exit menu item to an
    -- event handler that closes the window.
    frame:Connect( wx.wxID_EXIT, wx.wxEVT_COMMAND_MENU_SELECTED,
        function ( event )
            frame:Close( true )
        end )

    -- Connect the selection event of the about menu item.
    frame:Connect( wx.wxID_ABOUT, wx.wxEVT_COMMAND_MENU_SELECTED,
        function ( event )
            wx.wxMessageBox( 'Приложение для конфигурирования проекта.\n'..
				"Реализовано на "..wxlua.wxLUA_VERSION_STRING..
				' и '..wx.wxVERSION_STRING..'.',
				"О программе", wx.wxOK + wx.wxICON_INFORMATION, frame )
        end )

	--
    frame:Connect( wx.wxID_SAVE, wx.wxEVT_COMMAND_MENU_SELECTED,
        function ( event )
            local filename = wx.wxFileSelector( 'Choose a file to save',
				'', 'test.prj.lua', 'prj.lua',
				"Project description (*.prj.lua)|*.prj.lua", wx.wxFD_SAVE +
				wx.wxFD_OVERWRITE_PROMPT )

			if filename == '' then
				return
			end

			save_table_to_file( g_project, "g_project", filename )
			add_visual_behavior( g_project )
        end )

	--
    frame:Connect( wx.wxID_OPEN, wx.wxEVT_COMMAND_MENU_SELECTED,
        function ( event )
            local filename = wx.wxFileSelector( 'Choose a file to open',
				'', 'test.prj.lua', 'prj.lua',
				"Project description (*.prj.lua)|*.prj.lua", wx.wxFD_OPEN +
				wx.wxFD_FILE_MUST_EXIST )

			if filename == '' then
				return
			end

			dofile( filename )

			g_tree:DeleteAllItems()

			add_visual_behavior( g_project )
			update_tree_veiw( g_project, g_tree )

			open_main_nodes( g_tree )
        end )

	--
    frame:Connect( wx.wxID_CLOSE, wx.wxEVT_COMMAND_MENU_SELECTED,
        function ( event )
			tree:DeleteAllItems()

			g_project =
				{
				name = "Новый проект",

				nodes = {}
				}

			add_visual_behavior( g_project )
			update_tree_veiw( g_project, g_tree )
        end )

end
-- ----------------------------------------------------------------------------
-- Create the tree control and attach it to the frame.
local function init_tree_control( tree, panel )
	tree:Connect( wx.wxEVT_COMMAND_TREE_KEY_DOWN,
		function( event )
			--print( event:GetKeyCode() )

			--Insert key.
			if event:GetKeyCode() == 322 then

				if g_selected_object == nil then
					return
				end

				local owner_tree_object = tree:GetItemParent(
					tree:GetSelection() )
				local owner_object = g_data[ owner_tree_object:GetValue() ]
				local insert_index = g_selected_object.sys_parent_property_name

				--Добавляем новый элемент.
				owner_object[ #owner_object + 1 ] = {}
				for i = #owner_object, insert_index + 1, -1 do

					for fields, value in pairs( owner_object[ i - 1 ] ) do
						owner_object[ i ][ fields ] =
						owner_object[ i - 1 ][ fields ]
					end

					owner_object[ i ].sys_parent_property_name = i
				end

				owner_object[ insert_index + 1 ].sys_selected = true

				tree:DeleteAllItems()
				update_tree_veiw( g_project, tree )
				tree:SelectItem( g_selected_tree_item )

			end

			--Delete key.
			if event:GetKeyCode() == 127 then

				if g_selected_object == nil then
					return
				end

				local owner_tree_object = tree:GetItemParent(
					tree:GetSelection() )
				local owner_object = g_data[ owner_tree_object:GetValue() ]
				local delete_index = g_selected_object.sys_parent_property_name

				--Удаляем поле, индексируемое числом.
				if type( delete_index ) == "number" then
					for i = delete_index, #owner_object - 1 do
							owner_object[ i ] = owner_object[ i + 1 ]
						end

					owner_object[ #owner_object ] = nil

					if owner_object[ delete_index ] ~= nil then
						owner_object[ delete_index ].sys_selected = true
					else
						owner_object.sys_selected = true
					end

				--Удаляем поле, индексируемое свойством.
				else
					owner_object[ delete_index ] = nil
					owner_object.sys_selected = true
				end

				tree:Hide()
				tree:DeleteAllItems()
				update_tree_veiw( g_project, tree )
				tree:SelectItem( g_selected_tree_item )
				tree:Show( true )
				tree:SetFocus()
			end
		end )

    tree:Connect( wx.wxEVT_COMMAND_TREE_SEL_CHANGED,
        function( event )
			panel:Hide()

            local item_id = event:GetItem()
			local object = g_data[ item_id:GetValue() ]

			g_selected_object = object

			if g_prev_activ_item ~= nil and
				g_prev_activ_item.sys_static_text ~= nil then

				for i = 1, #g_prev_activ_item.sys_static_text do

					g_prev_activ_item.sys_static_text[ i ]:Destroy()
					g_prev_activ_item.sys_static_text[ i ] = nil

					g_prev_activ_item.sys_text[ i ]:Destroy()
					g_prev_activ_item.sys_text[ i ] = nil
				end

                g_prev_activ_item.flex_grid_sizer = nil
			end

			--Отображаем все поля.
			index = 1
			object.sys_static_text = {}
			object.sys_text = {}

			for fields, value in pairs( object ) do

				if ( type( value ) == "string" or
					type( value ) == "number" ) and
					not ( type( fields ) == "string" and
					string.sub( fields, 1, 4 ) == "sys_" ) then

				    local par_name = fields
				    if type( fields ) == "number" then
						par_name = string.format( "[ %d ]", fields )
					end

					local par_value = value
				    if type( par_value ) == "number" then
						par_value = string.format( "%d", par_value )
					end

					object.sys_static_text[ index ] = wx.wxStaticText( panel,
						wx.wxID_ANY, par_name )
					object.sys_text[ index ] = wx.wxTextCtrl( panel, 10 ,
						par_value, wx.wxDefaultPosition, wx.wxDefaultSize,
						wx.wxTE_PROCESS_ENTER )

					object.sys_text[ index ]:Connect( 10,
						wx.wxEVT_COMMAND_TEXT_ENTER,
						function( event )

						object[ fields ] = event:GetString()

						if object.get_name ~= nil then
							tree:SetItemText( tree:GetSelection(),
								object:get_name() )
						end

					end )

					index = index + 1
				end

			end

			if index > 1 then
				object.flex_grid_sizer = wx.wxFlexGridSizer( 2, 2, 5, 5 )
				object.flex_grid_sizer:AddGrowableCol( 1 )

				for i = 1, index - 1 do
					object.flex_grid_sizer:Add( object.sys_static_text[ i ], 0,
						wx.wxALIGN_CENTER_VERTICAL + wx.wxALL, 5 )
					object.flex_grid_sizer:Add( object.sys_text[ i ], 0,
						wx.wxGROW + wx.wxALIGN_CENTER_VERTICAL + wx.wxALL, 5 )
				end

				panel:SetSizer( object.flex_grid_sizer )
			end

			panel:Layout() -- help sizing the windows before being shown
			panel:Show( true )

			g_prev_activ_item = object
		end)
end
-- ----------------------------------------------------------------------------
local function main()
    g_frame = wx.wxFrame( wx.NULL, wx.wxID_ANY, "PTUSA: конфигурация проекта",
                        wx.wxDefaultPosition, wx.wxSize( 850, 700 ),
                        wx.wxDEFAULT_FRAME_STYLE )

	create_main_menu( g_frame )

    local splitter = wx.wxSplitterWindow( g_frame, wx.wxID_ANY )
    splitter:SetMinimumPaneSize( 50 ) -- don't let it unsplit
    splitter:SetSashGravity( .3 )

    -- create our treectrl
    g_tree = wx.wxTreeCtrl( splitter, wx.wxID_ANY,
                          wx.wxDefaultPosition, wx.wxSize( -1, 500 ),
                          wx.wxTR_LINES_AT_ROOT + wx.wxTR_HAS_BUTTONS )
	g_panel = wx.wxPanel( splitter, wx.wxID_ANY )
	init_tree_control( g_tree, g_panel )

    splitter:SplitVertically( g_tree, g_panel, 200 )

    g_frame:Layout() -- help sizing the windows before being shown

	add_visual_behavior( g_project )
	update_tree_veiw( g_project, g_tree )

	g_selected_object = nil

	wx.wxGetApp():SetTopWindow( g_frame )
    g_frame:Show( true )
end
-- ----------------------------------------------------------------------------
main()

-- Call wx.wxGetApp():MainLoop() last to start the wxWidgets event loop,
-- otherwise the wxLua program will exit immediately.
-- Does nothing if running from wxLua, wxLuaFreeze, or wxLuaEdit since the
-- MainLoop is already running or will be started by the C++ program.

--[[local root = g_tree:GetRootItem()
g_tree:Expand( root )

local cookie = 1
local child = g_tree:GetFirstChild( root, cookie )
g_tree:Expand( child )]]

open_main_nodes( g_tree )

wx.wxGetApp():MainLoop()
