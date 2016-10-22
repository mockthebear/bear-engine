#pragma once
#include "../../settings/definitions.hpp"
#include <tuple>
#include <iostream>

typedef std::string jStringParam;
typedef int jint;
typedef float jfloat;

template<typename F> struct GenericJniCaller{
    static bool Call(F f){
        return false;
    };
};
template<> struct GenericJniCaller <std::string>{
    static jStringParam Call(std::string e){
        return jStringParam(e);
    };
};
template<> struct GenericJniCaller <int>{
    static jint Call(int e){
        return int(e);
    };
};
template<> struct GenericJniCaller <float>{
    static jfloat Call(float e){
        return float(e);
    };
};

class JniHelper{
    public:
        template <class Ret,typename ... Types> static Ret CallFunction(Types... arg){
            //JNIEnv* env = (JNIEnv*)SDL_AndroidGetJNIEnv();
            //jobject activity = (jobject)SDL_AndroidGetActivity();
            //jclass clazz(env->GetObjectClass(activity));


            //jstring jStringParam = env->NewStringUTF( par.c_str() );
            //jmethodID method_id = env->GetMethodID(clazz, name.c_str(), "(Ljava/lang/String;)V");

            //auto ArgumentList = argument::makeTypes<Types ...>(arg...);
            //readLuaValues<sizeof...(Types)>::Read(ArgumentList);
            auto t = GenericJniCaller<std::string>::Call("ad");
            //std::tuple<int,float> t(1,3.4f);
            //auto t2 = std::tuple_cat(t,std::make_pair("Foo", "bar"));
            auto f = [=](){
                std::cout << "oiq\n";
            };
            std::cout << t << "\n";
            //Ret rData = m_expander<sizeof...(Types),Ret>::expand(f,arg...);


            //env->CallVoidMethod(activity, method_id,jStringParam);

            //env->DeleteLocalRef( jStringParam );

            //env->DeleteLocalRef(activity);
            //env->DeleteLocalRef(clazz);
        }
    private:




        /*template<int N,typename Ret> struct m_expander {
            template<typename Function, typename... Args>
                static Ret expand(const Function& f, const Args&... args) {
                    return m_expander<N-1,Ret>::expand(args...);
            }
        };
        template<typename Ret> struct m_expander <0,Ret> {
            template<typename Function, typename... Args>
                static Ret expand(const Function& f,const Args&... args) {
                    return m_functionCaller<Ret>(f, args...);
            }
        };
        template<typename Ret, typename F, typename... Args> Ret m_functionCaller(const F& f,const Args&... args) {
            Ret ret = f(args...);
            return (Ret)ret;
        }*/


};
