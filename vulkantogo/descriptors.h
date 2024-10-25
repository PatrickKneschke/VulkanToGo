#pragma once


#include "vk_core.h"
#include "storage.h"

#include <span>


namespace vktg
{


    /// @brief Used to create and cache descriptor set layouts.
    class DescriptorLayoutCache {

        public:

            /// @brief Creates descriptor set layout from given create info or return matching layout from cache.
            /// @param layoutInfo Vulkan descriptor set layout create info.
            /// @return Vulkan descriptor set layout.
            vk::DescriptorSetLayout CreateLayout( vk::DescriptorSetLayoutCreateInfo *layoutInfo);
            /// @brief Creates descriptor set layout from given list of descriptor layout bindings or return matching layout from cache.
            /// @param layoutInfo List of Vulkan descriptor set layout bindings.
            /// @return Vulkan descriptor set layout.
            vk::DescriptorSetLayout CreateLayout( std::span<vk::DescriptorSetLayoutBinding> bindings);
            /// @brief Destroy all cached descriptor set layouts.
            void DestroyLayouts();

        private:

            std::unordered_map<uint64_t, vk::DescriptorSetLayout> mLayouts;
    };


    /// @brief Used to allocate descriptor sets. Creates new descriptor pools as needed.
    class DescriptorSetAllocator {

        public:

            /// @brief Initialize descriptor set allocator with large default pool sizes.
            DescriptorSetAllocator();
            /// @brief Initialize descriptor set allocator with given pool sizes.
            /// @param poolSizes List of pool sizes.
            /// @param maxSetsPerPool Maximum number of descriptor sets allocated per pool.
            DescriptorSetAllocator( std::span<vk::DescriptorPoolSize> poolSizes, uint32_t maxSetsPerPool);

            /// @brief Allocates descriptor set with given descriptor set layout,
            /// @param layout Descriptor set layout.
            /// @return Allocated Vulkan descriptor set.
            vk::DescriptorSet Allocate( vk::DescriptorSetLayout layout);
            /// @brief Resets all created descriptor pools,
            void ResetPools();
            /// @brief Destroys all created descriptor pools.
            void DestroyPools();


        private:

            /// @brief Fetches unused descriptor pool or creates new one if no unused pool is available.
            /// @return Vulkan descriptor pool.
            vk::DescriptorPool GetNewPool();


            uint32_t mMaxSetsPerPool;
            std::vector<vk::DescriptorPoolSize> mPoolSizes;

            vk::DescriptorPool mCurrPool;
            std::vector<vk::DescriptorPool> mUsedPools;
            std::vector<vk::DescriptorPool> mFreePools;
    };


    /// @brief Used to build descriptor set from submitted buffer and image descriptor infos.
    class DescriptorSetBuilder {

        public:

            DescriptorSetBuilder() = delete;
            /// @brief Initialize descriptor set builder using given descriptor set allocator and descriptor set layout cache.
            /// @param allocator Pointer to descriptor set allocator.
            /// @param layoutCache Pointer to descriptor set layout cache.
            DescriptorSetBuilder( DescriptorSetAllocator *allocator, DescriptorLayoutCache *layoutCache) : pAllocator {allocator}, pLayoutCache {layoutCache} {}

            /// @brief Bind a buffer to a specific descriptor binding and crates descriptor write for later update.
            /// @param binding Descriptor set binding.
            /// @param type Descriptor type.
            /// @param stages Shader stage.
            /// @param bufferInfo Descriptor buffer info.
            /// @return Reference to DescriptorSetBuilder for chaining.
            DescriptorSetBuilder& BindBuffer( uint32_t binding, vk::DescriptorType type, vk::ShaderStageFlags stages, vk::DescriptorBufferInfo* bufferInfo);
            /// @brief Bind an image to a specific descriptor binding and crates descriptor write for later update.
            /// @param binding Descriptor set binding.
            /// @param type Descriptor type.
            /// @param stages Shader stage.
            /// @param bufferInfo Descriptor image info.
            /// @return Reference to DescriptorSetBuilder for chaining.
            DescriptorSetBuilder& BindImage( uint32_t binding, vk::DescriptorType type, vk::ShaderStageFlags stages, vk::DescriptorImageInfo* imageInfo);

            /// @brief Build and update descriptor set with provided bindings.
            /// @param pLayout Optional pointer to retrieve descriptor set layout of created descriptor set.
            /// @return Vulkan descriptor set.
            vk::DescriptorSet Build( vk::DescriptorSetLayout *pLayout = nullptr);

        private:

            DescriptorSetAllocator* pAllocator;
            DescriptorLayoutCache* pLayoutCache;

            std::vector<vk::DescriptorSetLayoutBinding> mBindings;
            std::vector<vk::WriteDescriptorSet> mWrites;
    };


    /// @brief Creates Vulkan descriptor pool.
    /// @param poolSizes List of pool sizes.
    /// @param maxSets Maximum number of descriptors allocatable from the pool.
    /// @param flags Descriptor pool creation flags.
    /// @return Vulkan descriptor pool.
    vk::DescriptorPool CreateDescriptorPool( std::span<vk::DescriptorPoolSize> poolSizes, uint32_t maxSets, vk::DescriptorPoolCreateFlags flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet);

    /// @brief Destroys given descriptor pool.
    /// @param pool Descriptor pool to destroy.
    void DestroyDesciptorPool( vk::DescriptorPool pool);

    /// @brief Creates descriptor set layout.
    /// @param bindings List of descriptor set layout bindings.
    /// @return Vulkan descriptor set layout,
    vk::DescriptorSetLayout CreateDescriptorSetLayout( std::span<vk::DescriptorSetLayoutBinding> bindings);

    /// @brief Destroys given descriptor set layout.
    /// @param layout Descriptor set layout.
    void DestroyDescriptorSetLayout( vk::DescriptorSetLayout layout);

    /// @brief Allocates descriptor set from given descriptor pool.
    /// @param pool Descriptor pool.
    /// @param layout Descriptor set layout.
    /// @return Vulkan descriptor set.
    vk::DescriptorSet AllocateDescriptorSet( vk::DescriptorPool pool, vk::DescriptorSetLayout layout);


    /// @brief Creates descriptor info for given buffer.
    /// @param buffer Vulkan buffer.
    /// @param offset Buffer offset.
    /// @param range Buffer range.
    /// @return Vulkan descriptor buffer info.
    vk::DescriptorBufferInfo GetDescriptorBufferInfo( vk::Buffer buffer, size_t offset = 0, size_t range = VK_WHOLE_SIZE);

    /// @brief Creates descriptor info for given image view.
    /// @param imageView Vulkan image view.
    /// @param sampler Optional Vulkan sampler if descriptor type is combined image sampler.
    /// @param layout Image layout.
    /// @return Vulkan descriptor image info.
    vk::DescriptorImageInfo GetDescriptorImageInfo( vk::ImageView imageView, vk::Sampler sampler = VK_NULL_HANDLE, vk::ImageLayout layout = vk::ImageLayout::eShaderReadOnlyOptimal);


} // namespace vktg
