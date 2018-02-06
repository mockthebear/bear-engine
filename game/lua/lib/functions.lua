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
	local box1 = obj1
	local box2 = obj2
	if not box2.w or box2.h then
		box2.w = 1
		box2.h = 1
	end
	if not box1.w or box1.h then
		box1.w = 1
		box1.h = 1
	end
	if not obj1.x and obj1.id then
		box1 = obj1:GetBox()
	end
	if not obj2.x and obj2.id  then
		box2 = obj2:GetBox()
	end
	if box1.x < box2.x + box2.w then
		return true
	end
	if box1.y < box2.y + box2.h then
		return true
	end

	if box2.x < box1.x + box1.w then
		return true
	end

	if box2.y < box1.y + box1.h then
		return true
	end

	return false
end

function hasFile(path)
	local f = io.open(path,'r')
	if f then
		f:close()
		return true
	end
	return false
end
