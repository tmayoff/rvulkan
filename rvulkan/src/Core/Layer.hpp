#ifndef LAYER_HPP
#define LAYER_HPP

class Layer {
 public:
  virtual ~Layer() = default;

  virtual void OnAttach();
  virtual void OnUpdate();
};

#endif  // LAYER_HPP
