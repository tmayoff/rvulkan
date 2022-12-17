#pragma once
#ifndef BUFFER_HPP_
#define BUFFER_HPP_

#include <vk_mem_alloc.h>

#include <vulkan/vulkan.hpp>

class Buffer {
 public:
  Buffer() = default;
  Buffer(size_t byteSize, VmaMemoryUsage usage, vk::BufferUsageFlags bufferUsage);

  const vk::Buffer& GetHandle() const { return buffer; }

  void SetData(void* data, uint32_t size);

 private:
  vk::Buffer buffer;
  VmaAllocation allocation = {};
  uint8_t* memory = nullptr;
};

#endif  // BUFFER_HPP_
