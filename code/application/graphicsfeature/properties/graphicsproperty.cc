//------------------------------------------------------------------------------
//  graphicsproperty.cc
//  (C) 2020 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "application/stdneb.h"
#include "graphicsproperty.h"
#include "basegamefeature/managers/entitymanager.h"
#include "basegamefeature/properties/transformableproperty.h"
#include "models/modelcontext.h"
#include "graphics/graphicsserver.h"
#include "visibility/visibilitycontext.h"

namespace Attr
{
__DefineAttribute(ModelResource);
}


namespace GraphicsFeature
{

__ImplementClass(GraphicsFeature::GraphicsProperty, 'GFXP', Game::Property);

//------------------------------------------------------------------------------
/**
*/
GraphicsProperty::GraphicsProperty()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
GraphicsProperty::~GraphicsProperty()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
GraphicsProperty::Init()
{
	this->data = {
		// Create a entity state buffer. Other properties should only get the state.
		Game::CreatePropertyState<State>(this->category, "GraphicsPropertyState"_atm),
		Game::GetPropertyData<Attr::WorldTransform>(this->category),
		Game::GetPropertyData<Attr::ModelResource>(this->category)
	};
}

//------------------------------------------------------------------------------
/**
*/
void
GraphicsProperty::OnActivate(Game::InstanceId instance)
{
	auto gfxEntity = Graphics::CreateEntity();
	this->data.state[instance.id].gfxEntity = gfxEntity;
	Models::ModelContext::RegisterEntity(gfxEntity);
	Models::ModelContext::Setup(gfxEntity, this->data.modelResource[instance.id], "NONE", nullptr);
	auto transform = this->data.worldTransform[instance.id];
	Models::ModelContext::SetTransform(gfxEntity, transform);
	Visibility::ObservableContext::RegisterEntity(gfxEntity);
	Visibility::ObservableContext::Setup(gfxEntity, Visibility::VisibilityEntityType::Model);
}

//------------------------------------------------------------------------------
/**
*/
void
GraphicsProperty::OnDeactivate(Game::InstanceId instance)
{
	Graphics::GraphicsEntityId gfxEntity = this->data.state[instance.id].gfxEntity;
	Visibility::ObservableContext::DeregisterEntity(gfxEntity);
	Models::ModelContext::DeregisterEntity(gfxEntity);
	Graphics::DestroyEntity(gfxEntity);
    this->data.state[instance.id].gfxEntity = Graphics::GraphicsEntityId::Invalid();
}

//------------------------------------------------------------------------------
/**
*/
void
GraphicsProperty::OnBeginFrame()
{
	SizeT num = Game::GetNumInstances(this->category);
    for (int i = 0; i < num; ++i)
    {
        Graphics::GraphicsEntityId gfxEntity = this->data.state[i].gfxEntity;
        if (gfxEntity != Graphics::GraphicsEntityId::Invalid())
        {
            Models::ModelContext::SetTransform(gfxEntity, this->data.worldTransform[i]);
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
GraphicsProperty::SetupExternalAttributes()
{
	SetupAttr(Attr::WorldTransform::Id());
	SetupAttr(Attr::ModelResource::Id());
}

} // namespace GraphicsFeature
