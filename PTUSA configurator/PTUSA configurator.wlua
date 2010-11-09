-- ----------------------------------------------------------------------------
require( "wx" )
-- ----------------------------------------------------------------------------
-- Тестовая конфигурация.
project =
	{
	name 	= "Выдача сыворотки",

	G_NODES =
        {
            {
            ntype   = 0,
            address = 1,
			comment = "Базовый узел",
            modules =
                {
                { 466, '', '', '', '', '',  },
                { 466, '', '', '', '', '',  },
                { 402, '', '', '', '', '',  },
                { 402, '', '', '', '', '',  },
                { 402, '', '', '', '', '',  },
                { 402, '', '', '', '', '',  },
                { 504, '', '', '', '', '',  },
                { 504, '', '', '', '', '',  },
                { 504, '', '', '', '', '',  },
                { 504, '', '', '', '', '',  },
                { 504, '', '', '', '', '',  },
                { 504, '', '', '', '', '',  },
                { 504, '', '', '', '', '',  },
                { 504, '', '', '', '', '',  },
                { 504, '', '', '', '', '',  },
                { 638, '', '', '', '', '',  },
                { 461, '', '', '', '', '',  },
                { 461, '', '', '', '', '',  },
                { 461, '', '', '', '', '',  },
                { 612, '', '', '', '', '',  },
                { 512, '', '', '', '', '',  },
                { 512, '', '', '', '', '',  },
                { 602, '', '', '', '', '',  },
                { 402, '', '', '', '', '',  },
                { 600, '', '', '', '', '',  },
                }
            },

            {
            ntype   = 0,
            address = 2,

			comment = "В аппаратном узел",

            modules =
                {
				{ 600, '', '', '', '', '',  },
                }
            },

        },

    }
-- ----------------------------------------------------------------------------
g_prev_activ_item  	 = nil
g_data			  	 = {}
g_selected_tree_item = nil
-- ----------------------------------------------------------------------------
-- Сохранение таблицы.
local function save_table_to_file( tbl, table_name, filename )

	local function delete_sys_info( tbl )
		for fields, value in pairs( tbl ) do
			if type( value ) == "function" or
				type( value ) == "userdata" or
				string.sub( fields, 1, 4 ) == "sys_" then

				tbl[ fields ] = nil
			end

			if type( value ) == "table" then
				delete_sys_info( value )
			end
		end

	end

	local function save_table( tbl, str, table_name, tab_str )
		--Проверка на массив.
		local is_array = false
		for fields, value in pairs( tbl ) do
			if type( value ) == "table" then
				is_array = true
				break
			end
		end
		if is_array == false then
			str = str..tab_str..'{ '

			for fields, value in pairs( tbl ) do
				local field_value = value
				if type( value ) == "string" then
					field_value = "'"..field_value.."'"
				end

			str = str..field_value..', '
			end

			str = string.sub( str, 1, string.len( str ) - 2 )
			return str..' },\n'
		end

		str = str..tab_str..table_name..' = \n'..tab_str..'\t{\n'

		for fields, value in pairs( tbl ) do

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

		str = string.sub( str, 1, string.len( str ) - 2 )


		str = str..'\n'..tab_str..'\t},\n\n'

		return str
	end

	delete_sys_info( tbl )

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
local function create_tree_veiw( tbl, tree, level, parent_property_name )

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
			create_tree_veiw( value, tree, root_id, fields )
		end
    end

end
-- ----------------------------------------------------------------------------
-- ----------------------------------------------------------------------------
function add_visual_behavior( project )
	--Отображение проекта.
	project.get_name = function( self )
		return self.name
	end

	--Отображение всех узлов Wago.
	project.G_NODES.get_name = function( self )
		return "Узлы Wago ("..#self..")"
	end

	--Отображение каждого узла Wago.
	for fields, value in pairs( project.G_NODES ) do
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
end
-- ----------------------------------------------------------------------------
-- ----------------------------------------------------------------------------
function main()
    frame = wx.wxFrame( wx.NULL, wx.wxID_ANY, "wxLua wxTreeCtrl Sample",
                        wx.wxDefaultPosition, wx.wxSize( 850, 700 ),
                        wx.wxDEFAULT_FRAME_STYLE )

    -- create the menubar and attach it
    local fileMenu = wx.wxMenu()
	fileMenu:Append( wx.wxID_NEW, "N&ew\tCtrl+N", "New description" )
	fileMenu:Append( wx.wxID_OPEN, "O&pen\tCtrl+O", "Open description" )
	fileMenu:Append( wx.wxID_SAVE, "S&ave\tCtrl+S", "Save description" )

	fileMenu:AppendSeparator()
	fileMenu:Append( wx.wxID_EXIT, "E&xit\tAlt+F4", "Quit the program" )
    local helpMenu = wx.wxMenu()
    helpMenu:Append(wx.wxID_ABOUT, "&About",
		"About the wxLua wxTreeCtrl Sample")

    local menuBar = wx.wxMenuBar()
    menuBar:Append(fileMenu, "&File")
    menuBar:Append(helpMenu, "&Help")

    frame:SetMenuBar(menuBar)

    -- connect the selection event of the exit menu item to an
    -- event handler that closes the window
    frame:Connect(wx.wxID_EXIT, wx.wxEVT_COMMAND_MENU_SELECTED,
        function (event)
            frame:Close(true)
        end )

    -- connect the selection event of the about menu item
    frame:Connect(wx.wxID_ABOUT, wx.wxEVT_COMMAND_MENU_SELECTED,
        function (event)
            wx.wxMessageBox( 'This is the "About" dialog of the wxLua'..
				'wxTreeCtrl sample.\n'..
				wxlua.wxLUA_VERSION_STRING.." built with "..
				wx.wxVERSION_STRING,
				"About wxLua", wx.wxOK + wx.wxICON_INFORMATION, frame )
        end )

	--
    frame:Connect(wx.wxID_SAVE, wx.wxEVT_COMMAND_MENU_SELECTED,
        function (event)
            local filename = wx.wxFileSelector( 'Choose a file to open',
				'', '', 'prj.lua',
				"Project description (*.prj.lua)|*.prj.lua" )

			save_table_to_file( project, "project", filename )
			add_visual_behavior( project )
        end )

    splitter = wx.wxSplitterWindow(frame, wx.wxID_ANY)
    splitter:SetMinimumPaneSize(50) -- don't let it unsplit
    splitter:SetSashGravity( .3 )

    -- create our treectrl
    tree = wx.wxTreeCtrl( splitter, wx.wxID_ANY,
                          wx.wxDefaultPosition, wx.wxSize(-1, 500),
                          wx.wxTR_LINES_AT_ROOT + wx.wxTR_HAS_BUTTONS )

	panel = wx.wxPanel( splitter, wx.wxID_ANY )

    splitter:SplitVertically(tree, panel, 200 )

    frame:Layout() -- help sizing the windows before being shown


	add_visual_behavior( project )
	create_tree_veiw( project, tree )



	g_selected_object = nil

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
				create_tree_veiw( project, tree )
				--panel:Show( true )
				--tree:Show( true )
				--tree:SetFocus()
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

				--panel:Hide()
				--tree:Hide()
				tree:DeleteAllItems()
				create_tree_veiw( project, tree )
				--panel:Show( true )
				--tree:Show( true )
				--tree:SetFocus()
				tree:SelectItem( g_selected_tree_item )
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

                panel:Layout() -- help sizing the windows before being shown
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

			panel:Show( true )
			end

			if index > 1 then
				object.flex_grid_sizer = wx.wxFlexGridSizer( 2, 2, 5, 5 )
				object.flex_grid_sizer:AddGrowableCol( 1 )

				for i = 1, index - 1 do
					object.flex_grid_sizer:Add( object.sys_static_text[ i ], 0,
						wx.wxALIGN_CENTER_VERTICAL + wx.wxALL, 5 )
					object.flex_grid_sizer:Add( object.sys_text[ i ], 0,
						wx.wxGROW + wx.wxALIGN_CENTER_VERTICAL + wx.wxALL, 5 )
					panel:SetSizer( object.flex_grid_sizer )
					panel:Layout() -- help sizing the windows before being shown
				end

			end

			g_prev_activ_item = object

--[[			if object.show ~= nil then
				--object:show( panel, true )
			end]]

		end)

	wx.wxGetApp():SetTopWindow(frame)

    frame:Show(true)
end
-- ----------------------------------------------------------------------------
main()

-- Call wx.wxGetApp():MainLoop() last to start the wxWidgets event loop,
-- otherwise the wxLua program will exit immediately.
-- Does nothing if running from wxLua, wxLuaFreeze, or wxLuaEdit since the
-- MainLoop is already running or will be started by the C++ program.

local root = tree:GetRootItem()
--tree:Expand( root )

local cookie = 0
local child = tree:GetFirstChild( root, cookie )
child = tree:GetFirstChild( child, cookie )
child = tree:GetFirstChild( child, cookie )
--tree:SortChildren( child )
child = tree:GetFirstChild( child, cookie )
--tree:SortChildren( child )
--print( child )
--tree:Expand( child )

tree:SelectItem( child )

wx.wxGetApp():MainLoop()
