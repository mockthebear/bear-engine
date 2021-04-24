function expandFlags(arg)
	local str = ""
	for i,b in pairs(arg) do 
		str = str .. "-D"..b.." "
	end
	return str
end


local fileTypes = {"cpp","c"}
local notAssets = {"hpp","h"}
local ignore	= {"cbp","exe","depend","layout","rc","ico","rc","dll","so"}
local COMPILER = "em++";
local COMPILERC= "emcc";	
local ASSETS_FOLDER = "game"
local SOURCE_FOLDER = "source"
local FILEOUT = "snakescape.html"
local LDFLAGS = "--emrun"
local DEFINEFLAGS = {"REMAKE_VETEX_ON_BIND","RENDER_OPENGLES2","SUPPORT_SINGLE_BUFFER", "NEED_SHADER_LOCATION"}
local PRELOADSTUFF = "--preload-file engine/ --preload-file lua/ --preload-file ui/ --preload-file data/ --preload-file test.burr --preload-file snd.burr --preload-file teste.txt"
local CFLAGS 	= "-s ASSERTIONS=1 "..expandFlags(DEFINEFLAGS).." -O2 -Oz -s USE_SDL=2 -s USE_SDL_TTF=2 -s USE_VORBIS=1 --use-preload-plugins -s ALLOW_MEMORY_GROWTH=1 -std=c++11"
local CCFLAGS 	= "-s ASSERTIONS=1 "..expandFlags(DEFINEFLAGS).." -O2 -Oz -s USE_SDL=2 -s USE_SDL_TTF=2 -s USE_VORBIS=1 --use-preload-plugins -s ALLOW_MEMORY_GROWTH=1"

local paral = 2

local OUTSTR = ""
local FILES = "";
local ASSETS = ""

local ret = os.execute(COMPILER.." -v")
if ret == 1 then
	os.execute("D:\\emscripten\\emsdk\\emsdk_env.bat")
end

function hasFile(fname)
	local f = io.open(fname,'r')
	if f then
		f:close()
		return true
	end
	return false
end



function checkFile(fname,root)
	root = root or ''
	local f = io.open(root..fname,'w')
	if not f then
		local primaryDir,tail = fname:match("(.-)/(.+)")
		if not hasFile(root..primaryDir.."/1.txt") then
			os.execute("cd "..root.." && mkdir "..primaryDir)
		else
			os.execute("del "..root..primaryDir.."/1.txt")
		end
		return checkFile(tail,root..primaryDir..'/')
	else
		os.execute("del "..root..fname)
		return true
	end
end

local cmdLines = ""
local owo = {}
function parseFolderRecursively(Fold)
	local f,er = io.open("source/beargine.cbp","r")
	if f then
		local str = f:read("*a");
		for i in str:gmatch("<Unit filename=\"(.-)\"%s*[/]*>") do
			if i:find("%.cpp") then
				local fileOut = "obj/emscripten/"..i:gsub("%.cpp",".bc")

				
					--checkFile(fileOut)
					local line = COMPILER.." -c source/"..i.." -o "..fileOut.." "..CFLAGS
					FILES = FILES .. "../obj/emscripten/"..i:gsub("%.cpp",".bc").." "
					--check the avaliability of the dir
					--print("Now:",line)
					cmdLines = cmdLines..fileOut.." "

					owo[#owo+1] = line
					--local ret = os.execute(line)
					if ret == 1 then
						return false
					end

			elseif i:find("%.c$") then
				local fileOut = "obj/emscripten/"..i:gsub("%.c$",".bc")
				local line = COMPILERC.." -c source/"..i.." -o "..fileOut.." "..CCFLAGS
				FILES = FILES .. "../obj/emscripten/"..i:gsub("%.c",".bc").." "
				--check the avaliability of the dir
				--print("Now:",line)
				cmdLines = cmdLines..fileOut.." "
				owo[#owo+1] = line
				--local ret = os.execute(line)
				if ret == 1 then
					return false
				end
			end
		end
	end
	return true;

end
if parseFolderRecursively(SOURCE_FOLDER) then
	local outp = FILES..' '..CFLAGS..' '..LDFLAGS.. ' '..PRELOADSTUFF
	--print("Ready files: ",outp)

	outp = "cd game/ && emcc -o kek.html "..outp

	--
	local a = io.open("OWO.txt","w")

	

	for i,b in pairs(owo) do 
		print("-->"..b)
		if i == #owo then
			paral = 0
		end 
		local ret = os.execute(b..(paral <= 0 and " &" or ""))
		paral = paral - 1
		if (paral < 0) then 
			paral = 2
		end
		a:write(b.."\r\n")
		if ret == 1 or ret == nil then
			return false
		end
	end

	a:write(outp)
	a:close()

	print("\n"..outp)
	local ret = os.execute(outp)
	if ret == 1 then
			return false
	end

	--os.execute("emrun kek.html")
end

--Build folder:

