project('vk-bootstrap', 'cpp', version: '0.6')

incs = include_directories('src')
vulkan = dependency('vulkan')

deps = [vulkan]

vk_bootstrap_lib = library('vk-bootstrap', 'src/VkBootstrap.cpp', dependencies: deps, include_directories: incs)
vk_bootstrap_dep = declare_dependency(dependencies: deps, include_directories: incs, link_with: vk_bootstrap_lib)
