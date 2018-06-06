#pragma once
//------------------------------------------------------------------------------
/**
	BaseComponent

	Components derive from this class.

	(C) 2018 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "game/entity.h"
#include "core/refcounted.h"
#include "componentmacros.h"
#include "util/bitfield.h"
#include "game/attr/attrid.h"
#include "game/attr/attributedefinition.h"

namespace Game
{

enum ComponentEvent
{
	OnBeginFrame	= 0,
	OnRender		= 1,
	OnEndFrame		= 2,
	OnRenderDebug	= 3,
	NumEvents		= 4
};

class BaseComponent : public Core::RefCounted
{
	__DeclareClass(BaseComponent)
public:
	BaseComponent();
	~BaseComponent();

	/// Returns a bitfield containing the events this component is subscribed to.
	const Util::BitField<ComponentEvent::NumEvents>& SubscribedEvents() const;

	/// Registers an entity to this component. Will try to reuse old datablocks (clearing them first)
	/// within componentData and create new data if no free id is available.
	virtual void RegisterEntity(const Entity& entity);

	/// Unregisters an entity from this component. The data will still exist in the buffer until Optimize() is called.
	/// Note that even though this keeps the data intact, registering the same entity again won't result in the same data being used.
	virtual void UnregisterEntity(const Entity& entity);

	/// returns an index to the instance data within the data buffer.
	virtual uint32_t GetInstance(const Entity& entity) const;

	/// returns a instances owner entity id
	virtual Entity GetOwner(const uint32_t& instance) const;

	/// perform garbage collection. Returns number of erased instances.
	virtual SizeT Optimize();

	/// Returns an attribute value as a variant from index. Needs to be overloaded in subclass.
	virtual Util::Variant GetAttributeValue(uint32_t instance, IndexT attributeIndex) const;
	
	/// Returns an attribute value as a variant from attribute id. Needs to be overloaded in subclass.
	virtual Util::Variant GetAttributeValue(uint32_t instance, Attr::AttrId attributeId) const;
	
	/// Returns attribute id at index.
	const Attr::AttrId& GetAttributeId(IndexT index) const;

	/// Returns an array with all attribute ids for this component
	const Util::FixedArray<Attr::AttrId>& GetAttributeIds() const;

	/// called at beginning of frame
	virtual void OnBeginFrame();

	/// called before rendering happens
	virtual void OnRender();

	/// called when game debug visualization is on
	virtual void OnRenderDebug();
	
protected:
	Util::BitField<ComponentEvent::NumEvents> events;

	Util::FixedArray<Attr::AttrId> attributes;
};

} // namespace Game