LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main

SDL_PATH := ../SDL2


LOCAL_CPPFLAGS := -std=c++0x -D__GXX_EXPERIMENTAL_CXX0X__ -fpermissive -fno-rtti
LOCAL_CFLAGS := -D__GXX_EXPERIMENTAL_CXX0X__ -fpermissive -fno-rtti
LOCAL_CPP_FEATURES := rtti exceptions

LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(SDL_PATH)/include $(LOCAL_PATH)/../lua

# Add your application source files here...
LOCAL_SRC_FILES := $(SDL_PATH)/src/main/android/SDL_android_main.c /// ../../../source/engine/camera.cpp / ../../../source/engine/collision.cpp / ../../../source/engine/defaultbehavior.cpp / ../../../source/engine/gamebase.cpp / ../../../source/engine/genericstate.cpp / ../../../source/engine/light.cpp / ../../../source/engine/music.cpp / ../../../source/engine/particlecreator.cpp / ../../../source/engine/particles.cpp / ../../../source/engine/renderhelp.cpp / ../../../source/engine/screenmanager.cpp / ../../../source/engine/shooteffect.cpp / ../../../source/engine/smarttexture.cpp / ../../../source/engine/smarttexturemanager.cpp / ../../../source/engine/sound.cpp / ../../../source/engine/sprite.cpp / ../../../source/engine/text.cpp / ../../../source/engine/tilemap.cpp / ../../../source/engine/tileset.cpp / ../../../source/engine/timer.cpp // ../../../source/framework/debughelper.cpp / ../../../source/framework/dirmanager.cpp / ../../../source/framework/gamefile.cpp / ../../../source/framework/geometry.cpp / ../../../source/framework/poolmanager.cpp / ../../../source/framework/resourcefiles.cpp / ../../../source/framework/resourcemanager.cpp / ../../../source/framework/threadpool.cpp / ../../../source/framework/typechecker.cpp / ../../../source/framework/userfile.cpp / ../../../source/framework/utils.cpp // ../../../source/game/ball.cpp / ../../../source/game/controlableobject.cpp / ../../../source/game/gamebehavior.cpp / ../../../source/game/title.cpp // ../../../source/input/accelerometer.cpp / ../../../source/input/inputmanager.cpp / ../../../source/input/joystick.cpp / ../../../source/input/keyboard.cpp / ../../../source/input/mouse.cpp / ../../../source/input/touch.cpp / ../../../source/input/touchkeys.cpp // ../../../source/luasystem/luainterface.cpp / ../../../source/luasystem/luaobject.cpp / ../../../source/main.cpp // ../../../source/performance/console.cpp // ../../../source/settings/configmanager.cpp // ../../../source/ui/base.cpp / ../../../source/ui/button.cpp / ../../../source/ui/checkbox.cpp / ../../../source/ui/label.cpp / ../../../source/ui/numberinput.cpp / ../../../source/ui/textinput.cpp / ../../../source/ui/window.cpp 

LOCAL_SHARED_LIBRARIES := SDL2 SDL2_image SDL2_ttf SDL2_mixer liblua

LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -llog -fpermissive -landroid





include $(BUILD_SHARED_LIBRARY)
