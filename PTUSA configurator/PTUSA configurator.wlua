-- ----------------------------------------------------------------------------
require("wx")
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
g_prev_activ_item = nil
g_data			  = {}
-- ----------------------------------------------------------------------------
local function print_table( tbl, tree, level )

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


    for fields, value in pairs( tbl ) do
		if type( value ) == "table" then

			print_table( value, tree, root_id )
		end
    end

end
-- ----------------------------------------------------------------------------
project.get_name = function( self )
	return self.name
end
-- ----------------------------------------------------------------------------
project.show = function( self, panel, is_show )

	if is_show == true then
		staticText = wx.wxStaticText( panel, wx.wxID_ANY, "Имя проекта" )

		text = wx.wxTextCtrl( panel, 10 , self.name, wx.wxDefaultPosition,
			wx.wxDefaultSize, wx.wxTE_PROCESS_ENTER )

		text:Connect( 10, wx.wxEVT_COMMAND_TEXT_ENTER,
			function( event )

			project.name = text:GetValue()

			tree:SetItemText( tree:GetSelection(), text:GetValue() )
			--tree:DeleteAllItems()
			--print_table( project, tree )

			--tree:Collapse( 1 )


        end )

		flex_grid_sizer = wx.wxFlexGridSizer( 2, 2, 5, 5 )
		flex_grid_sizer:AddGrowableCol( 1 )
		flex_grid_sizer:Add( staticText, 0,
			wx.wxALIGN_CENTER_VERTICAL + wx.wxALL, 5 )
		flex_grid_sizer:Add( text, 0,
			wx.wxGROW + wx.wxALIGN_CENTER_VERTICAL + wx.wxALL, 5 )
		panel:SetSizer( flex_grid_sizer )
		panel:Layout() -- help sizing the windows before being shown

	else
		staticText:Destroy()
		staticText = nil
		text:Destroy()
		text = nil

		flex_grid_sizer = nil

		panel:Layout() -- help sizing the windows before being shown

	end
end
-- ----------------------------------------------------------------------------
project.G_NODES.get_name = function( self )
	return "Узлы Wago ("..#self..")"
end
-- ----------------------------------------------------------------------------
for fields, value in pairs( project.G_NODES ) do
	if type( value ) == "table" then

		value.get_name = function( self )
			return self.address..". ".."Узел Wago"
		end

		value.set_property = function( self, prop, new_value )

			if prop == 'ntype' then
				value.ntype = new_value

			elseif prop == 'address' then
				value.address = new_value

			elseif prop == 'comment' then
				value.comment = new_value

			end

		end

		value.show = function( self, panel, is_show )

            if is_show == true then
                staticText = wx.wxStaticText( panel, wx.wxID_ANY, "Комментарий" )

                text = wx.wxTextCtrl( panel, 10 , value.comment, wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxTE_PROCESS_ENTER )

                text:Connect( 10, wx.wxEVT_COMMAND_TEXT_ENTER,
                    function( event )

					print( text:GetValue() )

                    --project.name = text:GetValue()

                    --tree:SetItemText( tree:GetSelection(), text:GetValue() )
                    --tree:DeleteAllItems()
                    --print_table( project, tree )

                    --tree:Collapse( 1 )


                end )

                flex_grid_sizer = wx.wxFlexGridSizer( 2, 2, 5, 5 )
                flex_grid_sizer:AddGrowableCol( 1 )
                flex_grid_sizer:Add( staticText, 0, wx.wxALIGN_CENTER_VERTICAL + wx.wxALL, 6 )
                flex_grid_sizer:Add( text, 0, wx.wxGROW + wx.wxALIGN_CENTER_VERTICAL + wx.wxALL, 6 )
                panel:SetSizer( flex_grid_sizer )
                panel:Layout() -- help sizing the windows before being shown

            else
                staticText:Destroy()
                staticText = nil
                text:Destroy()
                text = nil

                flex_grid_sizer = nil

                panel:Layout() -- help sizing the windows before being shown

            end
        end

		for fields, value in pairs( value ) do
			if fields == "modules" then

				value.get_name = function( self )
					return "Модули ("..#self..")"
				end


				for fields, value in pairs( value ) do
					if type( value ) == "table" then

						value.get_name = function( self )
							return string.format( "%d", value[ 1 ] )
						end
					end

				end

			end

		end

	end
end
-- ----------------------------------------------------------------------------
function main()
    frame = wx.wxFrame( wx.NULL, wx.wxID_ANY, "wxLua wxTreeCtrl Sample",
                        wx.wxDefaultPosition, wx.wxSize( 850, 700 ),
                        wx.wxDEFAULT_FRAME_STYLE )

    -- create the menubar and attach it
    local fileMenu = wx.wxMenu()
    fileMenu:Append(wx.wxID_EXIT, "E&xit", "Quit the program")
    local helpMenu = wx.wxMenu()
    helpMenu:Append(wx.wxID_ABOUT, "&About", "About the wxLua wxTreeCtrl Sample")

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
            wx.wxMessageBox('This is the "About" dialog of the wxLua wxTreeCtrl sample.\n'..
                            wxlua.wxLUA_VERSION_STRING.." built with "..wx.wxVERSION_STRING,
                            "About wxLua",
                            wx.wxOK + wx.wxICON_INFORMATION,
                            frame)
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

	print_table( project, tree )

--~ 	tree:Connect( wx.wxEVT_COMMAND_TREE_SEL_CHANGED,
--~ 		function( event )
--~ 			print( event:GetString() )
--~ 		end )


g_selected_object = nil

 tree:Connect( wx.wxEVT_COMMAND_TREE_KEY_DOWN,
        function( event )
			print( event:GetKeyCode() )

			if event:GetKeyCode() == 127 then

				if g_selected_object == nil then
					return
				end

				for fields, value in pairs( g_selected_object ) do
					print( fields, ' -- ', value )

				end

				g_selected_object[ 1 ] = nil

				tree:DeleteAllItems()
			    print_table( project, tree )

			end


		end)


    tree:Connect( wx.wxEVT_COMMAND_TREE_SEL_CHANGED,
        function( event )
            local item_id = event:GetItem()
			local object = g_data[ item_id:GetValue() ]

			g_selected_object = object

			--if g_prev_activ_item ~= nil and g_prev_activ_item.show ~= nil then
				--g_prev_activ_item:show( panel, false )
			--end

			if g_prev_activ_item ~= nil and g_prev_activ_item.staticText ~= nil then

				--print( #g_prev_activ_item.staticText )
				for i = 1, #g_prev_activ_item.staticText do

					g_prev_activ_item.staticText[ i ]:Destroy()
					g_prev_activ_item.staticText[ i ] = nil

					g_prev_activ_item.Text[ i ]:Destroy()
					g_prev_activ_item.Text[ i ] = nil
				end

                g_prev_activ_item.flex_grid_sizer = nil

                panel:Layout() -- help sizing the windows before being shown
			end


			--Отображаем все поля.
			index = 1
			object.staticText = {}
			object.Text = {}

			for fields, value in pairs( object ) do

				if type( value ) == "string" then

				    local par_name = fields
				    if type( fields ) == "number" then
						par_name = string.format( "[ %d ]", fields )
					end

					object.staticText[ index ] = wx.wxStaticText( panel, wx.wxID_ANY, par_name )
					object.Text[ index ] = wx.wxTextCtrl( panel, 10 , value, wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxTE_PROCESS_ENTER )

					object.Text[ index ]:Connect( 10, wx.wxEVT_COMMAND_TEXT_ENTER,
						function( event )

						object[ fields ] = event:GetString()

						if object.get_name ~= nil then
							tree:SetItemText( tree:GetSelection(), object:get_name() )
						end

					end )


					index = index + 1
				end

				if type( value ) == "number" then

				    local par_name = fields
				    if type( fields ) == "number" then
						par_name = string.format( "[ %d ]", fields )
					end

					object.staticText[ index ] = wx.wxStaticText( panel, wx.wxID_ANY, par_name )
					object.Text[ index ] = wx.wxTextCtrl( panel, 10 , string.format( "%d", value ),
						wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxTE_PROCESS_ENTER )

					object.Text[ index ]:Connect( 10, wx.wxEVT_COMMAND_TEXT_ENTER,
						function( event )

						object[ fields ] = event:GetString()

						if object.get_name ~= nil then
							tree:SetItemText( tree:GetSelection(), object:get_name() )
						end

					end )

					index = index + 1
				end


			end

			if index > 1 then
				object.flex_grid_sizer = wx.wxFlexGridSizer( 2, 2, 5, 5 )
				object.flex_grid_sizer:AddGrowableCol( 1 )

				for i = 1, index - 1 do
					object.flex_grid_sizer:Add( object.staticText[ i ], 0, wx.wxALIGN_CENTER_VERTICAL + wx.wxALL, 5 )
					object.flex_grid_sizer:Add( object.Text[ i ], 0, wx.wxGROW + wx.wxALIGN_CENTER_VERTICAL + wx.wxALL, 5 )
					panel:SetSizer( object.flex_grid_sizer )
					panel:Layout() -- help sizing the windows before being shown
				end

			end

			g_prev_activ_item = object

			if object.show ~= nil then
				--object:show( panel, true )
			end

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
child = tree:GetFirstChild( child, cookie )
--print( child )
--tree:Expand( child )

tree:SelectItem( child )

wx.wxGetApp():MainLoop()
