
#include "samplers.h"


namespace vktg
{


    SamplerBuilder::SamplerBuilder() {

        Reset();
    }

    
    void SamplerBuilder::Reset() {
    
        mSamplerinfo
            .setMinFilter( vk::Filter::eNearest )
            .setMagFilter( vk::Filter::eNearest )
            .setMipmapMode( vk::SamplerMipmapMode::eNearest )
            .setAddressModeU( vk::SamplerAddressMode::eClampToBorder )
            .setAddressModeV( vk::SamplerAddressMode::eClampToBorder )
            .setAddressModeW( vk::SamplerAddressMode::eClampToBorder )
            .setAnisotropyEnable( false )
            .setMinLod( 0.f )
            .setMaxLod( VK_LOD_CLAMP_NONE )
            .setMipLodBias( 0.f )
            .setCompareEnable( false )
            .setBorderColor( vk::BorderColor::eIntOpaqueBlack )
		    .setPNext( nullptr );

        mReductionInfo.setReductionMode( vk::SamplerReductionMode::eWeightedAverage );
    }

    
    vk::Sampler SamplerBuilder::Build() {

        return CreateSampler( mSamplerinfo);
    }


    SamplerBuilder& SamplerBuilder::SetFilter( vk::Filter filter) {
        
        SetMinFilter( filter);
        SetMagFilter( filter);

        return *this;
    }


    SamplerBuilder& SamplerBuilder::SetMinFilter( vk::Filter filter) {

        mSamplerinfo.setMinFilter( filter );

        return *this;
    }


    SamplerBuilder& SamplerBuilder::SetMagFilter( vk::Filter filter) {

        mSamplerinfo.setMagFilter( filter );

        return *this;
    }


    SamplerBuilder& SamplerBuilder::SetMipMapMode( vk::SamplerMipmapMode mode) {

        mSamplerinfo.setMipmapMode( mode );   

        return *this;     
    }


    SamplerBuilder& SamplerBuilder::SetLod( float minLod, float maxLod, float mipLodBias) {
        
        mSamplerinfo
            .setMinLod( minLod )
            .setMaxLod( maxLod )
            .setMipLodBias( mipLodBias );

        return *this;
    }


    SamplerBuilder& SamplerBuilder::SetAddressMode( vk::SamplerAddressMode mode) {

        SetAddressModeU( mode );
        SetAddressModeV( mode );
        SetAddressModeW( mode );

        return *this;
    }


    SamplerBuilder& SamplerBuilder::SetAddressModeU( vk::SamplerAddressMode mode) {

        mSamplerinfo.setAddressModeU( mode );

        return *this;
    }


    SamplerBuilder& SamplerBuilder::SetAddressModeV( vk::SamplerAddressMode mode) {

        mSamplerinfo.setAddressModeV( mode );

        return *this;
    }


    SamplerBuilder& SamplerBuilder::SetAddressModeW( vk::SamplerAddressMode mode) {

        mSamplerinfo.setAddressModeW( mode );

        return *this;
    }

    
    SamplerBuilder& SamplerBuilder::SetBorderColor( vk::BorderColor color) {
        
        mSamplerinfo.setBorderColor( color );

        return *this;
    }

    
    SamplerBuilder& SamplerBuilder::EnableAnisotropy( bool enable, float maxAnisotropy) {

        mSamplerinfo
            .setAnisotropyEnable( enable )
            .setMaxAnisotropy( maxAnisotropy );

        return *this;
    }


    SamplerBuilder& SamplerBuilder::EnableComparison(bool enable, vk::CompareOp compareOp) {
        
        mSamplerinfo
            .setCompareEnable( enable )
            .setCompareOp( compareOp );

        return *this;
    }


    SamplerBuilder& SamplerBuilder::SetReductionMode( vk::SamplerReductionMode mode) {

        mReductionInfo.setReductionMode( mode );
        mSamplerinfo.setPNext( &mReductionInfo );

        return *this;
    }


    SamplerBuilder& SamplerBuilder::ResetReductionMode() {

        mSamplerinfo.setPNext( nullptr );
     
        return *this;
    }

    
    vk::Sampler CreateSampler(const vk::SamplerCreateInfo &samplerinfo) {

        vk::Sampler sampler;
        VK_CHECK( Device().createSampler( &samplerinfo, nullptr, &sampler) );

        return sampler;
    }


    void DestroySampler(vk::Sampler sampler) {

        Device().destroySampler( sampler);
    }


} // namespace vktg