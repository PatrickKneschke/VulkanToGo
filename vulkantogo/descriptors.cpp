
#include "descriptors.h"


namespace vktg
{


    vk::DescriptorSetLayout DescriptorLayoutCache::CreateLayout( vk::DescriptorSetLayoutCreateInfo *layoutInfo) {

        // hash descriptor set layout
        uint64_t layoutId = 0;
        for (uint32_t i = 0; i < layoutInfo->bindingCount; i++)
        {
            auto binding = layoutInfo->pBindings[i];
            layoutId = (layoutId << 1) ^ ( (size_t)binding.binding << 32 | (uint32_t)binding.descriptorType << 16 |(uint32_t)binding.stageFlags );
        }

        if (mLayouts.find( layoutId) == mLayouts.end())
        {
            vk::DescriptorSetLayout newLayout;
            VK_CHECK( Device().createDescriptorSetLayout( layoutInfo, nullptr, &newLayout) );
            mLayouts[layoutId] = newLayout;
        }

        return mLayouts[layoutId];
    }

    vk::DescriptorSetLayout DescriptorLayoutCache::CreateLayout( std::span<vk::DescriptorSetLayoutBinding> bindings) {

        auto layoutInfo = vk::DescriptorSetLayoutCreateInfo{}
            .setBindingCount( (uint32_t)bindings.size() )
            .setPBindings( bindings.data() );

        return CreateLayout( &layoutInfo);
    }

    void DescriptorLayoutCache::DestroyLayouts() {
    
        for (auto &[info, layout] : mLayouts)
        {
            DestroyDescriptorSetLayout( layout);
        }
    }

    
    vk::DescriptorPool CreateDescriptorPool( std::span<vk::DescriptorPoolSize> poolSizes, uint32_t maxSets, vk::DescriptorPoolCreateFlags flags) {

        auto poolInfo = vk::DescriptorPoolCreateInfo{}
            .setFlags( flags )
            .setPoolSizeCount( (uint32_t)poolSizes.size() )
            .setPPoolSizes( poolSizes.data() )
            .setMaxSets( maxSets );

        vk::DescriptorPool pool;
        VK_CHECK( Device().createDescriptorPool( &poolInfo, nullptr, &pool) );

        return pool;
    }

    void DestroyDesciptorPool( vk::DescriptorPool pool) {
    
        Device().destroyDescriptorPool( pool);
    }

    vk::DescriptorSetLayout CreateDescriptorSetLayout(std::span<vk::DescriptorSetLayoutBinding> bindings) {

        auto layoutInfo = vk::DescriptorSetLayoutCreateInfo{}
            .setBindingCount( (uint32_t)bindings.size() )
            .setPBindings( bindings.data() );

        vk::DescriptorSetLayout layout;
        VK_CHECK( Device().createDescriptorSetLayout( &layoutInfo, nullptr, &layout) );
        
        return layout;
    }

    void DestroyDescriptorSetLayout( vk::DescriptorSetLayout layout) {

        Device().destroyDescriptorSetLayout( layout);
    }

    vk::DescriptorSet AllocateDescriptorSet( vk::DescriptorPool pool, vk::DescriptorSetLayout layout) {
        
        auto allocInfo = vk::DescriptorSetAllocateInfo{}
            .setDescriptorPool( pool )
            .setDescriptorSetCount( 1 )
            .setPSetLayouts( &layout );

        vk::DescriptorSet descriptorSet;
        VK_CHECK( Device().allocateDescriptorSets( &allocInfo, &descriptorSet) );

        return descriptorSet;
    }


} // namespace vktg