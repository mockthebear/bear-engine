function onLoad()
	print("Loaded!")

	local obj = LuaObject(32,32)

	obj.Render = function(this)
		g_render.DrawFillSquare({x=this:GetX(),y=this:GetY(),w=16,h=16},
		math.random(0,255),math.random(0,255),math.random(0,255),255)
	end

	obj.Update = function(this,dt)
		this:SetX(this:GetX() + 2.0 * dt )
	end

	local ui = widgets.Window({pos = {x=200,y=200,},size = {x=150,y=140},str = "bulge",update = function(this)
		local child = this:GetChildById("fps")
		if child then
			child:SetText("FPS: "..g_screen.GetFps())
		end
	end})


	ui:AddComponent(widgets.Button({str = "Close", pos = {x=48,y=108}, action =function() ui:Close() end}))
	ui:AddComponent(widgets.Label({str = "FPS info", pos = {x=12,y=12}, id = "fps"}))
	g_ui.AddWindow(ui)

end
