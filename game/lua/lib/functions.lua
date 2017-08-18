--heyo


function choose(...)
	local arg = { n = select("#", ...); ... }
	if #arg.n == 0 then
		return nil
	end
	return arg[math.random(1,arg.n)]
end





luaPrint = print

_G.print = function(...)
	local args = { n = select("#", ...); ... }
	if args.n == 0 then
		g_console.Print("<empty>")
	end
	local str = ""
	for i = 1, args.n do
		str = str .. tostring(args[i])..'\t ,'
	end
	if #args > 0 and str:len() > 3 then
		str = str:sub(1,str:len()-3)
	end
	g_console.Print(str)
end


function IsColliding(a,b)
	if( a.y + (a.h or 1) < b.y ) then
		return false;
    end
	if( a.y > b.y + (b.h or 1) ) then
		return false;
    end
	if( (a.x + (a.w or 1)) < b.x ) then
		return false;
    end
	if( a.x >  b.x + (b.w or 1)) then
		return false;
	end
	return true;
end
