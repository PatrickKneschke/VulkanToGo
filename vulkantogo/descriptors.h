#pragma once


#include "vk_core.h"
#include "storage.h"

#include <span>


namespace vktg
{


    class DescriptorLayoutCache {

        public:

            vk::DescriptorSetLayout CreateLayout( vk::DescriptorSetLayoutCreateInfo *layoutInfo);
            vk::DescriptorSetLayout CreateLayout( std::span<vk::DescriptorSetLayoutBinding> bindings);
            void DestroyLayouts();

        private:

            std::unordered_map<uint64_t, vk::DescriptorSetLayout> mLayouts;
    };


    class DescriptorSetAllocator {

        public:

            DescriptorSetAllocator();
            DescriptorSetAllocator( std::span<vk::DescriptorPoolSize> poolSizes, uint32_t maxSetsPerPool);

            vk::DescriptorSet Allocate( vk::DescriptorSetLayout layout);
            void ResetPools();
            void DestroyPools();


        private:

            vk::DescriptorPool GetNewPool();


            uint32_t mMaxSetsPerPool;
            std::vector<vk::DescriptorPoolSize> mPoolSizes;

            vk::DescriptorPool mCurrPool;
            std::vector<vk::DescriptorPool> mUsedPools;
            std::vector<vk::DescriptorPool> mFreePools;
    };


    class DescriptorSetBuilder {

        public:

            DescriptorSetBuilder( DescriptorSetAllocator *allocator, DescriptorLayoutCache *layoutCache) : pAllocator {allocator}, pLayoutCache {layoutCache} {}

            DescriptorSetBuilder& BindBuffer( uint32_t binding, vk::DescriptorType type, vk::ShaderStageFlags stages, vk::DescriptorBufferInfo* bufferInfo);
            DescriptorSetBuilder& BindImage( uint32_t binding, vk::DescriptorType type, vk::ShaderStageFlags stages, vk::DescriptorImageInfo* imageInfo);

            vk::DescriptorSet Build( vk::DescriptorSetLayout *pLayout = nullptr);

        private:

            DescriptorSetAllocator* pAllocator;
            DescriptorLayoutCache* pLayoutCache;

            std::vector<vk::DescriptorSetLayoutBinding> mBindings;
            std::vector<vk::WriteDescriptorSet> mWrites;
    };


    vk::DescriptorPool CreateDescriptorPool( std::span<vk::DescriptorPoolSize> poolSizes, uint32_t maxSets, vk::DescriptorPoolCreateFlags flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet);

    void DestroyDesciptorPool( vk::DescriptorPool pool);

    vk::DescriptorSetLayout CreateDescriptorSetLayout( std::span<vk::DescriptorSetLayoutBinding> bindings);

    void DestroyDescriptorSetLayout( vk::DescriptorSetLayout layout);

    vk::DescriptorSet AllocateDescriptorSet( vk::DescriptorPool pool, vk::DescriptorSetLayout layout);


    vk::DescriptorBufferInfo GetDescriptorBufferInfo( vk::Buffer buffer, size_t offset = 0, size_t range = VK_WHOLE_SIZE);

    vk::DescriptorImageInfo GetDescriptorImageInfo( vk::ImageView imageView, vk::Sampler sampler = VK_NULL_HANDLE, vk::ImageLayout layout = vk::ImageLayout::eShaderReadOnlyOptimal);


} // namespace vktg
