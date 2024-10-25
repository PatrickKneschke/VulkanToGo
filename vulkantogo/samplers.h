#pragma once


#include "vk_core.h"


namespace vktg
{


    /// @brief Used to create Vulkan samplers.
    class SamplerBuilder {

        public:

            SamplerBuilder();

            /// @brief Resets sampler builder settings.
            void Reset();
            /// @brief Creates Vulkan sampler with specified settings.
            /// @return Vulkan sampler.
            vk::Sampler Build();

            /// @brief Set both min and mag filter used in sampler to the same method.
            /// @param filter Filter method.
            /// @return Reference to SamplerBuilder for chaining.
            SamplerBuilder& SetFilter( vk::Filter filter);
            /// @brief Set min filter method used in sampler.
            /// @param filter Filter method.
            /// @return Reference to SamplerBuilder for chaining.
            SamplerBuilder& SetMinFilter( vk::Filter filter);
            /// @brief Set mag filter method used in sampler.
            /// @param filter Filter method.
            /// @return Reference to SamplerBuilder for chaining.
            SamplerBuilder& SetMagFilter( vk::Filter filter);

            /// @brief Specify mipmap mode used in sampler.
            /// @param mode Sampler mipmap mode
            /// @return Reference to SamplerBuilder for chaining.
            SamplerBuilder& SetMipMapMode( vk::SamplerMipmapMode mode);
            /// @brief Specify lod settings used in sampler.
            /// @param minLod Minimum lod.
            /// @param maxLod Maximum lod.
            /// @param mipLodBias Mip lod bias.
            /// @return Reference to SamplerBuilder for chaining.
            SamplerBuilder& SetLod( float minLod, float maxLod, float mipLodBias = 0.f);

            /// @brief Set U, V and W address mode used in sampler to the same value.
            /// @param mode Address mode.
            /// @return Reference to SamplerBuilder for chaining.
            SamplerBuilder& SetAddressMode( vk::SamplerAddressMode mode);
            /// @brief Set U address mode used in sampler.
            /// @param mode Address mode.
            /// @return Reference to SamplerBuilder for chaining.
            SamplerBuilder& SetAddressModeU( vk::SamplerAddressMode mode);
            /// @brief Set V address mode used in sampler.
            /// @param mode Address mode.
            /// @return Reference to SamplerBuilder for chaining.
            SamplerBuilder& SetAddressModeV( vk::SamplerAddressMode mode);
            /// @brief Set W address mode used in sampler.
            /// @param mode 
            /// @return Reference to SamplerBuilder for chaining.
            SamplerBuilder& SetAddressModeW( vk::SamplerAddressMode mode);

            /// @brief Specify border color used in sampler.
            /// @param color Border color.
            /// @return Reference to SamplerBuilder for chaining.
            SamplerBuilder& SetBorderColor( vk::BorderColor color);

            /// @brief Specify sampler anisotropy setting.
            /// @param enable Enable/Disable sampler anisotropy.
            /// @param maxAnisotropy Maximum sampler anisotropy.
            /// @return Reference to SamplerBuilder for chaining.
            SamplerBuilder& EnableAnisotropy( bool enable, float maxAnisotropy = 1.f);
            /// @brief Specify sampler comparison setting.
            /// @param enable Enable/Disable sampler comparison.
            /// @param compareOp Compare operation.
            /// @return Reference to SamplerBuilder for chaining.
            SamplerBuilder& EnableComparison( bool enable, vk::CompareOp compareOp = vk::CompareOp::eGreater);

            /// @brief Specify sampler reduction mode.
            /// @param mode Sampler reduction mode.
            /// @return Reference to SamplerBuilder for chaining.
            SamplerBuilder& SetReductionMode( vk::SamplerReductionMode mode);
            /// @brief Reset sampler reduction mode.
            /// @return Reference to SamplerBuilder for chaining.
            SamplerBuilder& ResetReductionMode();


        private:

            vk::SamplerCreateInfo mSamplerinfo;
            vk::SamplerReductionModeCreateInfo mReductionInfo;
    };

    /// @brief Creates Vulkan sampler from given sampler create info.
    /// @param samplerinfo Sampler creation info.
    /// @return Vulkan sampler.
    vk::Sampler CreateSampler( const vk::SamplerCreateInfo &samplerinfo);

    /// @brief Destroys given Vulkan sampler
    /// @param sampler Sampler to destroy.
    void DestroySampler( vk::Sampler sampler);

    
} // namespace vktg
