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

end
