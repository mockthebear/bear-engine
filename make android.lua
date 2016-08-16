local androidmk =[[LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main

SDL_PATH := ../SDL2-2.0.4

LOCAL_CPPFLAGS := -std=c++11 -D__GXX_EXPERIMENTAL_CXX0X__ -fpermissive -fno-rtti
LOCAL_CFLAGS := -D__GXX_EXPERIMENTAL_CXX0X__ -fno-rtti
LOCAL_CPP_FEATURES := rtti exceptions

LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(SDL_PATH)/include $(LOCAL_PATH)/../lua

# Add your application source files here...
LOCAL_SRC_FILES := $(SDL_PATH)/src/main/android/SDL_android_main.c @@@@

LOCAL_SHARED_LIBRARIES := SDL2 SDL2_image SDL2_ttf SDL2_mixer liblua

LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -llog -fpermissive -landroid

include $(BUILD_SHARED_LIBRARY)

]]
require("ex")

--
function printDir(dir,sub,str)
	if not str then
		str = ""
	end
	print(dir)
	for i,b in os.dir(dir) do
		if (i.type == "directory") then
			str = str ..'/'..printDir(dir..i.name,i.name)
		else
			if i.name:match("(.+)%.cpp") then
				str = str ..("/ ../../../source/"..(sub and sub..'/' or '')..""..i.name..' ')
			end
		end
	end
	return str;
end
local data = printDir("source/");

local FILE = io.open("android-project/jni/src/Android.mk","r")
STR = FILE:read(-1)
FILE:close()
androidmk = androidmk:gsub("@@@@",data);
if STR ~= androidmk then
	print("File remake")
	local FILE = io.open("android-project/jni/src/Android.mk","w")

	FILE:write(androidmk)
	FILE:close()
end

local ret = os.execute("step1.bat")
if ret == 0 then
	ret = os.execute("step2.bat")
	if ret == 0 then
		os.execute("step3.bat")

	else
		io.read()
	end
else
	io.read()
end


