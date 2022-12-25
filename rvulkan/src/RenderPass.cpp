#include "RenderPass.hpp"

#include <vulkan/vulkan_enums.hpp>

#include "Pipeline.hpp"
#include "VulkanContext.hpp"

RenderPass::RenderPass(const VulkanContext& context, const PipelineOptions& pipelineOptions) {
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

  renderPass = context.GetLogicalDevice().GetHandle().createRenderPass(renderPassInfo);
  pipeline = Pipeline(context, pipelineOptions, renderPass);
}

void RenderPass::Render(const RenderPassState& state) { state.Commands.draw(3, 1, 0, 0); }
