#include "RenderPass.hpp"

#include <vulkan/vulkan_core.h>

#include <rvulkan/vulkan_context.hpp>
#include <vulkan/LogicalDevice.hpp>
#include <vulkan/vulkan_enums.hpp>

#include "Pipeline.hpp"

RenderPass::RenderPass(const std::shared_ptr<VulkanContext>& context,
                       const PipelineOptions& pipelineOptions)
    : device(context->GetLogicalDevice()->GetHandle()) {
  vk::AttachmentDescription color_attachment(
      {}, context->GetSurfaceFormat().format, vk::SampleCountFlagBits::e1,
      vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, vk::AttachmentLoadOp::eDontCare,
      vk::AttachmentStoreOp::eDontCare, vk::ImageLayout::eUndefined,
      vk::ImageLayout::ePresentSrcKHR);

  vk::AttachmentReference colour_ref(0, vk::ImageLayout::eColorAttachmentOptimal);

  vk::SubpassDependency dependency(
      0, VK_SUBPASS_EXTERNAL, vk::PipelineStageFlagBits::eColorAttachmentOutput,
      vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::AccessFlagBits::eMemoryRead,
      vk::AccessFlagBits::eColorAttachmentWrite);

  vk::SubpassDescription subpass({}, vk::PipelineBindPoint::eGraphics, {}, colour_ref);

  vk::RenderPassCreateInfo render_pass_info({}, color_attachment, subpass, dependency);

  renderPass = context->GetLogicalDevice()->GetHandle().createRenderPass(render_pass_info);
  pipeline = std::make_shared<Pipeline>(context, pipelineOptions, renderPass);
}

RenderPass::~RenderPass() {
  pipeline.reset();

  device.destroyShaderModule(shader.GetFragmentModule());
  device.destroyShaderModule(shader.GetVertexModule());
  device.destroyRenderPass(renderPass);
}
