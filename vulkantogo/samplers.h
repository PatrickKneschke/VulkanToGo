#pragma once


#include "vk_core.h"


namespace vktg
{


    class SamplerBuilder {

        public:

            SamplerBuilder();

            void Reset();
            vk::Sampler Build();

            SamplerBuilder& SetFilter( vk::Filter filter);
            SamplerBuilder& SetMinFilter( vk::Filter filter);
            SamplerBuilder& SetMagFilter( vk::Filter filter);

            SamplerBuilder& SetMipMapMode( vk::SamplerMipmapMode mode);
            SamplerBuilder& SetLod( float minLod, float maxLod, float mipLodBias = 0.f);

            SamplerBuilder& SetAddressMode( vk::SamplerAddressMode mode);
            SamplerBuilder& SetAddressModeU( vk::SamplerAddressMode mode);
            SamplerBuilder& SetAddressModeV( vk::SamplerAddressMode mode);
            SamplerBuilder& SetAddressModeW( vk::SamplerAddressMode mode);

            SamplerBuilder& SetBorderColor( vk::BorderColor color);

            SamplerBuilder& EnableAnisotropy( bool enable, float maxAnisotropy = 1.f);
            SamplerBuilder& EnableComparison( bool enable, vk::CompareOp compareOp = vk::CompareOp::eGreater);

            SamplerBuilder& SetReductionMode( vk::SamplerReductionMode mode);
            SamplerBuilder& ResetReductionMode();


        private:

            vk::SamplerCreateInfo mSamplerinfo;
            vk::SamplerReductionModeCreateInfo mReductionInfo;
    };

    vk::Sampler CreateSampler( const vk::SamplerCreateInfo &samplerinfo);

    void DestroySampler( vk::Sampler sampler);

    
} // namespace vktg
