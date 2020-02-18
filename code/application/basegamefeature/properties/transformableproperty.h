#pragma once
//------------------------------------------------------------------------------
/**
    Game::TransformableProperty

    Handles entities that can be parented to each other, and that should move with each other.

    (C) 2020 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "game/property.h"
#include "game/database/attribute.h"
#include "basegamefeature/managers/entitymanager.h"

namespace Attr
{
__DeclareAttribute(LocalTransform, Math::matrix44, 'Lm44', Math::matrix44::identity());
__DeclareAttribute(WorldTransform, Math::matrix44, 'Wm44', Math::matrix44::identity());
__DeclareAttribute(Parent, Game::Entity , 'TFPT', Game::Entity::Invalid());
}

namespace Game
{

class TransformableProperty : public Game::Property
{
    __DeclareClass(TransformableProperty);
public:
    TransformableProperty();
    ~TransformableProperty();

    void Init() override;

    void OnActivate(Game::InstanceId instance) override;
    void OnDeactivate(Game::InstanceId instance) override;

    void SetupExternalAttributes() override;
private:
    struct Data 
    {
        Game::PropertyData<Attr::Owner> owner;
        Game::PropertyData<Attr::LocalTransform> localTransform;
        Game::PropertyData<Attr::WorldTransform> worldTransform;
        Game::PropertyData<Attr::Parent> parent;
    } data;
};

} // namespace Game
