#ifndef IMGUI_UTILS_HPP
#define IMGUI_UTILS_HPP

#include <imgui.h>

#include <string>

inline bool LabeledInputText(const std::string& label, std::string& input) {
  ImGui::Text("%s", label.c_str());
  ImGui::SameLine();
  return ImGui::InputText((std::string("##") + label).c_str(), input.data(), 256);
}

#endif  // IMGUI_UTILS_HPP
