--heyo

widgets = {}

widgets.style = {
	str = "",
	pos = {x=32,y=32},
	size = {x=32,y=32},
	bgcolor = {r=180,g=180,b=200,a=255},
	textcolor = {r=0,g=0,b=12,a=255},
	bordercolor = {r=0,g=0,b=0,a=255},
	update = false,
}

function widgets.SwapStyle(data,name,val)
	if not data[name] then
		data[name] = val
	end
	return data
end

function widgets.FormatPattern(data)
	for i,b in pairs(widgets.style) do
		data = widgets.SwapStyle(data,i,b)
	end
	return data
end

function widgets.Window(data)
	data = widgets.FormatPattern(data)
	local tex = Text(data.str,15,data.textcolor)
	local ui = LuaUi()

	if data.id then
		ui:SetId(tostring(data.id))
	end

	ui.data = data

	ui:SetWidth(data.size.x)
	ui:SetHeight(data.size.y)

	ui:SetX(data.pos.x)
	ui:SetY(data.pos.y)
	local tex = Text(data.str,12,data.textcolor)
	ui:SetTextObj(tex)
	ui.texSize = tex:GetHeight()

	if data.update then

		ui.Update = data.update
	end



	ui.Render = function(this)
		local data = this.data
		g_render.DrawFillSquare({x=this:GetScreenX(),y=this:GetScreenY(),w=this:GetWidth(),h=this:GetHeight()},
		data.bgcolor.r,data.bgcolor.g,data.bgcolor.b,data.bgcolor.a)
		g_render.DrawOutlineSquare({x=this:GetScreenX(),y=this:GetScreenY(),w=this:GetWidth(),h=this:GetHeight()},
		data.bordercolor.r,data.bordercolor.g,data.bordercolor.b,data.bordercolor.a)
		g_render.DrawOutlineSquare({x=this:GetScreenX(),y=this:GetScreenY(),w=this:GetWidth(),h=this.texSize+1},
		data.bordercolor.r,data.bordercolor.g,data.bordercolor.b,data.bordercolor.a)
	end

	return ui

end

function widgets.Label(data)

	data = widgets.FormatPattern(data)

	local tex = Text(data.str,15,data.textcolor)
	local ui = LuaUi()

	if data.id then
		ui:SetId(tostring(data.id))
	end
	ui.data = data

	ui:SetWidth(tex:GetWidth())
	ui:SetHeight(tex:GetHeight())
	ui:SetTextObj(tex)

	ui:SetX(data.pos.x)
	ui:SetY(data.pos.y)

	return ui
end


function widgets.Button(data)

	data = widgets.SwapStyle(data,"bgcolor",{r=120,g=120,b=208,a=255})

	data = widgets.FormatPattern(data)

	local tex = Text(data.str,15,data.textcolor)
	local ui = LuaUi()

	if data.id then
		ui:SetId(tostring(data.id))
	end
	ui.data = data

	ui:SetWidth(tex:GetWidth())
	ui:SetHeight(tex:GetHeight())
	ui:SetTextObj(tex)

	ui:SetX(data.pos.x)
	ui:SetY(data.pos.y)

	ui.Render = function(this)
		local data = this.data
		g_render.DrawFillSquare({x=this:GetScreenX()-1,y=this:GetScreenY()-1,w=this:GetWidth()+2,h=this:GetHeight()+2},
		data.bgcolor.r,data.bgcolor.g,data.bgcolor.b,data.bgcolor.a)
		g_render.DrawOutlineSquare({x=this:GetScreenX()-1,y=this:GetScreenY()-1,w=this:GetWidth()+2,h=this:GetHeight()+2},
		data.bordercolor.r,data.bordercolor.g,data.bordercolor.b,data.bordercolor.a)
	end


	ui.OnMousePress = function(this,button)
		local mother = this:GetMother()
		if mother then
			if mother.interact == nil then
				mother.interact = this
			else
				return true
			end
		end
		data.action(this,button)
	end
	return ui

end

