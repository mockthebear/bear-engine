require("ex")


local fileTypes = {"cpp","c"}
local notAssets = {"hpp","h"}
local ignore	= {"cbp","exe","depend","layout","rc","ico","rc","dll","so"}
local COMPILER = "em++";
local ASSETS_FOLDER = "game"
local SOURCE_FOLDER = "source"
local FILEOUT = "snakescape.html"
local CFLAGS = "  -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s USE_SDL_TTF=2 --use-preload-plugins -s ALLOW_MEMORY_GROWTH=1 -std=c++11 -O1 -O2 -O3 -Oz"


local OUTSTR = ""
local FILES = "";
local ASSETS = ""

function parseFolderRecursively(Fold)

	for i,b in os.dir(Fold) do
		if i.type == "file" then
			local found = false;
			for _,ftype in pairs(ignore) do
				if i.name:match("(.-)%."..ftype) then
					found = true;
					break;
				end
			end
			if not found then
				for _,ftype in pairs(fileTypes) do
					if i.name:match("(.-)%."..ftype) then
						FILES = FILES .." ".. Fold..'/'..i.name
						found = true;
						break;
					end
				end
				if not found then
					for _,ftype in pairs(notAssets) do
						if i.name:match("(.-)%."..ftype) then
							found = true;
							break;
						end
					end
					if not found then
						ASSETS = ASSETS .." --preload-file \""..Fold..'/'..i.name..'"'
					end
				end
			end
		elseif i.type == "directory" then
			parseFolderRecursively(Fold..'/'..i.name)
		end
	end

end
parseFolderRecursively(SOURCE_FOLDER)
parseFolderRecursively(ASSETS_FOLDER)
--print(FILES)

OUTSTR = COMPILER .. " " .. FILES .." -o "..FILEOUT .." "..CFLAGS.." "..ASSETS
print(OUTSTR)
local ret = os.execute(OUTSTR)
print("RET is "..ret)
if ret == 0 then
os.execute(FILEOUT)
else
	io.read()
end
