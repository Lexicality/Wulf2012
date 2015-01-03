//
// WulfGame/Game/EntityManager.h
// Copyright (C) 2012 Lexi Robinson
// This code is freely available under the MIT licence.
//
#include "WulfConstants.h"
#include "Game/Entity.h"
#include <map>
#include <vector>

namespace Wulf {
	CREATE_EXCEPTION(InvalidEntID, std::runtime_error);
	CREATE_EXCEPTION(InvalidClassname, std::logic_error);
	class EntityManager {
		struct EntFactory {
			virtual Entity* Create() = 0;
			virtual ~EntFactory() {}
		};
		template<class Derived>
		struct SpecificEntFactory : public EntityManager::EntFactory {
			Entity* Create() { return new Derived; }
			~SpecificEntFactory() {}
		};
	public:
		static EntityManager& GetInstance() { return instance; }

		void Think(double dtime);

		// Registration
		template <class E>
		void RegisterEntity(const std::string& classname)
		{
			factories[classname] = new SpecificEntFactory<E>();
		}

		Entity* CreateEntity(const std::string& classname, Vector pos, Vector heading);

		Entity* GetEntity(EntID entnum) const;

		void PurgeEntities();
		~EntityManager();

	private:
		// Ent management
		std::map<std::string, EntFactory*> factories;
		std::vector<Entity*> ents;
		// Instancing
		static EntityManager instance;
		EntityManager() {}
		EntityManager(const EntityManager& other);
		EntityManager& operator=(const EntityManager& other);
	};
}
