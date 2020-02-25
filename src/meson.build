

sources = [
    'main.cpp',
    'Window/Window.cpp',
    'Camera/Camera.cpp',
    'Mesh/Mesh.cpp'
]


depends = [
    dependency('SDL2'),
    dependency('GL'),
    dependency('GLEW'),
    dependency('glm')
]


executable('OpenGL_SDL', sources, dependencies: depends, install: true)
install_subdir('../res', install_dir: 'bin/')