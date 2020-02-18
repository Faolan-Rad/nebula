#pragma once
//------------------------------------------------------------------------------
/**
	@class	Game::EntityManager

	Keeps track of all existing entites.

	

	(C) 2018-2020 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "core/singleton.h"
#include "ids/idgenerationpool.h"
#include "game/entity.h"
#include "game/manager.h"
#include "util/delegate.h"
#include "game/database/database.h"

namespace Attr
{
__DeclareAttribute(Owner, Game::Entity, 'OWNR', Game::Entity::Invalid());
}

namespace Game
{

class EntityManager : public Game::Manager
{
	__DeclareClass(EntityManager)
	__DeclareSingleton(EntityManager)
public:
	/// constructor
	EntityManager();
	/// destructor
	~EntityManager();

	/// Generate a new entity from a category
	Entity CreateEntity();
	
	/// Create n amount of entities at the same time.
	Util::Array<Entity> CreateEntities(uint n);

	/// Delete an entity.
	void DeleteEntity(const Entity& e);
	
	/// Check if an entity ID is still valid.
	bool IsValid(const Entity& e) const;

	/// Returns number of active entities
	uint GetNumEntities() const;

	/// Returns the world db
	Ptr<Game::Database> GetWorldDatabase() const;

	/// Invalidates all entities and essentially resets the manager.
	void InvalidateAllEntities();

	/// Register a deletion callback to an entity
	void RegisterDeletionCallback(const Entity& e, Util::Delegate<void(Entity)> const& del);
	
	/// Deregister a deletion callback to an entity. Note that this is not super fast.
	void DeregisterDeletionCallback(const Entity& e, Util::Delegate<void(Entity)> const& del);
private:
	friend class CategoryManager;

	/// Generation pool
	Ids::IdGenerationPool pool;

	/// Number of entities alive
	SizeT numEntities;

	/// Contains all callbacks for deletion to components for each entity
	Util::HashTable<Entity, Util::Array<Util::Delegate<void(Entity)>>> deletionCallbacks;

	/// Contains the entire world database
	Ptr<Game::Database> worldDatabase;
};

//------------------------------------------------------------------------------
/**
*/
inline bool
EntityManager::IsValid(const Entity& e) const
{
	return this->pool.IsValid(e.id);
}

//------------------------------------------------------------------------------
/**
*/
inline uint
EntityManager::GetNumEntities() const
{
	return this->numEntities;
}

//------------------------------------------------------------------------------
/**
*/
inline Ptr<Game::Database>
EntityManager::GetWorldDatabase() const
{
	return this->worldDatabase;
}

} // namespace Game