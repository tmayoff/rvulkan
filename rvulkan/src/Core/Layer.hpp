#ifndef LAYER_HPP
#define LAYER_HPP

class Layer {
 public:
  virtual ~Layer() = default;

  virtual void OnAttach() = 0;
  virtual void OnUpdate() = 0;
};

#endif  // LAYER_HPP
