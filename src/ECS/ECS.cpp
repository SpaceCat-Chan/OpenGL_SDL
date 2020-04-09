#include "ECS.hpp"

KeyTracker UserInput::Keyboard;
std::vector<std::function<Error(World &, DSeconds)>> UserInput::PrePoll;
std::vector<std::function<Error(World &, DSeconds)>> UserInput::PostPoll;
std::vector<std::function<Error(World &, DSeconds, SDL_Event *)>> UserInput::PreEvent;
std::vector<std::function<Error(World &, DSeconds, SDL_Event *)>> UserInput::PostEvent;

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
 * \param Min one point to be scaled
 * \param Max one point to be scaled
 * 
 * \param Amount the amount to scale Min and Max by
 * 
 * \return the new Min and Max
 */
std::tuple<glm::dvec3, glm::dvec3> ExpandArea(glm::dvec3 Min, glm::dvec3 Max, double Amount = sqrt2)
{
	glm::dvec3 Mid = glm::mix(Min, Max, 0.5);
	Min = Mid + ((Mid - Min) * Amount);
	Max = Mid - ((Mid + Amount) * Amount);
	return {Min, Max};
}

template <class MeshType>
std::enable_if_t<std::is_same<MeshType, Mesh>::value || std::is_same<MeshType, TexturedMesh>::value, std::tuple<glm::dvec3, glm::dvec3>> CalculateTransformedMinMax(MeshType &Mesh, glm::dmat4x4 Transform)
{
	std::array<glm::vec3, 6> VertexExtremes;

	VertexExtremes[0] = Transform * glm::vec4(Mesh.GetMostExtremeVertex(Mesh::Side::NegX), 1);
	VertexExtremes[1] = Transform * glm::vec4(Mesh.GetMostExtremeVertex(Mesh::Side::NegY), 1);
	VertexExtremes[2] = Transform * glm::vec4(Mesh.GetMostExtremeVertex(Mesh::Side::NegZ), 1);
	VertexExtremes[4] = Transform * glm::vec4(Mesh.GetMostExtremeVertex(Mesh::Side::PosY), 1);
	VertexExtremes[5] = Transform * glm::vec4(Mesh.GetMostExtremeVertex(Mesh::Side::PosZ), 1);
	VertexExtremes[3] = Transform * glm::vec4(Mesh.GetMostExtremeVertex(Mesh::Side::PosX), 1);

	glm::dvec3 ResultMin = {
		VertexExtremes[0].x,
		VertexExtremes[1].y,
		VertexExtremes[2].z};

	glm::dvec3 ResultMax = {
		VertexExtremes[3].x,
		VertexExtremes[4].y,
		VertexExtremes[5].z};

	if (true) //this is here until i figure out how to check for rotation
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

		return ExpandArea(ResultMin, ResultMax);
	}
	return {ResultMin, ResultMax};
}

template <class MeshType>
std::enable_if_t<std::is_same<MeshType, Meshes>::value, std::tuple<glm::dvec3, glm::dvec3>> CalculateTransformedMinMax(MeshType Mesh, glm::dmat4x4 Transform)
{
	if (Mesh.Type == Meshes::MeshType::Static)
	{
		return CalculateTransformedMinMax(Meshes::StaticMeshes[Mesh.MeshIndex], Transform);
	}
	else
	{
		return CalculateTransformedMinMax(Meshes::TexturedMeshes[Mesh.MeshIndex], Transform);
	}
}

// ForceMainShader is there for future use
void Render(World &GameWorld, bool ForceMainShader = false)
{
	for (size_t MeshIndex = 0; MeshIndex < GameWorld.PositionComponents.size(); MeshIndex++)
	{
		if (GameWorld.MeshComponents[MeshIndex])
		{
			constexpr size_t MaxLightAmount = 8;

			std::vector<glm::dvec3> LightPositions;
			std::vector<glm::dvec3> LightColors;
			std::vector<glm::dvec3> LightAttenuationPacked;
			std::vector<glm::dvec3> LightDirection;
			std::vector<double> LightCutoffAngle;
			std::vector<int> LightType;

			glm::dmat4x4 MeshTransform;
			if (GameWorld.ChildrenComponents[MeshIndex])
			{
				MeshTransform = GameWorld.ChildrenComponents[MeshIndex]->CalculateFullTransform(GameWorld, MeshIndex);
			}
			else if (GameWorld.TransformComponents[MeshIndex])
			{
				MeshTransform = GameWorld.TransformComponents[MeshIndex]->CalculateFull();
			}
			else
			{
				MeshTransform = glm::dmat4x4(1);
			}

			//probably closest to multiple returns we get
			auto [Min, Max] = CalculateTransformedMinMax(*GameWorld.MeshComponents[MeshIndex], MeshTransform);

			if (GameWorld.MeshComponents[MeshIndex]->AffectedByLights)
			{
				for (size_t LightIndex = 0; LightIndex < GameWorld.PositionComponents.size() && LightPositions.size() < MaxLightAmount; LightIndex++)
				{
					if (GameWorld.LightComponents[LightIndex] == nullptr)
					{
						continue;
					}
					auto &Light = *GameWorld.LightComponents[LightIndex];
					if (Light.LightType == LightInfo::Type::Direction)
					{
						LightPositions.push_back({0, 0, 0});
						LightColors.push_back(Light.Color);
						LightAttenuationPacked.push_back({Light.Constant, Light.Linear, Light.Quadratic});
						LightDirection.push_back(Light.Direction);
						LightCutoffAngle.push_back(Light.CutoffAngle);
						LightType.push_back(1);
					}
					else
					{
						glm::dvec3 LightPosition = Light.Position;
						if (GameWorld.ChildrenComponents[LightIndex])
						{
							LightPosition = GameWorld.ChildrenComponents[LightIndex]->CalculateFullTransform(GameWorld, LightIndex) * glm::dvec4(LightPosition, 1);
						}
						else if (GameWorld.TransformComponents[LightIndex])
						{
							auto &Transform = *GameWorld.TransformComponents[LightIndex];
							if (Transform.PositionSpace == Transform::Space::Model)
							{
								LightPosition = Transform(glm::dvec4(LightPosition, 1));
							}
						}
						glm::dvec3 Position = glm::clamp(glm::dvec3(LightPosition), Min, Max);

						bool WithinRange = glm::pow(Light.CutoffDistance, 2) > glm::pow(Position.x, 2) + glm::pow(Position.y, 2) + glm::pow(Position.z, 2);
						if (WithinRange)
						{
							LightPositions.push_back(LightPosition);
							LightColors.push_back(Light.Color);
							LightAttenuationPacked.push_back({Light.Constant, Light.Linear, Light.Quadratic});
							LightDirection.push_back(Light.Direction);
							LightCutoffAngle.push_back(Light.CutoffAngle);
							LightType.push_back(0);
						}
					}
				}

				for (size_t i = 0; i < LightPositions.size(); i++)
				{
					LightPositions[i] = glm::dvec3(GameWorld.View.GetView() * glm::dvec4(LightPositions[i], 1));
					LightDirection[i] = glm::dvec3(GameWorld.View.GetView() * glm::dvec4(LightDirection[i], 0));
					LightCutoffAngle[i] = glm::cos(LightCutoffAngle[i]);
					//std::cout << "LightCutoffAngle[i]: " << LightCutoffAngle[i] << '[' << i << ']' << '\n';
				}

				GameWorld.ShaderProgram.SetUniform("u_Camera_LightPosition", LightPositions);
				GameWorld.ShaderProgram.SetUniform("u_LightColor", LightColors);
				GameWorld.ShaderProgram.SetUniform("u_LightAttenuationPacked", LightAttenuationPacked);
				GameWorld.ShaderProgram.SetUniform("u_LightDirection", LightDirection);
				GameWorld.ShaderProgram.SetUniform("u_LightCutoffAngle", LightCutoffAngle);
				GameWorld.ShaderProgram.SetUniform("u_LightType", LightType);
			}
			GameWorld.ShaderProgram.SetUniform("u_AmountOfLights", (GLuint)LightPositions.size());

			GameWorld.ShaderProgram.SetUniform("MVP", GameWorld.View.GetMVP() * MeshTransform);
			GameWorld.ShaderProgram.SetUniform("u_Model", MeshTransform);
			GameWorld.ShaderProgram.SetUniform("u_Color", glm::dvec3(1, 1, 1));
			GameWorld.ShaderProgram.SetUniform("u_FullBright", !GameWorld.MeshComponents[MeshIndex]->AffectedByLights);
			Render(*GameWorld.MeshComponents[MeshIndex], GameWorld.ShaderProgram, GameWorld.View, true, true);
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
	for (size_t i = 0; i < GameWorld.PositionComponents.size(); i++)
	{
		if (GameWorld.TransformComponents[i])
		{
			auto &Transform = *GameWorld.TransformComponents[i];
			auto &Position = GameWorld.PositionComponents[i];
			if (Transform.PositionComponentSpace == Transform::Space::View)
			{
				bool HitAutoPosition = false;
				for (auto &Matrix : Transform.Tranformations)
				{
					if (Matrix.first == Transform::Type::AutoPosition)
					{
						if (HitAutoPosition == false && Position)
						{
							Matrix.second = glm::translate(glm::dmat4x4(1), *Position);
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

Error UserInputSystem(World &GameWorld, DSeconds dt)
{

	for (auto &Handler : UserInput::PrePoll)
	{
		Handler(GameWorld, dt);
	}

	SDL_Event Event;
	UserInput::Keyboard.Update(1);
	while (SDL_PollEvent(&Event))
	{
		for (auto &Handler : UserInput::PreEvent)
		{
			Handler(GameWorld, dt, &Event);
		}

		UserInput::Keyboard.UpdateKey(&Event);
		switch (Event.type)
		{

		case SDL_QUIT:
			GameWorld.Quit = true;
			break;

		case SDL_MOUSEMOTION:
			constexpr double Sensitivity = 0.1;
			GameWorld.View.OffsetPitchYaw((double)Event.motion.yrel * Sensitivity * -1, (double)Event.motion.xrel * Sensitivity, 0, -89, 89);
		}

		for (auto &Handler : UserInput::PostEvent)
		{
			Handler(GameWorld, dt, &Event);
		}
	}

	for (auto &Handler : UserInput::PostPoll)
	{
		Handler(GameWorld, dt);
	}

	if (UserInput::Keyboard[SDL_SCANCODE_ESCAPE].Clicked)
	{
		GameWorld.Quit = true;
	}

	if (UserInput::Keyboard[SDL_SCANCODE_W].Active)
	{
		GameWorld.View.Move((glm::normalize(GameWorld.View.GetViewVector())) * (1.0 * dt.count()));
	}
	if (UserInput::Keyboard[SDL_SCANCODE_S].Active)
	{
		GameWorld.View.Move((glm::normalize(GameWorld.View.GetViewVector())) * (1.0 * -dt.count()));
	}

	if (UserInput::Keyboard[SDL_SCANCODE_E].Active)
	{
		*GameWorld.PositionComponents[9] += glm::dvec3{0.2, 0, 0} * dt.count();
	}
	if (UserInput::Keyboard[SDL_SCANCODE_Q].Active)
	{
		*GameWorld.PositionComponents[14] += glm::dvec3{0, 0.05, 0} * dt.count();
	}

	if (UserInput::Keyboard[SDL_SCANCODE_LEFT].Clicked)
	{
		GameWorld.View.OffsetPitchYaw(0, 0, -10);
	}
	if (UserInput::Keyboard[SDL_SCANCODE_RIGHT].Clicked)
	{
		GameWorld.View.OffsetPitchYaw(0, 0, 10);
	}

	return Error(Error::Type::None);
}
