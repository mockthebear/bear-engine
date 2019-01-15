#include "luainterface.hpp"
#include "luatools.hpp"
#include "luagamestate.hpp"
#include "luaobject.hpp"

//Bindings

#ifndef DISABLE_LUAINTERFACE


void LuaInterface::RegisterObjects(){
    static LuaCFunctionLambda GarbageCollectorFunction = [](lua_State* La)
    {
        bear::out << "Calling gc\n";
        return 1;
    };

    ClassRegister<LuaObject>::RegisterClassOutside(LuaManager::L,"LuaObject",[](lua_State* La)
    {
        int y = GenericLuaGetter<int>::Call(La);
        int x = GenericLuaGetter<int>::Call(La);
        LuaObject *t = (LuaObject*)BearEngine->GetCurrentState().Pool.AddInstance(LuaObject(x,y));
        return t;
    });

    ClassRegister<LuaObject>::RegisterClassMethod(LuaManager::L,"LuaObject","Destroy",&LuaObject::Destroy);
    ClassRegister<LuaObject>::RegisterClassMethod(LuaManager::L,"LuaObject","Kill",&LuaObject::Kill);
    ClassRegister<LuaObject>::RegisterClassMethod(LuaManager::L,"LuaObject","SetX",&LuaObject::SetX, false);
    ClassRegister<LuaObject>::RegisterClassMethod(LuaManager::L,"LuaObject","SetY",&LuaObject::SetY, false);
    ClassRegister<LuaObject>::RegisterClassMethod(LuaManager::L,"LuaObject","GetY",&LuaObject::GetY);
    ClassRegister<LuaObject>::RegisterClassMethod(LuaManager::L,"LuaObject","GetX",&LuaObject::GetX);
    ClassRegister<LuaObject>::RegisterClassMethod(LuaManager::L,"LuaObject","GetSolid",&LuaObject::GetSolid);
    ClassRegister<LuaObject>::RegisterClassMethod(LuaManager::L,"LuaObject","SetSolid",&LuaObject::SetSolid, false);
    ClassRegister<LuaObject>::RegisterClassMethod(LuaManager::L,"LuaObject","SetWidth",&LuaObject::SetWidth, false);
    ClassRegister<LuaObject>::RegisterClassMethod(LuaManager::L,"LuaObject","GetWidth",&LuaObject::GetWidth);
    ClassRegister<LuaObject>::RegisterClassMethod(LuaManager::L,"LuaObject","SetHeight",&LuaObject::SetHeight, false);
    ClassRegister<LuaObject>::RegisterClassMethod(LuaManager::L,"LuaObject","GetHeight",&LuaObject::GetHeight);
    ClassRegister<LuaObject>::RegisterClassMethod(LuaManager::L,"LuaObject","SetBox",&LuaObject::SetBox, false);
    ClassRegister<LuaObject>::RegisterClassMethod(LuaManager::L,"LuaObject","GetBox",&LuaObject::GetBox);
    ClassRegister<LuaObject>::RegisterClassMethod(LuaManager::L,"LuaObject","GetPoolIndex",&LuaObject::GetPoolIndex);
    ClassRegister<LuaObject>::RegisterClassMethod(LuaManager::L,"LuaObject","GetMyRef",&LuaObject::GetMyRef);
    ClassRegister<LuaObject>::RegisterClassMethod(LuaManager::L,"LuaObject","GetMyObj",&LuaObject::GetMyObj);


    ClassRegister<LuaObject>::RegisterClassMethod(LuaManager::L,"LuaObject","SetSpeed",&LuaObject::SetSpeed, false);
    ClassRegister<LuaObject>::RegisterClassMethod(LuaManager::L,"LuaObject","GetSpeed",&LuaObject::GetSpeed);

    ClassRegister<LuaObject>::RegisterClassMethod(LuaManager::L,"LuaObject","SetStep",&LuaObject::SetStep, false);
    ClassRegister<LuaObject>::RegisterClassMethod(LuaManager::L,"LuaObject","GetStep",&LuaObject::GetStep);

    ClassRegister<LuaObject>::RegisterClassMethod(LuaManager::L,"LuaObject","SetData",&LuaObject::SetData);
    ClassRegister<LuaObject>::RegisterClassMethod(LuaManager::L,"LuaObject","GetData",&LuaObject::GetData);


    ClassRegister<LuaObject>::RegisterClassMethod(LuaManager::L,"LuaObject","NotifyDamage"        ,&LuaObject::NotifyDamage);
    ClassRegister<LuaObject>::RegisterClassMethod(LuaManager::L,"LuaObject","NotifyCollision"        ,&LuaObject::NotifyCollision);
    ClassRegister<LuaObject>::RegisterClassMethod(LuaManager::L,"LuaObject","Render"        ,&LuaObject::Render);
    ClassRegister<LuaObject>::RegisterClassMethod(LuaManager::L,"LuaObject","Update"        ,&LuaObject::Update);
    ClassRegister<LuaObject>::RegisterClassMethod(LuaManager::L,"LuaObject","IsDead"        ,&LuaObject::IsDead);
    ClassRegister<LuaObject>::RegisterClassMethod(LuaManager::L,"LuaObject","Is"        ,&LuaObject::Is);

    ClassRegister<LuaObject>::RegisterClassMethod(LuaManager::L,"LuaObject","hasPerspective",&LuaObject::hasPerspective);

    TypeObserver<LuaObject,bool>::RegisterMethod(LuaManager::L,"solid",&LuaObject::solid);
    TypeObserver<LuaObject,Point>::RegisterMethod(LuaManager::L,"speed",&LuaObject::speed);
    TypeObserver<LuaObject,Point>::RegisterMethod(LuaManager::L,"step",&LuaObject::step);
    TypeObserver<LuaObject,bool>::RegisterMethod(LuaManager::L,"forceUpdate",&LuaObject::forceUpdate);
    TypeObserver<LuaObject,bool>::RegisterMethod(LuaManager::L,"active",&LuaObject::Active);
    TypeObserver<LuaObject,bool>::RegisterMethod(LuaManager::L,"forceRender",&LuaObject::forceRender);
    TypeObserver<LuaObject,int>::RegisterMethod(LuaManager::L,"perspective",&LuaObject::perspective);

    ClassRegister<LuaGameState>::RegisterClassOutside(LuaManager::L,"LuaGameState",[](lua_State* La)
    {
        LuaGameState *t = new LuaGameState();
        return t;
    });

    ClassRegister<LuaGameState>::RegisterClassMethod(LuaManager::L,"LuaGameState","Setup",&LuaGameState::Setup);

    TypeObserver<LuaGameState,bool>::RegisterMethod(LuaManager::L,"canClose",&LuaGameState::canClose);


    ClassRegister<GameObject>::RegisterClassVirtual(LuaManager::L,"GameObject",[](lua_State* La)
    {
        uint64_t y = GenericLuaGetter<uint64_t>::Call(La);
        GameObject *t = (GameObject*)y;
        return t;
    },&GarbageCollectorFunction);

    GlobalMethodRegister::RegisterGlobalTable(LuaManager::L,"g_state");
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_state","Signal",std::function< bool (SignalRef)>([](SignalRef sr){
        return Game::GetCurrentState().Signal(sr);
    }));


    ClassRegister<GameObject>::RegisterClassMethod(LuaManager::L,"GameObject","Is",&GameObject::Is);
    ClassRegister<GameObject>::RegisterClassMethod(LuaManager::L,"GameObject","GetMyRef",&GameObject::GetMyRef);
    ClassRegister<GameObject>::RegisterClassMethod(LuaManager::L,"GameObject","GetHash",&GameObject::GetHash);
    ClassRegister<GameObject>::RegisterClassMethod(LuaManager::L,"GameObject","SetX",&GameObject::SetX, false);
    ClassRegister<GameObject>::RegisterClassMethod(LuaManager::L,"GameObject","SetY",&GameObject::SetY, false);
    ClassRegister<GameObject>::RegisterClassMethod(LuaManager::L,"GameObject","GetY",&GameObject::GetY);
    ClassRegister<GameObject>::RegisterClassMethod(LuaManager::L,"GameObject","GetX",&GameObject::GetX);
    ClassRegister<GameObject>::RegisterClassMethod(LuaManager::L,"GameObject","SetWidth",&GameObject::SetWidth, false);
    ClassRegister<GameObject>::RegisterClassMethod(LuaManager::L,"GameObject","GetWidth",&GameObject::GetWidth);
    ClassRegister<GameObject>::RegisterClassMethod(LuaManager::L,"GameObject","SetHeight",&GameObject::SetHeight, false);
    ClassRegister<GameObject>::RegisterClassMethod(LuaManager::L,"GameObject","GetHeight",&GameObject::GetHeight);
    ClassRegister<GameObject>::RegisterClassMethod(LuaManager::L,"GameObject","SetBox",&GameObject::SetBox, false);
    ClassRegister<GameObject>::RegisterClassMethod(LuaManager::L,"GameObject","GetBox",&GameObject::GetBox);
    ClassRegister<GameObject>::RegisterClassMethod(LuaManager::L,"GameObject","GetSolid",&GameObject::GetSolid);
    ClassRegister<GameObject>::RegisterClassMethod(LuaManager::L,"GameObject","SetSolid",&GameObject::SetSolid, false);
    ClassRegister<GameObject>::RegisterClassMethod(LuaManager::L,"GameObject","Kill",&GameObject::Kill);

    ClassRegister<GameObject>::RegisterClassMethod(LuaManager::L,"GameObject","SetSpeed",&GameObject::SetSpeed, false);
    ClassRegister<GameObject>::RegisterClassMethod(LuaManager::L,"GameObject","GetSpeed",&GameObject::GetSpeed);

    ClassRegister<GameObject>::RegisterClassMethod(LuaManager::L,"GameObject","SetStep",&GameObject::SetStep, false);
    ClassRegister<GameObject>::RegisterClassMethod(LuaManager::L,"GameObject","GetStep",&GameObject::GetStep);

    ClassRegister<GameObject>::RegisterClassMethod(LuaManager::L,"GameObject","SetData",&GameObject::SetData);
    ClassRegister<GameObject>::RegisterClassMethod(LuaManager::L,"GameObject","GetData",&GameObject::GetData);

    ClassRegister<GameObject>::RegisterClassMethod(LuaManager::L,"GameObject","hasPerspective"        ,&GameObject::hasPerspective);
    ClassRegister<GameObject>::RegisterClassMethod(LuaManager::L,"GameObject","canForceRender"        ,&GameObject::canForceRender);
    ClassRegister<GameObject>::RegisterClassMethod(LuaManager::L,"GameObject","canForceUpdate"        ,&GameObject::canForceUpdate);


    ClassRegister<GameObject>::RegisterClassMethod(LuaManager::L,"GameObject","NotifyDamage"        ,&GameObject::NotifyDamage);
    ClassRegister<GameObject>::RegisterClassMethod(LuaManager::L,"GameObject","NotifyCollision"        ,&GameObject::NotifyCollision);
    ClassRegister<GameObject>::RegisterClassMethod(LuaManager::L,"GameObject","Render"        ,&GameObject::Render);
    ClassRegister<GameObject>::RegisterClassMethod(LuaManager::L,"GameObject","Update"        ,&GameObject::Update);
    ClassRegister<GameObject>::RegisterClassMethod(LuaManager::L,"GameObject","IsDead"        ,&GameObject::IsDead);

    TypeObserver<GameObject,bool>::RegisterMethod(LuaManager::L,"solid",&GameObject::solid);

    TypeObserver<GameObject,Point>::RegisterMethod(LuaManager::L,"step",&GameObject::step);


    LambdaRegister(L,"Typeof",std::function<int(std::string)>([](std::string objName){
        return Types::GetInstance().getTypeId(objName);
    }) );

    LambdaRegister(L,"Typename",std::function<std::string(int)>([](int objId){
        return Types::GetInstance().getTypeName(objId);
    }) );



}

void LuaInterface::RegisterUI(){
    GlobalMethodRegister::RegisterGlobalTable(LuaManager::L,"g_ui");

    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_ui","AddWindow",std::function<void(LuaUi *)>([](LuaUi *ui)
    {
        BearEngine->GetCurrentState().AddWindow(ui);
    }));

    ClassRegister<LuaUi>::RegisterClassOutside(LuaManager::L,"LuaUi",[](lua_State* La)
    {
        LuaUi *t = new LuaUi();
        //BearEngine->GetCurrentState().AddWindow(t);
        return t;
    },nullptr);

    ClassRegister<LuaUi>::RegisterClassMethod(LuaManager::L,"LuaUi","SetFocused",&LuaUi::SetFocused);
    ClassRegister<LuaUi>::RegisterClassMethod(LuaManager::L,"LuaUi","SetAlpha",&LuaUi::SetAlpha);
    ClassRegister<LuaUi>::RegisterClassMethod(LuaManager::L,"LuaUi","Close",&LuaUi::Close);
    ClassRegister<LuaUi>::RegisterClassMethod(LuaManager::L,"LuaUi","SetPosition",&LuaUi::SetPosition);
    ClassRegister<LuaUi>::RegisterClassMethod(LuaManager::L,"LuaUi","SetText",&LuaUi::SetTextStr);
    ClassRegister<LuaUi>::RegisterClassMethod(LuaManager::L,"LuaUi","SetTextObj",&LuaUi::SetTextObj);
    ClassRegister<LuaUi>::RegisterClassMethod(LuaManager::L,"LuaUi","GetText",&LuaUi::GetText);
    ClassRegister<LuaUi>::RegisterClassMethod(LuaManager::L,"LuaUi","IsDead",&LuaUi::IsDead);
    ClassRegister<LuaUi>::RegisterClassMethod(LuaManager::L,"LuaUi","Is",&LuaUi::Is);
    ClassRegister<LuaUi>::RegisterClassMethod(LuaManager::L,"LuaUi","IsInside",&LuaUi::IsInside);
    ClassRegister<LuaUi>::RegisterClassMethod(LuaManager::L,"LuaUi","Hide",&LuaUi::Hide);
    ClassRegister<LuaUi>::RegisterClassMethod(LuaManager::L,"LuaUi","Show",&LuaUi::Show);
    ClassRegister<LuaUi>::RegisterClassMethod(LuaManager::L,"LuaUi","Refresh",&LuaUi::Refresh);
    ClassRegister<LuaUi>::RegisterClassMethod(LuaManager::L,"LuaUi","SetFocused",&LuaUi::SetFocused);
    ClassRegister<LuaUi>::RegisterClassMethod(LuaManager::L,"LuaUi","AddComponent",&LuaUi::AddComponent2);
    ClassRegister<LuaUi>::RegisterClassMethod(LuaManager::L,"LuaUi","GetX",&LuaUi::GetX);
    ClassRegister<LuaUi>::RegisterClassMethod(LuaManager::L,"LuaUi","GetY",&LuaUi::GetY);
    ClassRegister<LuaUi>::RegisterClassMethod(LuaManager::L,"LuaUi","GetTrueX",&LuaUi::GetTrueX);
    ClassRegister<LuaUi>::RegisterClassMethod(LuaManager::L,"LuaUi","GetTrueY",&LuaUi::GetTrueY);
    ClassRegister<LuaUi>::RegisterClassMethod(LuaManager::L,"LuaUi","SetX",&LuaUi::SetX, false);
    ClassRegister<LuaUi>::RegisterClassMethod(LuaManager::L,"LuaUi","SetY",&LuaUi::SetY, false);
    ClassRegister<LuaUi>::RegisterClassMethod(LuaManager::L,"LuaUi","SetBox",&LuaUi::SetBox, false);
    ClassRegister<LuaUi>::RegisterClassMethod(LuaManager::L,"LuaUi","GetBox",&LuaUi::GetBox);
    ClassRegister<LuaUi>::RegisterClassMethod(LuaManager::L,"LuaUi","GetScreenY",&LuaUi::GetScreenY);
    ClassRegister<LuaUi>::RegisterClassMethod(LuaManager::L,"LuaUi","GetScreenX",&LuaUi::GetScreenX);
    ClassRegister<LuaUi>::RegisterClassMethod(LuaManager::L,"LuaUi","SetWidth",&LuaUi::SetWidth , false);
    ClassRegister<LuaUi>::RegisterClassMethod(LuaManager::L,"LuaUi","GetWidth",&LuaUi::GetWidth);
    ClassRegister<LuaUi>::RegisterClassMethod(LuaManager::L,"LuaUi","SetHeight",&LuaUi::SetHeight, false);
    ClassRegister<LuaUi>::RegisterClassMethod(LuaManager::L,"LuaUi","GetHeight",&LuaUi::GetHeight);
    ClassRegister<LuaUi>::RegisterClassMethod(LuaManager::L,"LuaUi","SetId",&LuaUi::SetId);
    ClassRegister<LuaUi>::RegisterClassMethod(LuaManager::L,"LuaUi","GetId",&LuaUi::GetId);
    ClassRegister<LuaUi>::RegisterClassMethod(LuaManager::L,"LuaUi","GetLastComponent",&LuaUi::GetLastComponent_Lua);
    ClassRegister<LuaUi>::RegisterClassMethod(LuaManager::L,"LuaUi","GetChildById",&LuaUi::GetChildById_Lua);
    ClassRegister<LuaUi>::RegisterClassMethod(LuaManager::L,"LuaUi","GetMother",&LuaUi::GetMother);
    ClassRegister<LuaUi>::RegisterClassMethod(LuaManager::L,"LuaUi","GetChilds",&LuaUi::GetChilds);
    ClassRegister<LuaUi>::RegisterClassMethod(LuaManager::L,"LuaUi","SetAsMain",&LuaUi::SetAsMain);
    ClassRegister<LuaUi>::RegisterClassMethod(LuaManager::L,"LuaUi","Destroy",&LuaUi::Destroy);
}

#endif // DISABLE_LUAINTERFACE

