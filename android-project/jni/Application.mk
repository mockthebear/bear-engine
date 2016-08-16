NDK_TOOLCHAIN_VERSION := 4.8
# Uncomment this if you're using STL in your project
# See CPLUSPLUS-SUPPORT.html in the NDK documentation for more information
# armeabi
APP_STL := gnustl_static


APP_ABI := armeabi-v7a
APP_CPPFLAGS += -std=c++11
