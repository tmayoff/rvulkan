#include "RenderPass.hpp"

#include "VulkanContext.hpp"

RenderPass::RenderPass() {
  auto& context = GetCurrentVulkanContext();

  vk::AttachmentDescription colorAttachment;
  colorAttachment.setFormat(context.GetSurfaceFormat().format)
      .setSamples(vk::SampleCountFlagBits::e1)
      .setLoadOp(vk::AttachmentLoadOp::eClear)
      .setStoreOp(vk::AttachmentStoreOp::eStore)
      .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
      .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
      .setInitialLayout(vk::ImageLayout::eUndefined)
      .setFinalLayout(vk::ImageLayout::ePresentSrcKHR);

  vk::AttachmentReference colorRef;
  colorRef.setLayout(vk::ImageLayout::eColorAttachmentOptimal);

  vk::SubpassDescription subpass;
  subpass.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics).setColorAttachments(colorRef);

  vk::RenderPassCreateInfo renderPassInfo;
  renderPassInfo.setAttachments(colorAttachment).setSubpasses(subpass);

  renderPass = context.GetDevice().createRenderPass(renderPassInfo);

  vk::FramebufferCreateInfo framebufferInfo;
  framebufferInfo.setRenderPass(renderPass)
      //   .setAttachments(const vk::ArrayProxyNoTemporaries<const vk::ImageView>& attachments_)

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

  vk::PipelineVertexInputStateCreateInfo vertexInputInfo(vk::PipelineVertexInputStateCreateFlags(),
                                                         {}, {});

  vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
  inputAssemblyInfo.setPrimitiveRestartEnable(false).setTopology(
      vk::PrimitiveTopology::eTriangleList);

  vk::Viewport viewport;
  viewport.setWidth((float)context.GetSurfaceExtent().width);
  viewport.setHeight((float)context.GetSurfaceExtent().height);

  vk::Rect2D scissors;
  scissors.setExtent(context.GetSurfaceExtent());

  vk::PipelineViewportStateCreateInfo viewportState;
  viewportState.setViewports(viewport);
  viewportState.setScissors(scissors);

  vk::PipelineRasterizationStateCreateInfo rasterizer;
  rasterizer.setPolygonMode(vk::PolygonMode::eFill)
      .setCullMode(vk::CullModeFlagBits::eBack)
      .setFrontFace(vk::FrontFace::eCounterClockwise)
      .setLineWidth(1.0f);

  vk::PipelineMultisampleStateCreateInfo multisample;
  multisample.setRasterizationSamples(vk::SampleCountFlagBits::e1).setMinSampleShading(1.0f);

  vk::PipelineColorBlendAttachmentState colorAttachment;
  colorAttachment.setBlendEnable(true)
      .setSrcColorBlendFactor(vk::BlendFactor::eSrcAlpha)
      .setDstColorBlendFactor(vk::BlendFactor::eOneMinusSrcColor)
      .setColorBlendOp(vk::BlendOp::eAdd)
      .setSrcAlphaBlendFactor(vk::BlendFactor::eOne)
      .setDstAlphaBlendFactor(vk::BlendFactor::eZero)
      .setAlphaBlendOp(vk::BlendOp::eAdd)
      .setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                         vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);

  vk::PipelineColorBlendStateCreateInfo colorBlend;
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
      .setLayout(layout);

  vk::ResultValue<vk::Pipeline> res =
      GetCurrentVulkanContext().GetDevice().createGraphicsPipeline({}, pipelineCreateInfo);
  pipeline = res.value;
}
