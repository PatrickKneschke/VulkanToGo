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


    /// @brief Pipeline builder base class.
    struct PipelineBuilder {

        const Pipeline::Type type = Pipeline::Type::eCompute;

        /// @brief Initialize pipeline builder of given type.
        /// @param type_ Pipeline type to build.
        PipelineBuilder( Pipeline::Type type_) : type{type_} {}

        virtual void Reset() = 0;
        virtual Pipeline Build() = 0;
    };


    /// @brief Used to create compute pipelines.
    struct ComputePipelineBuilder : public PipelineBuilder {

        vk::PipelineShaderStageCreateInfo shaderInfo;
        std::vector<vk::DescriptorSetLayout> descriptorLayouts;
        std::vector<vk::PushConstantRange> pushConstants;

        ComputePipelineBuilder();
        /// @brief Resets pipeline builder to default setting.
        void Reset() override;
        /// @brief Creates compute pipeline with specified settings.
        /// @return Created Pipeline object. 
        Pipeline Build() override;

        /// @brief Set the compute shader used in the pipeline.
        /// @param shaderModule Shader module.
        /// @param pSpecialization Optional shader specialization constants.
        /// @param entryPointName Optional name of shader entry point function.
        /// @return Reference to ComputePipelineBuilder for chaining.
        ComputePipelineBuilder& SetShader( vk::ShaderModule shaderModule, vk::SpecializationInfo *pSpecialization = nullptr, std::string_view entryPointName = "main");
        /// @brief Add descriptor set layout to pipeline.
        /// @param descriptorLayout Descriptor set layout.
        /// @return Reference to ComputePipelineBuilder for chaining.
        ComputePipelineBuilder& AddDescriptorLayout( vk::DescriptorSetLayout descriptorLayout);
        /// @brief Add push constant to pipeline.
        /// @param pushConstant Push constant range.
        /// @return Reference to ComputePipelineBuilder for chaining.
        ComputePipelineBuilder& AddPushConstant( vk::PushConstantRange pushConstant);
    };


    /// @brief Used to create graphics pipelines.
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
        /// @brief Resets pipeline builder to default setting.
        void Reset() override;
        /// @brief Creates compute pipeline with specified settings.
        /// @return Created Pipeline object. 
        Pipeline Build() override;

        /// @brief Adds shader used in the graphics pipeline.
        /// @param shaderModule Shader module.
        /// @param shaderStage Shader stage.
        /// @param pSpecialization Optional shader specialization constants.
        /// @param entryPointName Optional of shader entry function name.
        /// @return Reference to GraphicsPipelineBuilder for chaining.
        GraphicsPipelineBuilder& AddShader( vk::ShaderModule shaderModule, vk::ShaderStageFlagBits shaderStage, vk::SpecializationInfo *pSpecialization = nullptr, std::string_view entryPointName = "main");
        /// @brief Removes all shader modules from pipeline builder.
        /// @return Reference to GraphicsPipelineBuilder for chaining.
        GraphicsPipelineBuilder& ClearShaders();
        /// @brief Adds single Vulkan descriptor set layout used in pipeline.
        /// @param layout Descriptor set layout.
        /// @return Reference to GraphicsPipelineBuilder for chaining.
        GraphicsPipelineBuilder& AddDescriptorLayout( vk::DescriptorSetLayout layout);
        /// @brief Adds multiple Vulkan descriptor set layouts used in pipeline.
        /// @param layouts List of descriptor set layouts.
        /// @return Reference to GraphicsPipelineBuilder for chaining.
        GraphicsPipelineBuilder& AddDescriptorLayouts( std::span<vk::DescriptorSetLayout> layouts);
        /// @brief Remove all descriptor set layouts from pipeline builder.
        /// @return Reference to GraphicsPipelineBuilder for chaining.
        GraphicsPipelineBuilder& ClearDescriptorLayouts();
        /// @brief Adds single push constant range used in pipeline
        /// @param pushConstant Push constant range.
        /// @return Reference to GraphicsPipelineBuilder for chaining.
        GraphicsPipelineBuilder& AddPushConstant( vk::PushConstantRange pushConst);
        /// @brief Adds multiple push constant ranges used in pipeline
        /// @param pushConstant List of push constant ranges.
        /// @return Reference to GraphicsPipelineBuilder for chaining.
        GraphicsPipelineBuilder& AddPushConstant( std::span<vk::PushConstantRange> pushConsts);
        /// @brief Removes all push constants from pipeline builder.
        /// @return Reference to GraphicsPipelineBuilder for chaining.
        GraphicsPipelineBuilder& ClearPushConstants();

        /// @brief Specify vertex input bindings.
        /// @param binding Vertex inout binding description
        /// @return Reference to GraphicsPipelineBuilder for chaining.
        GraphicsPipelineBuilder& SetVertexInputBindng( vk::VertexInputBindingDescription binding);
        /// @brief Specify vertex attributes.
        /// @param attributes List of vertex attribute descriptions.
        /// @return Reference to GraphicsPipelineBuilder for chaining.
        GraphicsPipelineBuilder& SetVertexAttributes( std::span<vk::VertexInputAttributeDescription> attributes);
        /// @brief Specify input assembly settings.
        /// @param topology Primitive topology.
        /// @param enablePrimitiveRestart Enable/Disable primitive restart.
        /// @return Reference to GraphicsPipelineBuilder for chaining.

        GraphicsPipelineBuilder& SetInputAssembly( vk::PrimitiveTopology topology, bool enablePrimitiveRestart = false);
        /// @brief Specify Culling settings.
        /// @param cullMode Cull mode flags.
        /// @param frontFace Front face orientation.
        /// @return Reference to GraphicsPipelineBuilder for chaining.

        GraphicsPipelineBuilder& SetCulling( vk::CullModeFlagBits cullMode, vk::FrontFace frontFace);
        /// @brief Specify depth bias settings.
        /// @param slope Depth bias slope.
        /// @param constant Depth bias constant.
        /// @param clamp Depth bias clamp.
        /// @return Reference to GraphicsPipelineBuilder for chaining.
        GraphicsPipelineBuilder& SetDepthBias( float slope, float constant, float clamp);
        /// @brief Specify polygon mode.
        /// @param polygonMode Polygon mode.
        /// @return Reference to GraphicsPipelineBuilder for chaining.
        GraphicsPipelineBuilder& SetPolygonMode( vk::PolygonMode polygonMode);
        /// @brief Specify line width.
        /// @param lineWidth Line width.
        /// @return Reference to GraphicsPipelineBuilder for chaining.
        GraphicsPipelineBuilder& SetLineWidth( float lineWidth);

        /// @brief Specify sample count for multisampling.
        /// @param sampleCount Sample count.
        /// @return Reference to GraphicsPipelineBuilder for chaining.
        GraphicsPipelineBuilder& SetMultisampleCount( vk::SampleCountFlagBits sampleCount);
        /// @brief Specify sample shading settings.
        /// @param enable Enable/Disable sample shading.
        /// @param minSample Minimum fraction of sample shading.
        /// @return Reference to GraphicsPipelineBuilder for chaining.
        GraphicsPipelineBuilder& EnableSampleShading( bool enable, float minSample = 1.0);
        /// @brief Specify if alpha to one is used in multisampling
        /// @param enable Enable/Disable alpha to one.
        /// @return Reference to GraphicsPipelineBuilder for chaining.
        GraphicsPipelineBuilder& EnableAlphaToOne( bool enable = true);
        /// @brief Specify if alpha to coverage is used in multisampling
        /// @param enable Enable/Disable alpha to coverage.
        /// @return Reference to GraphicsPipelineBuilder for chaining.
        GraphicsPipelineBuilder& EnableAlphaToCoverage( bool enable = true);

        /// @brief Specify depth settings
        /// @param enableTest Enable/Disable depth test.
        /// @param enableWrite Enable/Disable depth write.
        /// @param compareOp Depth compare operation.
        /// @return Reference to GraphicsPipelineBuilder for chaining.
        GraphicsPipelineBuilder& EnableDepth( bool enableTest = true, bool enableWrite = true, vk::CompareOp compareOp = vk::CompareOp::eLess);
        /// @brief Specify depth bound settings.
        /// @param enable Enable/Disable depth bounds.
        /// @param minDepth Minimum depth bound.
        /// @param maxDepth Maximum depth bound.
        /// @return Reference to GraphicsPipelineBuilder for chaining.
        GraphicsPipelineBuilder& EnableDepthBounds( bool enable, float minDepth = 0.0, float maxDepth = 1.0);
        /// @brief Specify stencul settings.
        /// @param enable Enable/Disable stencil.
        /// @param front Front stencil state.
        /// @param back Back stencil state.
        /// @return Reference to GraphicsPipelineBuilder for chaining.
        GraphicsPipelineBuilder& EnableStencil( bool enable, vk::StencilOpState front = vk::StencilOpState{}, vk::StencilOpState back = vk::StencilOpState{});

        /// @brief Specify blending settings.
        /// @param enable Enable/Disable blending.
        /// @param srcBlend Source blend factor.
        /// @param dstBlend Destination blend factor.
        /// @param blendOp Blending operation.
        /// @return Reference to GraphicsPipelineBuilder for chaining.
        GraphicsPipelineBuilder& EnableBlending( bool enable = true, vk::BlendFactor srcBlend = vk::BlendFactor::eSrcAlpha, vk::BlendFactor dstBlend = vk::BlendFactor::eOneMinusSrcAlpha, vk::BlendOp blendOp = vk::BlendOp::eAdd);

        /// @brief Specify pipeline dynamic states used in pipeline.
        /// @param dynStates List of dynamic states.
        /// @return Reference to GraphicsPipelineBuilder for chaining.
        GraphicsPipelineBuilder& SetDynamicStates( std::initializer_list<vk::DynamicState> dynStates);

        /// @brief Specify formats for color attachments used in dynamic rendering.
        /// @param formats List of color attachment formats. 
        /// @return Reference to GraphicsPipelineBuilder for chaining.
        GraphicsPipelineBuilder& SetColorFormats( std::span<vk::Format> formats);
        /// @brief Specify format for depth(stencil) attachment used in dynamic rendering
        /// @param format Depth attachment format.
        /// @return Reference to GraphicsPipelineBuilder for chaining.
        GraphicsPipelineBuilder& SetDepthFormat( vk::Format format);
        /// @brief Specify stencil format used in dynamic rendering. Just a placeholder for now, as only combined depth-stencil attachment is supported.
        /// @param format Stencil attachment format.
        /// @return Reference to GraphicsPipelineBuilder for chaining.
        GraphicsPipelineBuilder& SetStencilFormat( vk::Format format);
    };


    /// @brief Creates Vulkan compute pipeline from pipeline create info.
    /// @param pipelineInfo Pipeline create info,
    /// @return Vulkan pipeline.
    vk::Pipeline CreateComputePipeline( vk::ComputePipelineCreateInfo &pipelineInfo);
    /// @brief Creates Vulkan graphics pipeline from pipeline create info.
    /// @param pipelineInfo Pipeline create info,
    /// @return Vulkan pipeline.
    vk::Pipeline CreateGraphicsPipeline( vk::GraphicsPipelineCreateInfo &pipelineInfo);

    /// @brief Destroys given pipeline.
    /// @param pipeline Pipeline to destroy.
    void DestroyPipeline( vk::Pipeline pipeline);

    /// @brief Creates Vulkan pipeline layout.
    /// @param descriptorLayouts List of descriptor set layouts.
    /// @param pushConstants List of push constant ranges.
    /// @return Vulkan pipeline layout.
    vk::PipelineLayout CreatePipelineLayout( std::span<vk::DescriptorSetLayout> descriptorLayouts, std::span<vk::PushConstantRange> pushConstants);

	/// @brief Destroys given pipeline layout.
	/// @param pipelineLayout Pipeline layout to destroy.
	void DestroyPipelineLayout( vk::PipelineLayout pipelineLayout);


    /// @brief Loads shader code from given path and creates a new Vulkan shader module.
    /// @param shaderPath Path to shader file.
    /// @return Vulkan shader module.
    vk::ShaderModule LoadShader(std::string_view shaderPath);

	/// @brief Create Vulkan shader module from given byte code.
	/// @param codeSize Byte code size in bytes.
	/// @param code Pointer to buffer holding byte code.
	/// @return Vulkan  shader module.
	vk::ShaderModule CreateShaderModule( size_t codeSize, const char* code);

    /// @brief Destroys given shader module.
    /// @param shaderModule Shader module to destroy.
    void DestroyShaderModule( vk::ShaderModule shaderModule);


    /// @brief Creates Vulkan viewport.
    /// @param x Viewport origin x coordinate.
    /// @param y Viewport origin y coordinate.
    /// @param width Viewport width.
    /// @param height Viewport height.
    /// @param minDepth Viewport minimum depth.
    /// @param maxDepth Viewport maximum depth.
    /// @return Vulkan viewport.
    vk::Viewport CreateViewport( float x, float y, float width, float height, float minDepth, float maxDepth);

    /// @brief Creates Vulkan 2D rectangle for scissor.
    /// @param x Scissor origin x.
    /// @param y Scissor origin y.
    /// @param width Scissor width.
    /// @param height Scissor height.
    /// @return Vulkam rectangle 2D.
    vk::Rect2D CreateScissor( uint32_t x, uint32_t y, uint32_t width, uint32_t height);


} // namespace vktg