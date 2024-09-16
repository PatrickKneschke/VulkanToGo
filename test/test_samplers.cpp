
#include <catch2/catch.hpp>

#include "../vulkantogo/samplers.h"


TEST_CASE( "create sampler", "[samplers]") {

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