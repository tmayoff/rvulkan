#ifndef LAYER_HPP
#define LAYER_HPP

class RenderContext;

#include <rvulkan/events/event.hpp>
#include <utility>

class Layer {
 public:
  explicit Layer(std::string layer_name) : layer_name(std::move(layer_name)) {}

  virtual ~Layer() = default;

  virtual void OnAttach() {}
  virtual void OnUpdate(const RenderContext& /*unused*/) {}
  virtual void OnImGuiUpdate() {}
  virtual void OnEvent(Event& /*unused*/) {}

 private:
  std::string layer_name;
};

#endif  // LAYER_HPP
