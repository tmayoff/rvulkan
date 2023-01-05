#include "Pipeline.hpp"

#include <vulkan/vulkan_core.h>

#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_structs.hpp>

#include "VulkanContext.hpp"
#include "core/log.hpp"
#include "renderer/Mesh.hpp"

static vk::Format ShaderDataTypeToVkFormat(ShaderDataType type) {
  switch (type) {
    case ShaderDataType::Float: return vk::Format::eR32Sfloat;
    case ShaderDataType::Float2: return vk::Format::eR32G32Sfloat;
    case ShaderDataType::Float3: return vk::Format::eR32G32B32Sfloat;
    case ShaderDataType::Float4: return vk::Format::eR32G32B32A32Sfloat;
    case ShaderDataType::Int: return vk::Format::eR32Sint;
    case ShaderDataType::Int2: return vk::Format::eR32G32Sint;
    case ShaderDataType::Int3: return vk::Format::eR32G32B32Sint;
    case ShaderDataType::Int4: return vk::Format::eR32G32B32A32Sint;
    case ShaderDataType::Bool: return vk::Format::eR32Sint;
    default: return vk::Format::eUndefined;
  }
}

Pipeline::Pipeline(const std::shared_ptr<VulkanContext>& context, const PipelineOptions& options,
                   const vk::RenderPass& renderPass)
    : context(context) {
  // CreateDescriptorSets(context, options);

  const std::array shader_stage_info = {
      vk::PipelineShaderStageCreateInfo({}, vk::ShaderStageFlagBits::eVertex,
                                        options.shader.GetVertexModule(), "main"),
      vk::PipelineShaderStageCreateInfo({}, vk::ShaderStageFlagBits::eFragment,
                                        options.shader.GetFragmentModule(), "main")};

  std::vector<vk::VertexInputAttributeDescription> vertex_attribute_descriptions;
  const std::vector<vk::VertexInputBindingDescription> vertex_binding_descriptions = {
      vk::VertexInputBindingDescription(0, options.bufferLayout.GetStride(),
                                        vk::VertexInputRate::eVertex)};

  int location = 0;
  for (const auto& input_attrib : options.bufferLayout) {
    vk::VertexInputAttributeDescription input_description(
        location, 0, ShaderDataTypeToVkFormat(input_attrib.GetType()), input_attrib.GetOffset());
    vertex_attribute_descriptions.push_back(input_description);
    location++;
  }

  vk::PipelineVertexInputStateCreateInfo vertex_input_info({}, vertex_binding_descriptions,
                                                           vertex_attribute_descriptions);

  vk::PipelineInputAssemblyStateCreateInfo input_assembly_info(
      {}, vk::PrimitiveTopology::eTriangleList);

  vk::Viewport viewport(0, 0, static_cast<float>(options.surface_extent.width),
                        static_cast<float>(options.surface_extent.height), 0.0F, 1.0F);

  vk::Rect2D scissors({0, 0}, {options.surface_extent.width, options.surface_extent.height});

  vk::PipelineViewportStateCreateInfo viewport_state({}, viewport, scissors);

  vk::PipelineRasterizationStateCreateInfo rasterizer(
      {}, VK_FALSE, VK_FALSE, vk::PolygonMode::eFill, vk::CullModeFlagBits::eBack,
      vk::FrontFace::eClockwise, VK_FALSE, 0.0F, 0.0F, 0.0F, 1.0F);

  vk::PipelineMultisampleStateCreateInfo multisample({}, vk::SampleCountFlagBits::e1);

  vk::ColorComponentFlags colour_component_flags(
      vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
      vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);
  vk::PipelineColorBlendAttachmentState colour_attachment(
      VK_FALSE, vk::BlendFactor::eZero, vk::BlendFactor::eZero, vk::BlendOp::eAdd,
      vk::BlendFactor::eZero, vk::BlendFactor::eZero, vk::BlendOp::eAdd, colour_component_flags);

  vk::PipelineColorBlendStateCreateInfo colour_blend_state(
      {}, VK_FALSE, vk::LogicOp::eNoOp, colour_attachment, {1.0F, 1.0F, 1.0F, 1.0F});

  vk::PushConstantRange push_constant({}, 0, sizeof(PushConstants));
  push_constant.setStageFlags(vk::ShaderStageFlagBits::eVertex);

  vk::PipelineLayoutCreateInfo pipeline_layout_create(vk::PipelineLayoutCreateFlags{});
  pipeline_layout_create.setPushConstantRanges(push_constant);

  layout = context->GetLogicalDevice().GetHandle().createPipelineLayout(pipeline_layout_create);

  std::array dynamic_states = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};
  vk::PipelineDynamicStateCreateInfo dynamic_state({}, dynamic_states);

  vk::GraphicsPipelineCreateInfo pipeline_create_info(
      {}, shader_stage_info, &vertex_input_info, &input_assembly_info, nullptr, &viewport_state,
      &rasterizer, &multisample, nullptr, &colour_blend_state, &dynamic_state, layout, renderPass);

  auto [result, value] =
      context->GetLogicalDevice().GetHandle().createGraphicsPipeline({}, pipeline_create_info);
  if (result != vk::Result::eSuccess) {
    logger::fatal("Failed to create vuklan pipeline");
  }

  pipeline = value;
}

Pipeline::~Pipeline() {
  context->GetLogicalDevice().GetHandle().destroyPipelineLayout(layout);
  context->GetLogicalDevice().GetHandle().destroyPipeline(pipeline);
}

void Pipeline::CreateDescriptorSets(const std::shared_ptr<VulkanContext>& context,
                                    const PipelineOptions& options) {
  // std::vector<vk::DescriptorSetLayoutBinding> descriptor_bindings;
  // for (size_t i = 0; i < options.uniform_buffer_layouts.size(); i++) {
  //   const BufferLayout b = options.uniform_buffer_layouts[i];

  //   vk::DescriptorSetLayoutBinding binding(i, vk::DescriptorType::eUniformBuffer,
  //                                          b.GetElements().size(),
  //                                          vk::ShaderStageFlagBits::eVertex);
  //   descriptor_bindings.push_back(binding);
  // }

  // vk::DescriptorSetLayoutCreateInfo layout_create_info(vk::DescriptorSetLayoutCreateFlags(),
  //                                                      descriptor_bindings);

  // descriptorset_layout =
  //     context->GetLogicalDevice().GetHandle().createDescriptorSetLayout(layout_create_info);

  // vk::DescriptorPoolSize pool_size(vk::DescriptorType::eUniformBuffer, 1);
  // vk::DescriptorPoolCreateInfo pool_create(vk::DescriptorPoolCreateFlags(), 1, 1, &pool_size);
  // descriptor_pool = context->GetLogicalDevice().GetHandle().createDescriptorPool(pool_create);

  // vk::DescriptorSetAllocateInfo alloc_info(descriptor_pool, descriptorset_layout);
  // descriptor_sets = context->GetLogicalDevice().GetHandle().allocateDescriptorSets(alloc_info);
}
