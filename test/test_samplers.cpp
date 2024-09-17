
#include <catch2/catch.hpp>

#include "../vulkantogo/samplers.h"


TEST_CASE( "create sampler", "[samplers]") {

    auto samplerInfo = vk::SamplerCreateInfo{}
        .setMinFilter( vk::Filter::eLinear )
        .setMagFilter( vk::Filter::eLinear )
        .setAddressModeU( vk::SamplerAddressMode::eClampToEdge )
        .setAddressModeV( vk::SamplerAddressMode::eClampToEdge )
        .setAddressModeW( vk::SamplerAddressMode::eClampToEdge );

    vk::Sampler sampler = vktg::CreateSampler( samplerInfo);

    REQUIRE_FALSE( !sampler );

    vktg::DestroySampler( sampler);
}


TEST_CASE( "create sampler with builder", "[samplers]") {

    vk::Sampler sampler = vktg::SamplerBuilder()
        .SetFilter( vk::Filter::eLinear )
        .SetAddressMode( vk::SamplerAddressMode::eMirroredRepeat )
        .SetLod( true, 16)
        .SetReductionMode( vk::SamplerReductionMode::eWeightedAverage )
        .EnableAnisotropy( true, 8.0)
        .Build();

    REQUIRE_FALSE( !sampler );

    vktg::DestroySampler( sampler);
}