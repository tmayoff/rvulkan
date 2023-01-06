#ifndef LAYER_HPP
#define LAYER_HPP

class RenderContext;

#include <rvulkan/events/event.hpp>

class Layer {
 public:
  virtual ~Layer() = default;

  virtual void OnAttach() {}
  virtual void OnUpdate(const RenderContext& /*unused*/) {}
  virtual void OnImGuiUpdate() {}
  virtual void OnEvent(Event& /*unused*/) {}
};

#endif  // LAYER_HPP
