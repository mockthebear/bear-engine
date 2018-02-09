--heyo


function choose(...)
	local arg = {...}
	if #arg == 0 then
		return nil
	end
	return arg[math.random(1,#arg)]
end


function getDistance(p1,p2)
	return math.sqrt( (p1.x-p2.x)^2 + (p1.y-p2.y)^2 )
end

function getCenter(p1)
	return {x = p1.x + p1.w/2,y = p1.y + p1.h/2}
end

function isColliding(obj1,obj2)
	local a = obj1
	local b = obj2
	if not b.w or not b.h then
		b.w = 1
		b.h = 1
	end
	if not a.w or not a.h then
		a.w = 1
		a.h = 1
	end
	if not obj1.x and obj1.id then
		a = obj1:GetBox()
	end
	if not obj2.x and obj2.id  then
		b = obj2:GetBox()
	end

	if (a.y + a.h) < b.y then

		return false
	end
	if a.y > (b.y + b.h) then
		return false
	end

	if (a.x + a.w) < b.x then
		return false
	end

	if a.x >  (b.x + b.w) then
		return false
	end

	return true
end

function hasFile(path)
	local f = io.open(path,'r')
	if f then
		f:close()
		return true
	end
	return false
end
