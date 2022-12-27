#include "RenderPass.hpp"

#include <vulkan/vulkan_enums.hpp>

#include "Pipeline.hpp"
#include "VulkanContext.hpp"

RenderPass::RenderPass(const std::shared_ptr<VulkanContext>& context,
                       const PipelineOptions& pipelineOptions) {
  vk::AttachmentDescription color_attachment(
      {}, context->GetSurfaceFormat().format, vk::SampleCountFlagBits::e1,
      vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, vk::AttachmentLoadOp::eDontCare,
      vk::AttachmentStoreOp::eDontCare, vk::ImageLayout::eUndefined,
      vk::ImageLayout::ePresentSrcKHR);

  vk::AttachmentReference colour_ref({}, vk::ImageLayout::eColorAttachmentOptimal);

  vk::SubpassDependency dependency(
      VK_SUBPASS_EXTERNAL, 0, vk::PipelineStageFlagBits::eColorAttachmentOutput,
      vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::AccessFlagBits::eMemoryRead,
      vk::AccessFlagBits::eColorAttachmentWrite);

  vk::SubpassDescription subpass({}, vk::PipelineBindPoint::eGraphics, {}, colour_ref);

  vk::RenderPassCreateInfo renderPassInfo({}, color_attachment, subpass, dependency);

  renderPass = context->GetLogicalDevice().GetHandle().createRenderPass(renderPassInfo);
  pipeline = Pipeline(context, pipelineOptions, renderPass);
}
