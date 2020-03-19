
#include <iostream>
#include <cassert>
#include <fstream>
#include <clocale>

#include <SDL.h>
#include <GL/glew.h>
#include <SDL_opengl.h>
#include <glm/ext.hpp>

#include "Window/Window.hpp"
#include "Camera/Camera.hpp"
#include "Mesh/Mesh.hpp"
#include "Shader/Shader.hpp"
#include "Texture/Texture.hpp"
#include "TexturedMesh/TexturedMesh.hpp"
#include "SDL-Helper-Libraries/KeyTracker/KeyTracker.hpp"

#include "ECS/ECS.hpp"

#include "Common.hpp"

std::string get_file_contents(const char *filename)
{
	std::ifstream in(filename, std::ios::in | std::ios::binary);
	if (in)
	{
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return (contents);
	}
	throw(errno);
}

void GLAPIENTRY
MessageCallback(GLenum source,
				GLenum type,
				GLuint id,
				GLenum severity,
				GLsizei length,
				const GLchar *message,
				const void *userParam)
{
	fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
			(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
			type, severity, message);
}

int main(int argc, char **argv)
{
	Window window;

	SDL_Init(SDL_INIT_VIDEO);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetSwapInterval(1);

	window.Load("Testy", 800, 600);
	window.Bind();

	glewInit();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, 0);

	World GameWorld;

	GameWorld.ShaderProgram.AddShaderFile("res/shader.vert", GL_VERTEX_SHADER);
	GameWorld.ShaderProgram.AddShaderFile("res/shader.frag", GL_FRAGMENT_SHADER);

	Meshes::TexturedMeshes.push_back(TexturedMesh("res/cube.obj"));

	GameWorld.View.CreateProjectionX(glm::radians(90.0), 4 / 3, 0.01, 1000);
	GameWorld.View.LookIn({0.5, 0.5, 0.5});
	GameWorld.View.MoveTo({-1, -1, -1});

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	bool Quit = false;
	SDL_Event Event;

	KeyTracker Keyboard;

	auto LastTime = std::chrono::high_resolution_clock::now();

	SDL_SetRelativeMouseMode(SDL_TRUE);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	size_t Cube = GameWorld.NewEntity();

	ActivateComponent<World::Mesh>(Cube, GameWorld, Meshes::MeshType::Textured, 0);

	while (!Quit)
	{
		auto Now = std::chrono::high_resolution_clock::now();
		DSeconds dt = std::chrono::duration_cast<DSeconds>(Now - LastTime);

		Keyboard.Update(1);
		while (SDL_PollEvent(&Event))
		{
			Keyboard.UpdateKey(&Event);
			switch (Event.type)
			{

			case SDL_QUIT:
				Quit = true;
				break;

			case SDL_MOUSEMOTION:
				constexpr double Sensitivity = 0.01;
				glm::dvec3 CurrentDirection = GameWorld.View.GetViewVector();
				glm::dmat4 Rotate = glm::rotate(glm::rotate(glm::dmat4x4(1), Event.motion.xrel * Sensitivity, glm::dvec3{0, 1, 0}), Event.motion.yrel * Sensitivity, glm::cross(CurrentDirection, glm::dvec3{0, 1, 0}));
				GameWorld.View.LookIn(glm::dvec4{CurrentDirection, 0} * Rotate);
			}
		}

		if (Keyboard[SDL_SCANCODE_ESCAPE].Clicked)
		{
			Quit = true;
		}

		if (Keyboard[SDL_SCANCODE_W].Active)
		{
			GameWorld.View.Move((glm::normalize(GameWorld.View.GetViewVector())) * (1.0 * dt.count()));
		}
		if (Keyboard[SDL_SCANCODE_S].Active)
		{
			GameWorld.View.Move((glm::normalize(GameWorld.View.GetViewVector())) * (1.0 * -dt.count()));
		}

		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		///*

		for(size_t i=0; i < GameWorld.Systems.size(); i++) {
			GameWorld.Systems[i](GameWorld, dt);
		}

		/*
		Cube.Bind(1);
		glDrawElements(GL_TRIANGLES, Cube.GetIndexCount(1), GL_UNSIGNED_INT, nullptr); //*/

		SDL_GL_SwapWindow(window);

		//std::cout << "CameraPosition {x, y, z}: {" << Yee.GetPosition().x << ", " << Yee.GetPosition().y << ", " << Yee.GetPosition().z << "}\n";

		LastTime = Now;
	}
	return 0;
}
