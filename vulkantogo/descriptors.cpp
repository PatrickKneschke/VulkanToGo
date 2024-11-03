
#include "descriptors.h"


namespace vktg
{


    vk::DescriptorSetLayout DescriptorLayoutCache::CreateLayout( const vk::DescriptorSetLayoutCreateInfo *layoutInfo) {

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


    DescriptorSetAllocator::DescriptorSetAllocator() : mMaxSetsPerPool{ 1000} {
    
        mPoolSizes = {
            vk::DescriptorPoolSize{ vk::DescriptorType::eSampler,              (uint32_t)( (float)mMaxSetsPerPool * 0.5f) },
            vk::DescriptorPoolSize{ vk::DescriptorType::eSampledImage,         mMaxSetsPerPool * 4 },
            vk::DescriptorPoolSize{ vk::DescriptorType::eCombinedImageSampler, mMaxSetsPerPool * 4 },
            vk::DescriptorPoolSize{ vk::DescriptorType::eStorageImage,         mMaxSetsPerPool * 4 },
            vk::DescriptorPoolSize{ vk::DescriptorType::eUniformTexelBuffer,   mMaxSetsPerPool * 1 },
            vk::DescriptorPoolSize{ vk::DescriptorType::eStorageTexelBuffer,   mMaxSetsPerPool * 1 },
            vk::DescriptorPoolSize{ vk::DescriptorType::eUniformBuffer,        mMaxSetsPerPool * 2 },
            vk::DescriptorPoolSize{ vk::DescriptorType::eUniformBufferDynamic, mMaxSetsPerPool * 1 },
            vk::DescriptorPoolSize{ vk::DescriptorType::eStorageBuffer,        mMaxSetsPerPool * 4 },
            vk::DescriptorPoolSize{ vk::DescriptorType::eStorageBufferDynamic, mMaxSetsPerPool * 1 },
            vk::DescriptorPoolSize{ vk::DescriptorType::eInputAttachment,      (uint32_t)( (float)mMaxSetsPerPool * 0.5f) }
        };
    }

    
    DescriptorSetAllocator::DescriptorSetAllocator( std::span<vk::DescriptorPoolSize> poolSizes, uint32_t maxSetsPerPool) : 
        mMaxSetsPerPool{maxSetsPerPool},
        mPoolSizes( poolSizes.begin(), poolSizes.end())
    {

    }


    vk::DescriptorSet DescriptorSetAllocator::Allocate( vk::DescriptorSetLayout layout) {

        if (!mCurrPool)
        {
            mCurrPool = GetNewPool();
        }

        auto allocInfo = vk::DescriptorSetAllocateInfo{}
            .setDescriptorPool( mCurrPool )
            .setDescriptorSetCount( 1 )
            .setPSetLayouts( &layout );

        vk::DescriptorSet descriptorSet;
        vk::Result result = Device().allocateDescriptorSets( &allocInfo, &descriptorSet);
        if (result == vk::Result::eErrorFragmentedPool  ||  result == vk::Result::eErrorOutOfPoolMemory)
        {
            mCurrPool = GetNewPool();
            VK_CHECK( Device().allocateDescriptorSets( &allocInfo, &descriptorSet) );
        }

        return descriptorSet;
    }


    void DescriptorSetAllocator::ResetPools() {
 
        for (auto pool : mUsedPools)
        {
            Device().resetDescriptorPool( pool);
            mFreePools.push_back( pool);
        }        
        mUsedPools.clear();
        mCurrPool = VK_NULL_HANDLE;
    }


    void DescriptorSetAllocator::DestroyPools() {

        for (auto pool : mFreePools) 
        {
            Device().destroyDescriptorPool( pool);
        }
        for (auto pool : mUsedPools) 
        {
            Device().destroyDescriptorPool( pool);
        }
    }


    vk::DescriptorPool DescriptorSetAllocator::GetNewPool() {
        
        vk::DescriptorPool pool;
        if (mFreePools.empty())
        {
            pool = CreateDescriptorPool( mPoolSizes, mMaxSetsPerPool);
        }
        else
        {
            pool = mFreePools.back();
            mFreePools.pop_back();
        }
        mUsedPools.push_back( pool);

        return pool;
    }


    DescriptorSetBuilder& DescriptorSetBuilder::BindBuffer( uint32_t binding, vk::DescriptorType type, vk::ShaderStageFlags stages, const vk::DescriptorBufferInfo *bufferInfo) {
        
        auto newBinding = vk::DescriptorSetLayoutBinding{}
            .setBinding( binding )
            .setDescriptorCount( 1 )
            .setDescriptorType( type )
            .setStageFlags( stages );
        mBindings.push_back( newBinding);

        auto newWrite = vk::WriteDescriptorSet{}
            .setDstBinding( binding )
            .setDescriptorCount( 1 )
            .setPBufferInfo( bufferInfo )
            .setDescriptorType( type );
            // set dest set in Build()
        mWrites.push_back( newWrite);
        
        return *this;
    }


    DescriptorSetBuilder& DescriptorSetBuilder::BindImage( uint32_t binding, vk::DescriptorType type, vk::ShaderStageFlags stages, const vk::DescriptorImageInfo *imageInfo) {

        auto newBinding = vk::DescriptorSetLayoutBinding{}
            .setBinding( binding )
            .setDescriptorCount( 1 )
            .setDescriptorType( type )
            .setStageFlags( stages );
        mBindings.push_back( newBinding );

        auto newWrite = vk::WriteDescriptorSet{}
            .setDstBinding( binding )
            .setDescriptorCount( 1 )
            .setPImageInfo( imageInfo )
            .setDescriptorType( type );
            // set dest set in Build()
        mWrites.push_back(newWrite);

        return *this;
    }


    vk::DescriptorSet DescriptorSetBuilder::Build( vk::DescriptorSetLayout *pLayout) {

        auto layoutCreateInfo = vk::DescriptorSetLayoutCreateInfo{}
            .setBindingCount( (uint32_t)mBindings.size())
            .setPBindings( mBindings.data());

        vk::DescriptorSetLayout layout = pLayoutCache->CreateLayout( &layoutCreateInfo);
        if (pLayout != nullptr)
        {
            *pLayout = layout;
        }

        vk::DescriptorSet descriptorSet = pAllocator->Allocate( layout);
        for (auto &write : mWrites) 
        {
            write.setDstSet( descriptorSet );
        }
        Device().updateDescriptorSets( (uint32_t)mWrites.size(), mWrites.data(), 0, nullptr);   

        return descriptorSet;
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

    
    vk::DescriptorBufferInfo GetDescriptorBufferInfo( vk::Buffer buffer, size_t offset, size_t range) {

        auto bufferInfo = vk::DescriptorBufferInfo{}
			.setBuffer( buffer )
			.setOffset( offset )
			.setRange( range );

		return bufferInfo;
    }


    vk::DescriptorImageInfo GetDescriptorImageInfo( vk::ImageView imageView, vk::Sampler sampler, vk::ImageLayout layout) {

        auto imageInfo = vk::DescriptorImageInfo{}
			.setImageView( imageView )
			.setImageLayout( layout )
			.setSampler( sampler );

		return imageInfo;
    }


} // namespace vktg