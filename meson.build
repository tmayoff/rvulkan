project('rvulkan', 'cpp',
  version : '0.1',
  default_options : ['warning_level=3',
                     'cpp_std=c++20'])

add_project_arguments('-DENABLE_ASSERTS', language: 'cpp')

glm_proj = subproject('glm')
glm = glm_proj.get_variable('glm_dep')


cmake = import('cmake')
vma_proj = cmake.subproject('vma')
vma = vma_proj.dependency('VulkanMemoryAllocator')

vulkan = dependency('vulkan')
sdl2   = dependency('SDL2')

sources = []

subdir('src')

exe = executable('rvulkan', sources,
                dependencies: [vulkan, sdl2, glm, vma],
                install : true)

test('basic', exe)
