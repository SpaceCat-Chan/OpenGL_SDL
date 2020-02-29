#define TINYOBJLOADER_IMPLEMENTATION
#include "Mesh.hpp"

void Mesh::LoadMesh(std::string Filename)
{
	tinyobj::attrib_t MeshAttributes;
	std::vector<tinyobj::shape_t> Shapes;
	std::vector<tinyobj::material_t> MeshMaterials;

	std::string Warn, Error;

	isfstream File(Filename, "r+");

	bool Result = tinyobj::LoadObj(&MeshAttributes, &Shapes, &MeshMaterials, &Warn, &Error, &File);

	if(!Warn.empty()) {
		std::cout << "Warning: " << Warn << '\n';
	}
	if(!Error.empty()) {
		std::cerr << "Error: " << Error << '\n';
	}

	if(Result == false) {
		std::cout << "Failed to load meshfile: " << Filename << '\n';
		return;
	}

	glBindVertexArray(m_VertexArray);
	glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer[0]);


	glBufferData(GL_ARRAY_BUFFER, MeshAttributes.vertices.size() * sizeof(GLfloat), MeshAttributes.vertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, nullptr);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer[1]);
	glBufferData(GL_ARRAY_BUFFER, MeshAttributes.texcoords.size() * sizeof(GLfloat), MeshAttributes.texcoords.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(1, 2, GL_FLOAT, false, 0, nullptr);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer[2]);
	glBufferData(GL_ARRAY_BUFFER, MeshAttributes.normals.size() * sizeof(GLfloat), MeshAttributes.normals.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(2, 3, GL_FLOAT, false, 0, nullptr);
	glEnableVertexAttribArray(2);


	m_IndexBuffer.clear();
	m_IndexAmount.size();
	for(size_t i=0; i < Shapes.size(); i++) {
		m_IndexBuffer.push_back(0);

		std::vector<GLuint> indices;
		for(size_t j=0; j < Shapes[i].mesh.indices.size(); j++) {
			assert(Shapes[i].mesh.indices[j].vertex_index == Shapes[i].mesh.indices[j].texcoord_index == Shapes[i].mesh.indices[j].normal_index);
			indices.push_back(Shapes[i].mesh.indices[j].vertex_index);
		}

		glGenBuffers(1, &m_IndexBuffer[i]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer[i]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
		m_IndexAmount.push_back(indices.size());

		std::cout << "IndexBuffer: " << i << "\nIndexBufferID: " << m_IndexBuffer[i] << "\nElements: \n";

		for(size_t j=0; j<indices.size(); j++) {
			std::cout << "VertexID: " << indices[j] << '\n';
		}
		std::cout << "\n";

		Materials.push_back(Material());
		if(Shapes[i].mesh.material_ids.size())
		Materials[i].Name = MeshMaterials[Shapes[i].mesh.material_ids[0]].name;
		Materials[i].Ambient = {MeshMaterials[Shapes[i].mesh.material_ids[0]].ambient[0], MeshMaterials[Shapes[i].mesh.material_ids[0]].ambient[1], MeshMaterials[Shapes[i].mesh.material_ids[0]].ambient[2]};
		Materials[i].Diffuse = {MeshMaterials[Shapes[i].mesh.material_ids[0]].diffuse[0], MeshMaterials[Shapes[i].mesh.material_ids[0]].diffuse[1], MeshMaterials[Shapes[i].mesh.material_ids[0]].diffuse[2]};
		Materials[i].Specular = {MeshMaterials[Shapes[i].mesh.material_ids[0]].specular[0], MeshMaterials[Shapes[i].mesh.material_ids[0]].specular[1], MeshMaterials[Shapes[i].mesh.material_ids[0]].specular[2]};
		Materials[i].SpecularWeight = MeshMaterials[Shapes[i].mesh.material_ids[0]].shininess;
		Materials[i].Alpha = MeshMaterials[Shapes[i].mesh.material_ids[0]].dissolve;

		Materials[i].IlluminationMode = MeshMaterials[Shapes[i].mesh.material_ids[0]].illum;

		std::cout << "Material " << Materials[i].Name << "\nAmbience {r, g, b}: {" << Materials[i].Ambient.r << ", " << Materials[i].Ambient.g << ", " << Materials[i].Ambient.b << "}\nDiffuse {r, g, b}: {" << Materials[i].Diffuse.r << ", " << Materials[i].Diffuse.g << ", " << Materials[i].Diffuse.b << "}\nSpecular {r, g, b}: {" << Materials[i].Specular.r << ", " << Materials[i].Specular.g << ", " << Materials[i].Specular.b << "}\nSpecularWeight: " << Materials[i].SpecularWeight << "\nAlpha: " << Materials[i].Alpha << "\nIlluminationMode: " << Materials[i].IlluminationMode << "\n\n";
	}


}