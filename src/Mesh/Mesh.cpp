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

struct Index : public tinyobj::index_t {
	int Material;
};

bool operator==(const Index &lhs, const Index &rhs)
{
	return lhs.normal_index == rhs.normal_index && lhs.texcoord_index == rhs.texcoord_index && lhs.vertex_index == rhs.vertex_index && lhs.Material == rhs.Material;
}

void Mesh::LoadMesh(std::string Filename, std::vector<std::string> &DiffuseFile, std::vector<std::string> &SpecularFile)
{
	DiffuseFile.clear();
	SpecularFile.clear();

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

	glBindVertexArray(m_VertexArray);

	m_IndexBuffer.clear();
	m_IndexAmount.size();
	std::vector<std::vector<GLuint>> ExpandedIndexes;
	std::vector<std::vector<Index>> StoredIndexes;

	//this is needed for forcing materials to be per vertex
	for (size_t i=0; i < Shapes.size(); i++) {
		StoredIndexes.emplace_back();
		for(size_t j=0; j < Shapes[i].mesh.num_face_vertices.size(); j++) {
			StoredIndexes[i].emplace_back();
			StoredIndexes[i].emplace_back();
			StoredIndexes[i].emplace_back();
			StoredIndexes[i][j*3].normal_index = Shapes[i].mesh.indices[j*3].normal_index;
			StoredIndexes[i][j*3+1].normal_index = Shapes[i].mesh.indices[j*3+1].normal_index;
			StoredIndexes[i][j*3+2].normal_index = Shapes[i].mesh.indices[j*3+2].normal_index;
			StoredIndexes[i][j*3].texcoord_index = Shapes[i].mesh.indices[j*3].texcoord_index;
			StoredIndexes[i][j*3+1].texcoord_index = Shapes[i].mesh.indices[j*3+1].texcoord_index;
			StoredIndexes[i][j*3+2].texcoord_index = Shapes[i].mesh.indices[j*3+2].texcoord_index;
			StoredIndexes[i][j*3].vertex_index = Shapes[i].mesh.indices[j*3].vertex_index;
			StoredIndexes[i][j*3+1].vertex_index = Shapes[i].mesh.indices[j*3+1].vertex_index;
			StoredIndexes[i][j*3+2].vertex_index = Shapes[i].mesh.indices[j*3+2].vertex_index;

			StoredIndexes[i][j*3].Material = Shapes[i].mesh.material_ids[j];
			StoredIndexes[i][j*3+1].Material = Shapes[i].mesh.material_ids[j];
			StoredIndexes[i][j*3+2].Material = Shapes[i].mesh.material_ids[j];			
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

				if(StoredIndexes[i][j].Material != -1)
				{
					Shininess.push_back(MeshMaterials[StoredIndexes[i][j].Material].shininess);
				}
				else
				{
					Shininess.push_back(1.0);
				}

				ExpandedIndexes[i].push_back((Positions.size()-1)/3);
			}
		}

		glGenBuffers(1, &m_IndexBuffer[i]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer[i]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, ExpandedIndexes[i].size() * sizeof(GLuint), ExpandedIndexes[i].data(), GL_STATIC_DRAW);
		m_IndexAmount.push_back(ExpandedIndexes[i].size());

		DiffuseFile.push_back(MeshMaterials[Shapes[i].mesh.material_ids[0]].diffuse_texname);
		SpecularFile.push_back(MeshMaterials[Shapes[i].mesh.material_ids[0]].specular_texname);
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