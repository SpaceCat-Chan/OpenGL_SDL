#define TINYOBJLOADER_IMPLEMENTATION
#include "Mesh.hpp"

class MTLLoader : public tinyobj::MaterialReader
{
public:
	MTLLoader() = default;
	~MTLLoader() = default;
	bool operator()(const std::string &matId,
					std::vector<tinyobj::material_t> *materials,
					std::map<std::string, int> *matMap,
					std::string *warn, std::string *err) override;
};

struct Index : public tinyobj::index_t
{
	int Material;
};

bool operator==(const Index &lhs, const Index &rhs)
{
	return lhs.normal_index == rhs.normal_index && lhs.texcoord_index == rhs.texcoord_index && lhs.vertex_index == rhs.vertex_index && lhs.Material == rhs.Material;
}

void Mesh::LoadMesh(std::string Filename, std::vector<std::string> &DiffuseFiles, std::vector<std::string> &SpecularFiles, std::vector<std::string> &BumpFiles)
{
	DiffuseFiles.clear();
	SpecularFiles.clear();
	BumpFiles.clear();

	tinyobj::attrib_t MeshAttributes;
	std::vector<tinyobj::shape_t> Shapes;
	std::vector<tinyobj::material_t> MeshMaterials;

	std::string Warn, Error;

	isfstream File(Filename, "r+");

	MTLLoader MTL;
	bool Result = tinyobj::LoadObj(&MeshAttributes, &Shapes, &MeshMaterials, &Warn, &Error, &File, &MTL);

	if (!Warn.empty())
	{
		std::cout << "Warning: " << Warn << '\n';
	}
	if (!Error.empty())
	{
		std::cerr << "Error: " << Error << '\n';
	}

	if (Result == false)
	{
		std::cout << "Failed to load meshfile: " << Filename << '\n';
		return;
	}

	std::vector<GLfloat> Positions;
	std::vector<GLfloat> UVCoords;
	std::vector<GLfloat> Normals;
	std::vector<GLfloat> Shininess;
	std::vector<std::vector<glm::vec3>> Tangents;
	std::vector<GLfloat> TangentsAvaregedAndSplit;

	glBindVertexArray(m_VertexArray);

	m_IndexBuffer.clear();
	m_IndexAmount.size();
	std::vector<std::vector<GLuint>> ExpandedIndexes;
	std::vector<std::vector<Index>> StoredIndexes;

	//this is needed for forcing materials to be per vertex
	for (size_t i = 0; i < Shapes.size(); i++)
	{
		StoredIndexes.emplace_back();
		Tangents.emplace_back();
		for (size_t j = 0; j < Shapes[i].mesh.num_face_vertices.size(); j++)
		{
			StoredIndexes[i].emplace_back();
			StoredIndexes[i].emplace_back();
			StoredIndexes[i].emplace_back();
			Tangents[i].emplace_back();
			Tangents[i].emplace_back();
			Tangents[i].emplace_back();
			StoredIndexes[i][j * 3].normal_index = Shapes[i].mesh.indices[j * 3].normal_index;
			StoredIndexes[i][j * 3 + 1].normal_index = Shapes[i].mesh.indices[j * 3 + 1].normal_index;
			StoredIndexes[i][j * 3 + 2].normal_index = Shapes[i].mesh.indices[j * 3 + 2].normal_index;
			StoredIndexes[i][j * 3].texcoord_index = Shapes[i].mesh.indices[j * 3].texcoord_index;
			StoredIndexes[i][j * 3 + 1].texcoord_index = Shapes[i].mesh.indices[j * 3 + 1].texcoord_index;
			StoredIndexes[i][j * 3 + 2].texcoord_index = Shapes[i].mesh.indices[j * 3 + 2].texcoord_index;
			StoredIndexes[i][j * 3].vertex_index = Shapes[i].mesh.indices[j * 3].vertex_index;
			StoredIndexes[i][j * 3 + 1].vertex_index = Shapes[i].mesh.indices[j * 3 + 1].vertex_index;
			StoredIndexes[i][j * 3 + 2].vertex_index = Shapes[i].mesh.indices[j * 3 + 2].vertex_index;

			StoredIndexes[i][j * 3].Material = Shapes[i].mesh.material_ids[j];
			StoredIndexes[i][j * 3 + 1].Material = Shapes[i].mesh.material_ids[j];
			StoredIndexes[i][j * 3 + 2].Material = Shapes[i].mesh.material_ids[j];

			if (StoredIndexes[i][j * 3].vertex_index != -1 &&
				StoredIndexes[i][j * 3 + 1].vertex_index != -1 &&
				StoredIndexes[i][j * 3 + 2].vertex_index != -1 &&
				StoredIndexes[i][j * 3].texcoord_index != -1 &&
				StoredIndexes[i][j * 3 + 1].texcoord_index != -1 &&
				StoredIndexes[i][j * 3 + 2].texcoord_index != -1 &&
				StoredIndexes[i][j * 3].normal_index != -1 &&
				StoredIndexes[i][j * 3 + 1].normal_index != -1 &&
				StoredIndexes[i][j * 3 + 2].normal_index != -1)
			{
				glm::vec3 pos1, pos2, pos3;
				pos1.x = MeshAttributes.vertices[StoredIndexes[i][j * 3].vertex_index * 3];
				pos1.y = MeshAttributes.vertices[StoredIndexes[i][j * 3].vertex_index * 3 + 1];
				pos1.z = MeshAttributes.vertices[StoredIndexes[i][j * 3].vertex_index * 3 + 2];

				pos2.x = MeshAttributes.vertices[StoredIndexes[i][j * 3 + 1].vertex_index * 3];
				pos2.y = MeshAttributes.vertices[StoredIndexes[i][j * 3 + 1].vertex_index * 3 + 1];
				pos2.z = MeshAttributes.vertices[StoredIndexes[i][j * 3 + 1].vertex_index * 3 + 2];

				pos3.x = MeshAttributes.vertices[StoredIndexes[i][j * 3 + 2].vertex_index * 3];
				pos3.y = MeshAttributes.vertices[StoredIndexes[i][j * 3 + 2].vertex_index * 3 + 1];
				pos3.z = MeshAttributes.vertices[StoredIndexes[i][j * 3 + 2].vertex_index * 3 + 2];

				glm::vec2 uv1, uv2, uv3;
				uv1.x = MeshAttributes.texcoords[StoredIndexes[i][j * 3].texcoord_index * 2];
				uv1.y = MeshAttributes.texcoords[StoredIndexes[i][j * 3].texcoord_index * 2 + 1];

				uv2.x = MeshAttributes.texcoords[StoredIndexes[i][j * 3 + 1].texcoord_index * 2];
				uv2.y = MeshAttributes.texcoords[StoredIndexes[i][j * 3 + 1].texcoord_index * 2 + 1];

				uv3.x = MeshAttributes.texcoords[StoredIndexes[i][j * 3 + 2].texcoord_index * 2];
				uv3.y = MeshAttributes.texcoords[StoredIndexes[i][j * 3 + 2].texcoord_index * 2 + 1];

				glm::vec3 edge1 = pos2 - pos1;
				glm::vec3 edge2 = pos3 - pos1;
				glm::vec2 deltaUV1 = uv2 - uv1;
				glm::vec2 deltaUV2 = uv3 - uv1;

				glm::vec3 Result;
				float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
				Result.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
				Result.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
				Result.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
				Result = glm::normalize(Result);

				Tangents[i][j * 3] = Result;
				Tangents[i][j * 3 + 1] = Result;
				Tangents[i][j * 3 + 2] = Result;
			}
		}
	}

	for (size_t i = 0; i < Shapes.size(); i++)
	{

		m_IndexBuffer.push_back(0);
		ExpandedIndexes.emplace_back();

		for (size_t j = 0; j < Shapes[i].mesh.indices.size(); j++)
		{
			Sint64 Found = -1;
			for (size_t k = 0; k < ExpandedIndexes[i].size(); k++)
			{
				if (StoredIndexes[i][j] == StoredIndexes[i][k])
				{
					Found = k;
					break;
				}
			}
			if (Found != -1)
			{
				ExpandedIndexes[i].push_back(ExpandedIndexes[i][Found]);
			}
			else
			{
				if (StoredIndexes[i][j].vertex_index != -1)
				{
					Positions.push_back(MeshAttributes.vertices[StoredIndexes[i][j].vertex_index * 3]);
					Positions.push_back(MeshAttributes.vertices[StoredIndexes[i][j].vertex_index * 3 + 1]);
					Positions.push_back(MeshAttributes.vertices[StoredIndexes[i][j].vertex_index * 3 + 2]);
				}
				else
				{
					Positions.push_back(0);
					Positions.push_back(0);
					Positions.push_back(0);
				}

				if (StoredIndexes[i][j].texcoord_index != -1)
				{
					UVCoords.push_back(MeshAttributes.texcoords[StoredIndexes[i][j].texcoord_index * 2]);
					UVCoords.push_back(MeshAttributes.texcoords[StoredIndexes[i][j].texcoord_index * 2 + 1]);
				}
				else
				{
					UVCoords.push_back(0);
					UVCoords.push_back(0);
				}

				if (StoredIndexes[i][j].normal_index != -1)
				{
					Normals.push_back(MeshAttributes.normals[StoredIndexes[i][j].normal_index * 3]);
					Normals.push_back(MeshAttributes.normals[StoredIndexes[i][j].normal_index * 3 + 1]);
					Normals.push_back(MeshAttributes.normals[StoredIndexes[i][j].normal_index * 3 + 2]);
				}
				else
				{
					Normals.push_back(0);
					Normals.push_back(0);
					Normals.push_back(0);
				}

				if (StoredIndexes[i][j].Material != -1)
				{
					Shininess.push_back(MeshMaterials[StoredIndexes[i][j].Material].shininess);
				}
				else
				{
					Shininess.push_back(1.0);
				}

				TangentsAvaregedAndSplit.push_back(Tangents[i][j].x);
				TangentsAvaregedAndSplit.push_back(Tangents[i][j].y);
				TangentsAvaregedAndSplit.push_back(Tangents[i][j].z);

				ExpandedIndexes[i].push_back((Positions.size() - 1) / 3);
			}
		}

		glGenBuffers(1, &m_IndexBuffer[i]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer[i]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, ExpandedIndexes[i].size() * sizeof(GLuint), ExpandedIndexes[i].data(), GL_STATIC_DRAW);
		m_IndexAmount.push_back(ExpandedIndexes[i].size());

		DiffuseFiles.push_back(MeshMaterials[Shapes[i].mesh.material_ids[0]].diffuse_texname);
		SpecularFiles.push_back(MeshMaterials[Shapes[i].mesh.material_ids[0]].specular_texname);
		BumpFiles.push_back(MeshMaterials[Shapes[i].mesh.material_ids[0]].bump_texname);
	}

	glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer[0]);
	glBufferData(GL_ARRAY_BUFFER, Positions.size() * sizeof(GLfloat), Positions.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, nullptr);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer[1]);
	glBufferData(GL_ARRAY_BUFFER, UVCoords.size() * sizeof(GLfloat), UVCoords.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(1, 2, GL_FLOAT, false, 0, nullptr);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer[2]);
	glBufferData(GL_ARRAY_BUFFER, Normals.size() * sizeof(GLfloat), Normals.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(2, 3, GL_FLOAT, false, 0, nullptr);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer[3]);
	glBufferData(GL_ARRAY_BUFFER, Shininess.size() * sizeof(GLfloat), Shininess.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(3, 1, GL_FLOAT, false, 0, nullptr);
	glEnableVertexAttribArray(3);

	glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer[4]);
	glBufferData(GL_ARRAY_BUFFER, TangentsAvaregedAndSplit.size() * sizeof(GLfloat), TangentsAvaregedAndSplit.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(4, 3, GL_FLOAT, false, 0, nullptr);
	glEnableVertexAttribArray(4);
}

bool MTLLoader::operator()(const std::string &matId,
						   std::vector<tinyobj::material_t> *materials,
						   std::map<std::string, int> *matMap,
						   std::string *warn, std::string *err)
{
	(void)err;
	(void)matId;

	std::string Filename = matId;
	if (Filename[0] != '/' && Filename[0] != '\\')
	{
		Filename = "res/" + Filename;
	}

	isfstream File(Filename, "r+");

	LoadMtl(matMap, materials, &File, warn, err);

	return true;
}