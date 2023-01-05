project('rvulkan', 'cpp',
  version : '0.1',
  default_options : ['warning_level=3',
                     'cpp_std=c++20'])

add_project_arguments('-DENABLE_ASSERTS', language: 'cpp')

glm_proj = subproject('glm')
glm = glm_proj.get_variable('glm_dep')

tracy        = dependency('tracy')
entt         = dependency('entt')
vulkan       = dependency('vulkan')
sdl2         = dependency('SDL2')
spdlog       = dependency('spdlog')
vma          = dependency('VulkanMemoryAllocator')

sources = []

subdir('rvulkan')
subdir('editor')
subdir('sandbox')
