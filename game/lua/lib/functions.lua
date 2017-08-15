--heyo


function choose(...)
	local arg = {...}
	if #arg == 0 then
		return nil
	end
	return arg[math.random(1,#arg)]
end



luaPrint = print

_G.print = function(...)
	local args = {...}
	if #args == 0 then
		g_console.Print("<empty>")
	end
	local str = ""
	for i,b in pairs(args) do
		str = str .. tostring(b)..'\t ,'
	end
	if #args > 0 and str:len() > 3 then
		str = str:sub(1,str:len()-3)
	end
	g_console.Print(str)
end
