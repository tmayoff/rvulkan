sources = []
subdir('src')

public_inc = include_directories('include')
deps = [entt, glm, imgui, sdl2, spdlog, tracy, vulkan, vma]

rvulkan_lib = library('rvulkan', sources, dependencies: deps, include_directories: include_directories('src', 'include'))
rvulkan_dep = declare_dependency(dependencies: deps, include_directories: public_inc, link_with: rvulkan_lib)
