#pragma once

namespace StoneEngine::System
{
	class ResourceHandle;

	template <class TResource>
	class Resource
	{
	public:
		[[nodiscard]] ResourceHandle GetResource() const;
		void AllocateResource() const;
	};
}
