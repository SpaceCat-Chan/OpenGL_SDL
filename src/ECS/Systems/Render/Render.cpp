#include "Render.hpp"

#include "ECS/ECS.hpp"

#include "ECS/TransformedMinMax.hpp"

void Render(
    Mesh &mesh,
    size_t Index,
    Shader &ShaderProgram,
    Camera &View,
    bool OutsideLight = false,
    bool OutsideMesh = false,
    bool OutsideTextures = false)
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
		ShaderProgram.SetUniform("u_AmountOfLights", 0);
	}
	if (!OutsideTextures)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
		ShaderProgram.SetUniform("u_Diffuse", 0);
		ShaderProgram.SetUniform("u_Specular", 0);
		ShaderProgram.SetUniform("u_Bump", 0);
		ShaderProgram.SetUniform("u_Disp", 0);
		ShaderProgram.SetUniform("u_UseTextures", 0);
	}
	else
	{
		ShaderProgram.SetUniform("u_UseTextures", 1);
	}
	glDrawElements(
	    GL_TRIANGLES,
	    mesh.GetIndexCount(Index),
	    GL_UNSIGNED_INT,
	    nullptr);
}
void Render(
    Mesh &mesh,
    Shader &ShaderProgram,
    Camera &View,
    bool OutsideLight = false,
    bool OutsideMesh = false,
    bool OutsideTextures = false)
{
	for (size_t i = 0; i < mesh.GetMeshCount(); i++)
	{
		Render(
		    mesh,
		    i,
		    ShaderProgram,
		    View,
		    OutsideLight,
		    OutsideMesh,
		    OutsideTextures);
	}
}

void Render(
    TexturedMesh &Mesh,
    Shader &ShaderProgram,
    Camera &View,
    bool OutsideLight = false,
    bool OutsideMesh = false)
{
	for (size_t i = 0; i < Mesh.GetMeshCount(); i++)
	{
		Mesh.Bind(i, ShaderProgram);
		Render(
		    Mesh.m_Mesh,
		    i,
		    ShaderProgram,
		    View,
		    OutsideLight,
		    OutsideMesh,
		    true);
	}
}

void Render(
    Meshes &Mesh,
    Shader &ShaderProgram,
    Camera &View,
    bool OutsideLight = false,
    bool OutsideMesh = false,
    bool OutsideTextures = false)
{
	if (Mesh.Type == Meshes::MeshType::Static)
	{
		Render(
		    Mesh.StaticMeshes[Mesh.MeshIndex],
		    ShaderProgram,
		    View,
		    OutsideLight,
		    OutsideMesh,
		    OutsideTextures);
	}
	else
	{
		Render(
		    Mesh.TexturedMeshes[Mesh.MeshIndex],
		    ShaderProgram,
		    View,
		    OutsideLight,
		    OutsideMesh);
	}
}

// ForceMainShader is there for future use
void Render(
    World &GameWorld,
    bool ForceMainShader = false,
    DSeconds dt = DSeconds(1))
{
	for (size_t MeshIndex = 0; MeshIndex < GameWorld.size(); MeshIndex++)
	{
		if (GameWorld[MeshIndex].Mesh())
		{
			constexpr size_t MaxLightAmount = 8;

			std::vector<glm::dvec3> LightPositions;
			std::vector<glm::dvec3> LightColors;
			std::vector<glm::dvec3> LightAttenuationPacked;
			std::vector<glm::dvec3> LightDirection;
			std::vector<double> LightCutoffAngle;
			std::vector<int> LightType;

			glm::dmat4x4 MeshTransform, OldMeshTransform;
			if (GameWorld[MeshIndex].Children())
			{
				MeshTransform =
				    GameWorld[MeshIndex].Children()->CalculateFullTransform(
				        GameWorld,
				        MeshIndex);
				OldMeshTransform =
				    GameWorld[MeshIndex].Children()->CalculateFullTransform(
				        GameWorld,
				        MeshIndex,
				        true);
			}
			else if (GameWorld[MeshIndex].Transform())
			{
				MeshTransform =
				    GameWorld[MeshIndex].Transform()->CalculateFull();
				if (GameWorld[MeshIndex].BasicBackup()->Transform_)
				{
					OldMeshTransform = GameWorld[MeshIndex]
					                       .BasicBackup()
					                       ->Transform_->CalculateFull();
				}
				else
				{
					OldMeshTransform = glm::dmat4{1};
				}
			}
			else
			{
				MeshTransform = glm::dmat4x4(1);
				OldMeshTransform = glm::dmat4{1};
			}

			// probably closest to multiple returns we get
			auto [NewMin, NewMax] = CalculateTransformedMinMax(
			    *GameWorld[MeshIndex].Mesh(),
			    MeshTransform);
			auto [OldMin, OldMax] = CalculateTransformedMinMax(
			    *GameWorld[MeshIndex].Mesh(),
			    OldMeshTransform);

			auto Min = glm::mix(OldMin, NewMin, dt.count());
			auto Max = glm::mix(OldMax, NewMax, dt.count());

			if (GameWorld[MeshIndex].Mesh()->AffectedByLights)
			{
				for (size_t LightIndex = 0;
				     LightIndex < GameWorld.size() &&
				     LightPositions.size() < MaxLightAmount;
				     LightIndex++)
				{
					if (!GameWorld[LightIndex].Light())
					{
						continue;
					}
					auto &Light = *GameWorld[LightIndex].Light();
					if (Light.LightType == LightInfo::Type::Direction)
					{
						LightPositions.push_back({0, 0, 0});
						LightColors.push_back(Light.Color);
						LightAttenuationPacked.push_back(
						    {Light.Constant, Light.Linear, Light.Quadratic});
						LightDirection.push_back(Light.Direction);
						LightCutoffAngle.push_back(Light.CutoffAngle);
						LightType.push_back(1);
					}
					else
					{
						glm::dvec3 LightPosition = Light.Position;
						if (GameWorld[LightIndex].Children())
						{
							LightPosition = GameWorld[LightIndex]
							                    .Children()
							                    ->CalculateFullTransform(
							                        GameWorld,
							                        LightIndex) *
							                glm::dvec4(LightPosition, 1);
						}
						else if (GameWorld[LightIndex].Transform())
						{
							auto &Transform =
							    *GameWorld[LightIndex].Transform();
							if (Transform.PositionSpace ==
							    Transform::Space::Model)
							{
								LightPosition =
								    Transform(glm::dvec4(LightPosition, 1));
							}
						}
						glm::dvec3 Position =
						    glm::clamp(glm::dvec3(LightPosition), Min, Max);

						bool WithinRange = glm::pow(Light.CutoffDistance, 2) >
						                   glm::pow(Position.x, 2) +
						                       glm::pow(Position.y, 2) +
						                       glm::pow(Position.z, 2);
						if (WithinRange)
						{
							LightPositions.push_back(LightPosition);
							LightColors.push_back(Light.Color);
							LightAttenuationPacked.push_back(
							    {Light.Constant,
							     Light.Linear,
							     Light.Quadratic});
							LightDirection.push_back(Light.Direction);
							LightCutoffAngle.push_back(Light.CutoffAngle);
							LightType.push_back(0);
						}
					}
				}

				for (size_t i = 0; i < LightPositions.size(); i++)
				{
					LightPositions[i] = glm::dvec3(
					    GameWorld.View.GetView() *
					    glm::dvec4(LightPositions[i], 1));
					LightDirection[i] = glm::dvec3(
					    GameWorld.View.GetView() *
					    glm::dvec4(LightDirection[i], 0));
					LightCutoffAngle[i] = glm::cos(LightCutoffAngle[i]);
					// std::cout << "LightCutoffAngle[i]: " <<
					// LightCutoffAngle[i] << '[' << i << ']' << '\n';
				}

				GameWorld.ShaderProgram.SetUniform(
				    "u_Camera_LightPosition",
				    LightPositions);
				GameWorld.ShaderProgram.SetUniform("u_LightColor", LightColors);
				GameWorld.ShaderProgram.SetUniform(
				    "u_LightAttenuationPacked",
				    LightAttenuationPacked);
				GameWorld.ShaderProgram.SetUniform(
				    "u_LightDirection",
				    LightDirection);
				GameWorld.ShaderProgram.SetUniform(
				    "u_LightCutoffAngle",
				    LightCutoffAngle);
				GameWorld.ShaderProgram.SetUniform("u_LightType", LightType);
			}
			GameWorld.ShaderProgram.SetUniform(
			    "u_AmountOfLights",
			    (GLuint)LightPositions.size());

			GameWorld.ShaderProgram.SetUniform(
			    "MVP_Old",
			    GameWorld.View.GetMVP() * OldMeshTransform);
			GameWorld.ShaderProgram.SetUniform(
			    "MVP",
			    GameWorld.View.GetMVP() * MeshTransform);
			GameWorld.ShaderProgram.SetUniform("u_Model_Old", OldMeshTransform);
			GameWorld.ShaderProgram.SetUniform("u_Model", MeshTransform);
			GameWorld.ShaderProgram.SetUniform(
			    "u_Lerp_Value",
			    GLfloat(dt.count()));
			GameWorld.ShaderProgram.SetUniform("u_Color", glm::dvec3(1, 1, 1));
			GameWorld.ShaderProgram.SetUniform(
			    "u_FullBright",
			    !GameWorld[MeshIndex].Mesh()->AffectedByLights);
			Render(
			    *GameWorld[MeshIndex].Mesh(),
			    GameWorld.ShaderProgram,
			    GameWorld.View,
			    true,
			    true);
		}
	}
}

Error RenderSystem(World &GameWorld, DSeconds dt)
{
	Render(GameWorld, false, dt);
	return Error(Error::Type::None);
}
