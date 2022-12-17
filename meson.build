project('rvulkan', 'cpp',
  version : '0.1',
  default_options : ['warning_level=3',
                     'cpp_std=c++20'])

add_project_arguments('-DENABLE_ASSERS', language: 'cpp')

glm_proj = subproject('glm')
glm = glm_proj.get_variable('glm_dep')

vulkan = dependency('vulkan')
sdl2   = dependency('SDL2')
vma    = dependency('VulkanMemoryAllocator')

sources = []

subdir('rvulkan')
subdir('sandbox')
