#pragma once
#include <concepts>
#include "typedefs.h"


namespace StoneEngine
{
	template<typename T>
	concept HasUpdate = requires(T t, U64 deltaTimeMicroseconds) {
		{ t.Update(deltaTimeMicroseconds) } -> std::same_as<void>;
	};

	/// <summary>
	/// This class represents the core engine loop
	/// This is responsible for calling update on any system that is registered on it
	/// Any application using the engine can register components and CoreLoop
	/// will handle calling update on them all
	/// </summary>
	class CoreLoop
	{
	public:
		void Update(U64 frameTime);

		template <class TComponent>
		void RegisterComponent(TComponent* component)
		{
			static_assert(HasUpdate<TComponent>, "RegisterComponent can only take classes that implement Update(uint64_t)");
		}
	};

}

