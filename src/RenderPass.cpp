#include "RenderPass.hpp"

#include "Context.hpp"

RenderPass::RenderPass(const RenderPassInfo& info) {
  vk::AttachmentDescription colorAttachment(
      vk::AttachmentDescriptionFlags(), info.colorAttachmentFormat, vk::SampleCountFlagBits::e1,
      vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, vk::AttachmentLoadOp::eDontCare,
      vk::AttachmentStoreOp::eDontCare, vk::ImageLayout::eUndefined,
      vk::ImageLayout::ePresentSrcKHR);

  vk::AttachmentReference colorAttachmentRef(0, vk::ImageLayout::eColorAttachmentOptimal);
  vk::SubpassDescription subpass(vk::SubpassDescriptionFlags(), vk::PipelineBindPoint::eGraphics,
                                 {}, colorAttachmentRef);

  vk::SubpassDependency dependency(0, 0, vk::PipelineStageFlagBits::eColorAttachmentOutput,
                                   vk::PipelineStageFlagBits::eColorAttachmentOutput,
                                   vk::AccessFlagBits::eNoneKHR,
                                   vk::AccessFlagBits::eColorAttachmentWrite);

  vk::RenderPassCreateInfo createInfo(vk::RenderPassCreateFlags(), colorAttachment, subpass);

  handle = Context::Get()->GetDevice().createRenderPass(createInfo);
}

RenderPass::~RenderPass() {}
