#include "ECS.hpp"

std::vector<Mesh> Meshes::StaticMeshes;
std::vector<TexturedMesh> Meshes::TexturedMeshes;

void Render(Mesh &mesh, size_t Index, Shader &ShaderProgram, Camera &View, bool OutsideLight = false, bool OutsideMesh = false, bool OutsideTextures = false)
{
	mesh.Bind(Index);

	ShaderProgram.SetUniform("MVP", View.GetMVP());
	ShaderProgram.SetUniform("u_View", View.GetView());
	if (!OutsideMesh)
	{
		ShaderProgram.SetUniform("u_Model", glm::mat4(1));
		ShaderProgram.SetUniform("u_Color", glm::dvec3(0, 0, 0));
	}
	if (!OutsideLight)
	{
		ShaderProgram.SetUniform("u_Camera_LightPosition", glm::dvec3(0, 0, 0));
		ShaderProgram.SetUniform("u_LightColor", glm::dvec3(0, 0, 0));
	}
	if (!OutsideTextures)
	{
		glBindTexture(GL_TEXTURE0, 0);
		ShaderProgram.SetUniform("u_Diffuse", 0);
		ShaderProgram.SetUniform("u_Specular", 0);
		ShaderProgram.SetUniform("u_Bump", 0);
		ShaderProgram.SetUniform("u_Disp", 0);
	}
	glDrawElements(GL_TRIANGLES, mesh.GetIndexCount(Index), GL_UNSIGNED_INT, nullptr);
}
void Render(Mesh &mesh, Shader &ShaderProgram, Camera &View, bool OutsideLight = false, bool OutsideMesh = false, bool OutsideTextures = false)
{
	for (size_t i = 0; i < mesh.GetMeshCount(); i++)
	{
		Render(mesh, i, ShaderProgram, View, OutsideLight, OutsideMesh, OutsideTextures);
	}
}

void Render(TexturedMesh &Mesh, Shader &ShaderProgram, Camera &View, bool OutsideLight = false, bool OutsideMesh = false)
{
	for (size_t i = 0; i < Mesh.GetMeshCount(); i++)
	{
		Mesh.Bind(i, ShaderProgram);
		Render(Mesh.m_Mesh, i, ShaderProgram, View, OutsideLight, OutsideMesh, true);
	}
}

void Render(Meshes &Mesh, Shader &ShaderProgram, Camera &View, bool OutsideLight = false, bool OutsideMesh = false, bool OutsideTextures = false)
{
	if (Mesh.Type == Meshes::MeshType::Static)
	{
		Render(Mesh.StaticMeshes[Mesh.MeshIndex], ShaderProgram, View, OutsideLight, OutsideMesh, OutsideTextures);
	}
	else
	{
		Render(Mesh.TexturedMeshes[Mesh.MeshIndex], ShaderProgram, View, OutsideLight, OutsideMesh);
	}
}

Error RenderSystem(World &GameWorld, DSeconds dt)
{
	for (size_t i = 0; i < GameWorld.ComponentMask.size(); i++)
	{
		if (GameWorld.ComponentMask[i][World::Components::Mesh])
		{
			Render(GameWorld.MeshComponents[i], GameWorld.ShaderProgram, GameWorld.View);
		}
	}
	return Error(Error::ErrorType::None);
}