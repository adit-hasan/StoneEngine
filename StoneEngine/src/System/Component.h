#pragma once

namespace StoneEngine::System
{
	template<class T>
	class View;

	template <class TComponent>
	class Component
	{
	public:
		View<TComponent> GetView();
	};
}