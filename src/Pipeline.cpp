#include "Pipeline.hpp"

#include "Context.hpp"

Pipeline::Pipeline(const PipelineInfo& info) {
  std::vector<vk::PipelineShaderStageCreateInfo> shaderStages;
  for (auto&& [stage, shader] : info.shader.GetShaderModules()) {
    shaderStages.push_back(vk::PipelineShaderStageCreateInfo(vk::PipelineShaderStageCreateFlags(),
                                                             stage, shader, "main"));
  }

  vk::PipelineVertexInputStateCreateInfo vertexInputInfo(vk::PipelineVertexInputStateCreateFlags(),
                                                         info.vertexBindingDescription,
                                                         info.vertexAttributeDescriptions);

  vk::PipelineInputAssemblyStateCreateInfo inputAssembly(
      vk::PipelineInputAssemblyStateCreateFlags(), vk::PrimitiveTopology::eTriangleList, VK_FALSE);

  vk::Viewport viewport(0, 0, static_cast<float>(info.extent.width),
                        static_cast<float>(info.extent.height), 0.0F, 1.0F);
  vk::Rect2D scissor({0, 0}, info.extent);

  vk::PipelineViewportStateCreateInfo viewportState(vk::PipelineViewportStateCreateFlags(),
                                                    viewport, scissor);

  vk::PipelineRasterizationStateCreateInfo rasterizer(
      vk::PipelineRasterizationStateCreateFlags(), VK_FALSE, VK_FALSE, vk::PolygonMode::eFill,
      vk::CullModeFlagBits::eBack, vk::FrontFace::eCounterClockwise, VK_FALSE, 0.0F, 0.0F, 0.0F,
      1.0F);

  vk::PipelineMultisampleStateCreateInfo multisampling(vk::PipelineMultisampleStateCreateFlags(),
                                                       vk::SampleCountFlagBits::e1, VK_FALSE, 1.0F,
                                                       nullptr, VK_FALSE, VK_FALSE);

  vk::PipelineColorBlendAttachmentState colorBlendAttachment(
      VK_FALSE, vk::BlendFactor::eOne, vk::BlendFactor::eZero, vk::BlendOp::eAdd,
      vk::BlendFactor::eOne, vk::BlendFactor::eZero, vk::BlendOp::eAdd,
      vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
          vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);

  vk::PipelineColorBlendStateCreateInfo colorBlending(
      vk::PipelineColorBlendStateCreateFlags(), VK_FALSE, vk::LogicOp::eNoOp, colorBlendAttachment);

  std::array<vk::DynamicState, 2> dynamicStates = {vk::DynamicState::eViewport,
                                                   vk::DynamicState::eScissor};

  vk::PipelineDynamicStateCreateInfo dynamicState(vk::PipelineDynamicStateCreateFlags(),
                                                  dynamicStates);

  vk::PipelineLayoutCreateInfo pipelineLayoutInfo((vk::PipelineLayoutCreateFlags()));
  pipelineLayout = Context::Get()->GetDevice().createPipelineLayout(pipelineLayoutInfo);

  vk::GraphicsPipelineCreateInfo pipelineInfo(
      vk::PipelineCreateFlags(), shaderStages, &vertexInputInfo, &inputAssembly, nullptr,
      &viewportState, &rasterizer, &multisampling, nullptr, &colorBlending, nullptr, pipelineLayout,
      info.renderPass->GetHandle());

  vk::Result result{};
  std::tie(result, handle) =
      Context::Get()->GetDevice().createGraphicsPipeline(nullptr, pipelineInfo);
}
