
#include <cassert>
#include <clocale>
#include <fstream>
#include <iostream>

#include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <glm/ext.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include "Camera/Camera.hpp"
#include "Mesh/Mesh.hpp"
#include "SDL-Helper-Libraries/KeyTracker/KeyTracker.hpp"
#include "Shader/Shader.hpp"
#include "Texture/Texture.hpp"
#include "Window/Window.hpp"
#include "minitrace/minitrace.h"

#include "ECS/ECS.hpp"

#include "Common.hpp"

void GLAPIENTRY MessageCallback(
    GLenum source, // NOLINT
    GLenum type,
    GLuint id, // NOLINT
    GLenum severity,
    GLsizei length, // NOLINT
    const GLchar *message,
    const void *userParam) // NOLINT
{
	SDL_SetRelativeMouseMode(SDL_FALSE);
	std::cerr << "GL CALLBACK: "
	          << (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "")
	          << " type = " << std::hex << type << ", severity = " << severity
	          << ", message = " << message << '\n';
}

int main(int argc, char **argv) // NOLINT
{
	mtr_init("trace.json");
	mtr_register_sigint_handler();
	Window window;

	MTR_META_PROCESS_NAME("OpenGL_SDL");
	MTR_META_THREAD_NAME("main thread");

	SDL_Init(SDL_INIT_VIDEO);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(
	    SDL_GL_CONTEXT_PROFILE_MASK,
	    SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetSwapInterval(1);

	window.Load("Testy", 800, 600);
	window.Bind();

	glewInit();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, nullptr);

	World GameWorld;

	GameWorld.ShaderProgram.AddShaderFile("res/shader.vert", GL_VERTEX_SHADER);
	GameWorld.ShaderProgram.AddShaderFile(
	    "res/shader.frag",
	    GL_FRAGMENT_SHADER);
	GameWorld.ShaderProgram.Link();

	std::cout << "res/cube.obj\n";
	Meshes::TexturedMeshes.emplace_back("res/cube.obj");
	std::cout << "\n\nres/BrickWall.obj\n";
	Meshes::TexturedMeshes.emplace_back("res/BrickWall.obj");
	std::cout << "\n\nres/3DBrickWall.obj\n";
	Meshes::TexturedMeshes.emplace_back("res/3DBrickWall.obj");
	{
		std::cout << "\n\nres/teapot.obj\n";
		Mesh Utah;
		std::vector<std::string> a;
		Utah.LoadMesh("res/teapot.obj", a, a, a, a, false);
		Meshes::StaticMeshes.push_back(std::move(Utah));
		// Meshes::SetupOctree(Meshes::MeshType::Static, 0);
	}
	{
		std::cout << "\n\nres/junc.obj\n";
		Mesh Utah;
		std::vector<std::string> a;
		Utah.LoadMesh("res/junc.obj", a, a, a, a, false);
		Meshes::StaticMeshes.push_back(std::move(Utah));
		// Meshes::SetupOctree(Meshes::MeshType::Static, 1);
	}
	{
		std::cout << "\n\nres/LightCube.obj\n";
		Mesh Utah;
		std::vector<std::string> a;
		Utah.LoadMesh("res/LightCube.obj", a, a, a, a, true);
		Meshes::StaticMeshes.push_back(std::move(Utah));
		Meshes::SetupOctree(Meshes::MeshType::Static, 2);
	}

	enum
	{
		CubeMesh,
		BrickWallMesh,
		BrickWall3DMesh
	};

	GameWorld.View.CreateProjectionX(glm::radians(90.0), 4.0 / 3.0, 0.01, 1000);
	GameWorld.View.LookIn();
	GameWorld.View.MoveTo({-1, -1, -1});

	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	KeyTracker Keyboard;

	auto LastTime = std::chrono::high_resolution_clock::now();

	// SDL_SetRelativeMouseMode(SDL_TRUE);

	{
		size_t CubeOne = GameWorld.NewEntity();
		size_t CubeTwo = GameWorld.NewEntity();
		size_t BrickFloor = GameWorld.NewEntity();
		size_t BrickWall = GameWorld.NewEntity();

		ActivateComponent<World::Mesh>(
		    CubeOne,
		    GameWorld,
		    Meshes::MeshType::Textured,
		    CubeMesh);
		ActivateComponent<World::Mesh>(
		    CubeTwo,
		    GameWorld,
		    Meshes::MeshType::Textured,
		    CubeMesh);
		ActivateComponent<World::Mesh>(
		    BrickFloor,
		    GameWorld,
		    Meshes::MeshType::Textured,
		    BrickWallMesh);
		ActivateComponent<World::Mesh>(
		    BrickWall,
		    GameWorld,
		    Meshes::MeshType::Textured,
		    BrickWall3DMesh);

		ActivateComponent<World::Position>(CubeTwo, GameWorld, 3, 3, 3);
		ActivateComponent<World::Transform>(CubeTwo, GameWorld);
		GameWorld[CubeTwo].Transform()->Tranformations.emplace_back(
		    Transform::Type::AutoPosition,
		    glm::dmat4x4(1));

		size_t CubeGroupParent = GameWorld.NewEntity();
		ActivateComponent<World::Children>(CubeGroupParent, GameWorld);
		GameWorld[CubeGroupParent].Children()->Children.push_back(CubeOne);
		GameWorld[CubeGroupParent].Children()->Children.push_back(CubeTwo);
		GameWorld[CubeGroupParent].Children()->Children.push_back(BrickFloor);
		GameWorld[CubeGroupParent].Children()->Children.push_back(BrickWall);
		GameWorld[CubeGroupParent].Children()->EnforceCorrectness(
		    GameWorld,
		    CubeGroupParent);

		CubeOne = GameWorld.CloneEntity(CubeOne);
		CubeTwo = GameWorld.CloneEntity(CubeTwo);
		BrickFloor = GameWorld.CloneEntity(BrickFloor);
		BrickWall = GameWorld.CloneEntity(BrickWall);

		CubeGroupParent = GameWorld.NewEntity();
		ActivateComponent<World::Position>(CubeGroupParent, GameWorld, 1, 0, 0);
		ActivateComponent<World::Transform>(CubeGroupParent, GameWorld);
		ActivateComponent<World::Children>(CubeGroupParent, GameWorld);
		GameWorld[CubeGroupParent].Transform()->Tranformations.emplace_back(
		    Transform::Type::AutoPosition,
		    glm::dmat4x4(1));

		GameWorld[CubeGroupParent].Children()->Children.push_back(CubeOne);
		GameWorld[CubeGroupParent].Children()->Children.push_back(CubeTwo);
		GameWorld[CubeGroupParent].Children()->Children.push_back(BrickFloor);
		GameWorld[CubeGroupParent].Children()->Children.push_back(BrickWall);
		GameWorld[CubeGroupParent].Children()->EnforceCorrectness(
		    GameWorld,
		    CubeGroupParent);

		CubeOne = GameWorld.CloneEntity(CubeOne);
		CubeTwo = GameWorld.CloneEntity(CubeTwo);
		BrickFloor = GameWorld.CloneEntity(BrickFloor);
		BrickWall = GameWorld.CloneEntity(BrickWall);

		size_t CubeGroup2Parent = GameWorld.NewEntity();
		ActivateComponent<World::Position>(
		    CubeGroup2Parent,
		    GameWorld,
		    0,
		    1,
		    0);
		ActivateComponent<World::Transform>(CubeGroup2Parent, GameWorld);
		ActivateComponent<World::Children>(CubeGroup2Parent, GameWorld);
		GameWorld[CubeGroup2Parent].Transform()->Tranformations.emplace_back(
		    Transform::Type::AutoPosition,
		    glm::dmat4x4(1));
		GameWorld[CubeGroup2Parent].Transform()->Tranformations.emplace_back(
		    Transform::Type::Rotate,
		    glm::rotate(
		        glm::dmat4x4(1),
		        glm::radians(90.0),
		        glm::dvec3{0, 0, -1}));

		GameWorld[CubeGroup2Parent].Children()->Children.push_back(CubeOne);
		GameWorld[CubeGroup2Parent].Children()->Children.push_back(CubeTwo);
		GameWorld[CubeGroup2Parent].Children()->Children.push_back(BrickFloor);
		GameWorld[CubeGroup2Parent].Children()->Children.push_back(BrickWall);
		GameWorld[CubeGroup2Parent].Children()->EnforceCorrectness(
		    GameWorld,
		    CubeGroup2Parent);

		GameWorld[CubeGroupParent].Children()->Children.push_back(
		    CubeGroup2Parent);
		GameWorld[CubeGroupParent].Children()->EnforceCorrectness(
		    GameWorld,
		    CubeGroupParent);
	}

	size_t Utah = GameWorld.NewEntity();
	ActivateComponent<World::Position>(Utah, GameWorld, -3, 0, 0);
	ActivateComponent<World::Mesh>(
	    Utah,
	    GameWorld,
	    Meshes::MeshType::Static,
	    0);
	ActivateComponent<World::Transform>(Utah, GameWorld);
	GameWorld[Utah].Transform()->Tranformations.emplace_back(
	    Transform::Type::AutoPosition,
	    glm::dmat4x4(1));

	Utah = GameWorld.NewEntity();
	ActivateComponent<World::Position>(Utah, GameWorld, 0, 0, -3);
	ActivateComponent<World::Mesh>(
	    Utah,
	    GameWorld,
	    Meshes::MeshType::Static,
	    1);
	ActivateComponent<World::Transform>(Utah, GameWorld);
	GameWorld[Utah].Transform()->Tranformations.emplace_back(
	    Transform::Type::AutoPosition,
	    glm::dmat4x4(1));

	size_t Light = GameWorld.NewEntity();
	ActivateComponent<World::Position>(Light, GameWorld, 0.5, 0.3, -1);
	ActivateComponent<World::Light>(Light, GameWorld);
	ActivateComponent<World::Mesh>(
	    Light,
	    GameWorld,
	    Meshes::MeshType::Static,
	    2,
	    false);
	ActivateComponent<World::Transform>(Light, GameWorld);
	GameWorld[Light].Transform()->Tranformations.emplace_back(
	    Transform::Type::AutoPosition,
	    glm::dmat4x4(1));

	Light = GameWorld.NewEntity();
	ActivateComponent<World::Position>(Light, GameWorld, 0.5, 1.5, 0.5);
	ActivateComponent<World::Light>(Light, GameWorld);
	ActivateComponent<World::Mesh>(
	    Light,
	    GameWorld,
	    Meshes::MeshType::Static,
	    2,
	    false);
	ActivateComponent<World::Transform>(Light, GameWorld);
	GameWorld[Light].Transform()->Tranformations.emplace_back(
	    Transform::Type::AutoPosition,
	    glm::dmat4x4(1));

	Light = GameWorld.CloneEntity(Light);
	GameWorld[Light].Position() = {1, 1.5, -2};
	GameWorld[Light].Light() = std::nullopt;
	Light = GameWorld.CloneEntity(Light);
	GameWorld[Light].Position() = {1.3, 1.5, -2};

	/*
	Light = GameWorld.NewEntity();
	ActivateComponent<World::Light>(Light, GameWorld);
	GameWorld.LightComponents[Light]->LightType = LightInfo::Type::Direction;
	GameWorld.LightComponents[Light]->Direction = {0.5, -0.5, 0.5};
	*/
	constexpr DSeconds TickRate{1.0 / 60.0};
	DSeconds TimeSinceLastUpdate{0};

	double FrameTime = 0;
	double TickTime = 0;

	while (!GameWorld.Quit)
	{
		{
			MTR_SCOPE("Main", "Main Loop", 0);
			auto Now = std::chrono::high_resolution_clock::now();
			DSeconds dt = std::chrono::duration_cast<DSeconds>(Now - LastTime);

			TimeSinceLastUpdate += dt;

			size_t Ticks = 0;
			for (; TimeSinceLastUpdate > TickRate;
			     TimeSinceLastUpdate -= TickRate)
			{
				{
					MTR_SCOPE("Main", "Update Loop", 0.000002);
					for (auto &RunGroup : GameWorld.UpdateSystems)
					{
						for (auto &[Active, System] : RunGroup)
						{
							if (Active)
							{
								System(GameWorld, TickRate);
							}
						}
					}
					Ticks++;
					if (Ticks > 10)
					{
						TimeSinceLastUpdate = DSeconds{0};
						break;
					}
				}
				mtr_flush();
			}

			glClearColor(0, 0, 0, 1);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			for (size_t i = 0; i < GameWorld.RenderSystems.size(); i++)
			{
				GameWorld.RenderSystems[i](
				    GameWorld,
				    DSeconds(TimeSinceLastUpdate / TickRate));
			}

			SDL_GL_SwapWindow(window);

			// std::cout << "CameraPosition {x, y, z}: {" <<
			// GameWorld.View.GetPosition().x << ", " <<
			// GameWorld.View.GetPosition().y << ", " <<
			// GameWorld.View.GetPosition().z << "}\n";

			LastTime = Now;
		}
		mtr_flush();
	}
	mtr_flush();
	mtr_shutdown();
	return 0;
}
