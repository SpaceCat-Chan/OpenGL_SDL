
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
	SDL_SetRelativeMouseMode(SDL_FALSE);
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

	std::cout << "res/cube.obj\n";
	Meshes::TexturedMeshes.push_back(TexturedMesh("res/cube.obj"));
	std::cout << "\n\nres/BrickWall.obj\n";
	Meshes::TexturedMeshes.push_back(TexturedMesh("res/BrickWall.obj"));
	std::cout << "\n\nres/3DBrickWall.obj\n";
	Meshes::TexturedMeshes.push_back(TexturedMesh("res/3DBrickWall.obj"));
	{
		std::cout << "\n\nres/teapot.obj\n";
		Mesh Utah;
		std::vector<std::string> a;
		Utah.LoadMesh("res/teapot.obj", a, a, a, a);
		Meshes::StaticMeshes.push_back(std::move(Utah));
	}
	{
		std::cout << "\n\nres/junc.obj\n";
		Mesh Utah;
		std::vector<std::string> a;
		Utah.LoadMesh("res/junc.obj", a, a, a, a);
		Meshes::StaticMeshes.push_back(std::move(Utah));
	}
	{
		std::cout << "\n\nres/LightCube.obj\n";
		Mesh Utah;
		std::vector<std::string> a;
		Utah.LoadMesh("res/LightCube.obj", a, a, a, a);
		Meshes::StaticMeshes.push_back(std::move(Utah));
	}


	enum {
		CubeMesh,
		BrickWallMesh,
		BrickWall3DMesh
	};

	GameWorld.View.CreateProjectionX(glm::radians(90.0), 4 / 3, 0.01, 1000);
	GameWorld.View.LookIn();
	GameWorld.View.MoveTo({-1, -1, -1});

	
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	KeyTracker Keyboard;

	auto LastTime = std::chrono::high_resolution_clock::now();

	SDL_SetRelativeMouseMode(SDL_TRUE);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	{
		size_t CubeOne = GameWorld.NewEntity();
		size_t CubeTwo = GameWorld.NewEntity();
		size_t BrickFloor = GameWorld.NewEntity();
		size_t BrickWall = GameWorld.NewEntity();

		ActivateComponent<World::Mesh>(CubeOne, GameWorld, Meshes::MeshType::Textured, CubeMesh);
		ActivateComponent<World::Mesh>(CubeTwo, GameWorld, Meshes::MeshType::Textured, CubeMesh);
		ActivateComponent<World::Mesh>(BrickFloor, GameWorld, Meshes::MeshType::Textured, BrickWallMesh);
		ActivateComponent<World::Mesh>(BrickWall, GameWorld, Meshes::MeshType::Textured, BrickWall3DMesh);

		ActivateComponent<World::Position>(CubeTwo, GameWorld, 0, 2, 0);
		ActivateComponent<World::Transform>(CubeTwo, GameWorld);
		GameWorld.TransformComponents[CubeTwo]->Tranformations.push_back({Transform::Type::AutoPosition, glm::dmat4x4(1)});

		size_t CubeGroupParent = GameWorld.NewEntity();
		ActivateComponent<World::Children>(CubeGroupParent, GameWorld);
		GameWorld.ChildrenComponents[CubeGroupParent]->Children.push_back(CubeOne);
		GameWorld.ChildrenComponents[CubeGroupParent]->Children.push_back(CubeTwo);
		GameWorld.ChildrenComponents[CubeGroupParent]->Children.push_back(BrickFloor);
		GameWorld.ChildrenComponents[CubeGroupParent]->Children.push_back(BrickWall);
		GameWorld.ChildrenComponents[CubeGroupParent]->EnforceCorrectness(GameWorld, CubeGroupParent);

		CubeOne = GameWorld.CloneEntity(CubeOne);
		CubeTwo = GameWorld.CloneEntity(CubeTwo);
		BrickFloor = GameWorld.CloneEntity(BrickFloor);
		BrickWall = GameWorld.CloneEntity(BrickWall);

		CubeGroupParent = GameWorld.NewEntity();
		ActivateComponent<World::Position>(CubeGroupParent, GameWorld, 1, 0, 0);
		ActivateComponent<World::Transform>(CubeGroupParent, GameWorld);
		ActivateComponent<World::Children>(CubeGroupParent, GameWorld);
		GameWorld.TransformComponents[CubeGroupParent]->Tranformations.push_back({Transform::Type::AutoPosition, glm::dmat4x4(1)});

		GameWorld.ChildrenComponents[CubeGroupParent]->Children.push_back(CubeOne);
		GameWorld.ChildrenComponents[CubeGroupParent]->Children.push_back(CubeTwo);
		GameWorld.ChildrenComponents[CubeGroupParent]->Children.push_back(BrickFloor);
		GameWorld.ChildrenComponents[CubeGroupParent]->Children.push_back(BrickWall);
		GameWorld.ChildrenComponents[CubeGroupParent]->EnforceCorrectness(GameWorld, CubeGroupParent);


		CubeOne = GameWorld.CloneEntity(CubeOne);
		CubeTwo = GameWorld.CloneEntity(CubeTwo);
		BrickFloor = GameWorld.CloneEntity(BrickFloor);
		BrickWall = GameWorld.CloneEntity(BrickWall);

		size_t CubeGroup2Parent = GameWorld.NewEntity();
		ActivateComponent<World::Position>(CubeGroup2Parent, GameWorld, 0, 1, 0);
		ActivateComponent<World::Transform>(CubeGroup2Parent, GameWorld);
		ActivateComponent<World::Children>(CubeGroup2Parent, GameWorld);
		GameWorld.TransformComponents[CubeGroup2Parent]->Tranformations.push_back({Transform::Type::AutoPosition, glm::dmat4x4(1)});
		GameWorld.TransformComponents[CubeGroup2Parent]->Tranformations.push_back({Transform::Type::Rotate, glm::rotate(glm::dmat4x4(1), glm::radians(90.0), glm::dvec3{0, 0, -1})});

		GameWorld.ChildrenComponents[CubeGroup2Parent]->Children.push_back(CubeOne);
		GameWorld.ChildrenComponents[CubeGroup2Parent]->Children.push_back(CubeTwo);
		GameWorld.ChildrenComponents[CubeGroup2Parent]->Children.push_back(BrickFloor);
		GameWorld.ChildrenComponents[CubeGroup2Parent]->Children.push_back(BrickWall);
		GameWorld.ChildrenComponents[CubeGroup2Parent]->EnforceCorrectness(GameWorld, CubeGroup2Parent);

	
		GameWorld.ChildrenComponents[CubeGroupParent]->Children.push_back(CubeGroup2Parent);
		GameWorld.ChildrenComponents[CubeGroupParent]->EnforceCorrectness(GameWorld, CubeGroupParent);
	}

	size_t Utah = GameWorld.NewEntity();
	ActivateComponent<World::Position>(Utah, GameWorld, -3, 0, 0);
	ActivateComponent<World::Mesh>(Utah, GameWorld, Meshes::MeshType::Static, 0);
	ActivateComponent<World::Transform>(Utah, GameWorld);
	GameWorld.TransformComponents[Utah]->Tranformations.push_back({Transform::Type::AutoPosition, glm::dmat4x4(1)});

	Utah = GameWorld.NewEntity();
	ActivateComponent<World::Position>(Utah, GameWorld, 0, 0, -3);
	ActivateComponent<World::Mesh>(Utah, GameWorld, Meshes::MeshType::Static, 1);
	ActivateComponent<World::Transform>(Utah, GameWorld);
	GameWorld.TransformComponents[Utah]->Tranformations.push_back({Transform::Type::AutoPosition, glm::dmat4x4(1)});

	size_t Light = GameWorld.NewEntity();
	ActivateComponent<World::Position>(Light, GameWorld, 0, 0.3, -1);
	ActivateComponent<World::Light>(Light, GameWorld);
	ActivateComponent<World::Mesh>(Light, GameWorld, Meshes::MeshType::Static, 2, false);
	ActivateComponent<World::Transform>(Light, GameWorld);
	GameWorld.TransformComponents[Light]->Tranformations.push_back({Transform::Type::AutoPosition, glm::dmat4x4(1)});

	Light = GameWorld.NewEntity();
	ActivateComponent<World::Position>(Light, GameWorld, 0.5, 1.5, 0.5);
	ActivateComponent<World::Light>(Light, GameWorld);
	ActivateComponent<World::Mesh>(Light, GameWorld, Meshes::MeshType::Static, 2, false);
	ActivateComponent<World::Transform>(Light, GameWorld);
	GameWorld.TransformComponents[Light]->Tranformations.push_back({Transform::Type::AutoPosition, glm::dmat4x4(1)});
	/*
	Light = GameWorld.NewEntity();
	ActivateComponent<World::Light>(Light, GameWorld);
	GameWorld.LightComponents[Light]->LightType = LightInfo::Type::Direction;
	GameWorld.LightComponents[Light]->Direction = {0.5, -0.5, 0.5};
	*/
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
