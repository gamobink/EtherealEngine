#pragma once
#include "../../Ecs/entityx/Entity.h"
#include "../../System/Application.h"
#include "../../Ecs/World.h"
#include "Core/reflection/reflection.h"
#include "Core/serialization/serialization.h"
#include "Core/serialization/cereal/types/vector.hpp"

namespace entityx
{

SAVE(Entity)
{
	auto& app = Singleton<Application>::getInstance();
	auto& world = app.getWorld();
	auto id = obj.id().index();
	ar(
		cereal::make_nvp("entity_id", id)
	);
	auto it = world.serializationMap.find(id);
	if (it == world.serializationMap.end())
	{
		world.serializationMap[id] = obj;
		ar(
			cereal::make_nvp("name", obj.getName()),
			cereal::make_nvp("components", obj.all_components())
		);
	}

}

LOAD(Entity)
{
	std::uint32_t id;
	std::string name;
	std::vector<ComponentHandle<Component>> components;

	ar(
		cereal::make_nvp("entity_id", id)
	);

	
	auto& app = Singleton<Application>::getInstance();
	auto& world = app.getWorld();
	auto it = world.serializationMap.find(id);
	if (it != world.serializationMap.end())
	{
		obj = it->second;
	}
	else
	{
		obj = world.entities.create();
		world.serializationMap[id] = obj;

		ar(
			cereal::make_nvp("name", name),
			cereal::make_nvp("components", components)
		);
		obj.setName(name);
		for (auto component : components)
		{
			auto component_shared = component.lock();
			obj.assign(component_shared);
			static const std::string context = "deserialized";
			component_shared->touch(context);
		}
	}

}

}