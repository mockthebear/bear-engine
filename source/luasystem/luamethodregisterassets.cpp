#include "luainterface.hpp"
#include "luatools.hpp"


#ifndef DISABLE_LUAINTERFACE
#include "../sound/sound.hpp"
#include "../engine/sprite.hpp"
#include "../framework/resourcemanager.hpp"


void LuaInterface::RegisterSpriteEffectClass(){
    GlobalMethodRegister::RegisterGlobalTable(LuaManager::L,"g_SpriteEffects");
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_SpriteEffects","CreateAnimatedSprite",std::function< int (Point,Point,std::string,int,int,int)>([](Point pos,Point speed,std::string spritePath,int amount,int delay,int repeat){
        SpriteEffect *p = SpriteEffectCreator::CreateAnimatedSprite(pos.x,pos.y,speed.x,speed.y,spritePath,amount,delay,repeat);
        if (!p){
            return -1;
        }
        return p->poolIndex;
    }),0,1.0,1);

    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_SpriteEffects","SetDepth",std::function<bool(int,int)>([](int part,int deph){
        SpriteEffect* p = (SpriteEffect*)Game::GetCurrentState().SpriteEffectPool->GetInstance(part);
        if (!p){
            return false;
        }
        p->Depth = deph;
        return true;
    }));
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
    ClassRegister<Animation>::RegisterClassMethod(L,"Animation","SetAnimationTime",&Animation::SetAnimationTime);
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
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","ResetAnimation",&Sprite::ResetAnimation);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","SetFrame",&Sprite::SetFrame,0,0);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","GetFrame",&Sprite::GetFrame);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","GetWidth",&Sprite::GetWidth);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","GetHeight",&Sprite::GetHeight);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","GetFrameWidth",&Sprite::GetFrameWidth);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","GetFrameHeight",&Sprite::GetFrameHeight);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","SetFrameCount",&Sprite::SetFrameCount);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","GetFrameCount",&Sprite::GetFrameCount);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","SetFrameTime",&Sprite::SetFrameTime);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","GetFrameTime",&Sprite::GetFrameTime);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","IsLoaded",&Sprite::IsLoaded);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","SetCenter",&Sprite::SetCenter);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","GetCenter",&Sprite::GetCenter);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","SetRepeatTimes",&Sprite::SetRepeatTimes);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","GetRepeatTimes",&Sprite::GetRepeatTimes);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","IsAnimationOver",&Sprite::IsAnimationOver);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","SetScaleX",&Sprite::SetScaleX);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","SetScaleY",&Sprite::SetScaleY);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","SetScale",&Sprite::SetScale);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","GetScale",&Sprite::GetScale);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","ReBlend",&Sprite::ReBlend);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","SetAlpha",&Sprite::SetAlpha);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","GetAlpha",&Sprite::GetAlpha);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","SetGrid",&Sprite::SetGrid);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","GetGrid",&Sprite::GetGrid);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","SetFlip",&Sprite::SetFlip);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","GetFlip",&Sprite::GetFlip);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","GetMe",&Sprite::GetMe);
}


#endif
