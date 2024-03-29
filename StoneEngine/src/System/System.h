#pragma once

#include "pch.h"

#include "Resource.h"
#include "Component.h"

namespace StoneEngine::System
{
	template<class T>
	concept ComponentType = std::is_base_of_v<Component<T>, T>;

	template<class T>
	concept ResourceType = std::is_base_of_v<Resource<T>, T>;

	template<class T>
	concept SystemDataType = ComponentType<T> || ResourceType<T>;

	template<class... Ts>
	concept SystemDataTypes = (SystemDataType<Ts> && ...);

	template<class... Ts> requires SystemDataTypes<Ts...>
	class Input 
	{
	};

	template<class... Ts> requires SystemDataTypes<Ts...>
	class Output
	{
	};

	// Both inputs and outputs have to only contain either Resource type or ComponentType
	template <class... Ts>
	class System
	{
		static_assert((std::is_same_v<Input<Ts...>, Input<Ts...>> && ...) || sizeof...(Ts) == 0,
			"Only one occurrence of Input is allowed");

		static_assert((std::is_same_v<Output<Ts...>, Output<Ts...>> && ...) || sizeof...(Ts) == 0,
			"Only one occurrence of Output is allowed");

		static_assert(sizeof...(Ts) <= 2), "Only upto 2 parameters are allowed");
	public:

		template <class TComponent> requires ComponentType<TComponent>
		View<TComponent> GetView() const
		{
			return new View<TComponent>();
		}

		template <class TResource> requires ResourceType<TResource>
		ResourceHandle GetResource() const
		{
			return new ResourceHandle();
		}


	};

	class PhysicsSystem : public System<Input<int, float>, Output<std::string, double>>
	{

	};
}