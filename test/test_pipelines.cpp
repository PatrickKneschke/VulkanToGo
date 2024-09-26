
#include <catch2/catch.hpp>

#include "../vulkantogo/pipelines.h"
#include "../vulkantogo/storage.h"

#include <vector>


TEST_CASE( "create pipeline layout", "[pipelines]") {

    std::vector<vk::DescriptorSetLayout> setLayouts;
    std::vector<vk::PushConstantRange> pushConstatns;

    vk::PipelineLayout pipelineLayout = vktg::CreatePipelineLayout( setLayouts, pushConstatns);

    REQUIRE_FALSE( !pipelineLayout );

    vktg::DestroyPipelineLayout( pipelineLayout);
}


TEST_CASE( "load shader, wrong path", "[pipelines]") {

    REQUIRE_THROWS( vktg::LoadShader( "wrong_shader_path.spv") );
}


TEST_CASE( "create shader module", "[commands]") {

    vk::ShaderModule shader = vktg::LoadShader( "../res/shaders/test_comp.spv");

    REQUIRE_FALSE( !shader );
}


TEST_CASE( "create compute pipeline builder", "[pipelines]") {

    vktg::ComputePipelineBuilder builder;

    REQUIRE( builder.type == vktg::Pipeline::Type::eCompute );
    REQUIRE( builder.descriptorLayouts.empty() );
    REQUIRE( builder.pushConstants.empty() );
    REQUIRE( builder.shaderInfo.stage == vk::ShaderStageFlagBits::eCompute );
}


TEST_CASE( "build compute pipeline", "[pipelines]") {

    vktg::ComputePipelineBuilder builder;

    vk::ShaderModule shader = vktg::LoadShader( "../res/shaders/test_comp.spv");

    auto setBinding = vk::DescriptorSetLayoutBinding{}
        .setStageFlags( vk::ShaderStageFlagBits::eCompute )
        .setDescriptorType( vk::DescriptorType::eStorageImage )
        .setBinding( 0 )
        .setDescriptorCount( 1 );

    auto setLayoutInfo = vk::DescriptorSetLayoutCreateInfo{}
        .setBindingCount( 1 )
        .setPBindings( &setBinding );

    vk::DescriptorSetLayout setLayout;
    VK_CHECK( vktg::Device().createDescriptorSetLayout( &setLayoutInfo, nullptr, &setLayout) );

    auto pushConstant = vk::PushConstantRange{}
        .setStageFlags( vk::ShaderStageFlagBits::eCompute )
        .setOffset( 0 )
        .setSize( 64 );

    builder
        .SetShader( shader,  nullptr, "main")
        .AddDescriptorLayout( setLayout)
        .AddPushConstant( pushConstant);
    vktg::Pipeline pipeline = builder.Build();

    REQUIRE( pipeline.type == vktg::Pipeline::Type::eCompute );
    REQUIRE_FALSE( !pipeline.pipelineLayout );
    REQUIRE_FALSE( !pipeline.pipeline );

    vktg::DestroyPipelineLayout( pipeline.pipelineLayout);
    vktg::DestroyPipeline( pipeline.pipeline);
}


TEST_CASE( "create graphics pipeline builder", "[pipelines]") {

    vktg::GraphicsPipelineBuilder builder;

    REQUIRE( builder.type == vktg::Pipeline::Type::eGraphics );
    REQUIRE( builder.shaderInfos.empty() );
    REQUIRE( builder.descriptorLayouts.empty() );
    REQUIRE( builder.pushConstants.empty() );
    REQUIRE( builder.vertexAttributes.empty() );
    REQUIRE( builder.dynamicStates.empty() );
    REQUIRE( builder.colorAttachmentFormats[0] == vk::Format::eR8G8B8A8Srgb );
}


TEST_CASE( "add shaders to graphics pipeline bulder", "[pipelines]") {

    vktg::GraphicsPipelineBuilder builder;

    vk::ShaderModule vertShader = vktg::LoadShader( "../res/shaders/test_vert.spv");
    vk::ShaderModule fragShader = vktg::LoadShader( "../res/shaders/test_frag.spv");

    builder
        .AddShader( vertShader, vk::ShaderStageFlagBits::eVertex, nullptr, "main")
        .AddShader( fragShader, vk::ShaderStageFlagBits::eFragment, nullptr, "main");

    REQUIRE( builder.shaderInfos[0].stage == vk::ShaderStageFlagBits::eVertex );
    REQUIRE( builder.shaderInfos[1].stage == vk::ShaderStageFlagBits::eFragment );
}


TEST_CASE( "set graphics pipeline builder dynamic states", "[pipelines]") {

    vktg::GraphicsPipelineBuilder builder;
    builder.SetDynamicStates( {vk::DynamicState::eViewport, vk::DynamicState::eScissor});

    REQUIRE( builder.dynamicStates.size() == 2 );
    REQUIRE( builder.dynamicStates[0] == vk::DynamicState::eViewport );
    REQUIRE( builder.dynamicStates[1] == vk::DynamicState::eScissor );
}


TEST_CASE( "set graphics pipeline builder vertex input binding", "[pipelines]") {

    vktg::GraphicsPipelineBuilder builder;
    builder.SetVertexInputBindng( 
        vk::VertexInputBindingDescription{}
            .setBinding( 0 )
            .setStride( 4 * sizeof(float) )
            .setInputRate( vk::VertexInputRate::eVertex )
    );

    REQUIRE( builder.vertexInputBinding.binding == 0 );
    REQUIRE( builder.vertexInputBinding.stride == 4 * sizeof(float) );
    REQUIRE( builder.vertexInputBinding.inputRate == vk::VertexInputRate::eVertex );
}


TEST_CASE( "set graphics pipeline builder vertex stributes", "[pipelines]") {

    vktg::GraphicsPipelineBuilder builder;

    std::vector<vk::VertexInputAttributeDescription> attributes = {
        vk::VertexInputAttributeDescription{}
            .setLocation( 0 )
            .setBinding( 0 )
            .setFormat( vk::Format::eR32G32B32A32Sfloat )
            .setOffset( 0 )
    };

    builder.SetVertexAttributes( attributes );

    REQUIRE( builder.vertexAttributes[0].location == 0 );
    REQUIRE( builder.vertexAttributes[0].binding == 0 );
    REQUIRE( builder.vertexAttributes[0].format == vk::Format::eR32G32B32A32Sfloat );
    REQUIRE( builder.vertexAttributes[0].offset == 0 );
}


TEST_CASE( "set graphics pipeline builder input assembly", "[pipelines]") {

    vktg::GraphicsPipelineBuilder builder;
    builder.SetInputAssembly( vk::PrimitiveTopology::eTriangleList, true);

    REQUIRE( builder.inputAssemblyInfo.topology == vk::PrimitiveTopology::eTriangleList );
    REQUIRE( builder.inputAssemblyInfo.primitiveRestartEnable == true );
}


TEST_CASE( "set graphics pipeline builder culling", "[pipelines]") {

    vktg::GraphicsPipelineBuilder builder;
    builder.SetCulling( vk::CullModeFlagBits::eBack, vk::FrontFace::eCounterClockwise);

    REQUIRE( builder.rasterizerInfo.cullMode == vk::CullModeFlagBits::eBack );
    REQUIRE( builder.rasterizerInfo.frontFace == vk::FrontFace::eCounterClockwise );
}


TEST_CASE( "set graphics pipeline builder depth bias", "[pipelines]") {

    vktg::GraphicsPipelineBuilder builder;
    builder.SetDepthBias( 0.5f, 0.2f, 0.9f);

    REQUIRE( builder.rasterizerInfo.depthBiasSlopeFactor == 0.5f );
    REQUIRE( builder.rasterizerInfo.depthBiasConstantFactor == 0.2f );
    REQUIRE( builder.rasterizerInfo.depthBiasClamp == 0.9f );
}


TEST_CASE( "set graphics pipeline builder polygon mode", "[pipelines]") {

    vktg::GraphicsPipelineBuilder builder;
    builder.SetPolygonMode( vk::PolygonMode::eFill );

    REQUIRE( builder.rasterizerInfo.polygonMode == vk::PolygonMode::eFill );
}


TEST_CASE( "set graphics pipeline builder line width", "[pipelines]") {

    vktg::GraphicsPipelineBuilder builder;
    builder.SetLineWidth( 2.f);
    
    REQUIRE( builder.rasterizerInfo.lineWidth == 2.f );
}


TEST_CASE( "set graphics pipeline builder multisample count", "[pipelines]") {

    vktg::GraphicsPipelineBuilder builder;
    builder.SetMultisampleCount( vk::SampleCountFlagBits::e8 );
    
    REQUIRE( builder.multisampleInfo.rasterizationSamples == vk::SampleCountFlagBits::e8 );
}


TEST_CASE( "enable graphics pipeline builder sample shading", "[pipelines]") {

    vktg::GraphicsPipelineBuilder builder;
    builder.EnableSampleShading( true, 0.5f);
    
    REQUIRE( builder.multisampleInfo.sampleShadingEnable == true );
    REQUIRE( builder.multisampleInfo.minSampleShading == 0.5f );
}


TEST_CASE( "enable graphics pipeline builder multisampling alpha to one", "[pipelines]") {

    vktg::GraphicsPipelineBuilder builder;
    builder.EnableAlphaToOne( true );
    
    REQUIRE( builder.multisampleInfo.alphaToOneEnable == true );
}


TEST_CASE( "enable graphics pipeline builder multisampling alpha to coverage", "[pipelines]") {

    vktg::GraphicsPipelineBuilder builder;
    builder.EnableAlphaToCoverage( true );
    
    REQUIRE( builder.multisampleInfo.alphaToCoverageEnable == true );
}


TEST_CASE( "enable graphics pipeline builder depth test and write", "[pipelines]") {

    vktg::GraphicsPipelineBuilder builder;
    builder.EnableDepth( true, false);
    
    REQUIRE( builder.depthStencilInfo.depthTestEnable == true );
    REQUIRE( builder.depthStencilInfo.depthWriteEnable == false );
}


TEST_CASE( "enable graphics pipeline builder depth bounds", "[pipelines]") {

    vktg::GraphicsPipelineBuilder builder;
    builder.EnableDepthBounds( true, 0.2f, 0.8f);
    
    REQUIRE( builder.depthStencilInfo.minDepthBounds == 0.2f );
    REQUIRE( builder.depthStencilInfo.maxDepthBounds == 0.8f );
}


TEST_CASE( "enable graphics pipeline builder stencil test", "[pipelines]") {

    vktg::GraphicsPipelineBuilder builder;
    builder.EnableStencil( true);
    
    REQUIRE( builder.depthStencilInfo.stencilTestEnable == true );
}


TEST_CASE( "enable graphics pipeline builder color blend", "[pipelines]") {

    vktg::GraphicsPipelineBuilder builder;
    builder.EnableBlending( true);
    
    REQUIRE( builder.colorBlendAttachment.blendEnable == true );
    REQUIRE( builder.colorBlendAttachment.srcColorBlendFactor == vk::BlendFactor::eSrcAlpha );
    REQUIRE( builder.colorBlendAttachment.dstColorBlendFactor == vk::BlendFactor::eOneMinusSrcAlpha );
    REQUIRE( builder.colorBlendAttachment.colorBlendOp == vk::BlendOp::eAdd );
    REQUIRE( builder.colorBlendAttachment.srcAlphaBlendFactor == vk::BlendFactor::eOne );
    REQUIRE( builder.colorBlendAttachment.dstAlphaBlendFactor == vk::BlendFactor::eZero );
    REQUIRE( builder.colorBlendAttachment.alphaBlendOp == vk::BlendOp::eAdd );
}


TEST_CASE( "set graphics pipeline builder color attachment formats", "[pipelines]") {

    vktg::GraphicsPipelineBuilder builder;
    std::vector<vk::Format> colorAttachmentFormats = {vk::Format::eR16G16B16A16Sfloat, vk::Format::eR8G8B8A8Srgb};
    builder.SetColorFormats( colorAttachmentFormats );
    
    REQUIRE( builder.renderInfo.colorAttachmentCount == 2 );
    REQUIRE( builder.renderInfo.pColorAttachmentFormats[0] == vk::Format::eR16G16B16A16Sfloat );
    REQUIRE( builder.renderInfo.pColorAttachmentFormats[1] == vk::Format::eR8G8B8A8Srgb );
}


TEST_CASE( "set graphics pipeline builder depth attachment format", "[pipelines]") {

    vktg::GraphicsPipelineBuilder builder;
    builder.SetDepthFormat( vk::Format::eD32Sfloat );
    
    REQUIRE( builder.renderInfo.depthAttachmentFormat == vk::Format::eD32Sfloat );
}


TEST_CASE( "set graphics pipeline builder stencil attachment format", "[pipelines]") {

    vktg::GraphicsPipelineBuilder builder;
    builder.SetStencilFormat( vk::Format::eS8Uint );
    
    REQUIRE( builder.renderInfo.stencilAttachmentFormat == vk::Format::eS8Uint );
}


TEST_CASE( "create graphics pipeline", "[pipelines]") {

    vktg::GraphicsPipelineBuilder builder;
    
    vk::ShaderModule vertShader = vktg::LoadShader( "../res/shaders/test_vert.spv");
    vk::ShaderModule fragShader = vktg::LoadShader( "../res/shaders/test_frag.spv");
    std::vector<vk::Format> colorAttachmentFormats = {vk::Format::eR16G16B16A16Sfloat};

    vktg::Pipeline pipeline = builder
        .AddShader( vertShader, vk::ShaderStageFlagBits::eVertex, nullptr, "main")
        .AddShader( fragShader, vk::ShaderStageFlagBits::eFragment, nullptr, "main")    
        .SetInputAssembly( vk::PrimitiveTopology::eTriangleList)
        .SetPolygonMode( vk::PolygonMode::eFill )
        .SetCulling( vk::CullModeFlagBits::eBack, vk::FrontFace::eCounterClockwise )
        .EnableDepth( true, true, vk::CompareOp::eLess )
        .EnableBlending( true )
        .SetDynamicStates( {vk::DynamicState::eViewport, vk::DynamicState::eScissor} )
        .SetDepthFormat( vk::Format::eD32Sfloat )
        .SetColorFormats( colorAttachmentFormats )
        .Build();
        
    REQUIRE_FALSE( !pipeline.pipelineLayout );
    REQUIRE_FALSE( !pipeline.pipeline );

    vktg::DestroyPipelineLayout( pipeline.pipelineLayout);
    vktg::DestroyPipeline( pipeline.pipeline);
}
