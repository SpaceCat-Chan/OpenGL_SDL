#include "ECS.hpp"

//internal linkage
static const double sqrt2 = std::sqrt(2.0);

std::vector<Mesh> Meshes::StaticMeshes;
std::vector<TexturedMesh> Meshes::TexturedMeshes;

void Render(Mesh &mesh, size_t Index, Shader &ShaderProgram, Camera &View, bool OutsideLight = false, bool OutsideMesh = false, bool OutsideTextures = false)
{
	mesh.Bind(Index);

	ShaderProgram.SetUniform("u_View", View.GetView());
	if (!OutsideMesh)
	{
		ShaderProgram.SetUniform("MVP", View.GetMVP());
		ShaderProgram.SetUniform("u_Model", glm::mat4(1));
		ShaderProgram.SetUniform("u_Color", glm::dvec3(1, 1, 1));
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

/**
 * \brief takes two vectors and scales them up by Amount relative to the point between them
 * 
 * \param Min one point to be scaled, will be changed by the scaling
 * \param Max one point to be scaled, will be changed by the scaling
 * 
 * \param Amount the amount to scale Min and Max by
 */
void ExpandArea(glm::dvec3 Min, glm::dvec3 Max, double Amount = sqrt2)
{
	glm::dvec3 Mid = glm::mix(Min, Max, 0.5);
	Min = Mid + ((Mid - Min) * Amount);
	Max = Mid - ((Mid + Amount) * Amount);
}

template <class MeshType>
std::enable_if_t<std::is_same<MeshType, Mesh>::value || std::is_same<MeshType, TexturedMesh>::value, void> CalculateTransformedMinMax(MeshType &Mesh, Transform Transformation, glm::vec3 &ResultMin, glm::vec3 &ResultMax)
{
	std::array<glm::vec3, 6> VertexExtremes;
	glm::mat4x4 FullMatrix = Transformation.CalculateFull();

	VertexExtremes[0] = FullMatrix * glm::vec4(Mesh.GetMostExtremeVertex(Mesh::Side::NegX), 1);
	VertexExtremes[1] = FullMatrix * glm::vec4(Mesh.GetMostExtremeVertex(Mesh::Side::NegY), 1);
	VertexExtremes[2] = FullMatrix * glm::vec4(Mesh.GetMostExtremeVertex(Mesh::Side::NegZ), 1);
	VertexExtremes[4] = FullMatrix * glm::vec4(Mesh.GetMostExtremeVertex(Mesh::Side::PosY), 1);
	VertexExtremes[5] = FullMatrix * glm::vec4(Mesh.GetMostExtremeVertex(Mesh::Side::PosZ), 1);
	VertexExtremes[3] = FullMatrix * glm::vec4(Mesh.GetMostExtremeVertex(Mesh::Side::PosX), 1);

	ResultMin = {
		VertexExtremes[0].x,
		VertexExtremes[1].y,
		VertexExtremes[2].z};

	ResultMax = {
		VertexExtremes[3].x,
		VertexExtremes[4].y,
		VertexExtremes[5].z};

	if (Transformation.ContainsRotations())
	{
		//rotation may change which side the extremes belong to
		for (auto &Vertex : VertexExtremes)
		{
			if (Vertex.x < ResultMin.x)
			{
				ResultMin.x = Vertex.x;
			}
			if (Vertex.y < ResultMin.y)
			{
				ResultMin.y = Vertex.y;
			}
			if (Vertex.z < ResultMin.z)
			{
				ResultMin.z = Vertex.z;
			}

			if (Vertex.x > ResultMax.x)
			{
				ResultMax.x = Vertex.x;
			}
			if (Vertex.y > ResultMax.y)
			{
				ResultMax.y = Vertex.y;
			}
			if (Vertex.z > ResultMax.z)
			{
				ResultMax.z = Vertex.z;
			}
		}

		ExpandArea(ResultMin, ResultMax);
	}
}

template <class MeshType>
std::enable_if_t<std::is_same<MeshType, Meshes>::value, void> CalculateTransformedMinMax(MeshType Mesh, Transform Transformation, glm::vec3 &ResultMin, glm::vec3 &ResultMax)
{
	if (Mesh.Type == Meshes::MeshType::Static)
	{
		CalculateTransformedMinMax(Meshes::StaticMeshes[Mesh.MeshIndex], Transformation, ResultMin, ResultMax);
	}
	else
	{
		CalculateTransformedMinMax(Meshes::TexturedMeshes[Mesh.MeshIndex], Transformation, ResultMin, ResultMax);
	}
}

// ForceMainShader is there for future use
void Render(World &GameWorld, bool ForceMainShader = false)
{
	for (size_t MeshIndex = 0; MeshIndex < GameWorld.ComponentMask.size(); MeshIndex++)
	{
		if (GameWorld.ComponentMask[MeshIndex][World::Components::Mesh])
		{
			if (GameWorld.MeshComponents[MeshIndex].Type == Meshes::MeshType::None)
			{
				continue;
			}
			constexpr size_t MaxLightAmount = 8;

			std::vector<glm::vec3> LightPositions;
			std::vector<glm::vec3> LightColors;

			for (size_t LightIndex = 0; LightIndex < GameWorld.ComponentMask.size() && LightPositions.size() < MaxLightAmount; LightIndex++)
			{
				if (GameWorld.ComponentMask[LightIndex][World::Components::Light] == false)
				{
					continue;
				}
				auto &Light = GameWorld.LightComponents[LightIndex];
				if (Light.LightType == LightInfo::Type::Direction)
				{
					//not implemented
				}
				else
				{
					glm::vec3 Min, Max;
					CalculateTransformedMinMax(GameWorld.MeshComponents[MeshIndex], GameWorld.TransformComponents[MeshIndex], Min, Max);
					glm::vec3 LightPosition = Light.Position;
					if (GameWorld.ComponentMask[LightIndex][World::Transform])
					{
						auto &Transform = GameWorld.TransformComponents[LightIndex];
						if (Transform.PositionSpace == Transform::Space::Model)
						{
							LightPosition = Transform(glm::dvec4(LightPosition, 1));
						}
					}
					glm::vec3 Position = glm::clamp(glm::vec3(LightPosition), Min, Max);

					bool WithinRange = glm::pow(Light.CutoffDistance, 2) > glm::pow(Position.x, 2) + glm::pow(Position.y, 2) + glm::pow(Position.z, 2);
					if (WithinRange)
					{
						LightPositions.push_back(LightPosition);
						LightColors.push_back(Light.Color);
					}
				}
			}

			for (size_t i = 0; i < LightPositions.size(); i++)
			{
				LightPositions[i] = glm::vec3(GameWorld.View.GetView() * glm::vec4(LightPositions[i], 1));
			}

			GameWorld.ShaderProgram.SetUniform("u_Camera_LightPosition", LightPositions);
			GameWorld.ShaderProgram.SetUniform("u_LightColor", LightColors);
			GameWorld.ShaderProgram.SetUniform("u_AmountOfLights", (GLuint)LightPositions.size());

			if (GameWorld.ComponentMask[MeshIndex][World::Transform])
			{
				GameWorld.ShaderProgram.SetUniform("MVP", GameWorld.View.GetMVP() * (glm::dmat4x4)GameWorld.TransformComponents[MeshIndex].CalculateFull());
				GameWorld.ShaderProgram.SetUniform("u_Model", GameWorld.TransformComponents[MeshIndex].CalculateFull());
				GameWorld.ShaderProgram.SetUniform("u_Color", glm::dvec3(1, 1, 1));
				Render(GameWorld.MeshComponents[MeshIndex], GameWorld.ShaderProgram, GameWorld.View, true, true);
			}
			else
			{
				Render(GameWorld.MeshComponents[MeshIndex], GameWorld.ShaderProgram, GameWorld.View, true);
			}
		}
	}
}

Error RenderSystem(World &GameWorld, DSeconds dt)
{
	Render(GameWorld);
	return Error(Error::Type::None);
}

Error AutoPositionSystem(World &GameWorld, DSeconds dt)
{
	for (size_t i = 0; i < GameWorld.ComponentMask.size(); i++)
	{
		if (GameWorld.ComponentMask[i][World::Transform])
		{
			auto &Transform = GameWorld.TransformComponents[i];
			auto &Position = GameWorld.PositionComponents[i];
			if (Transform.PositionComponentSpace == Transform::Space::View)
			{
				bool HitAutoPosition = false;
				for (auto &Matrix : Transform.Tranformations)
				{
					if (Matrix.first == Transform::Type::AutoPosition)
					{
						if (HitAutoPosition == false && GameWorld.ComponentMask[i][World::Position])
						{
							Matrix.second = glm::translate(glm::dmat4x4(1), Position);
							HitAutoPosition = true;
						}
						else
						{
							Matrix.second = glm::dmat4x4(1);
						}
					}
				}
			}
		}
	}
	return Error(Error::Type::None);
}