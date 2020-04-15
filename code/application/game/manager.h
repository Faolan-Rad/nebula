#pragma once
//------------------------------------------------------------------------------
/**
    @class Game::Manager
    
    Managers are Singleton objects which care about some "specific global 
    stuff". They should be subclassed by applications to implement
    globals aspects of the application (mainly global game play related).
    
    Managers are created and triggered by game features. The frame trigger
    functions are invoked when the gameserver triggers the game feature.
    
    Standard-Nebula uses several managers to offer timing information
    (TimeManager), create components (ComponentManager), 
    manage game entities (EntityManager) and so forth.

    (C) 2007 Radon Labs GmbH
    (C) 2013-2020 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"

//------------------------------------------------------------------------------
namespace Game
{

ID_32_TYPE(ManagerHandle)

struct ManagerAPI
{
    /// called when attached to game server
    void(*OnActivate)() = nullptr;
    /// called when removed from game server
    void(*OnDeactivate)() = nullptr;
    /// called before frame by the game server
    void(*OnBeginFrame)() = nullptr;
    /// called per-frame by the game server
    void(*OnFrame)() = nullptr;
    /// called after frame by the game server
    void(*OnEndFrame)() = nullptr;
    /// called after loading game state
    void(*OnLoad)() = nullptr;
    /// called before saving game state
    void(*OnSave)() = nullptr;
    /// called by Game::Server::Start()
    void(*OnStart)() = nullptr;
    /// render a debug visualization 
    void(*OnRenderDebug)() = nullptr;
};

//class Manager : public Core::RefCounted
//{
//    __DeclareClass(Manager)
//public:
//    /// constructor
//    Manager();
//    /// destructor
//    virtual ~Manager();
//
//    /// called when attached to game server
//    virtual void OnActivate();
//    /// called when removed from game server
//    virtual void OnDeactivate();
//    /// return true if currently active
//    bool IsActive() const;
//    /// called before frame by the game server
//    virtual void OnBeginFrame();
//    /// called per-frame by the game server
//    virtual void OnFrame();
//    /// called after frame by the game server
//    virtual void OnEndFrame();
//    /// called after loading game state
//    virtual void OnLoad();
//    /// called before saving game state
//    virtual void OnSave();
//    /// called by Game::Server::Start()
//    virtual void OnStart();
//    /// render a debug visualization 
//    virtual void OnRenderDebug();
//
//private:
//    bool isActive;
//};

//------------------------------------------------------------------------------
/**
*/
//inline bool
//Manager::IsActive() const
//{
//    return this->isActive;
//}

}; // namespace Game
//------------------------------------------------------------------------------
 
    
