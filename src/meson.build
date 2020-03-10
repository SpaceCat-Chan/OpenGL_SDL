

sources = [
    'main.cpp',
    'Window/Window.cpp',
    'Camera/Camera.cpp',
    'Mesh/Mesh.cpp',
    'Shader/Shader.cpp',
    'Texture/Texture.cpp',
    '../SDL-Helper-Libraries/KeyTracker/KeyTracker.cpp',
    '../SDL-Helper-Libraries/sfstream/sfstream.cpp'
]


depends = [
    dependency('SDL2'),
    dependency('GL'),
    dependency('GLEW'),
    dependency('glm')
]

includes = [
    include_directories('../'),
    include_directories('./')
]

executable('OpenGL_SDL', sources, dependencies: depends, install: true, include_directories: includes)
install_subdir('../res', install_dir: 'bin/')