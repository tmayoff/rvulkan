#include "Pipeline.hpp"

#include "VulkanContext.hpp"

Pipeline::Pipeline(const PipelineOptions& options, const vk::RenderPass renderPass) {
  auto& context = GetCurrentVulkanContext();

  const std::array shaderStageInfos = {
      vk::PipelineShaderStageCreateInfo(vk::PipelineShaderStageCreateFlags(),
                                        vk::ShaderStageFlagBits::eVertex,
                                        options.shader.GetVertexModule(), "main"),
      vk::PipelineShaderStageCreateInfo(vk::PipelineShaderStageCreateFlags(),
                                        vk::ShaderStageFlagBits::eFragment,
                                        options.shader.GetFragmentModule(), "main")};

  std::vector<vk::VertexInputAttributeDescription> vertexAttributeDescriptions;
  const std::vector<vk::VertexInputBindingDescription> vertexBindingDescriptions = {
      vk::VertexInputBindingDescription(0, options.bufferLayout.GetStride(),
                                        vk::VertexInputRate::eVertex)};

  int location = 0;
  for (const auto& inputAttrib : options.bufferLayout) {
    vk::VertexInputAttributeDescription inputDesc{};
    inputDesc.setLocation(location)
        .setBinding(0)
        .setOffset(inputAttrib.Offset)
        .setFormat(ShaderDataTypeToVkForamt(inputAttrib.Type));

    vertexAttributeDescriptions.push_back(inputDesc);
    location++;
  }

  vk::PipelineVertexInputStateCreateInfo vertexInputInfo(vk::PipelineVertexInputStateCreateFlags(),
                                                         vertexBindingDescriptions,
                                                         vertexAttributeDescriptions);

  vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
  inputAssemblyInfo.setPrimitiveRestartEnable(VK_FALSE).setTopology(
      vk::PrimitiveTopology::eTriangleList);

  vk::Viewport viewport;
  viewport.setX(0)
      .setY(0)
      .setWidth(static_cast<float>(context.GetSurfaceExtent().width))
      .setHeight(static_cast<float>(context.GetSurfaceExtent().height))
      .setMinDepth(0.0F)
      .setMaxDepth(1.0F);

  vk::Rect2D scissors;
  scissors.setOffset({0, 0}).setExtent(context.GetSurfaceExtent());

  vk::PipelineViewportStateCreateInfo viewportState;
  viewportState.setViewports(viewport).setScissors(scissors);

  vk::PipelineRasterizationStateCreateInfo rasterizer;
  rasterizer.setDepthClampEnable(VK_FALSE)
      .setRasterizerDiscardEnable(VK_FALSE)
      .setPolygonMode(vk::PolygonMode::eFill)
      .setCullMode(vk::CullModeFlagBits::eBack)
      .setFrontFace(vk::FrontFace::eClockwise)
      .setDepthBiasEnable(VK_FALSE)
      .setLineWidth(1.0F);

  vk::PipelineMultisampleStateCreateInfo multisample;
  multisample.setSampleShadingEnable(VK_FALSE).setRasterizationSamples(vk::SampleCountFlagBits::e1);

  vk::PipelineColorBlendAttachmentState colorAttachment;
  colorAttachment.setBlendEnable(VK_FALSE)
      .setSrcColorBlendFactor(vk::BlendFactor::eOne)
      .setDstColorBlendFactor(vk::BlendFactor::eZero)
      .setColorBlendOp(vk::BlendOp::eAdd)
      .setSrcAlphaBlendFactor(vk::BlendFactor::eOne)
      .setDstAlphaBlendFactor(vk::BlendFactor::eZero)
      .setAlphaBlendOp(vk::BlendOp::eAdd)
      .setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                         vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);

  vk::PipelineColorBlendStateCreateInfo colorBlend{};
  colorBlend.setLogicOpEnable(VK_FALSE)
      .setLogicOp(vk::LogicOp::eCopy)
      .setAttachments(colorAttachment)
      .setBlendConstants({0.0F, 0.0F, 0.0F, 0.0F});

  layout = context.GetDevice().createPipelineLayout(vk::PipelineLayoutCreateInfo());

  vk::GraphicsPipelineCreateInfo pipelineCreateInfo;
  pipelineCreateInfo.setStages(shaderStageInfos)
      .setPVertexInputState(&vertexInputInfo)
      .setPInputAssemblyState(&inputAssemblyInfo)
      .setPViewportState(&viewportState)
      .setPRasterizationState(&rasterizer)
      .setPMultisampleState(&multisample)
      .setPColorBlendState(&colorBlend)
      .setRenderPass(renderPass)
      .setSubpass(0)
      .setLayout(layout);

  vk::ResultValue<vk::Pipeline> res =
      GetCurrentVulkanContext().GetDevice().createGraphicsPipeline({}, pipelineCreateInfo);
  pipeline = res.value;
}
