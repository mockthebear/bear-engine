--heyo

widgets = {}

widgets.style = {
	str = "",
	pos = {x=32,y=32},
	size = {x=32,y=32},
	bgcolor = {r=180,g=180,b=200,a=255},
	textcolor = {r=0,g=0,b=12,a=255},
	textsize = 15,
	bordercolor = {r=0,g=0,b=0,a=255},
	update = false,
	alignment = {},
	font = "ui/LiberationSans-Bold.ttf"
}

function  Copy(t)
	local t2 = {}
	for i,b in pairs(t) do
		t2[i] = b
	end
	return t2
end

function widgets.ProcessAlignment(this)
	--[[alignment = {
		top = 'parent.top',
		left = 'jaaj.top',
	}]]
	for i,b in pairs(this.alignment) do
		local obj,where = b:match("(.-)%.(.+)")
		if not obj then
			error("Malformed: "..b)
			return
		end
		local thisExtra = 0
		if where:find("%s") then
			where,added = where:match("(.-)%s([%-%+%d%s]+)")
			if not where then
				error("Malformed: "..b)
				return
			end

			if added:find("%+") then
				added = added:gsub("%+","")
			end
			thisExtra = tonumber(added) or 0
		end
		if obj == 'parent' then
			obj = this:GetMother()
		else
			if not this:GetMother() then
				obj = nil
			end
			obj = this:GetMother():GetChildById(obj)
		end
		if obj then
			i = i:lower()
			local pt = {x = 0, y = 0}
			if where == 'top' then
				pt.y = obj:GetY()
			elseif where == 'bottom' then
				pt.y = obj:GetY()+obj:GetHeight()
			elseif where == 'left' then
				pt.x = obj:GetX()
			elseif where == 'right' then
				pt.x = obj:GetX()+obj:GetWidth()
			elseif where == 'center' then
				pt.x = obj:GetX()+obj:GetWidth()/2
				pt.y = obj:GetY()+obj:GetHeight()/2
			end

			if i == 'top' then
				this:SetY( pt.y + thisExtra )
			elseif i == 'bottom' then
				this:SetY( pt.y -  this:GetHeight() + thisExtra)
			elseif i == 'left' then
				this:SetX( pt.x + thisExtra )
			elseif i == 'right' then
				this:SetX( pt.x -  this:GetWidth() + thisExtra)
			elseif i == 'vcenter' or i == "verticalcenter" then
				this:SetX( pt.x -  this:GetWidth()/2 + thisExtra)
			elseif i == 'hcenter' or i == "horizontalcenter" then
				this:SetY( pt.y -  this:GetHeight()/2 + thisExtra)
			elseif i == 'center' then
				this:SetX( pt.x -  this:GetWidth()/2 + thisExtra)
				this:SetY( pt.y -  this:GetHeight()/2 + thisExtra)
			end

		end
	end
end

function widgets.SwapStyle(data,name,val)
	if not data[name] then
		data[name] = val
	end
	return data
end

function widgets.FormatPattern(data,ui)
	for i,b in pairs(data) do
		ui[i] = b
	end
	for i,b in pairs(widgets.style) do
		ui = widgets.SwapStyle(ui,i,b)
	end
	return ui
end

function widgets.Window(data)
	local ui = LuaUi()


	ui.pressed = false
	ui.dragging = false
	ui.canMove = true


	ui = widgets.FormatPattern(data,ui)
	local tex = Text(ui.str or ui.text,15,ui.textcolor)
	tex:SetFont(ui.font)

	if ui.id then
		ui:SetId(tostring(ui.id))
	end




	ui:SetWidth(ui.size.x)
	ui:SetHeight(ui.size.y)

	ui:SetX(ui.pos.x)
	ui:SetY(ui.pos.y)
	local tex = Text(ui.str or ui.text,12,ui.textcolor)
	tex:SetFont(ui.font)
	ui:SetTextObj(tex)
	ui.texSize = tex:GetHeight()

	ui.Update = function(this,dt)
		if not this.interact then
			if this.canMove then
				if this.dragging then
					local mousePos = g_input.GetMouse()
					this:SetX(mousePos.x + this.dragging.x)
					this:SetY(mousePos.y + this.dragging.y)
				end
			end
			this.pressed = false
			if this.update then
				this.update(this,dt)
			end
		else
			if g_input.IsMouseReleased(1) then
				this.interact = nil
			end
		end
	end


	ui.OnMousePress = function(this,button)
		g_input.HaltInput()
		if not this.pressed then
			local mousePos = g_input.GetMouse()

			this.dragging = {x = this:GetTrueX() - mousePos.x,y = this:GetTrueY() - mousePos.y}
		end
	end

	ui.OnMouseRelease = function(this,button)
		g_input.HaltInput()
		this.dragging = false
	end


	ui.Render = function(this)
		g_render.DrawFillSquare({x=this:GetScreenX(),y=this:GetScreenY(),w=this:GetWidth(),h=this:GetHeight()},
		this.bgcolor.r,this.bgcolor.g,this.bgcolor.b,this.bgcolor.a)
		g_render.DrawOutlineSquare({x=this:GetScreenX(),y=this:GetScreenY(),w=this:GetWidth(),h=this:GetHeight()},
		this.bordercolor.r,this.bordercolor.g,this.bordercolor.b,this.bordercolor.a)
		g_render.DrawOutlineSquare({x=this:GetScreenX(),y=this:GetScreenY(),w=this:GetWidth(),h=this.texSize+1},
		this.bordercolor.r,this.bordercolor.g,this.bordercolor.b,this.bordercolor.a)
	end
	ui:SetAsMain(true)
	return ui

end

function widgets.SpriteLabel(data,spr)
	local ui = LuaUi()
	ui = widgets.FormatPattern(data,ui)

	ui.spr = spr

	if ui.id then
		ui:SetId(tostring(ui.id))
	end

	ui.Released = Copy(ui.bordercolor)
	ui.PressedColor = {r = 255,g=0,b=0,a=255}


	ui:SetWidth(spr:GetFrameWidth())
	ui:SetHeight(spr:GetFrameHeight())

	ui.Update = function(this)
		widgets.ProcessAlignment(this)
		local mousePos = g_input.GetMouse()
		local MyRect = {x=this:GetScreenX()-1,y=this:GetScreenY()-1,w=this:GetWidth()+2,h=this:GetHeight()+2}
		if g_input.IsMousePressed(1) and IsColliding(MyRect,mousePos) then
			this.bordercolor = this.PressedColor
		else
			this.bordercolor = this.Released
		end
	end

	ui.Render = function(this)
		this.spr:Render({x=this:GetScreenX(),y=this:GetScreenY()},0)
		local MyRect = {x=this:GetScreenX()-1,y=this:GetScreenY()-1,w=this:GetWidth()+2,h=this:GetHeight()+2}
		g_render.DrawOutlineSquare(MyRect, this.bordercolor.r,this.bordercolor.g,this.bordercolor.b,this.bordercolor.a)
	end

	ui.OnMousePress = function(this,button)
		g_input.HaltInput()
		local mother = this:GetMother()
		if mother then
			if mother.interact == nil then
				mother.interact = this
			else
				return true
			end
		end
		if this.action then
			this.action(this)
		end
	end

	ui:SetX(ui.pos.x)
	ui:SetY(ui.pos.y)


	return ui


end


function widgets.Label(data)
	local ui = LuaUi()
	ui = widgets.FormatPattern(data,ui)



	if ui.id then
		ui:SetId(tostring(ui.id))
	end


	ui.texts = {}


	ui.SetText = function(this,text)
		this.str = text .. '\n'
		this.text = text .. '\n'
		this.maxHeight = 0
		this.maxWidth = 0
		this.textLines = 1
		for i in this.text:gmatch("(.-)\n") do
			if i and i:len() > 0 then

				if not this.texts[this.textLines] then
					this.texts[this.textLines] = Text(i,ui.textsize,this.textcolor)
					this.texts[this.textLines]:SetFont(this.font)
				end
				if this.texts[this.textLines]:GetText() ~= i then
					this.texts[this.textLines]:SetText(i)
				end


				this.maxHeight = this.maxHeight + this.texts[this.textLines]:GetHeight()
				this.maxWidth = math.max(this.maxWidth,this.texts[this.textLines]:GetWidth())
				this.textLines = this.textLines +1

			end
		end
		if #this.texts > this.textLines then
			for i=this.textLines,#this.texts do
				if this.texts[i]:GetText() ~= " " then
					this.texts[i]:SetText(" ")
				end
			end
		end

		this:SetWidth(ui.maxWidth)
		this:SetHeight(ui.maxHeight)
	end

	ui.Update = function(this)
		widgets.ProcessAlignment(this)
	end

	ui.Render = function(this)
		this.maxHeight = 0
		for i,b in pairs(this.texts) do

			b:Render({x = this:GetScreenX(), y = this:GetScreenY() + this.maxHeight})
			this.maxHeight = this.maxHeight + b:GetHeight()
		end
	end

	ui:SetX(ui.pos.x)
	ui:SetY(ui.pos.y)

	ui:SetText(ui.str or ui.text)

	return ui
end

function widgets.Checkbox(data)
	local ui = LuaUi()


	ui = widgets.FormatPattern(data,ui)
	ui = widgets.SwapStyle(ui,"state",0)



	if ui.id then
		ui:SetId(tostring(ui.id))
	end


	ui.spr = Sprite("ui/checkbox.png")
	local tex = Text(ui.str or ui.text,12,ui.textcolor)
	tex:SetFont(ui.font)
	tex:SetAliasign(true)
	ui.tex = tex

	ui:SetWidth(16 + tex:GetWidth())
	ui:SetHeight(math.max(16,tex:GetHeight()))

	ui:SetX(ui.pos.x)
	ui:SetY(ui.pos.y)

	ui.Render = function(this)

		if this.state == 1 then
			this.spr:SetClip(0,0,16,16)
		elseif this.state == 0 then
			this.spr:SetClip(0,16,16,16)
		else
			this.spr:SetClip(0,32,16,16)
		end
		this.spr:Render({x=this:GetScreenX(),y=this:GetScreenY()},0)
		if this.tex:IsWorking() then
			this.tex:Render({x=this:GetScreenX()+17,y=this:GetScreenY()})
		end
	end
	ui.Update = function(this)
		widgets.ProcessAlignment(this)
	end



	ui.OnMousePress = function(this,button)
		g_input.HaltInput()
		local mother = this:GetMother()
		if mother then
			if mother.interact == nil then
				mother.interact = this
			else
				return true
			end
		end
		local state = this.state
		if state == 0 then
			this.state = 1
		else
			this.state = 0
		end
		if this.action then
			this.action(this,this.state)
		end
	end
	return ui

end


function widgets.Button(data)
	local ui = LuaUi()

	ui = widgets.SwapStyle(ui,"bgcolor",{r=120,g=120,b=208,a=255})
	ui = widgets.SwapStyle(ui,"bgcolor2",{r=150,g=150,b=238,a=255})
	ui = widgets.SwapStyle(ui,"enabled",true)

	ui = widgets.FormatPattern(data,ui)

	local tex = Text(ui.str or ui.text,ui.textsize,ui.textcolor)
	tex:SetFont(ui.font)


	if ui.id then
		ui:SetId(tostring(ui.id))
	end


	ui:SetWidth(tex:GetWidth())
	ui:SetHeight(tex:GetHeight())
	ui:SetTextObj(tex)

	ui:SetX(ui.pos.x)
	ui:SetY(ui.pos.y)

	ui.Released = Copy(ui.bgcolor2)
	ui.PressedColor =  Copy(ui.bgcolor2)
	ui.PressedColor.r = 100
	ui.PressedColor.g = 100
	ui.PressedColor.b = 120
	ui.SetEnable = function(this,en)
		this.enabled = en
	end
	ui.Render = function(this)


		local mousePos = g_input.GetMouse()
		local MyRect = {x=this:GetScreenX()-1,y=this:GetScreenY()-1,w=this:GetWidth()+2,h=this:GetHeight()+2}
		if IsColliding(MyRect,mousePos) or not this.enabled then
			g_render.DrawFillSquare(MyRect, this.bgcolor2.r,this.bgcolor2.g,this.bgcolor2.b,this.bgcolor2.a)
		else
			g_render.DrawFillSquare(MyRect, this.bgcolor.r,this.bgcolor.g,this.bgcolor.b,this.bgcolor.a)
		end
		g_render.DrawOutlineSquare(MyRect, this.bordercolor.r,this.bordercolor.g,this.bordercolor.b,this.bordercolor.a)

		if not this.enabled then
			g_render.DrawLineColor({x=this:GetScreenX(),y=this:GetScreenY()},{x=this:GetScreenX()+this:GetWidth(),y=this:GetScreenY()+this:GetHeight()},255,0,0,255)
			g_render.DrawLineColor({x=this:GetScreenX()+this:GetWidth(),y=this:GetScreenY()},{x=this:GetScreenX(),y=this:GetScreenY()+this:GetHeight()},255,0,0,255)
		end

	end

	ui.Update = function(this,dt)
		widgets.ProcessAlignment(this)
		local mousePos = g_input.GetMouse()
		local MyRect = {x=this:GetScreenX()-1,y=this:GetScreenY()-1,w=this:GetWidth()+2,h=this:GetHeight()+2}
		if not this.enabled or g_input.IsMousePressed(1) and IsColliding(MyRect,mousePos) then
			this.bgcolor2 = this.PressedColor
		else
			this.bgcolor2 = this.Released
		end
	end

	ui.OnMousePress = function(this,button)
		g_input.HaltInput()
		local mother = this:GetMother()
		if mother then
			if mother.interact == nil then
				mother.interact = this
			else
				return true
			end
		end
		if this.enabled then
			this.action(this,button)
		end
	end
	return ui

end

