
#include "pipelines.h"

#include <fstream>


namespace vktg
{


    /***    COMPUTE PIPELINE BUILDER    ***/
    
    
    ComputePipelineBuilder::ComputePipelineBuilder() : PipelineBuilder( Pipeline::Type::eCompute)
    {
        Reset();
    }


    void ComputePipelineBuilder::Reset() {

        shaderInfo = vk::PipelineShaderStageCreateInfo{}
            .setStage( vk::ShaderStageFlagBits::eCompute)
            .setPName( "main" );
        descriptorLayouts.clear();
        pushConstants.clear();
    }


    Pipeline ComputePipelineBuilder::Build() {

        Pipeline pipeline;
		pipeline.type = Pipeline::Type::eCompute;
		pipeline.pipelineLayout = CreatePipelineLayout( descriptorLayouts, pushConstants);

		auto pipelineInfo = vk::ComputePipelineCreateInfo{}
			.setStage( shaderInfo)
			.setLayout( pipeline.pipelineLayout);

        pipeline.pipeline = CreateComputePipeline( pipelineInfo);

		return pipeline;
    }

    
    ComputePipelineBuilder& ComputePipelineBuilder::SetShader( vk::ShaderModule shaderModule, const vk::SpecializationInfo *pSpecialization, std::string_view entryPointName) {

        shaderInfo
            .setModule( shaderModule )
            .setPName( entryPointName.data() )
            .setPSpecializationInfo( pSpecialization );

        return *this;
    }


    ComputePipelineBuilder& ComputePipelineBuilder::AddDescriptorLayout( vk::DescriptorSetLayout descriptorLayout) {
    
        descriptorLayouts.push_back( descriptorLayout);

        return *this;
    }

    
    ComputePipelineBuilder& ComputePipelineBuilder::AddPushConstant( const vk::PushConstantRange &pushConstant) {
    
        pushConstants.push_back( pushConstant);

        return *this;
    }


    /***    GRAPHICS PIÜELINE BUILDER    ***/


    GraphicsPipelineBuilder::GraphicsPipelineBuilder() : PipelineBuilder( Pipeline::Type::eGraphics) 
    {
        Reset();
    }

    
    void GraphicsPipelineBuilder::Reset() {

		shaderInfos.clear();
        descriptorLayouts.clear();
		pushConstants.clear();

		vertexInputBinding = vk::VertexInputBindingDescription{};
		vertexAttributes.clear();
		tesselationInfo = vk::PipelineTessellationStateCreateInfo{};
		inputAssemblyInfo = vk::PipelineInputAssemblyStateCreateInfo{};
		viewportInfo = vk::PipelineViewportStateCreateInfo{}
            .setViewportCount( 1 )
            .setScissorCount( 1 );
		rasterizerInfo = vk::PipelineRasterizationStateCreateInfo{};
		multisampleInfo = vk::PipelineMultisampleStateCreateInfo{};
		depthStencilInfo = vk::PipelineDepthStencilStateCreateInfo{};	
		colorBlendAttachment = vk::PipelineColorBlendAttachmentState{}
            .setColorWriteMask( vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA );
		dynamicStates.clear();
		renderInfo = vk::PipelineRenderingCreateInfo{}
            .setDepthAttachmentFormat( vk::Format::eD32Sfloat );
		colorAttachmentFormats = {vk::Format::eR8G8B8A8Srgb};
    }

    
    Pipeline GraphicsPipelineBuilder::Build() {

        Pipeline pipeline;
		pipeline.type = Pipeline::Type::eGraphics;
		pipeline.pipelineLayout = CreatePipelineLayout( descriptorLayouts, pushConstants);

        auto vertexInputInfo = vk::PipelineVertexInputStateCreateInfo{};
        if (!vertexAttributes.empty())
        {
            vertexInputInfo
                .setVertexAttributeDescriptionCount( (uint32_t)vertexAttributes.size() )
                .setPVertexAttributeDescriptions( vertexAttributes.data() )
                .setVertexBindingDescriptionCount( 1 )
                .setPVertexBindingDescriptions( &vertexInputBinding );
        }

        auto colorBlendInfo = vk::PipelineColorBlendStateCreateInfo{}
            .setAttachmentCount( 1 )
            .setPAttachments( &colorBlendAttachment );

        auto dynamicStateInfo = vk::PipelineDynamicStateCreateInfo{}
            .setDynamicStateCount( (uint32_t)dynamicStates.size() )
            .setPDynamicStates( dynamicStates.data() );


		auto pipelineInfo = vk::GraphicsPipelineCreateInfo{}
			.setStageCount( (uint32_t)shaderInfos.size() )
			.setPStages( shaderInfos.data() )
			.setPVertexInputState( &vertexInputInfo )
			.setPTessellationState( &tesselationInfo)
			.setPInputAssemblyState( &inputAssemblyInfo )
			.setPViewportState( &viewportInfo )
			.setPRasterizationState( &rasterizerInfo )
			.setPMultisampleState( &multisampleInfo )
			.setPDepthStencilState( &depthStencilInfo )
			.setPColorBlendState( &colorBlendInfo )
			.setPDynamicState( &dynamicStateInfo )
			.setLayout( pipeline.pipelineLayout )
			.setBasePipelineHandle( VK_NULL_HANDLE )
			.setBasePipelineIndex( -1 )
			.setPNext( &renderInfo );
		
		pipeline.pipeline = CreateGraphicsPipeline( pipelineInfo);

		return pipeline;
    }

    
    GraphicsPipelineBuilder& GraphicsPipelineBuilder::AddShader( vk::ShaderModule shaderModule, vk::ShaderStageFlagBits shaderStage, const vk::SpecializationInfo *pSpecialization, std::string_view entryPointName) {

        shaderInfos.emplace_back( 
            vk::PipelineShaderStageCreateInfo{}
                .setModule( shaderModule )
                .setStage( shaderStage )
                .setPName( entryPointName.data() )
                .setPSpecializationInfo( pSpecialization )
        );

        return *this;
    }

    
    GraphicsPipelineBuilder& GraphicsPipelineBuilder::ClearShaders() {

        shaderInfos.clear();

        return *this;
    }


    GraphicsPipelineBuilder& GraphicsPipelineBuilder::AddDescriptorLayout( vk::DescriptorSetLayout layout) {
    
        descriptorLayouts.push_back( layout);

        return *this;
    }

    
    GraphicsPipelineBuilder &GraphicsPipelineBuilder::AddDescriptorLayouts(std::span<vk::DescriptorSetLayout> layouts) {
        
        for (auto layout : layouts)
        {
            descriptorLayouts.push_back( layout);
        }

        return *this;
    }


    GraphicsPipelineBuilder& GraphicsPipelineBuilder::ClearDescriptorLayouts() {
    
        descriptorLayouts.clear();

        return *this;
    }


    GraphicsPipelineBuilder& GraphicsPipelineBuilder::AddPushConstant( const vk::PushConstantRange &pushConst) {
    
        pushConstants.push_back( pushConst);

        return *this;
    }

    
    GraphicsPipelineBuilder &GraphicsPipelineBuilder::AddPushConstant(std::span<vk::PushConstantRange> pushConsts) {
       
        for (auto pc : pushConsts)
        {
            pushConstants.push_back( pc);
        }

        return *this;
    }


    GraphicsPipelineBuilder &GraphicsPipelineBuilder::ClearPushConstants()
    {

        pushConstants.clear();

        return *this;
    }

    GraphicsPipelineBuilder &GraphicsPipelineBuilder::SetVertexInputBindng( const vk::VertexInputBindingDescription &binding) {

        vertexInputBinding = binding;

        return *this;
    }

    
    GraphicsPipelineBuilder &GraphicsPipelineBuilder::SetVertexAttributes( std::span<vk::VertexInputAttributeDescription> attributes) {

        vertexAttributes = std::vector<vk::VertexInputAttributeDescription>( attributes.begin(), attributes.end());

        return *this;
    }


    GraphicsPipelineBuilder &GraphicsPipelineBuilder::SetInputAssembly(vk::PrimitiveTopology topology, bool enablePrimitiveRestart) {

        inputAssemblyInfo
            .setTopology( topology )
            .setPrimitiveRestartEnable( enablePrimitiveRestart );

        return *this;
    }

    GraphicsPipelineBuilder& GraphicsPipelineBuilder::SetCulling( vk::CullModeFlagBits cullMode, vk::FrontFace frontFace) {
    
        rasterizerInfo
            .setCullMode( cullMode )
            .setFrontFace( frontFace );

        return *this;
    }

    
    GraphicsPipelineBuilder& GraphicsPipelineBuilder::SetDepthBias( float slope, float constant, float clamp) {

        rasterizerInfo
            .setDepthBiasSlopeFactor( slope )
            .setDepthBiasConstantFactor( constant )
            .setDepthBiasClamp( clamp );

        return *this;
    }


    GraphicsPipelineBuilder& GraphicsPipelineBuilder::SetPolygonMode( vk::PolygonMode polygonMode) {
   
        rasterizerInfo.setPolygonMode( polygonMode );

        return *this;
    }


    GraphicsPipelineBuilder& GraphicsPipelineBuilder::SetLineWidth( float lineWidth) {

        rasterizerInfo.setLineWidth( lineWidth );

        return *this;
    }


    GraphicsPipelineBuilder& GraphicsPipelineBuilder::SetMultisampleCount( vk::SampleCountFlagBits sampleCount) {

        multisampleInfo.setRasterizationSamples( sampleCount );

        return *this;
    }


    GraphicsPipelineBuilder& GraphicsPipelineBuilder::EnableSampleShading( bool enable, float minSample) {

        multisampleInfo
            .setSampleShadingEnable( enable )
            .setMinSampleShading( minSample );

        return *this;
    }


    GraphicsPipelineBuilder& GraphicsPipelineBuilder::EnableAlphaToOne( bool enable) {

        multisampleInfo.setAlphaToOneEnable( enable );

        return *this;
    }


    GraphicsPipelineBuilder& GraphicsPipelineBuilder::EnableAlphaToCoverage( bool enable) {

        multisampleInfo.setAlphaToCoverageEnable( enable );

        return *this;
    }

    
    GraphicsPipelineBuilder& GraphicsPipelineBuilder::EnableDepth( bool enableTest, bool enableWrite, vk::CompareOp compareOp) {

        depthStencilInfo
            .setDepthTestEnable( enableTest )
            .setDepthWriteEnable( enableWrite )
            .setDepthCompareOp( compareOp );

        return *this;
    }


    GraphicsPipelineBuilder& GraphicsPipelineBuilder::EnableDepthBounds( bool enable, float minDepth, float maxDepth) {

        depthStencilInfo
            .setDepthBoundsTestEnable( enable )
            .setMinDepthBounds( minDepth )
            .setMaxDepthBounds( maxDepth );

        return *this;
    }


    GraphicsPipelineBuilder& GraphicsPipelineBuilder::EnableStencil( bool enable, const vk::StencilOpState &front, const vk::StencilOpState &back) {
    
        depthStencilInfo
            .setStencilTestEnable( enable )
            .setFront( front )
            .setBack( back );

        return *this;
    }

    
    GraphicsPipelineBuilder& GraphicsPipelineBuilder::EnableBlending( bool enable, vk::BlendFactor srcBlend, vk::BlendFactor dstBlend, vk::BlendOp blendOp) {

        colorBlendAttachment
            .setBlendEnable( enable )
            .setSrcColorBlendFactor( srcBlend )
            .setDstColorBlendFactor( dstBlend )
            .setColorBlendOp( blendOp )
            .setSrcAlphaBlendFactor( vk::BlendFactor::eOne )
            .setDstAlphaBlendFactor( vk::BlendFactor::eZero )
            .setAlphaBlendOp( vk::BlendOp::eAdd );

        return *this;
    }


    GraphicsPipelineBuilder& GraphicsPipelineBuilder::SetDynamicStates( std::span<vk::DynamicState> dynStates) {
    
        dynamicStates = std::vector<vk::DynamicState>( dynStates.begin(), dynStates.end());

        return *this;
    }


    GraphicsPipelineBuilder& GraphicsPipelineBuilder::SetColorFormats( std::span<vk::Format> formats) {

        colorAttachmentFormats = std::vector<vk::Format>( formats.begin(), formats.end());

        renderInfo
            .setColorAttachmentCount( (uint32_t)colorAttachmentFormats.size() )
            .setPColorAttachmentFormats( colorAttachmentFormats.data() );

        return *this;
    }
    

    GraphicsPipelineBuilder& GraphicsPipelineBuilder::SetDepthFormat( vk::Format format) {
    
        renderInfo.setDepthAttachmentFormat( format );

        return *this;
    }

    
    GraphicsPipelineBuilder& GraphicsPipelineBuilder::SetStencilFormat( vk::Format format) {
    
        renderInfo.setStencilAttachmentFormat( format );

        return *this;
    }


    /***    UTILITIES    ***/


    vk::Pipeline CreateComputePipeline(vk::ComputePipelineCreateInfo &pipelineInfo) {

        vk::Pipeline pipeline;
        VK_CHECK( Device().createComputePipelines( nullptr, 1, &pipelineInfo, nullptr, &pipeline) );

        return pipeline;
    }


    vk::Pipeline CreateGraphicsPipeline(vk::GraphicsPipelineCreateInfo &pipelineInfo) {

        vk::Pipeline pipeline;
        VK_CHECK( Device().createGraphicsPipelines( nullptr, 1, &pipelineInfo, nullptr, &pipeline) );

        return pipeline;
    }


    void DestroyPipeline(vk::Pipeline pipeline)
    {

        Device().destroyPipeline( pipeline);
    }


    vk::PipelineLayout CreatePipelineLayout( std::span<vk::DescriptorSetLayout> descriptorLayouts, std::span<vk::PushConstantRange> pushConstants) {

        auto layoutInfo = vk::PipelineLayoutCreateInfo{}
            .setSetLayoutCount( (uint32_t)descriptorLayouts.size() )
            .setPSetLayouts( descriptorLayouts.data() )
            .setPushConstantRangeCount( (uint32_t)pushConstants.size() )
            .setPPushConstantRanges( pushConstants.data() );

        vk::PipelineLayout pipelineLayout;
        VK_CHECK( Device().createPipelineLayout( &layoutInfo, nullptr, &pipelineLayout) );

        return pipelineLayout;
    }


    void DestroyPipelineLayout( vk::PipelineLayout pipelineLayout) {

        Device().destroyPipelineLayout( pipelineLayout);
    }


    vk::ShaderModule LoadShader( std::string_view shaderPath) {

        std::ifstream file( shaderPath.data(), std::ios::binary);

        // get file size 
        file.seekg(0, file.end);
        std::streamsize size = file.tellg();
        file.seekg(0, file.beg);

        auto buffer = std::vector<char>( size, 0);
        if ( !file.read( buffer.data(), size))
        {
            throw std::runtime_error( "Unable to read file\n");
        }

        return vktg::CreateShaderModule( buffer.size(), buffer.data());
    }

    
    vk::ShaderModule CreateShaderModule( size_t codeSize, const char *code) {

        auto shaderInfo = vk::ShaderModuleCreateInfo{}
            .setCodeSize( codeSize )
            .setPCode( reinterpret_cast<const uint32_t*>( code ) );

        vk::ShaderModule shaderModule;
		VK_CHECK( Device().createShaderModule( &shaderInfo, nullptr, &shaderModule) );
        
        return shaderModule;
    }


    void DestroyShaderModule( vk::ShaderModule shaderModule) {

        Device().destroyShaderModule( shaderModule);
    }


    vk::Viewport CreateViewport( float x, float y, float width, float height, float minDepth, float maxDepth) {
        
        auto viewport = vk::Viewport{}
            .setX( x )
            .setY( y )
            .setWidth( width )
            .setHeight( height )
            .setMinDepth( minDepth )
            .setMaxDepth( maxDepth );

        return viewport;
    }


    vk::Rect2D CreateScissor( uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
        
        auto scissor = vk::Rect2D{}
            .setOffset( vk::Offset2D{(int32_t)x, (int32_t)y})
            .setExtent( vk::Extent2D{width, height} );
    
        return scissor;
    }
    

} // namespace vktg