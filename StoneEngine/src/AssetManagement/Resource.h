#pragma once

namespace StoneEngine::AssetManagement
{
	class FixedSizeMemoryPool;
	class DynamicMemoryPool;

	class IResource
	{
	public:
		virtual ~IResource() = default;

		virtual void Load() = 0;
	};

	/// <summary>
	/// This is a templated Resource class that initializes a cache and object pool
	/// for each ResourceType. This allows all resources of the same type to be
	/// contiguous in memory
	/// </summary>
	/// <typeparam name="TResource"></typeparam>
	/// <typeparam name="cacheSize">The fixed size of memory for cache in bytes</typeparam>
	/// <typeparam name="initialPoolSize">The initial size of the dynamic pool sze in bytes</typeparam>
	template <class TResource, size_t cacheSize = 10, size_t initialPoolSize = 100>
	class Resource : public IResource
	{
		using TCache = FixedSizeMemoryPool<cacheSize>;
		using TObjectPool = DynamicMemoryPool<initialPoolSize>;

		static TCache Cache;
		static TObjectPool Pool;
	};

	class Shader : public Resource<Shader, 10, 20>
	{

	};
}
