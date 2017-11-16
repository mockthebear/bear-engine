


local fileTypes = {"cpp","c"}
local notAssets = {"hpp","h"}
local ignore	= {"cbp","exe","depend","layout","rc","ico","rc","dll","so"}
local COMPILER = "em++";
local ASSETS_FOLDER = "game"
local SOURCE_FOLDER = "source"
local FILEOUT = "snakescape.html"
local CFLAGS = "  -s USE_SDL=2 -s USE_SDL_TTF=2 --use-preload-plugins -s ALLOW_MEMORY_GROWTH=1 -std=c++11 -O1 -O2 -O3 -Oz"


local OUTSTR = ""
local FILES = "";
local ASSETS = ""

function parseFolderRecursively(Fold)
	local f,er = io.open("source/beargine.cbp","r")
	if f then
		local str = f:read("*a");
		for i in str:gmatch("<Unit filename=\"(.-)\"%s*[/]*>") do 
			if i:find("%.cpp") then
				local line = COMPILER.." -c source/"..i.." -o obj/emscripten/"..i:gsub("%.cpp",".o").." -s USE_SDL=2 -s USE_SDL_TTF=2 -std=c++11"
				FILES = FILES .. "obj/emscripten/"..i:gsub("%.cpp",".o").." "
				--check the avaliability of the dir
				local ret = os.execute(line)
				print(ret,line)
				if not ret then
					return false
				end
			end
		end
	end
	return true;

end
if parseFolderRecursively(SOURCE_FOLDER) then 
	 os.execute("em++ -o kek.html "..FILES)
end

