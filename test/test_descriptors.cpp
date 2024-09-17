
#include <catch2/catch.hpp>

#include "../vulkantogo/descriptors.h"
#include "../vulkantogo/samplers.h"
#include "../vulkantogo/storage.h"

#include <vector>


TEST_CASE( "create descriptor pool", "[descriptors]") {

    uint32_t maxSetsPerPool = 1000;
    std::vector<vk::DescriptorPoolSize> poolSizes = {
        vk::DescriptorPoolSize{ vk::DescriptorType::eSampler,              (uint32_t)( (float)maxSetsPerPool * 0.5f) },
        vk::DescriptorPoolSize{ vk::DescriptorType::eSampledImage,         maxSetsPerPool * 4 },
        vk::DescriptorPoolSize{ vk::DescriptorType::eCombinedImageSampler, maxSetsPerPool * 4 },
        vk::DescriptorPoolSize{ vk::DescriptorType::eStorageImage,         maxSetsPerPool * 4 },
        vk::DescriptorPoolSize{ vk::DescriptorType::eUniformTexelBuffer,   maxSetsPerPool * 1 },
        vk::DescriptorPoolSize{ vk::DescriptorType::eStorageTexelBuffer,   maxSetsPerPool * 1 },
        vk::DescriptorPoolSize{ vk::DescriptorType::eUniformBuffer,        maxSetsPerPool * 2 },
        vk::DescriptorPoolSize{ vk::DescriptorType::eUniformBufferDynamic, maxSetsPerPool * 1 },
        vk::DescriptorPoolSize{ vk::DescriptorType::eStorageBuffer,        maxSetsPerPool * 4 },
        vk::DescriptorPoolSize{ vk::DescriptorType::eStorageBufferDynamic, maxSetsPerPool * 1 },
        vk::DescriptorPoolSize{ vk::DescriptorType::eInputAttachment,      (uint32_t)( (float)maxSetsPerPool * 0.5f) }
    };

    vk::DescriptorPool pool = vktg::CreateDescriptorPool( poolSizes, maxSetsPerPool);

    REQUIRE_FALSE( !pool );

    vktg::DestroyDesciptorPool( pool);
}


TEST_CASE( "create descriptor set layout", "[descriptors]") {

    std::vector<vk::DescriptorSetLayoutBinding> bindings = {
        vk::DescriptorSetLayoutBinding{}
            .setBinding( 0 )
            .setDescriptorCount( 1 )
            .setDescriptorType( vk::DescriptorType::eStorageBuffer )   
    };

    vk::DescriptorSetLayout layout = vktg::CreateDescriptorSetLayout( bindings);

    REQUIRE_FALSE( !layout );

    vktg::DestroyDescriptorSetLayout( layout);
}


TEST_CASE( "allocate descriptor set", "[descriptors]") {

    uint32_t maxSetsPerPool = 10;
    std::vector<vk::DescriptorPoolSize> poolSizes = {
        vk::DescriptorPoolSize{ vk::DescriptorType::eStorageBuffer, maxSetsPerPool}
    };
    vk::DescriptorPool pool = vktg::CreateDescriptorPool( poolSizes, maxSetsPerPool);

    std::vector<vk::DescriptorSetLayoutBinding> bindings = {
        vk::DescriptorSetLayoutBinding{}
            .setBinding( 0 )
            .setDescriptorCount( 1 )
            .setDescriptorType( vk::DescriptorType::eStorageBuffer )   
    };
    vk::DescriptorSetLayout layout = vktg::CreateDescriptorSetLayout( bindings);

    vk::DescriptorSet descriptorSet = vktg::AllocateDescriptorSet( pool, layout);

    REQUIRE_FALSE( !descriptorSet );

    vktg::DestroyDescriptorSetLayout( layout);
    vktg::DestroyDesciptorPool( pool);
}


TEST_CASE( "descriptor layout cache", "[descriptors]") {

    vktg::DescriptorLayoutCache cache;

    std::vector<vk::DescriptorSetLayoutBinding> bindings = {
        vk::DescriptorSetLayoutBinding{}
            .setBinding( 0 )
            .setDescriptorCount( 1 )
            .setDescriptorType( vk::DescriptorType::eStorageBuffer )   
    };
    vk::DescriptorSetLayout layout = cache.CreateLayout( bindings);

    REQUIRE_FALSE( !layout );

    cache.DestroyLayouts();
}


TEST_CASE( "descriptor allocator", "[descriptors]") {

    uint32_t maxSetsPerPool = 10;
    std::vector<vk::DescriptorPoolSize> poolSizes = {
        vk::DescriptorPoolSize{ vk::DescriptorType::eStorageBuffer, maxSetsPerPool}
    };
    vktg::DescriptorSetAllocator allocator( poolSizes, maxSetsPerPool);

    std::vector<vk::DescriptorSetLayoutBinding> bindings = {
        vk::DescriptorSetLayoutBinding{}
            .setBinding( 0 )
            .setDescriptorCount( 1 )
            .setDescriptorType( vk::DescriptorType::eStorageBuffer )   
    };
    vk::DescriptorSetLayout layout = vktg::CreateDescriptorSetLayout( bindings);

    vk::DescriptorSet descriptorSet = allocator.Allocate( layout);

    REQUIRE_FALSE( !descriptorSet );

    allocator.ResetPools();
    allocator.DestroyPools();
}


TEST_CASE( "get descriptor buffer info", "[descriptors]") {

    vktg::Buffer buffer = vktg::CreateBuffer( 256, vk::BufferUsageFlagBits::eUniformBuffer);
    vk::DescriptorBufferInfo bufferInfo = vktg::GetDescriptorBufferInfo( buffer.buffer, 128, 64);

    REQUIRE_FALSE( !bufferInfo.buffer );
    REQUIRE( bufferInfo.offset == 128 );
    REQUIRE( bufferInfo.range == 64 );
}


TEST_CASE( "descriptor set builder", "[descriptors]") {

    vktg::DescriptorLayoutCache layoutCache;
    vktg::DescriptorSetAllocator setAllocator;
    vk::Sampler sampler = vktg::SamplerBuilder().Build();

    vktg::Buffer buffer = vktg::CreateBuffer( 256, vk::BufferUsageFlagBits::eUniformBuffer);
    vk::DescriptorBufferInfo bufferInfo = vktg::GetDescriptorBufferInfo( buffer.buffer, 128, 64);

    vktg::Image image = vktg::CreateImage( 256, 256, vk::Format::eR8G8B8A8Srgb, vk::ImageUsageFlagBits::eSampled);
    vk::DescriptorImageInfo imageInfo = vktg::GetDescriptorImageInfo( image.imageView, sampler, vk::ImageLayout::eShaderReadOnlyOptimal);

    vk::DescriptorSet descriptorSet = vktg::DescriptorSetBuilder( &setAllocator, &layoutCache)
        .BindBuffer( 0, vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eVertex, &bufferInfo)
        .BindImage( 1, vk::DescriptorType::eSampledImage, vk::ShaderStageFlagBits::eFragment, &imageInfo)
        .Build();

    REQUIRE_FALSE( !descriptorSet );
}