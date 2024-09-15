#pragma once


#include "vk_core.h"

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


    vk::DescriptorPool CreateDescriptorPool( std::span<vk::DescriptorPoolSize> poolSizes, uint32_t maxSets, vk::DescriptorPoolCreateFlags flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet);

    void DestroyDesciptorPool( vk::DescriptorPool pool);

    vk::DescriptorSetLayout CreateDescriptorSetLayout( std::span<vk::DescriptorSetLayoutBinding> bindings);

    void DestroyDescriptorSetLayout( vk::DescriptorSetLayout layout);

    vk::DescriptorSet AllocateDescriptorSet( vk::DescriptorPool pool, vk::DescriptorSetLayout layout);


} // namespace vktg
