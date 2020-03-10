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

bool operator==(const tinyobj::index_t &lhs, const tinyobj::index_t &rhs)
{
	return lhs.normal_index == rhs.normal_index && lhs.texcoord_index == rhs.texcoord_index && lhs.vertex_index == rhs.vertex_index;
}

void Mesh::LoadMesh(std::string Filename)
{
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

	std::vector<float> Positions;
	std::vector<float> UVCoords;
	std::vector<float> Normals;

	glBindVertexArray(m_VertexArray);

	m_IndexBuffer.clear();
	m_IndexAmount.size();
	std::vector<std::vector<GLuint>> ExpandedIndecies;
	std::vector<std::vector<tinyobj::index_t>> ExpToOrig;
	for (size_t i = 0; i < Shapes.size(); i++)
	{

		m_IndexBuffer.push_back(0);
		ExpandedIndecies.emplace_back();
		//ExpToOrig.emplace_back();

		for (size_t j = 0; j < Shapes[i].mesh.indices.size(); j++)
		{
			Sint64 Found = -1;
			//for (size_t k = 0; k < Positions.size() / 3; k++)
			for (size_t k = 0; k < ExpandedIndecies[i].size(); k++)
			{
				//if (Shapes[i].mesh.indices[j] == Shapes[i].mesh.indices[k])
				if (
					false &&
					Shapes[i].mesh.indices[j].vertex_index != -1 &&
					Shapes[i].mesh.indices[j].texcoord_index != -1 &&
					Shapes[i].mesh.indices[j].normal_index != -1 &&
					MeshAttributes.vertices[Shapes[i].mesh.indices[j].vertex_index * 3] == Positions[k * 3] &&
					MeshAttributes.vertices[Shapes[i].mesh.indices[j].vertex_index * 3 + 1] == Positions[k * 3 + 1] &&
					MeshAttributes.vertices[Shapes[i].mesh.indices[j].vertex_index * 3 + 2] == Positions[k * 3 + 2] &&
					MeshAttributes.texcoords[Shapes[i].mesh.indices[j].texcoord_index * 2] == UVCoords[k * 2] &&
					MeshAttributes.vertices[Shapes[i].mesh.indices[j].texcoord_index * 2 + 1] == UVCoords[k * 2 + 1] &&
					MeshAttributes.normals[Shapes[i].mesh.indices[j].normal_index * 3] == Positions[k * 3] &&
					MeshAttributes.normals[Shapes[i].mesh.indices[j].normal_index * 3 + 1] == Normals[k * 3 + 1] &&
					MeshAttributes.normals[Shapes[i].mesh.indices[j].normal_index * 3 + 2] == Normals[k * 3 + 2])
				{
					Found = k;
					break;
				}
			}
			if (Found != -1)
			{
				//ExpToOrig[i].push_back(Shapes[i].mesh.indices[j]);
				ExpandedIndecies[i].push_back(Found);
			}
			else
			{
				if (Shapes[i].mesh.indices[j].vertex_index != -1)
				{
					Positions.push_back(MeshAttributes.vertices[Shapes[i].mesh.indices[j].vertex_index * 3]);
					Positions.push_back(MeshAttributes.vertices[Shapes[i].mesh.indices[j].vertex_index * 3 + 1]);
					Positions.push_back(MeshAttributes.vertices[Shapes[i].mesh.indices[j].vertex_index * 3 + 2]);
				}
				else
				{
					Positions.push_back(0);
					Positions.push_back(0);
					Positions.push_back(0);
				}

				if (Shapes[i].mesh.indices[j].texcoord_index != -1)
				{
					UVCoords.push_back(MeshAttributes.texcoords[Shapes[i].mesh.indices[j].texcoord_index * 2]);
					UVCoords.push_back(MeshAttributes.texcoords[Shapes[i].mesh.indices[j].texcoord_index * 2 + 1]);
				}
				else
				{
					UVCoords.push_back(0);
					UVCoords.push_back(0);
				}

				if (Shapes[i].mesh.indices[j].normal_index != -1)
				{
					Normals.push_back(MeshAttributes.normals[Shapes[i].mesh.indices[j].normal_index * 3]);
					Normals.push_back(MeshAttributes.normals[Shapes[i].mesh.indices[j].normal_index * 3 + 1]);
					Normals.push_back(MeshAttributes.normals[Shapes[i].mesh.indices[j].normal_index * 3 + 2]);
				}
				else
				{
					Normals.push_back(0);
					Normals.push_back(0);
					Normals.push_back(0);
				}

				ExpandedIndecies[i].push_back((Positions.size()-1)/3);
				//ExpandedIndecies[i].push_back((ExpandedIndecies[i].size());
			}
			//ExpToOrig[i].push_back(Shapes[i].mesh.indices[j]);
		}

		glGenBuffers(1, &m_IndexBuffer[i]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer[i]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, ExpandedIndecies[i].size() * sizeof(GLuint), ExpandedIndecies[i].data(), GL_STATIC_DRAW);
		m_IndexAmount.push_back(ExpandedIndecies[i].size());

		std::cout << "IndexBuffer: " << i << "\nIndexBufferID: " << m_IndexBuffer[i] << "\nElements: \n";

		for (size_t j = 0; j < ExpandedIndecies[i].size(); j++)
		{
			std::cout << "VertexID: " << ExpandedIndecies[i][j] << '\n';
		}
		std::cout << "\n";

		Materials.push_back(Material());
		if (Shapes[i].mesh.material_ids.size() && Shapes[i].mesh.material_ids[0] != -1)
		{
			Materials[i].Name = MeshMaterials[Shapes[i].mesh.material_ids[0]].name;
			Materials[i].Ambient = {MeshMaterials[Shapes[i].mesh.material_ids[0]].ambient[0], MeshMaterials[Shapes[i].mesh.material_ids[0]].ambient[1], MeshMaterials[Shapes[i].mesh.material_ids[0]].ambient[2]};
			Materials[i].Diffuse = {MeshMaterials[Shapes[i].mesh.material_ids[0]].diffuse[0], MeshMaterials[Shapes[i].mesh.material_ids[0]].diffuse[1], MeshMaterials[Shapes[i].mesh.material_ids[0]].diffuse[2]};
			Materials[i].Specular = {MeshMaterials[Shapes[i].mesh.material_ids[0]].specular[0], MeshMaterials[Shapes[i].mesh.material_ids[0]].specular[1], MeshMaterials[Shapes[i].mesh.material_ids[0]].specular[2]};
			Materials[i].SpecularWeight = MeshMaterials[Shapes[i].mesh.material_ids[0]].shininess;
			Materials[i].Alpha = MeshMaterials[Shapes[i].mesh.material_ids[0]].dissolve;

			Materials[i].IlluminationMode = MeshMaterials[Shapes[i].mesh.material_ids[0]].illum;
		}

		std::cout << "Material " << Materials[i].Name << "\nAmbience {r, g, b}: {" << Materials[i].Ambient.r << ", " << Materials[i].Ambient.g << ", " << Materials[i].Ambient.b << "}\nDiffuse {r, g, b}: {" << Materials[i].Diffuse.r << ", " << Materials[i].Diffuse.g << ", " << Materials[i].Diffuse.b << "}\nSpecular {r, g, b}: {" << Materials[i].Specular.r << ", " << Materials[i].Specular.g << ", " << Materials[i].Specular.b << "}\nSpecularWeight: " << Materials[i].SpecularWeight << "\nAlpha: " << Materials[i].Alpha << "\nIlluminationMode: " << Materials[i].IlluminationMode << "\n\n";
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