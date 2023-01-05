#ifndef LAYER_HPP
#define LAYER_HPP

#include "events/event.hpp"
#include "renderer/render_context.hpp"

class Layer {
 public:
  virtual ~Layer() = default;

  virtual void OnAttach() = 0;
  virtual void OnUpdate(const RenderContext& render_context) = 0;
  virtual void OnEvent(Event&) = 0;
};

#endif  // LAYER_HPP
