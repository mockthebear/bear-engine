#include "luainterface.hpp"
#include "luatools.hpp"


#ifndef DISABLE_LUAINTERFACE
#include "../sound/sound.hpp"
#include "../engine/sprite.hpp"
#include "../framework/resourcemanager.hpp"


void LuaInterface::RegisterSpriteEffectClass(){

}


void LuaInterface::RegisterSound(){
    ClassRegister<Sound>::RegisterClassOutside(LuaManager::L,"Sound",[](lua_State* La)
    {
        std::string name;
        if (lua_gettop(La) == 2)
        {
            name = GenericLuaGetter<std::string>::Call(La);
            return LuaReferenceCounter<Sound>::makeReference(Game::GetCurrentState().Assets.make<Sound>(name.c_str()));
        }else{
            return new Sound();
        }

    });

    ClassRegister<Sound>::RegisterClassMethod(L,"Sound","Open",&Sound::Open);
    ClassRegister<Sound>::RegisterClassMethod(L,"Sound","SetVolume",&Sound::SetVolume);
    ClassRegister<Sound>::RegisterClassMethod(L,"Sound","Play",&Sound::Play,false);
    ClassRegister<Sound>::RegisterClassMethod(L,"Sound","SetRepeat",&Sound::SetRepeat);
    ClassRegister<Sound>::RegisterClassMethod(L,"Sound","Stop",&Sound::Stop);
    ClassRegister<Sound>::RegisterClassMethod(L,"Sound","Kill",&Sound::Kill);
    ClassRegister<Sound>::RegisterClassMethod(L,"Sound","Pause",&Sound::Pause);
    ClassRegister<Sound>::RegisterClassMethod(L,"Sound","Resume",&Sound::Resume);
    ClassRegister<Sound>::RegisterClassMethod(L,"Sound","Toggle",&Sound::Toggle);
    ClassRegister<Sound>::RegisterClassMethod(L,"Sound","GetPosition",&Sound::GetPosition);
    ClassRegister<Sound>::RegisterClassMethod(L,"Sound","GetDuration",&Sound::GetDuration);
    ClassRegister<Sound>::RegisterClassMethod(L,"Sound","IsOpen",&Sound::IsOpen);
    ClassRegister<Sound>::RegisterClassMethod(L,"Sound","IsPlaying",&Sound::IsPlaying);
    ClassRegister<Sound>::RegisterClassMethod(L,"Sound","IsPaused",&Sound::IsPaused);
    ClassRegister<Sound>::RegisterClassMethod(L,"Sound","PrePlay",&Sound::PrePlay);
    ClassRegister<Sound>::RegisterClassMethod(L,"Sound","GetFileName",&Sound::GetFileName);
    ClassRegister<Sound>::RegisterClassMethod(L,"Sound","SetClassType",&Sound::SetClassType);
    ClassRegister<Sound>::RegisterClassMethod(L,"Sound","GetClassType",&Sound::GetClassType);
    ClassRegister<Sound>::RegisterClassMethod(L,"Sound","SetPitch",&Sound::SetPitch);
    ClassRegister<Sound>::RegisterClassMethod(L,"Sound","FadeOut",&Sound::FadeOut,0.0f,1.0f);
    ClassRegister<Sound>::RegisterClassMethod(L,"Sound","FadeIn",&Sound::FadeIn,false,128.0f,1.0f);
}

void LuaInterface::RegisterFonts(){
    // Text("str",[12,[{r=255,g=255,b=255,a=255}]])   -> Text
    ClassRegister<Text>::RegisterClassOutside(LuaManager::L,"Text",[](lua_State* La)
    {
        std::string name;
        int size = 12;
        SDL_Color color = {100,100,100,255};
        if (lua_gettop(La) >= 4)
        {
            color = GenericLuaGetter<SDL_Color>::Call(La);
        }
        if (lua_gettop(La) >= 3)
        {
            size = GenericLuaGetter<int>::Call(La);
        }

        name = GenericLuaGetter<std::string>::Call(La);

        Text *t = new Text(name,size,color);
        t->SetKeepAlive(true);
        t->RemakeTexture();
        return t;
    });
    //  text:Close()
    ClassRegister<Text>::RegisterClassMethod(L,"Text","Close",&Text::Close);
    //  text:SetText("text")
    ClassRegister<Text>::RegisterClassMethod(L,"Text","SetText",&Text::SetText);
    //  text:SetColor({r=255,g=255,b=255,a=255})
    ClassRegister<Text>::RegisterClassMethod(L,"Text","SetColor",&Text::SetColor);
    ClassRegister<Text>::RegisterClassMethod(L,"Text","SetStyle",&Text::SetStyle);
    //  text:SetAlpha(255)
    ClassRegister<Text>::RegisterClassMethod(L,"Text","SetAlpha",&Text::SetAlpha);
    //  text:GetText() -> string
    ClassRegister<Text>::RegisterClassMethod(L,"Text","GetText",&Text::GetText);
    //  text:GetHeight() -> number
    ClassRegister<Text>::RegisterClassMethod(L,"Text","GetHeight",&Text::GetHeight);
    //  text:GetHeight() -> number
    ClassRegister<Text>::RegisterClassMethod(L,"Text","GetWidth",&Text::GetWidth);
    //  text:SetScaleX(0.5)
    ClassRegister<Text>::RegisterClassMethod(L,"Text","SetScaleX",&Text::SetScaleX);
    //  text:SetScaleX(0.5)
    ClassRegister<Text>::RegisterClassMethod(L,"Text","SetScaleY",&Text::SetScaleY);
    //  text:RemakeTexture() -> boolean
    ClassRegister<Text>::RegisterClassMethod(L,"Text","RemakeTexture",&Text::RemakeTexture);
    //  text:IsWorking() -> boolean
    ClassRegister<Text>::RegisterClassMethod(L,"Text","IsWorking",&Text::IsWorking);
    //  text:SetRotation(90)
    ClassRegister<Text>::RegisterClassMethod(L,"Text","SetRotation",&Text::SetRotation);
    //  text:Render({x=10,y=10})
    ClassRegister<Text>::RegisterClassMethod(L,"Text","Render",&Text::RenderLua);
    ClassRegister<Text>::RegisterClassMethod(L,"Text","SetAliasign",&Text::SetAliasign);
    ClassRegister<Text>::RegisterClassMethod(L,"Text","SetFont",&Text::SetFont);

    ClassRegister<CustomFont>::RegisterClassOutside(LuaManager::L,"CustomFont",[](lua_State* La)
    {
        std::string name = GenericLuaGetter<std::string>::Call(La);
        CustomFont *t = LuaReferenceCounter<CustomFont>::makeReference(new CustomFont(name));
        return t;
    });

    ClassRegister<CustomFont>::RegisterClassMethod(L,"CustomFont","Render",&CustomFont::Renderl,255);
    ClassRegister<CustomFont>::RegisterClassMethod(L,"CustomFont","RenderCentered",&CustomFont::RenderCentered,TEXT_CENTER_HORIZONTAL,255);
    ClassRegister<CustomFont>::RegisterClassMethod(L,"CustomFont","GetSizes",&CustomFont::GetSizes);
    ClassRegister<CustomFont>::RegisterClassMethod(L,"CustomFont","SetSprite",&CustomFont::SetSprite);

    TypeObserver<CustomFont,bool>::RegisterMethod(LuaManager::L,"loaded",&CustomFont::loaded);

}


void LuaInterface::RegisterAssets(){
     ClassRegister<ColorReplacer>::RegisterClassOutside(LuaManager::L,"ColorReplacer",[](lua_State* La)
    {
        return LuaReferenceCounter<ColorReplacer>::makeReference(ColorReplacer());
    });
    ClassRegister<ColorReplacer>::RegisterClassMethod(L,"ColorReplacer","clear",&ColorReplacer::clear);
    ClassRegister<ColorReplacer>::RegisterClassMethod(L,"ColorReplacer","AddReplacer",&ColorReplacer::AddReplacer);
    ClassRegister<ColorReplacer>::RegisterClassMethod(L,"ColorReplacer","Get",&ColorReplacer::Get);




    ClassRegister<AssetMannager>::RegisterClassOutside(LuaManager::L,"AssetMannager",[](lua_State* La)
    {
        return LuaReferenceCounter<AssetMannager>::makeReference(AssetMannager());
    });

    ClassRegister<AssetMannager>::RegisterClassMethod(L,"AssetMannager","LoadSprite",&AssetMannager::lua_loadSprite,"");
    //ClassRegister<AssetMannager>::RegisterClassMethod(L,"AssetMannager","LoadSound",&AssetMannager::lua_loadSound,"");



    GlobalMethodRegister::RegisterGlobalTable(LuaManager::L,"g_assets");
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_assets","LoadResources",std::function<bool(std::string,std::string)>([](std::string file,std::string alias)
    {
        return ResourceManager::GetInstance().Load(file,alias);
    }));

    /*GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_assets","CreateAlias",std::function<bool(std::string,std::string)>([](std::string file,std::string alias){
        return Game::GetCurrentState().Assets.load<Sprite>(file,alias);
    }));*/



    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_assets","EraseResource",std::function<void(std::string)>([](std::string alias)
    {
        ResourceManager::GetInstance().Erase(alias);
    }));

    GlobalMethodRegister::RegisterGlobalTable(LuaManager::L,"g_sound");
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_sound","PlayOnce",std::function<int(std::string)>([](std::string str)
    {
        return Sound::PlayOnce(str.c_str());
    }));
}


void LuaInterface::RegisterSprite(){
    ClassRegister<Animation>::RegisterClassOutside(LuaManager::L,"Animation",[](lua_State* La)
    {
        return LuaReferenceCounter<Animation>::makeReference(Animation());
    });

    ClassRegister<Animation>::RegisterClassMethod(L,"Animation","SetGridSize",&Animation::SetGridSize);
    ClassRegister<Animation>::RegisterClassMethod(L,"Animation","Update",&Animation::Update);
    ClassRegister<Animation>::RegisterClassMethod(L,"Animation","Pause",&Animation::Pause);
    ClassRegister<Animation>::RegisterClassMethod(L,"Animation","IsAnimationEnd",&Animation::IsAnimationEnd);
    ClassRegister<Animation>::RegisterClassMethod(L,"Animation","IsFrameEnd",&Animation::IsFrameEnd);
    ClassRegister<Animation>::RegisterClassMethod(L,"Animation","SetAnimation",&Animation::SetAnimation,-1.0f,0,0);
    ClassRegister<Animation>::RegisterClassMethod(L,"Animation","ResetAnimation",&Animation::ResetAnimation);
    ClassRegister<Animation>::RegisterClassMethod(L,"Animation","SetFrameTime",&Animation::SetFrameTime);
    ClassRegister<Animation>::RegisterClassMethod(L,"Animation","GetFrameTime",&Animation::GetFrameTime);
    ClassRegister<Animation>::RegisterClassMethod(L,"Animation","SetRepeatTimes",&Animation::SetRepeatTimes);
    ClassRegister<Animation>::RegisterClassMethod(L,"Animation","Render",&Animation::RenderL);

    TypeObserver<Animation,bool>::RegisterMethod(LuaManager::L,"CanRepeat",&Animation::CanRepeat);
    TypeObserver<Animation,uint32_t>::RegisterMethod(LuaManager::L,"Loops",&Animation::Loops);
    TypeObserver<Animation,uint32_t>::RegisterMethod(LuaManager::L,"sprX",&Animation::sprX);
    TypeObserver<Animation,uint32_t>::RegisterMethod(LuaManager::L,"sprY",&Animation::sprY);
    TypeObserver<Animation,uint32_t>::RegisterMethod(LuaManager::L,"sprW",&Animation::sprW);
    TypeObserver<Animation,uint32_t>::RegisterMethod(LuaManager::L,"sprH",&Animation::sprH);
    TypeObserver<Animation,uint32_t>::RegisterMethod(LuaManager::L,"MaxFrames",&Animation::MaxFrames);
    TypeObserver<Animation,uint32_t>::RegisterMethod(LuaManager::L,"LastFrame",&Animation::LastFrame);
    TypeObserver<Animation,float>::RegisterMethod(LuaManager::L,"SprDelay",&Animation::SprDelay);
    TypeObserver<Animation,float>::RegisterMethod(LuaManager::L,"SprMaxDelay",&Animation::SprMaxDelay);


    ClassRegister<Sprite>::RegisterClassOutside(LuaManager::L,"Sprite",[](lua_State* La)
    {
        std::string name;
        if (lua_gettop(La) == 3)
        {
            ColorReplacer r = GenericLuaGetter<ColorReplacer>::Call(La);
            name = GenericLuaGetter<std::string>::Call(La);
            Sprite *t = LuaReferenceCounter<Sprite>::makeReference(Game::GetCurrentState().Assets.make<Sprite>(name,r));
            return t;
        }
        else if (lua_gettop(La) == 2)
        {
            name = GenericLuaGetter<std::string>::Call(La);
        }else{

            return new Sprite();
        }
        Sprite *t = LuaReferenceCounter<Sprite>::makeReference(Game::GetCurrentState().Assets.make<Sprite>(name));
        return t;
    });

    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","Open",&Sprite::Openf);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","SetClip",&Sprite::SetClip);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","Render",&Sprite::Renderpoint,0.0);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","Update",&Sprite::Update);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","GetWidth",&Sprite::GetWidth);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","GetHeight",&Sprite::GetHeight);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","IsLoaded",&Sprite::IsLoaded);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","SetCenter",&Sprite::SetCenter);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","GetCenter",&Sprite::GetCenter);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","SetScaleX",&Sprite::SetScaleX);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","SetScaleY",&Sprite::SetScaleY);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","SetScale",&Sprite::SetScale);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","GetScale",&Sprite::GetScale);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","ReBlend",&Sprite::ReBlend);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","SetAlpha",&Sprite::SetAlpha);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","GetAlpha",&Sprite::GetAlpha);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","SetFlip",&Sprite::SetFlip);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","GetFlip",&Sprite::GetFlip);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","GetMe",&Sprite::GetMe);


    ClassRegister<AnimatedSprite>::RegisterClassMethod(L,"AnimatedSprite","Open",&AnimatedSprite::Openf);
    ClassRegister<AnimatedSprite>::RegisterClassMethod(L,"AnimatedSprite","SetClip",&AnimatedSprite::SetClip);
    ClassRegister<AnimatedSprite>::RegisterClassMethod(L,"AnimatedSprite","Render",&AnimatedSprite::Renderpoint,0.0);
    ClassRegister<AnimatedSprite>::RegisterClassMethod(L,"AnimatedSprite","Update",&AnimatedSprite::Update);
    ClassRegister<AnimatedSprite>::RegisterClassMethod(L,"AnimatedSprite","SetFrame",&AnimatedSprite::SetFrame,-1,0);
    ClassRegister<AnimatedSprite>::RegisterClassMethod(L,"AnimatedSprite","GetFrame",&AnimatedSprite::GetFrame);
    ClassRegister<AnimatedSprite>::RegisterClassMethod(L,"AnimatedSprite","GetWidth",&AnimatedSprite::GetWidth);
    ClassRegister<AnimatedSprite>::RegisterClassMethod(L,"AnimatedSprite","GetHeight",&AnimatedSprite::GetHeight);
    ClassRegister<AnimatedSprite>::RegisterClassMethod(L,"AnimatedSprite","GetFrameWidth",&AnimatedSprite::GetFrameWidth);
    ClassRegister<AnimatedSprite>::RegisterClassMethod(L,"AnimatedSprite","GetFrameHeight",&AnimatedSprite::GetFrameHeight);
    ClassRegister<AnimatedSprite>::RegisterClassMethod(L,"AnimatedSprite","SetFrameCount",&AnimatedSprite::SetFrameCount);
    ClassRegister<AnimatedSprite>::RegisterClassMethod(L,"AnimatedSprite","GetFrameCount",&AnimatedSprite::GetFrameCount);
    ClassRegister<AnimatedSprite>::RegisterClassMethod(L,"AnimatedSprite","SetFrameTime",&AnimatedSprite::SetFrameTime);
    ClassRegister<AnimatedSprite>::RegisterClassMethod(L,"AnimatedSprite","GetFrameTime",&AnimatedSprite::GetFrameTime);
    ClassRegister<AnimatedSprite>::RegisterClassMethod(L,"AnimatedSprite","IsLoaded",&AnimatedSprite::IsLoaded);
    ClassRegister<AnimatedSprite>::RegisterClassMethod(L,"AnimatedSprite","SetCenter",&AnimatedSprite::SetCenter);
    ClassRegister<AnimatedSprite>::RegisterClassMethod(L,"AnimatedSprite","GetCenter",&AnimatedSprite::GetCenter);
    ClassRegister<AnimatedSprite>::RegisterClassMethod(L,"AnimatedSprite","SetRepeatTimes",&AnimatedSprite::SetRepeatTimes);
    ClassRegister<AnimatedSprite>::RegisterClassMethod(L,"AnimatedSprite","GetRepeatTimes",&AnimatedSprite::GetRepeatTimes);
    ClassRegister<AnimatedSprite>::RegisterClassMethod(L,"AnimatedSprite","SetScaleX",&AnimatedSprite::SetScaleX);
    ClassRegister<AnimatedSprite>::RegisterClassMethod(L,"AnimatedSprite","SetScaleY",&AnimatedSprite::SetScaleY);
    ClassRegister<AnimatedSprite>::RegisterClassMethod(L,"AnimatedSprite","SetScale",&AnimatedSprite::SetScale);
    ClassRegister<AnimatedSprite>::RegisterClassMethod(L,"AnimatedSprite","GetScale",&AnimatedSprite::GetScale);
    ClassRegister<AnimatedSprite>::RegisterClassMethod(L,"AnimatedSprite","ReBlend",&AnimatedSprite::ReBlend);
    ClassRegister<AnimatedSprite>::RegisterClassMethod(L,"AnimatedSprite","SetAlpha",&AnimatedSprite::SetAlpha);
    ClassRegister<AnimatedSprite>::RegisterClassMethod(L,"AnimatedSprite","GetAlpha",&AnimatedSprite::GetAlpha);
    ClassRegister<AnimatedSprite>::RegisterClassMethod(L,"AnimatedSprite","SetFlip",&AnimatedSprite::SetFlip);
    ClassRegister<AnimatedSprite>::RegisterClassMethod(L,"AnimatedSprite","GetFlip",&AnimatedSprite::GetFlip);
    ClassRegister<AnimatedSprite>::RegisterClassMethod(L,"AnimatedSprite","GetMe",&AnimatedSprite::GetMe);
    ClassRegister<AnimatedSprite>::RegisterClassMethod(L,"AnimatedSprite","SetGridSize",&AnimatedSprite::SetGridSize);
    ClassRegister<AnimatedSprite>::RegisterClassMethod(L,"AnimatedSprite","Pause",&AnimatedSprite::Pause);
    ClassRegister<AnimatedSprite>::RegisterClassMethod(L,"AnimatedSprite","IsAnimationEnd",&AnimatedSprite::IsAnimationEnd);
    ClassRegister<AnimatedSprite>::RegisterClassMethod(L,"AnimatedSprite","IsFrameEnd",&AnimatedSprite::IsFrameEnd);
    ClassRegister<AnimatedSprite>::RegisterClassMethod(L,"AnimatedSprite","SetAnimation",&AnimatedSprite::SetAnimation,-1.0f,0,0);
    ClassRegister<AnimatedSprite>::RegisterClassMethod(L,"AnimatedSprite","ResetAnimation",&Animation::ResetAnimation);

    TypeObserver<AnimatedSprite,bool>::RegisterMethod(LuaManager::L,"CanRepeat",&AnimatedSprite::CanRepeat);
    TypeObserver<AnimatedSprite,uint32_t>::RegisterMethod(LuaManager::L,"Loops",&AnimatedSprite::Loops);
    TypeObserver<AnimatedSprite,uint32_t>::RegisterMethod(LuaManager::L,"sprX",&AnimatedSprite::sprX);
    TypeObserver<AnimatedSprite,uint32_t>::RegisterMethod(LuaManager::L,"sprY",&AnimatedSprite::sprY);
    TypeObserver<AnimatedSprite,uint32_t>::RegisterMethod(LuaManager::L,"sprW",&AnimatedSprite::sprW);
    TypeObserver<AnimatedSprite,uint32_t>::RegisterMethod(LuaManager::L,"sprH",&AnimatedSprite::sprH);
    TypeObserver<AnimatedSprite,uint32_t>::RegisterMethod(LuaManager::L,"MaxFrames",&AnimatedSprite::MaxFrames);
    TypeObserver<AnimatedSprite,uint32_t>::RegisterMethod(LuaManager::L,"LastFrame",&AnimatedSprite::LastFrame);
    TypeObserver<AnimatedSprite,float>::RegisterMethod(LuaManager::L,"SprDelay",&AnimatedSprite::SprDelay);
    TypeObserver<AnimatedSprite,float>::RegisterMethod(LuaManager::L,"SprMaxDelay",&AnimatedSprite::SprMaxDelay);
}


#endif
