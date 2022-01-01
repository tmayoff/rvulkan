#include "Pipeline.hpp"

#include "VulkanContext.hpp"
#include "vulkan/vulkan.hpp"

Pipeline::Pipeline(const PipelineOptions& options, const vk::RenderPass renderPass) {
  auto& context = GetCurrentVulkanContext();

  std::array shaderStageInfos = {
      vk::PipelineShaderStageCreateInfo(vk::PipelineShaderStageCreateFlags(),
                                        vk::ShaderStageFlagBits::eVertex,
                                        options.shader.GetVertexModule(), "main"),
      vk::PipelineShaderStageCreateInfo(vk::PipelineShaderStageCreateFlags(),
                                        vk::ShaderStageFlagBits::eFragment,
                                        options.shader.GetFragmentModule(), "main")};

  std::vector<vk::VertexInputAttributeDescription> vertexAttributeDescriptions;
  std::vector<vk::VertexInputBindingDescription> vertexBindingDescriptions = {
      vk::VertexInputBindingDescription(0, options.bufferLayout.GetStride(),
                                        vk::VertexInputRate::eVertex)};

  int location = 0;
  for (auto& inputAttrib : options.bufferLayout) {
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
  inputAssemblyInfo.setPrimitiveRestartEnable(false).setTopology(
      vk::PrimitiveTopology::eTriangleList);

  vk::Viewport viewport;
  viewport.setX(0)
      .setY(0)
      .setWidth((float)context.GetSurfaceExtent().width)
      .setHeight((float)context.GetSurfaceExtent().height)
      .setMinDepth(0.0f)
      .setMaxDepth(1.0f);

  vk::Rect2D scissors;
  scissors.setOffset({0, 0}).setExtent(context.GetSurfaceExtent());

  vk::PipelineViewportStateCreateInfo viewportState;
  viewportState.setViewports(viewport).setScissors(scissors);

  vk::PipelineRasterizationStateCreateInfo rasterizer;
  rasterizer.setDepthClampEnable(VK_FALSE)
      .setRasterizerDiscardEnable(false)
      .setPolygonMode(vk::PolygonMode::eFill)
      .setCullMode(vk::CullModeFlagBits::eBack)
      .setFrontFace(vk::FrontFace::eClockwise)
      .setDepthBiasEnable(false)
      .setLineWidth(1.0f);

  vk::PipelineMultisampleStateCreateInfo multisample;
  multisample.setSampleShadingEnable(false).setRasterizationSamples(vk::SampleCountFlagBits::e1);

  vk::PipelineColorBlendAttachmentState colorAttachment;
  colorAttachment.setBlendEnable(false)
      .setSrcColorBlendFactor(vk::BlendFactor::eOne)
      .setDstColorBlendFactor(vk::BlendFactor::eZero)
      .setColorBlendOp(vk::BlendOp::eAdd)
      .setSrcAlphaBlendFactor(vk::BlendFactor::eOne)
      .setDstAlphaBlendFactor(vk::BlendFactor::eZero)
      .setAlphaBlendOp(vk::BlendOp::eAdd)
      .setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                         vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);

  vk::PipelineColorBlendStateCreateInfo colorBlend{};
  colorBlend.setLogicOpEnable(false)
      .setLogicOp(vk::LogicOp::eCopy)
      .setAttachments(colorAttachment)
      .setBlendConstants({0.0f, 0.0f, 0.0f, 0.0f});

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
