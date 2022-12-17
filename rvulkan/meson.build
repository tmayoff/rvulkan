sources = []
subdir('src')

inc = include_directories('src')
deps = [sdl2, vulkan, vma, glm]

rvulkan_lib = library('rvulkan', sources, dependencies: deps, include_directories: inc)
rvulkan_dep = declare_dependency(dependencies: deps, include_directories: inc, link_with: rvulkan_lib)
