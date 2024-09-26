#pragma once


#include "vk_core.h"

#include <span>
#include <vector>


namespace vktg
{
    

    struct Pipeline {

        enum class Type : uint8_t {
            eCompute = 0,
            eGraphics
        };

        Type type;
        vk::Pipeline pipeline;
        vk::PipelineLayout pipelineLayout;
    };


    struct PipelineBuilder {

        const Pipeline::Type type = Pipeline::Type::eCompute;

        PipelineBuilder( Pipeline::Type type_) : type{type_} {}

        virtual void Reset() = 0;
        virtual Pipeline Build() = 0;
    };


    struct ComputePipelineBuilder : public PipelineBuilder {

        vk::PipelineShaderStageCreateInfo shaderInfo;
        std::vector<vk::DescriptorSetLayout> descriptorLayouts;
        std::vector<vk::PushConstantRange> pushConstants;

        ComputePipelineBuilder();
        void Reset() override;
        Pipeline Build() override;

        ComputePipelineBuilder& SetShader( vk::ShaderModule shaderModule, vk::SpecializationInfo *pSpecialization = nullptr, std::string_view entryPointName = "main");
        ComputePipelineBuilder& AddDescriptorLayout( vk::DescriptorSetLayout descriptorLayout);
        ComputePipelineBuilder& AddPushConstant( vk::PushConstantRange pushConstant);
    };


    struct GraphicsPipelineBuilder : public PipelineBuilder {

		std::vector<vk::PipelineShaderStageCreateInfo>   shaderInfos;
        std::vector<vk::DescriptorSetLayout>             descriptorLayouts;
		std::vector<vk::PushConstantRange>               pushConstants;

		vk::VertexInputBindingDescription          		 vertexInputBinding;
		std::vector<vk::VertexInputAttributeDescription> vertexAttributes;
		vk::PipelineTessellationStateCreateInfo    		 tesselationInfo;
		vk::PipelineInputAssemblyStateCreateInfo   		 inputAssemblyInfo;
		vk::PipelineViewportStateCreateInfo        		 viewportInfo;
		vk::PipelineRasterizationStateCreateInfo   		 rasterizerInfo;
		vk::PipelineMultisampleStateCreateInfo 	   		 multisampleInfo;
		vk::PipelineDepthStencilStateCreateInfo    		 depthStencilInfo;
		vk::PipelineColorBlendAttachmentState	   		 colorBlendAttachment;
		std::vector<vk::DynamicState>                    dynamicStates;
		vk::PipelineRenderingCreateInfo 				 renderInfo;
		std::vector<vk::Format>							 colorAttachmentFormats;

        GraphicsPipelineBuilder();
        void Reset() override;
        Pipeline Build() override;

        GraphicsPipelineBuilder& AddShader( vk::ShaderModule shaderModule, vk::ShaderStageFlagBits shaderStage, vk::SpecializationInfo *pSpecialization = nullptr, std::string_view entryPointName = "main");
        GraphicsPipelineBuilder& ClearShaders();
        GraphicsPipelineBuilder& AddDescriptorLayout( vk::DescriptorSetLayout descriptorLayout);
        GraphicsPipelineBuilder& ClearDescriptorLayouts();
        GraphicsPipelineBuilder& AddPushConstant( vk::PushConstantRange pushConstant);
        GraphicsPipelineBuilder& ClearPushConstants();

        // vertex input
        GraphicsPipelineBuilder& SetVertexInputBindng( vk::VertexInputBindingDescription binding);
        GraphicsPipelineBuilder& SetVertexAttributes( std::span<vk::VertexInputAttributeDescription> attributes);
        // input assembly
        GraphicsPipelineBuilder& SetInputAssembly( vk::PrimitiveTopology topology, bool enablePrimitiveRestart = false);
        // rasterization
        GraphicsPipelineBuilder& SetCulling( vk::CullModeFlagBits cullMode, vk::FrontFace frontFace);
        GraphicsPipelineBuilder& SetDepthBias( float slope, float constant, float clamp);
        GraphicsPipelineBuilder& SetPolygonMode( vk::PolygonMode polygonMode);
        GraphicsPipelineBuilder& SetLineWidth( float lineWidth);
        // multisampling
        GraphicsPipelineBuilder& SetMultisampleCount( vk::SampleCountFlagBits sampleCount);
        GraphicsPipelineBuilder& EnableSampleShading( bool enable, float minSample = 1.0);
        GraphicsPipelineBuilder& EnableAlphaToOne( bool enable = true);
        GraphicsPipelineBuilder& EnableAlphaToCoverage( bool enable = true);
        // depth stencil
        GraphicsPipelineBuilder& EnableDepth( bool enableTest = true, bool enableWrite = true, vk::CompareOp compareOp = vk::CompareOp::eLess);
        GraphicsPipelineBuilder& EnableDepthBounds( bool enable, float minDepth = 0.0, float maxDepth = 1.0);
        GraphicsPipelineBuilder& EnableStencil( bool enable, vk::StencilOpState front = vk::StencilOpState{}, vk::StencilOpState back = vk::StencilOpState{});
        // color blend
        GraphicsPipelineBuilder& EnableBlending( bool enable = true, vk::BlendFactor srcBlend = vk::BlendFactor::eSrcAlpha, vk::BlendFactor dstBlend = vk::BlendFactor::eOneMinusSrcAlpha, vk::BlendOp blendOp = vk::BlendOp::eAdd);
        // dynamic states
        GraphicsPipelineBuilder& SetDynamicStates( std::initializer_list<vk::DynamicState> dynStates);
        // rendering
        GraphicsPipelineBuilder& SetColorFormats( std::span<vk::Format> formats);
        GraphicsPipelineBuilder& SetDepthFormat( vk::Format format);
        GraphicsPipelineBuilder& SetStencilFormat( vk::Format format);
    };


    void DestroyPipeline( vk::Pipeline pipeline);

    vk::PipelineLayout CreatePipelineLayout( std::span<vk::DescriptorSetLayout> descriptorLayouts, std::span<vk::PushConstantRange> pushConstants);

	void DestroyPipelineLayout( vk::PipelineLayout pipelineLayout);


    vk::ShaderModule LoadShader(std::string_view shaderPath);

	vk::ShaderModule CreateShaderModule( size_t codeSize, const char* code);

    void DestroyShaderModule( vk::ShaderModule shaderModule);


    vk::Viewport CreateViewport( float x, float y, float width, float height, float minDepth, float maxDepth);

    vk::Rect2D CreateScissor( uint32_t x, uint32_t y, uint32_t width, uint32_t height);


} // namespace vktg