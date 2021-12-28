#include <memory>

#include "Application.hpp"

int main() {
  auto app = std::make_unique<Application>();
  app->Run();
}
