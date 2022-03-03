#include "Application.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <set>
#include <stdexcept>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>

#include "Assert.hpp"
#include "Context.hpp"
#include "Shader.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"

void Application::Run() {
  InitWindow();
  InitVulkan();
  MainLoop();
  Cleanup();
}

void Application::InitWindow() {
  SDL_Init(SDL_INIT_VIDEO);

  window = SDL_CreateWindow("dvulkan", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                            static_cast<int32_t>(WIDTH), static_cast<int32_t>(HEIGHT),
                            SDL_WINDOW_VULKAN | SDL_WINDOW_SHOWN);
  if (window == nullptr)
    throw std::runtime_error("Failed to create window: " + std::string(SDL_GetError()));
}

void Application::InitVulkan() {
  auto ctx = Context::Get();

  // TODO Setup Debugging

  CreateSwapchain();
  CreateImageViews();

  RenderPassInfo info{};
  info.colorAttachmentFormat = swapchainFormat;
  renderPass = RenderPass(info);

  CreateDescriptorSetLayout();
  CreateGraphicsPipeline();
  CreateFramebuffers();

  {
    // Create Command Pool
    QueueFamilyIndices queueIndices = ctx->FindQueueFamilies(ctx->GetPhysicalDevice());
    vk::CommandPoolCreateInfo poolInfo(vk::CommandPoolCreateFlags(),
                                       queueIndices.graphicsFamily.value());
    commandPool = ctx->GetDevice().createCommandPool(poolInfo);
  }

  {
    // Create Vertex Buffer
    vk::DeviceSize bufSize = sizeof(Vertex) * vertices.size();

    auto [stagingBuf, stagingMem] = CreateBuffer(
        bufSize, vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

    uint8_t *data = static_cast<uint8_t *>(ctx->GetDevice().mapMemory(stagingMem, 0, bufSize));
    memcpy(data, vertices.data(), sizeof(Vertex) * vertices.size());
    ctx->GetDevice().unmapMemory(stagingMem);

    std::tie(vertexBuffer, vertexBufferMemory) = CreateBuffer(
        bufSize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
        vk::MemoryPropertyFlagBits::eDeviceLocal);

    CopyBuffer(stagingBuf, vertexBuffer, bufSize);
  }

  {
    // Index buffer
    vk::DeviceSize bufSize = sizeof(indices[0]) * indices.size();

    auto [stagingBuf, stagingMem] = CreateBuffer(
        bufSize, vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

    void *data = ctx->GetDevice().mapMemory(stagingMem, 0, bufSize);
    memcpy(data, indices.data(), bufSize);
    ctx->GetDevice().unmapMemory(stagingMem);

    std::tie(indexBuffer, indexBufferMemory) = CreateBuffer(
        bufSize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer,
        vk::MemoryPropertyFlagBits::eDeviceLocal);

    CopyBuffer(stagingBuf, indexBuffer, bufSize);
  }

  CreateUniformBuffers();
  CreateDescriptorPool();
  CreateDescriptorSets();

  CreateCommandBuffers();

  {
    imagesInFlight.resize(swapchainImages.size(), VK_NULL_HANDLE);

    // Create Sync Objects
    vk::SemaphoreCreateInfo semaphoreInfo((vk::SemaphoreCreateFlags()));
    vk::FenceCreateInfo fenceInfo(vk::FenceCreateFlagBits::eSignaled);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
      imageAvailableSempaphores.at(i) = ctx->GetDevice().createSemaphore(semaphoreInfo);
      renderFinishedSemaphores.at(i) = ctx->GetDevice().createSemaphore(semaphoreInfo);
      inFlightFences.at(i) = ctx->GetDevice().createFence(fenceInfo);
    }
  }
}

void Application::MainLoop() {
  auto ctx = Context::Get();

  while (running) {
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
      switch (e.type) {
        case SDL_QUIT:
          running = false;
          break;
        case SDL_WINDOWEVENT_RESIZED:
          framebufferResized = true;
          break;
      }
    }

    while (vk::Result::eTimeout ==
           ctx->GetDevice().waitForFences(inFlightFences.at(currentFrame), VK_TRUE, UINT64_MAX))
      ;

    auto currentBuffer = ctx->GetDevice().acquireNextImageKHR(
        swapchain, UINT64_MAX, imageAvailableSempaphores.at(currentFrame));

    if (currentBuffer.result == vk::Result::eErrorOutOfDateKHR) {
      RecreateSwapchain();
      continue;
    }

    if (currentBuffer.result != vk::Result::eSuccess &&
        currentBuffer.result != vk::Result::eSuboptimalKHR) {
      throw std::runtime_error("Failed to acquire swapchain image");
    }

    uint32_t imageIndex = currentBuffer.value;

    UpdateUniformBuffers(imageIndex);

    if (imagesInFlight.at(imageIndex)) {
      while (vk::Result::eTimeout ==
             ctx->GetDevice().waitForFences(imagesInFlight.at(imageIndex), VK_TRUE, UINT64_MAX)) {
        ;
      }
    }

    imagesInFlight[imageIndex] = inFlightFences.at(currentFrame);

    std::array<vk::Semaphore, 1> signalSemaphores = {renderFinishedSemaphores.at(currentFrame)};
    std::array<vk::PipelineStageFlags, 1> waitStages = {
        vk::PipelineStageFlagBits::eColorAttachmentOutput};

    vk::SubmitInfo submitInfo(imageAvailableSempaphores.at(currentFrame), waitStages,
                              commandBuffers, signalSemaphores);

    ctx->GetDevice().resetFences(inFlightFences.at(currentFrame));

    ctx->GetGraphicsQueue().submit(submitInfo, inFlightFences.at(currentFrame));

    // Present
    vk::PresentInfoKHR presentInfo(signalSemaphores, swapchain, imageIndex);
    vk::Result result = ctx->GetPresentQueue().presentKHR(presentInfo);

    if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR ||
        framebufferResized) {
      framebufferResized = false;
      RecreateSwapchain();
    } else if (result != vk::Result::eSuccess)
      throw std::runtime_error("Failed to present swapchain image");

    ctx->GetPresentQueue().waitIdle();
    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
  }

  ctx->GetDevice().waitIdle();
}

void Application::UpdateUniformBuffers(uint32_t currentImage) {
  auto ctx = Context::Get();
  static auto startTime = std::chrono::high_resolution_clock::now();

  auto currentTime = std::chrono::high_resolution_clock::now();
  float time =
      std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

  UniformBufferObject ubo{};
  ubo.model = glm::rotate(glm::mat4(1.0F), time * glm::radians(90.0F), glm::vec3(0.0F, 0.0F, 1.0F));
  ubo.view = glm::lookAt(glm::vec3(2.0F, 2.0F, 2.0F), glm::vec3(0.0F), glm::vec3(0.0F, 0.0F, 1.0F));
  ubo.proj = glm::perspective(
      glm::radians(45.0F),
      static_cast<float>(swapchainExtent.width) / static_cast<float>(swapchainExtent.height), 0.1F,
      1000.0F);
  ubo.proj[1][1] *= -1;

  void *data = ctx->GetDevice().mapMemory(uniformBuffersMemory[currentImage], 0, sizeof(ubo));
  memcpy(data, &ubo, sizeof(ubo));
  ctx->GetDevice().unmapMemory(uniformBuffersMemory[currentImage]);
}

void Application::Cleanup() { SDL_Quit(); }

void Application::RecreateSwapchain() {
  auto ctx = Context::Get();

  vkDeviceWaitIdle(ctx->GetDevice());

  CleanupSwapchain();

  CreateSwapchain();
  CreateImageViews();

  RenderPassInfo info{};
  info.colorAttachmentFormat = swapchainFormat;
  renderPass = RenderPass(info);

  CreateGraphicsPipeline();
  CreateFramebuffers();
  CreateUniformBuffers();
  CreateDescriptorPool();
  CreateCommandBuffers();
}

void Application::CleanupSwapchain() {
  auto ctx = Context::Get();

  for (auto &swapchainFramebuffer : swapchainFramebuffers)
    ctx->GetDevice().destroyFramebuffer(swapchainFramebuffer);

  ctx->GetDevice().freeCommandBuffers(commandPool, commandBuffers);

  // ctx->GetDevice().destroyPipeline();
  // ctx->GetDevice().destroyPipelineLayout(pipelineLayout);
  for (auto &swapchainImageView : swapchainImageViews)
    ctx->GetDevice().destroyImageView(swapchainImageView);

  ctx->GetDevice().destroySwapchainKHR(swapchain);
}

void Application::CreateSwapchain() {
  auto ctx = Context::Get();

  // Create swapchain
  SwapchainSupportDetails details = ctx->QuerySwapchainSupportDetails(ctx->GetPhysicalDevice());
  auto format = ChooseSwapchainFormat(details.formats);
  auto presentMode = ChooseSwapchainPresentMode(details.presentModes);
  auto extent = ChooseSwapExtent(details.capabilites);

  swapchainFormat = format.format;
  swapchainExtent = extent;

  uint32_t imageCount = details.capabilites.minImageCount + 1;
  if (details.capabilites.maxImageCount > 0 && imageCount > details.capabilites.maxImageCount)
    imageCount = details.capabilites.maxImageCount;

  QueueFamilyIndices indices = ctx->FindQueueFamilies(ctx->GetPhysicalDevice());
  std::vector<uint32_t> queueFamilyIndices = {indices.graphicsFamily.value(),
                                              indices.presentFamily.value()};

  vk::SharingMode sharingMode = vk::SharingMode::eConcurrent;
  if (indices.graphicsFamily == indices.presentFamily) {
    sharingMode = vk::SharingMode::eExclusive;
    queueFamilyIndices.clear();
  }

  vk::SwapchainCreateInfoKHR createInfo(
      {}, ctx->GetSurface(), imageCount, format.format, format.colorSpace, extent, 1,
      vk::ImageUsageFlagBits::eColorAttachment, sharingMode, queueFamilyIndices,
      details.capabilites.currentTransform, vk::CompositeAlphaFlagBitsKHR::eOpaque, presentMode,
      VK_TRUE);

  swapchain = ctx->GetDevice().createSwapchainKHR(createInfo);
  swapchainImages = ctx->GetDevice().getSwapchainImagesKHR(swapchain);
}

void Application::CreateImageViews() {
  auto ctx = Context::Get();

  // Swapchain Image Views
  swapchainImageViews.resize(swapchainImages.size());
  for (size_t i = 0; i < swapchainImages.size(); i++) {
    vk::ImageViewCreateInfo createInfo(
        {}, swapchainImages[i], vk::ImageViewType::e2D, swapchainFormat,
        {vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity,
         vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity},
        {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1});

    swapchainImageViews[i] = ctx->GetDevice().createImageView(createInfo);
  }
}

void Application::CreateDescriptorSetLayout() {
  auto ctx = Context::Get();

  vk::DescriptorSetLayoutBinding uboLayoutBinding(0, vk::DescriptorType::eUniformBuffer, 1,
                                                  vk::ShaderStageFlagBits::eVertex);

  vk::DescriptorSetLayoutCreateInfo layoutInfo(vk::DescriptorSetLayoutCreateFlags(),
                                               uboLayoutBinding);
  descriptorSetLayout = ctx->GetDevice().createDescriptorSetLayout(layoutInfo);
}

void Application::CreateDescriptorSets() {
  auto ctx = Context::Get();

  std::vector<vk::DescriptorSetLayout> layouts(swapchainImages.size(), descriptorSetLayout);
  vk::DescriptorSetAllocateInfo allocInfo(descriptorPool, layouts);
  descriptorSets = ctx->GetDevice().allocateDescriptorSets(allocInfo);

  for (size_t i = 0; i < swapchainImages.size(); i++) {
    vk::DescriptorBufferInfo bufInfo(uniformBuffers[i], 0, sizeof(UniformBufferObject));

    vk::WriteDescriptorSet descriptorWrite(
        descriptorSets[i], 0, 0, vk::DescriptorType::eUniformBuffer, nullptr, bufInfo, nullptr);

    ctx->GetDevice().updateDescriptorSets(descriptorWrite, {});
  }
}

void Application::CreateGraphicsPipeline() {
  auto ctx = Context::Get();

  Shader shader(ctx->GetDevice(), {{vk::ShaderStageFlagBits::eVertex, "assets/vertex.glsl"},
                                   {vk::ShaderStageFlagBits::eFragment, "assets/fragment.glsl"}});

  PipelineInfo info;
  info.renderPass = &renderPass;
  info.shader = shader;
  info.vertexBindingDescription = Vertex::GetBindingDescription();
  info.vertexAttributeDescriptions = Vertex::GetAttributeDescriptions();
  info.extent = swapchainExtent;
  pipeline = Pipeline(info);
}

void Application::CreateFramebuffers() {
  auto ctx = Context::Get();

  // Create Framebuffers
  swapchainFramebuffers.resize(swapchainImageViews.size());
  for (size_t i = 0; i < swapchainImageViews.size(); i++) {
    std::array<vk::ImageView, 1> attachments = {swapchainImageViews[i]};
    vk::FramebufferCreateInfo fbInfo(vk::FramebufferCreateFlags(), renderPass.GetHandle(),
                                     attachments, swapchainExtent.width, swapchainExtent.height, 1);
    swapchainFramebuffers[i] = ctx->GetDevice().createFramebuffer(fbInfo);
  }
}

void Application::CreateUniformBuffers() {
  vk::DeviceSize bufSize = sizeof(UniformBufferObject);

  uniformBuffers.resize(swapchainImages.size());
  uniformBuffersMemory.resize(swapchainImages.size());

  for (size_t i = 0; i < swapchainImages.size(); i++) {
    std::tie(uniformBuffers[i], uniformBuffersMemory[i]) = CreateBuffer(
        bufSize, vk::BufferUsageFlagBits::eUniformBuffer,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
  }
}

void Application::CreateDescriptorPool() {
  auto ctx = Context::Get();

  vk::DescriptorPoolSize poolSize(vk::DescriptorType::eUniformBuffer,
                                  static_cast<uint32_t>(swapchainImages.size()));

  vk::DescriptorPoolCreateInfo poolInfo(vk::DescriptorPoolCreateFlags(),
                                        static_cast<uint32_t>(swapchainImages.size()), poolSize);
  descriptorPool = ctx->GetDevice().createDescriptorPool(poolInfo);
}

void Application::CreateCommandBuffers() {
  auto ctx = Context::Get();

  // Create Command Buffers
  vk::CommandBufferAllocateInfo allocInfo(commandPool, vk::CommandBufferLevel::ePrimary,
                                          swapchainFramebuffers.size());
  commandBuffers = ctx->GetDevice().allocateCommandBuffers(allocInfo);

  // Begin Recording command buffers
  for (size_t i = 0; i < commandBuffers.size(); i++) {
    vk::CommandBufferBeginInfo beginInfo;

    vk::ClearValue clearValue(vk::ClearColorValue(std::array<float, 4>{0.2f, 0.2f, 0.2f, 1.0F}));
    vk::RenderPassBeginInfo renderPassInfo(renderPass.GetHandle(), swapchainFramebuffers[i],
                                           vk::Rect2D({0, 0}, swapchainExtent), clearValue);

    commandBuffers[i].begin(beginInfo);
    commandBuffers[i].beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
    commandBuffers[i].bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline.GetHandle());

    commandBuffers[i].bindVertexBuffers(0, vertexBuffer, {0});
    commandBuffers[i].bindIndexBuffer(indexBuffer, 0, vk::IndexType::eUint16);

    // commandBuffers[i].bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipeline.GetLayout(),
    // 0,
    //                                      descriptorSets[i], {});
    commandBuffers[i].drawIndexed(static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
    commandBuffers[i].endRenderPass();
    commandBuffers[i].end();
  }
}

auto Application::ChooseSwapchainFormat(const std::vector<vk::SurfaceFormatKHR> &availableFormats)
    -> vk::SurfaceFormatKHR {
  for (const auto &format : availableFormats) {
    if (format.format == vk::Format::eB8G8R8Srgb &&
        format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
      return format;
  }

  return availableFormats[0];
}

auto Application::ChooseSwapchainPresentMode(
    const std::vector<vk::PresentModeKHR> &availablePresentModes) -> vk::PresentModeKHR {
  for (const auto &mode : availablePresentModes) {
    if (mode == vk::PresentModeKHR::eMailbox) return mode;
  }

  return vk::PresentModeKHR::eFifo;
}

auto Application::ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR &capabilites) -> vk::Extent2D {
  if (capabilites.currentExtent.width != UINT32_MAX) return capabilites.currentExtent;

  int width = 0;
  int height = 0;
  SDL_GetWindowSize(window, &width, &height);

  vk::Extent2D actualExtent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

  actualExtent.width = std::clamp(actualExtent.width, capabilites.minImageExtent.width,
                                  capabilites.maxImageExtent.width);
  actualExtent.height = std::clamp(actualExtent.height, capabilites.minImageExtent.height,
                                   capabilites.maxImageExtent.height);

  return actualExtent;
}

auto Application::FindMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties)
    -> uint32_t {
  auto ctx = Context::Get();
  auto memProperties = ctx->GetPhysicalDevice().getMemoryProperties();

  for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
    if (((typeFilter & (1U << i)) != 0U) &&
        (memProperties.memoryTypes[i].propertyFlags & properties))
      return i;
  }

  throw std::runtime_error("Failed to find suitable memory type");
}

auto Application::CreateBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage,
                               vk::MemoryPropertyFlags properties)
    -> std::pair<vk::Buffer, vk::DeviceMemory> {
  auto ctx = Context::Get();

  vk::BufferCreateInfo bufferInfo(vk::BufferCreateFlags(), size, usage,
                                  vk::SharingMode::eExclusive);
  auto buf = ctx->GetDevice().createBuffer(bufferInfo);

  vk::MemoryRequirements memReqs = ctx->GetDevice().getBufferMemoryRequirements(buf);
  vk::MemoryAllocateInfo allocInfo(memReqs.size,
                                   FindMemoryType(memReqs.memoryTypeBits, properties));
  auto mem = ctx->GetDevice().allocateMemory(allocInfo);

  ctx->GetDevice().bindBufferMemory(buf, mem, 0);

  return {buf, mem};
}

void Application::CopyBuffer(vk::Buffer src, vk::Buffer dst, vk::DeviceSize size) {
  auto ctx = Context::Get();

  vk::CommandBufferAllocateInfo allocInfo(commandPool, vk::CommandBufferLevel::ePrimary, 1);

  std::vector<vk::CommandBuffer> buf = ctx->GetDevice().allocateCommandBuffers(allocInfo);

  vk::CommandBufferBeginInfo beginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
  buf[0].begin(beginInfo);

  vk::BufferCopy copyRegion(0, 0, size);
  buf[0].copyBuffer(src, dst, copyRegion);

  buf[0].end();

  vk::SubmitInfo submitInfo(0, {}, {}, 1, buf.data(), 0, {});
  ctx->GetGraphicsQueue().submit(submitInfo);
  ctx->GetGraphicsQueue().waitIdle();
}
