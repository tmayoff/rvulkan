sources = []
subdir('src')

inc = include_directories('src')
deps = [sdl2, spdlog, vulkan, vma, glm, vk_bootstrap]

rvulkan_lib = library('rvulkan', sources, dependencies: deps, include_directories: inc)
rvulkan_dep = declare_dependency(dependencies: deps, include_directories: inc, link_with: rvulkan_lib)
