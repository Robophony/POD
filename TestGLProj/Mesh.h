#include <GL/glew.h>
#include <glm/glm.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include "tiny_obj_loader.h"
#include <string>
#include "Shader.h"

class Mesh{

/*
struct Vertex 
{
	glm::vec3 Position;
	glm::vec3 Normal;
};
*/

public:
	/*This struct represents a vertex in 3D space*/
	struct Vertex
	{
		Vertex() : Position(0.0f, 0.0f, 0.0f), Normal(0.0f, 0.0f, 1.0f) {} // default values
		glm::vec3 Position; /// x,y,z 
		glm::vec3 Normal; // determines which way the vertex is 'facing'

	};

	Mesh();
	Mesh(Shader* shader); // create an empty mesh
	Mesh(char * filename, Shader *shader);
	~Mesh(void);

	void render(glm::mat4 ModelView, glm::mat4 Projection, int x);
	void recalculateVertexNormals();
	void reverseNormals();
	void changeNormals();
	// void addTriangle(struct Vertex& v1, struct Vertex& v2, struct Vertex& v3); // add a triangle to the mesh
	void addVertexIndex(GLuint index);
	GLuint addNewVertex(struct Vertex& v);

private:
	bool LoadMesh();
	bool Count(std::vector<std::string*> text);
	bool CreateData();
	bool CreateBuffers();

private:
	Shader *m_shader;
	std::vector<Vertex> Vertices;
	std::vector<GLuint> Indices;
	std::map<GLuint, GLuint> vMap;
	std::vector<tinyobj::shape_t> shapes; //a list of meshes and their respective materials

	glm::vec3 *position;
	glm::vec3 *normals;
	GLuint *pIndices;
	GLuint *nIndices;

	char* m_filename;
	GLuint m_vCount;
	GLuint m_iCount;
	GLuint m_nCount;

	GLuint m_VBO;
	GLuint m_IBO;

	bool UsingDefault;


	void updateVertexBuffer(); //initialize your VBO and update when triangles are added
	void updateIndexBuffer(); //initialize your IBO buffer and update when triangles are added
};
