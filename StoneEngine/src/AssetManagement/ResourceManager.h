#pragma once

#include "Core/Utils.h"

namespace StoneEngine::AssetManagement
{
	class Resource;
	class Resource::Handle;

	class ResourceManager final
	{
	public:
		DISALLOW_COPY_AND_MOVE(ResourceManager);

		template<class TResource>
		std::shared_ptr<TResource> Get() const;

		template<class TResource>
		Resource::Handle Add() const;

	private:
		std::map<
	};
}
