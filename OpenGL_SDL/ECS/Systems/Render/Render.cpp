#include "Render.hpp"

#include "ECS/ECS.hpp"

#include "ECS/ComponentInterface.hpp"
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

struct LightsToRender
{
	std::vector<glm::dvec3> Positions;
	std::vector<glm::dvec3> Colors;
	std::vector<glm::dvec3> AttenuationsPacked;
	std::vector<glm::dvec3> Directions;
	std::vector<double> CutoffAngles;
	std::vector<int> Types;
};

LightsToRender
LightsAffectingMesh(const World &GameWorld, size_t id, double Interpolation)
{
	constexpr size_t MaxLightAmount = 8;

	LightsToRender Lights;

	glm::dmat4 MeshTransform = CalcTransform(GameWorld, id, false);
	glm::dmat4 OldMeshTransform = CalcTransform(GameWorld, id, true);

	// probably closest to multiple returns we get
	auto [NewMin, NewMax]
	    = CalculateTransformedMinMax(*GameWorld[id].Mesh(), MeshTransform);
	auto [OldMin, OldMax]
	    = CalculateTransformedMinMax(*GameWorld[id].Mesh(), OldMeshTransform);

	auto Min = glm::mix(OldMin, NewMin, Interpolation);
	auto Max = glm::mix(OldMax, NewMax, Interpolation);

	if (GameWorld[id].Mesh()->AffectedByLights)
	{
		for (size_t LightIndex = 0; LightIndex < GameWorld.size()
		                            && Lights.Positions.size() < MaxLightAmount;
		     LightIndex++)
		{
			if (!GameWorld[LightIndex].Light())
			{
				continue;
			}
			auto &Light = *GameWorld[LightIndex].Light();
			if (Light.LightType == LightInfo::Type::Direction)
			{
				Lights.Positions.push_back({0, 0, 0});
				Lights.Colors.push_back(Light.Color);
				Lights.AttenuationsPacked.push_back(
				    {Light.Constant, Light.Linear, Light.Quadratic});
				Lights.Directions.push_back(Light.Direction);
				Lights.CutoffAngles.push_back(Light.CutoffAngle);
				Lights.Types.push_back(1);
			}
			else
			{
				glm::dmat4 LightTransform
				    = CalcTransform(GameWorld, LightIndex);
				glm::dvec3 LightPosition
				    = LightTransform * glm::dvec4{Light.Position, 1};
				glm::dvec3 Position
				    = glm::clamp(glm::dvec3(LightPosition), Min, Max);

				bool WithinRange = glm::pow(Light.CutoffDistance, 2)
				                   > glm::pow(Position.x, 2)
				                         + glm::pow(Position.y, 2)
				                         + glm::pow(Position.z, 2);
				if (WithinRange)
				{
					Lights.Positions.push_back(LightPosition);
					Lights.Colors.push_back(Light.Color);
					Lights.AttenuationsPacked.push_back(
					    {Light.Constant, Light.Linear, Light.Quadratic});
					Lights.Directions.push_back(Light.Direction);
					Lights.CutoffAngles.push_back(Light.CutoffAngle);
					Lights.Types.push_back(0);
				}
			}
		}

		for (size_t i = 0; i < Lights.Positions.size(); i++)
		{
			Lights.Positions[i] = glm::dvec3(
			    GameWorld.View.GetView() * glm::dvec4(Lights.Colors[i], 1));
			Lights.Directions[i] = glm::dvec3(
			    GameWorld.View.GetView() * glm::dvec4(Lights.Directions[i], 0));
			Lights.CutoffAngles[i] = glm::cos(Lights.CutoffAngles[i]);
			// std::cout << "LightCutoffAngle[i]: " <<
			// LightCutoffAngle[i] << '[' << i << ']' << '\n';
		}
	}
	return Lights;
}

void Render(
    World &GameWorld,
    bool ForceMainShader = false,
    DSeconds dt = DSeconds(1))
{
	for (size_t MeshIndex = 0; MeshIndex < GameWorld.size(); MeshIndex++)
	{
		if (GameWorld[MeshIndex].Mesh())
		{
			Shader &ShaderToUse
			    = EntityShader(GameWorld, MeshIndex, ForceMainShader);
			ShaderToUse.Bind();

			LightsToRender Lights
			    = LightsAffectingMesh(GameWorld, MeshIndex, dt.count());

			ShaderToUse.SetUniform("u_Camera_LightPosition", Lights.Positions);
			ShaderToUse.SetUniform("u_LightColor", Lights.Colors);
			ShaderToUse.SetUniform(
			    "u_LightAttenuationPacked",
			    Lights.AttenuationsPacked);
			ShaderToUse.SetUniform("u_LightDirection", Lights.Directions);
			ShaderToUse.SetUniform("u_LightCutoffAngle", Lights.CutoffAngles);
			ShaderToUse.SetUniform("u_LightType", Lights.Types);
			ShaderToUse.SetUniform(
			    "u_AmountOfLights",
			    static_cast<GLuint>(Lights.Positions.size()));

			glm::dmat4 OldTransform = CalcTransform(GameWorld, MeshIndex, true);
			glm::dmat4 NewTransform = CalcTransform(GameWorld, MeshIndex);

			ShaderToUse.SetUniform(
			    "MVP_Old",
			    GameWorld.View.GetMVP() * OldTransform);
			ShaderToUse.SetUniform(
			    "MVP",
			    GameWorld.View.GetMVP() * NewTransform);
			ShaderToUse.SetUniform("u_Model_Old", OldTransform);
			ShaderToUse.SetUniform("u_Model", NewTransform);
			ShaderToUse.SetUniform("u_Lerp_Value", GLfloat(dt.count()));
			ShaderToUse.SetUniform("u_Color", glm::dvec3(1, 1, 1));
			ShaderToUse.SetUniform(
			    "u_FullBright",
			    !GameWorld[MeshIndex].Mesh()->AffectedByLights);
			Render(
			    *GameWorld[MeshIndex].Mesh(),
			    ShaderToUse,
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
