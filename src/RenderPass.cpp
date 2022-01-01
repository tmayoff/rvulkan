#include "RenderPass.hpp"

#include "VulkanContext.hpp"

RenderPass::RenderPass() {
  auto& context = GetCurrentVulkanContext();

  vk::AttachmentDescription colorAttachment{};
  colorAttachment.setFormat(context.GetSurfaceFormat().format)
      .setSamples(vk::SampleCountFlagBits::e1)
      .setLoadOp(vk::AttachmentLoadOp::eClear)
      .setStoreOp(vk::AttachmentStoreOp::eStore)
      .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
      .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
      .setInitialLayout(vk::ImageLayout::eUndefined)
      .setFinalLayout(vk::ImageLayout::ePresentSrcKHR);

  vk::AttachmentReference colorRef{};
  colorRef.setLayout(vk::ImageLayout::eColorAttachmentOptimal);

  vk::SubpassDependency dependency{};
  dependency.setSrcSubpass(VK_SUBPASS_EXTERNAL)
      .setDstSubpass(0)
      .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
      .setSrcAccessMask(vk::AccessFlagBits::eMemoryRead)
      .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
      .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite);

  vk::SubpassDescription subpass{};
  subpass.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics).setColorAttachments(colorRef);

  vk::RenderPassCreateInfo renderPassInfo;
  renderPassInfo.setAttachments(colorAttachment).setSubpasses(subpass).setDependencies(dependency);

  renderPass = context.GetDevice().createRenderPass(renderPassInfo);

  createPipeline();
}

void RenderPass::createPipeline() {
  auto& context = GetCurrentVulkanContext();

  shader = Shader(Shader::ReadFile("./assets/vert.spv"), Shader::ReadFile("./assets/frag.spv"));

  std::array shaderStageInfos = {
      vk::PipelineShaderStageCreateInfo(vk::PipelineShaderStageCreateFlags(),
                                        vk::ShaderStageFlagBits::eVertex, shader.GetVertexModule(),
                                        "main"),
      vk::PipelineShaderStageCreateInfo(vk::PipelineShaderStageCreateFlags(),
                                        vk::ShaderStageFlagBits::eFragment,
                                        shader.GetFragmentModule(), "main")};

  std::vector<vk::VertexInputAttributeDescription> vertexAttributeDescriptions;
  std::vector<vk::VertexInputBindingDescription> vertexBindingDescriptions;

  vk::PipelineVertexInputStateCreateInfo vertexInputInfo(
      (vk::PipelineVertexInputStateCreateFlags()));

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
