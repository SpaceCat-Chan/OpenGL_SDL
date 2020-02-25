
#include <iostream>
#include <cassert>
#include <fstream>
#include <cerrno>
#include <clocale>

#include <SDL.h>
#include <GL/glew.h>
#include <SDL_opengl.h>
#include <glm/ext.hpp>

#include "Window/Window.hpp"
#include "Camera/Camera.hpp"
#include "Mesh/Mesh.hpp"

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

	std::string Vertex, Fragment;
	unsigned int VertShader, FragShader;
	unsigned int Program;

	Vertex = get_file_contents("res/Shader.vert");
	Fragment = get_file_contents("res/Shader.frag");

	VertShader = glCreateShader(GL_VERTEX_SHADER);
	FragShader = glCreateShader(GL_FRAGMENT_SHADER);

	const char *InternalVert = Vertex.c_str(), *InternalFrag = Fragment.c_str();
	GLint VertSize = Vertex.size(), FragSize = Fragment.size();

	glShaderSource(VertShader, 1, &InternalVert, &VertSize);
	glShaderSource(FragShader, 1, &InternalFrag, &FragSize);
	glCompileShader(VertShader);
	glCompileShader(FragShader);
	int VertComp;
	glGetShaderiv(VertShader, GL_COMPILE_STATUS, &VertComp);
	if (VertComp == 0)
	{
		int maxLength;
		glGetShaderiv(VertShader, GL_INFO_LOG_LENGTH, &maxLength);

		/* The maxLength includes the NULL character */
		char *vertexInfoLog;
		vertexInfoLog = (char *)malloc(maxLength);

		glGetShaderInfoLog(VertShader, maxLength, &maxLength, vertexInfoLog);
		std::cout << vertexInfoLog;

		/* Handle the error in an appropriate way such as displaying a message or writing to a log file. */
		/* In this simple program, we'll just leave */
		free(vertexInfoLog);
		return 1;
	}
	int FragComp;
	glGetShaderiv(FragShader, GL_COMPILE_STATUS, &FragComp);
	if (FragComp == 0)
	{
		int maxLength;
		glGetShaderiv(FragShader, GL_INFO_LOG_LENGTH, &maxLength);

		/* The maxLength includes the NULL character */
		char *vertexInfoLog;
		vertexInfoLog = (char *)malloc(maxLength);

		glGetShaderInfoLog(FragShader, maxLength, &maxLength, vertexInfoLog);
		std::cout << vertexInfoLog;

		/* Handle the error in an appropriate way such as displaying a message or writing to a log file. */
		/* In this simple program, we'll just leave */
		free(vertexInfoLog);
		return 1;
	}

	Program = glCreateProgram();
	glAttachShader(Program, VertShader);
	glAttachShader(Program, FragShader);

	glLinkProgram(Program);
	GLuint MatrixID = glGetUniformLocation(Program, "MVP");

	Mesh Cube;
	Cube.LoadMesh("res/cube.obj");

	Camera Yee;
	Yee.CreateProjection(35, 4/3, 1, 100);
	Yee.LookAt({0, 0, 0});
	Yee.MoveTo({-10, 0, -10});

	bool Quit = false;
	SDL_Event Event;
	while (!Quit)
	{
		while (SDL_PollEvent(&Event))
		{
			switch (Event.type)
			{

			case SDL_QUIT:
				Quit = true;
			}
		}
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		Cube.Bind();

		glm::mat4 Matrix = Yee.GetMVP();
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &Matrix[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, Cube.GetIndexCount());

		SDL_GL_SwapWindow(window);
	}
	return 0;
}
