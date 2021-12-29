#include "Renderer.hpp"

#include "Application.hpp"

Renderer::Renderer(const vk::Device &device) : device(device) {
  createRenderPass();
  createGraphicsPipeline();
}

void Renderer::createGraphicsPipeline() {}

void Renderer::createRenderPass() {}
