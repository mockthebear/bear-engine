--[[
	Part 1:
	Called from c++
]]

function onLoad()
	print("Hello from onLoad")

end

--[[
	Part 2:
	Lua managed state
]]

function MakeLuaState()
	print("Making an state")
	local state = LuaGameState()

	state.timer = 255.0

	state.OnBegin = function(this)
	    print("hi")
		--g_test.showInt(123)
		--g_test.showInt(g_test.Sum(120,3) * 10 + 4)


		g_test.testOptional()
		g_test.testOptional(1)
		g_test.testOptional(1,2)
		g_test.testOptional(1,2,3)

		g_test.testOptionalVec({1,2,3})
		g_test.testOptionalVec({1,2,3},1)
		g_test.testOptionalVec({1,2,3},1,2)
		g_test.testOptionalVec({1,2,3},1,2,3)




		print("Hello state!")
	end

	state.OnFinish = function(this)
		print("bye state")
	end

	state.OnResume = function(this)

	end

	state.OnPause = function(this)

	end

	state.OnUpdate = function(this,dt)
		this.timer = this.timer - 10 * dt
		if this.timer <= 0 or g_input.IsAnyKeyPressed() then
			this.data.canClose = true
			MakeLuaStateWithObjects()
		end
	end

	state.OnRender = function(this)
		g_render.DrawFillSquare({x=0,y=0,w=640,h=480},state.timer,0,0,255)

	end


	print("Setting up")
	state:Setup()
end

--[[
	Part 3:
	Lua managed state with lua managed objects
]]


function MakeLuaStateWithObjects()

	local state = LuaGameState()

	state.timer = 255.0

	state.OnBegin = function(this)
		print("Hello state with objects")
		g_camera.Initiate()
		--Should initiate the camera system. Everything outside screen is ignored


		local Obj = LuaObject(32,32)

		Obj:SetWidth(32)
		Obj:SetHeight(32)


		Obj.Update = function(self,dt)
			self:SetX( self:GetX() + 3.0 * dt)
		end

		Obj.Render = function(self)
			g_render.DrawFillSquare(self:GetBox(),math.random(0,255),math.random(0,255),0,255)
		end

	end

	state.OnFinish = function(this)

	end

	state.OnResume = function(this)

	end

	state.OnPause = function(this)

	end

	state.OnUpdate = function(this,dt)
		this.timer = this.timer - 10 * dt
		if this.timer <= 0 or g_input.IsAnyKeyPressed() then
			this.data.canClose = true
			MakeLuaStateWithComplexObjects()
		end
	end

	state.OnRender = function(this)
		g_render.DrawFillSquare({x=0,y=0,w=640,h=480},0,state.timer,0,255)

	end


	print("Is set")
	state:Setup()
end


--[[
	Part 4:
	Lua managed state with lua managed complex objects
	also testing any kind of interaction between c++/lua
]]

function MakeObject()
	local Obj = LuaObject(32,32 + math.random(1,500))
	Obj.speed = math.random(1,1000)/100.0;
	Obj:SetWidth(32)
	Obj:SetHeight(32 )

	Obj.spr = Sprite("test:bear.png")

	Obj.OnUpdate = function(self,dt)
		local gObj = self:GetMyObj()
		gObj:SetX( gObj:GetX() + self.speed * dt)
		if self:GetX() >= 400 then
			self:Destroy()
			collectgarbage()
		end

	end

	Obj.OnRender = function(self)
		g_render.DrawFillSquare(self:GetBox(),math.random(0,255),math.random(0,255),0,255)
		self.spr:Render(self:GetBox(),0) --Also works: {x = posX, y = posY}
	end
	return Obj
end
function MakeLuaStateWithComplexObjects()
	local state = LuaGameState()


	--Some UI




	state.timer = 200.0

	state.text = Text("owo",12,{r=255,g=255,b=0,a=100})



	state.OnBegin = function(this)
		--Should initiate the camera system. Everything outside screen is ignored
		g_camera.Initiate()

		g_assets.LoadResources("test.burr","test")


		local w = widgets.Window({
			size = {x=100,y=128},
		})

		w:AddComponent(widgets.Label({
			str = "Label",
			id = "testlabel",
			alignment = { top = 'parent.top',left = 'parent.left', },
		}))

		w:AddComponent(widgets.Button({
			str = "Button",
			action = function (thisButton,mouseKey)
				print("Pressed!!!!!!!")
				MakeObject()
				w:GetChildById('testlabel'):SetText("Pressed")
			end,
			alignment = { top = 'testlabel.bottom + 5',left = 'parent.left + 2', },
		}))

		w:AddComponent(widgets.Button({
			str = "Next test",
			action = function (thisButton,mouseKey)
				this.data.canClose = true
			end,
			alignment = { bottom = 'parent.bottom -4',right = 'parent.right -4', },
		}))

		g_ui.AddWindow(w)
		this.w = w;

		MakeObject()
	end

	state.OnFinish = function(this)
		--g_assets.EraseResource("test")
	end

	state.OnResume = function(this)

	end

	state.OnPause = function(this)

	end

	state.OnUpdate = function(this,dt)
		this.timer = this.timer - dt
		if this.timer <= 0 then
			this.data.canClose = true
		end
		this.text:SetText(tostring(this.timer))
	end

	state.OnRender = function(this)
		g_render.DrawFillSquare({x=0,y=0,w=640,h=480},0,0,state.timer,255)

		this.text:Render({x=32,y=200})

	end



	state:Setup()
end
