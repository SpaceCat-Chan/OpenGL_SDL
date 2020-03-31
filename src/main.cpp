
#include <iostream>
#include <cassert>
#include <fstream>
#include <clocale>

#include <SDL.h>
#include <GL/glew.h>
#include <SDL_opengl.h>
#include <glm/ext.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

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
	GameWorld.ShaderProgram.Link();

	Meshes::TexturedMeshes.push_back(TexturedMesh("res/cube.obj"));

	GameWorld.View.CreateProjectionX(glm::radians(90.0), 4 / 3, 0.01, 1000);
	GameWorld.View.LookIn();
	GameWorld.View.MoveTo({-1, -1, -1});

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	KeyTracker Keyboard;

	auto LastTime = std::chrono::high_resolution_clock::now();

	SDL_SetRelativeMouseMode(SDL_TRUE);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	size_t Cube = GameWorld.NewEntity();
	ActivateComponent<World::Mesh>(Cube, GameWorld, Meshes::MeshType::Textured, 0);

	Cube = GameWorld.NewEntity();
	ActivateComponent<World::Mesh>(Cube, GameWorld, Meshes::MeshType::Textured, 0);
	ActivateComponent<World::Position>(Cube, GameWorld, 1, 0, 0);
	ActivateComponent<World::Transform>(Cube, GameWorld);
	ActivateComponent<World::Light>(Cube, GameWorld);
	GameWorld.TransformComponents[Cube]->Tranformations.push_back({Transform::Type::AutoPosition, glm::mat4x4(1)});
	GameWorld.LightComponents[Cube]->LightType = LightInfo::Type::Point;
	GameWorld.LightComponents[Cube]->Position = {0, 0.3, -1};

	Cube = GameWorld.NewEntity();
	ActivateComponent<World::Mesh>(Cube, GameWorld, Meshes::MeshType::Textured, 0);
	ActivateComponent<World::Position>(Cube, GameWorld, 0, 1, 0);
	ActivateComponent<World::Transform>(Cube, GameWorld);
	GameWorld.TransformComponents[Cube]->Tranformations.push_back({Transform::Type::AutoPosition, glm::mat4x4(1)});
	GameWorld.TransformComponents[Cube]->Tranformations.push_back({Transform::Type::Rotate, glm::rotate(glm::dmat4x4(1), glm::radians(90.0), glm::dvec3(0, 0, -1))});
	ActivateComponent<World::Children>(Cube, GameWorld);
	GameWorld.ChildrenComponents[Cube]->Parent = Cube - 1;
	GameWorld.ChildrenComponents[Cube]->EnforceCorrectness(GameWorld, Cube);

	size_t Light = GameWorld.NewEntity();
	ActivateComponent<World::Light>(Light, GameWorld);
	GameWorld.LightComponents[Light]->LightType = LightInfo::Type::Point;
	GameWorld.LightComponents[Light]->Position = {0, 0.3, -1};

	Light = GameWorld.NewEntity();
	ActivateComponent<World::Light>(Light, GameWorld);
	GameWorld.LightComponents[Light]->LightType = LightInfo::Type::Point;
	GameWorld.LightComponents[Light]->Position = {0.5, 1.5, 0.5};

	while (!GameWorld.Quit)
	{
		auto Now = std::chrono::high_resolution_clock::now();
		DSeconds dt = std::chrono::duration_cast<DSeconds>(Now - LastTime);

		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for (size_t i = 0; i < GameWorld.Systems.size(); i++)
		{
			GameWorld.Systems[i](GameWorld, dt);
		}

		SDL_GL_SwapWindow(window);

		//std::cout << "CameraPosition {x, y, z}: {" << GameWorld.View.GetPosition().x << ", " << GameWorld.View.GetPosition().y << ", " << GameWorld.View.GetPosition().z << "}\n";

		LastTime = Now;
	}
	return 0;
}
