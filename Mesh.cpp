#include "Mesh.h"

using namespace glm;
using namespace std;

tinyobj::shape_t frames[5]; // outlines of shields around planet
tinyobj::shape_t faces[40]; // individual faces for each frame
Shader frameShader; // shader for frame
int frameIndex = 0; // index of frame
// 0 = 6 sides, 1 = 8 sides, 2 = 10 sides, 3 = 12 sides
int faceIndex = 0; // index of face

Mesh::Mesh()
{

}

Mesh::Mesh(Shader* shader)
{
	m_shader = shader;
	glGenBuffers(1, &m_VBO); // generate a 'name' for the VBO
	glGenBuffers(1, &m_IBO); // generate a 'name' for the IBO
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO); // Bind VBO.
	glEnableVertexAttribArray((*m_shader)["vertexPosition"]); // Enable vertex attribute.
	glEnableVertexAttribArray((*m_shader)["vertexNormal"]); // Enable normal attribute.
}

Mesh::Mesh(char * filename, Shader *shader)
{
	m_vCount = 0;
	m_iCount = 0;
	m_nCount = 0;
	m_filename = filename;
	m_shader = shader;
	UsingDefault = true;
	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_IBO);

	std::string file(filename);
	if (file.compare(7, 6, "sphere") == 0) {
		tinyobj::LoadObj(shapes, filename);
		float tminX;
		float tmaxX;
		float tminY;
		float tmaxY;
		float tminZ;
		float tmaxZ;
		for (int i = 0; i < shapes.size(); i++) {
			//set min and max x y z values
			float minX = shapes[i].mesh.positions[0];
			float maxX = shapes[i].mesh.positions[0];
			float minY = shapes[i].mesh.positions[1];
			float maxY = shapes[i].mesh.positions[1];
			float minZ = shapes[i].mesh.positions[2];
			float maxZ = shapes[i].mesh.positions[2];

			int counter = 0; // 0 = x, 1 = y, 2 = z
			//loop through each position
			for (int j = 3; j < shapes[i].mesh.positions.size(); j++) {
				float temp = shapes[i].mesh.positions[j]; // tmep position to compare
				// if x is compared
				if (counter == 0) {
					// new max x
					if (temp > maxX) {
						maxX = temp;
					}
					// new min x
					if (temp < minX) {
						minX = temp;
					}
					counter++; // increment counter
				}
				// if y is compared
				else if (counter == 1) {
					// mew max y
					if (temp > maxY) {
						maxY = temp;
					}
					// new min y
					if (temp < minY) {
						minY = temp;
					}
					counter++; // increment counter
				}
				// if z is compared
				else {
					// new max z
					if (temp > maxZ) {
						maxZ = temp;
					}
					// new min z
					if (temp < minZ) {
						minZ = temp;
					}
					counter = 0; // reset counter
				}
			}

			tminX = minX;
			tmaxX = maxX;
			tminY = minY;
			tmaxY = maxY;
			tminZ = minZ;
			tmaxZ = maxZ;

			frames[frameIndex].mesh.normals = { 1.0f, 0.0f, 0.0f }; // set default normals

			// frame 0, cube
			// change distance from center
			tminX -= .2;
			tmaxX += .2;
			tminY -= .2;
			tmaxY += .2;
			tminZ -= .2;
			tmaxZ += .2;

			vec3 vertices1[] = { vec3(tminX, tminY, tminZ), // min, 0
								vec3(tminX, tminY, tmaxZ), // bottom z, 1
								vec3(tmaxX, tminY, tmaxZ), // bottom x & z, 2
								vec3(tmaxX, tminY, tminZ), // bottom x, 3
								vec3(tmaxX, tmaxY, tminZ), // top z, 4
								vec3(tminX, tmaxY, tminZ), // top x & z, 5
								vec3(tminX, tmaxY, tmaxZ), // top x, 6
								vec3(tmaxX, tmaxY, tmaxZ) }; // max, 7

			frames[frameIndex].mesh.positions = { vertices1[0].x, vertices1[0].y, vertices1[0].z, // 0
											vertices1[1].x, vertices1[1].y, vertices1[1].z, // 1
											vertices1[2].x, vertices1[2].y, vertices1[2].z, // 2
											vertices1[3].x, vertices1[3].y, vertices1[3].z, // 3
											vertices1[4].x, vertices1[4].y, vertices1[4].z, // 4
											vertices1[5].x, vertices1[5].y, vertices1[5].z, // 5
											vertices1[6].x, vertices1[6].y, vertices1[6].z, // 6
											vertices1[7].x, vertices1[7].y, vertices1[7].z }; // 7

			// indices for edges in pairs
			frames[frameIndex].mesh.indices = { 0, 1, 1, 2,
										 2, 3, 3, 0,
										 4, 5, 5, 6,
										 6, 7, 7, 4,
										 3, 4, 7, 2,
										 6, 1, 0, 5 };

			// next frame index, diamond
			frameIndex = 1;

			tminX = minX;
			tmaxX = maxX;
			tminY = minY;
			tmaxY = maxY;
			tminZ = minZ;
			tmaxZ = maxZ;

			// change distance from center
			float tmp = (1.0 / 2) * sqrt(3.0);
			tminX -= tmp;
			tmaxX += tmp;
			tminY -= .5;
			tmaxY += .5;
			tminZ -= .5;
			tmaxZ += .5;

			frames[frameIndex].mesh.normals = { 1.0f, 0.0f, 0.0f }; // set default normals

			vec3 vertices2[] = { vec3(0, tminY, tminZ), // bottom v0
								vec3(0, tminY, tmaxZ), // bottom v1
								vec3(0, tmaxY, tmaxZ), // top v2
								vec3(0, tmaxY, tminZ), // top v3
								vec3(tminX, 0, 0), // mid v4
								vec3(tmaxX, 0, 0) // mid v5
								}; 

			frames[frameIndex].mesh.positions = { vertices2[0].x, vertices2[0].y, vertices2[0].z, // 0
											vertices2[1].x, vertices2[1].y, vertices2[1].z, // 1
											vertices2[2].x, vertices2[2].y, vertices2[2].z, // 2
											vertices2[3].x, vertices2[3].y, vertices2[3].z, // 3
											vertices2[4].x, vertices2[4].y, vertices2[4].z, // 4
											vertices2[5].x, vertices2[5].y, vertices2[5].z // 5
												}; 

			// indices for edges in pairs
			frames[frameIndex].mesh.indices = { 0, 1, 1, 2, 2, 3, 3, 0,
												1, 4, 0, 4, 2, 4, 3, 4,
												1, 5, 0, 5, 2, 5, 3, 5,
												};

			// next frame index, jewel
			frameIndex = 2;

			tminX = minX;
			tmaxX = maxX;
			tminY = minY;
			tmaxY = maxY;
			tminZ = minZ;
			tmaxZ = maxZ;

			// change distance from center
			tminX -= .3;
			tmaxX += .3;
			tminY -= .3;
			tmaxY += .3;
			tminZ -= .3;
			tmaxZ += .3;

			frames[frameIndex].mesh.normals = { 1.0f, 0.0f, 0.0f }; // set default normals

			vec3 vertices3[] = {
								vec3(0, tminY, tminZ), // bottom v0
								vec3(0, tminY, tmaxZ), // bottom v1
								vec3(0, tmaxY, tmaxZ), // top v2
								vec3(0, tmaxY, tminZ), // top v3
								vec3(tminX, tminY / 2, tminZ / 2), // midb v4
								vec3(tminX, tminY / 2, tmaxZ / 2), // midb v5
								vec3(tminX, tmaxY / 2, tmaxZ / 2), // midt v6
								vec3(tminX, tmaxY / 2, tminZ / 2), // midt v7
								vec3(tmaxX, tminY / 2, tminZ / 2), // midb v8
								vec3(tmaxX, tminY / 2, tmaxZ / 2), // midb v9
								vec3(tmaxX, tmaxY / 2, tmaxZ / 2), // midt v10
								vec3(tmaxX, tmaxY / 2, tminZ / 2) // midt v11
			};

			frames[frameIndex].mesh.positions = {
											vertices3[0].x, vertices3[0].y, vertices3[0].z, // 0
											vertices3[1].x, vertices3[1].y, vertices3[1].z, // 1
											vertices3[2].x, vertices3[2].y, vertices3[2].z, // 2
											vertices3[3].x, vertices3[3].y, vertices3[3].z, // 3
											vertices3[4].x, vertices3[4].y, vertices3[4].z, // 4
											vertices3[5].x, vertices3[5].y, vertices3[5].z, // 5
											vertices3[6].x, vertices3[6].y, vertices3[6].z, // 6
											vertices3[7].x, vertices3[7].y, vertices3[7].z, // 7
											vertices3[8].x, vertices3[8].y, vertices3[8].z, // 8
											vertices3[9].x, vertices3[9].y, vertices3[9].z, // 9
											vertices3[10].x, vertices3[10].y, vertices3[10].z, // 10
											vertices3[11].x, vertices3[11].y, vertices3[11].z // 11
			};

			// indices for edges in pairs
			frames[frameIndex].mesh.indices = {
												0, 1, 1, 2, 2, 3, 3, 0,
												4, 5, 5, 6, 6, 7, 7, 4,
												8, 9, 9, 10, 10, 11, 11, 8,
												0, 4, 1, 5, 2, 6, 3, 7,
												0, 8, 1, 9, 2, 10, 3, 11
			};

			// next frame index, dodecahedron
			frameIndex = 3;

			tminX = minX;
			tmaxX = maxX;
			tminY = minY;
			tmaxY = maxY;
			tminZ = minZ;
			tmaxZ = maxZ;

			// change distance from center
			tminX += .1;
			tmaxX -= .1;
			tminY += .1;
			tmaxY -= .1;
			tminZ += .1;
			tmaxZ -= .1;
			float dist = tmaxX - tminX;
			tmp = 1.618f;
			

			frames[frameIndex].mesh.normals = { 1.0f, 0.0f, 0.0f }; // set default normals

			vec3 vertices4[] = {
								vec3(tminX, tminY, tminZ), // blf v0
								vec3(tminX, tminY, tmaxZ), // blc v1
								vec3(tmaxX, tminY, tmaxZ), // brc v2
								vec3(tmaxX, tminY, tminZ), // brf v3
								vec3(tmaxX, tmaxY, tminZ), // trf v4
								vec3(tminX, tmaxY, tminZ), // tlf v5
								vec3(tminX, tmaxY, tmaxZ), // tlc v6
								vec3(tmaxX, tmaxY, tmaxZ), // trc v7
								vec3(0, 1 / tmp * tmaxY, tmp * tminZ), // ft v8
								vec3(0, 1 / tmp * tminY, tmp * tminZ), // fb v9
								vec3(0, 1 / tmp * tmaxY, tmp * tmaxZ), // ct v10
								vec3(0, 1 / tmp * tminY, tmp * tmaxZ), // cb v11
								vec3(tmp * tmaxX, 0, 1 / tmp * tminZ), // rf v12
								vec3(tmp * tmaxX, 0, 1 / tmp * tmaxZ), // rc v13
								vec3(tmp * tminX, 0, 1 / tmp * tminZ), // lf v14 
								vec3(tmp * tminX, 0, 1 / tmp * tmaxZ), // lc v15
								vec3(1 / tmp * tmaxX, tmp * tmaxY, 0), // tr v16
								vec3(1 / tmp * tminX, tmp * tmaxY, 0), // tl v17
								vec3(1 / tmp * tmaxX, tmp * tminY, 0), // br v18
								vec3(1 / tmp * tminX, tmp * tminY, 0) // bl v19
			};

			frames[frameIndex].mesh.positions = {
											vertices4[0].x, vertices4[0].y, vertices4[0].z, // 0
											vertices4[1].x, vertices4[1].y, vertices4[1].z, // 1
											vertices4[2].x, vertices4[2].y, vertices4[2].z, // 2
											vertices4[3].x, vertices4[3].y, vertices4[3].z, // 3
											vertices4[4].x, vertices4[4].y, vertices4[4].z, // 4
											vertices4[5].x, vertices4[5].y, vertices4[5].z, // 5
											vertices4[6].x, vertices4[6].y, vertices4[6].z, // 6
											vertices4[7].x, vertices4[7].y, vertices4[7].z, // 7
											vertices4[8].x, vertices4[8].y, vertices4[8].z, // 8
											vertices4[9].x, vertices4[9].y, vertices4[9].z, // 9
											vertices4[10].x, vertices4[10].y, vertices4[10].z, // 10
											vertices4[11].x, vertices4[11].y, vertices4[11].z, // 11
											vertices4[12].x, vertices4[12].y, vertices4[12].z, // 12
											vertices4[13].x, vertices4[13].y, vertices4[13].z, // 13
											vertices4[14].x, vertices4[14].y, vertices4[14].z, // 14
											vertices4[15].x, vertices4[15].y, vertices4[15].z, // 15
											vertices4[16].x, vertices4[16].y, vertices4[16].z, // 16
											vertices4[17].x, vertices4[17].y, vertices4[17].z, // 17
											vertices4[18].x, vertices4[18].y, vertices4[18].z, // 18
											vertices4[19].x, vertices4[19].y, vertices4[19].z // 19
			};

			// indices for edges in pairs
			frames[frameIndex].mesh.indices = {
				8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
				0, 9, 0, 19, 0, 14, 1, 19, 1, 15, 1, 11,
				2, 18, 2, 13, 2, 11, 3, 18, 3, 9, 3, 12,
				4, 8, 4, 12, 4, 16, 5, 8, 5, 17, 5, 14,
				6, 17, 6, 15, 6, 10, 7, 16, 7, 10, 7, 13
			};
		}
	}

	LoadMesh();
}

Mesh::~Mesh(void)
{

}

void Mesh::render(glm::mat4 ModelView, glm::mat4 Projection, int x) {

	// check is bounding box is off and switch on
	if (x >= 0) {
		//green phong shading
		frameShader.InitializeFromFile("shaders/phong3.vert", "shaders/green.frag");
		//frameShader.AddAttribute("vertexPosition");
		//frameShader.AddAttribute("vertexNormal");

		frameShader.Activate(); // activate bound shader

		//update the variables in the shader program
		frameShader.SetUniform("Projection", Projection); // send projection to vertex shader
		frameShader.SetUniform("ModelView", ModelView);  // send modelview to vertex shader
		frameShader.SetUniform("lightPosition", glm::vec4(1.0, 0.0, 0.0, 1.0)); // send light position to vertex shader

		// render bounding box edges at x index
		glBegin(GL_LINES);
		// loop through each indice at index x
		for (int i = 0; i < frames[x].mesh.indices.size(); i++) {
			int index = frames[x].mesh.indices[i];
			glVertex3f(frames[x].mesh.positions[3 * index],
				frames[x].mesh.positions[(3 * index) + 1],
				frames[x].mesh.positions[(3 * index) + 2]);
		}
		glEnd();

		frameShader.DeActivate(); // deactivate bound shader
	}

	m_shader->Activate(); // Bind shader.
	m_shader->SetUniform("Projection", Projection);
	m_shader->SetUniform("ModelView", ModelView);
	//m_shader->SetUniform("lightPosition", glm::vec4(1.0, 0.0, 0.0, 1.0));
	
	glEnableVertexAttribArray((*m_shader)["vertexPosition"]); // Enable vertex attribute.
	glEnableVertexAttribArray((*m_shader)["vertexNormal"]); // Enable normal attribute.

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO); // Bind VBO.
	glVertexAttribPointer((*m_shader)["vertexPosition"], 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0); // Attribute pointer.
	glVertexAttribPointer((*m_shader)["vertexNormal"], 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)12);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO); // Bind IBO.
	glDrawElements(GL_TRIANGLES, Indices.size(), GL_UNSIGNED_INT, 0); // Draw.

	glDisableVertexAttribArray((*m_shader)["vertexPosition"]); // Disable vertex attribute.
	glDisableVertexAttribArray((*m_shader)["vertexNormal"]); // Disable normal attribute.

	m_shader->DeActivate(); // Unbind shader.
}


/*When a new vertex is added, you need to tell the VBO about it.*/
void Mesh::updateVertexBuffer()
{
	// Tell OpenGL which VBO you want to work on right now
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	// Fillthe VBO with vertex data.
	glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(Vertex), &Vertices[0], GL_STATIC_DRAW);
}
void Mesh::updateIndexBuffer() {
	// Bind ibo to the index buffer.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	// Fill index buffer with index data.
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size() * sizeof(GLuint), &Indices[0], GL_STATIC_DRAW);

}

/*
Add an index to the indices list to be rendered referencing the list of vertices
@param index - the indesdx of the vertex to render
*/
void Mesh::addVertexIndex(GLuint index)
{
	Indices.push_back(index);
	updateIndexBuffer();
}

/*
Add a new vertex to the list of vertices
@param v - the vertex to add
@return the index of the new vertex in the vertices array
*/
GLuint Mesh::addNewVertex(struct Vertex& v) {
	Vertices.push_back(v);
	updateVertexBuffer();
	return Vertices.size() - 1;
}



void Mesh::changeNormals()
{
	if(UsingDefault) { // Toggles manually generated normals.
		recalculateVertexNormals();
		UsingDefault = false;
	}

	else { // Toggles normals from file.
		CreateData();
		UsingDefault = true;
	}
}

void Mesh::recalculateVertexNormals(){

	Vertices.clear(); // Clear all data from vertex list.
	Indices.clear(); // Clear all data from index list.
	Vertices.resize(m_vCount);
	Indices.resize(m_iCount);

	// Store position data from file in vertex list.
	for(GLuint i = 0; i < Vertices.size(); i++)
		Vertices[i].Position = position[i];

	// Store position indices in index list.
	for(GLuint i = 0; i < Indices.size(); i++)
		Indices[i] = pIndices[i];

	// Generate normals
	for(GLuint i = 0; i < Indices.size(); i += 3)
	{
		// Indices for triangle
		GLuint index0 = Indices[i];
		GLuint index1 = Indices[i + 1];
		GLuint index2 = Indices[i + 2];

		// Generate the normal for that triangle.
		glm::vec3 v1 = Vertices[index1].Position - Vertices[index0].Position;
		glm::vec3 v2 = Vertices[index2].Position - Vertices[index0].Position;

		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		// Add that normal for each vertex.
		Vertices[index0].Normal += normal;
		Vertices[index1].Normal += normal;
		Vertices[index2].Normal += normal;
	}

	// Loop through all normals and normalize them (same as averaging).
	for(GLuint i = 0; i < m_vCount; i++)
		glm::vec3 temp = Vertices[i].Normal;

	CreateBuffers(); // Refill buffers.
}
void Mesh::reverseNormals(){
	
	// Multiply all normals by -1.
	for(GLuint i = 0; i < Vertices.size(); i++)
		Vertices[i].Normal *= -1;

	// Refill array buffer with new normal data.
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(Vertex), &Vertices[0], GL_STATIC_DRAW);
}

bool Mesh::LoadMesh()
{
	std::ifstream file(m_filename);
	std::vector<std::string*> text;
	int vIndex = 0, fIndex = 0, nIndex = 0;
	char buf[1024];
	float x, y, z;
	float nx = 0, ny = 0, nz = 0;
	int ix = 0, iy = 0, iz = 0, inx = 0, iny = 0, inz = 0;

	// Load obj file.
	if(!file.is_open()) {
		std::cerr << "Unable to open file: " << m_filename << std::endl;
		return false;
	}
	
	// Read contents of file into string list.
	while(!file.eof()) {
		file.getline(buf, 1024);
		text.push_back(new std::string(buf));
	}
	
	// Get vertex counts to initialize arrays at the right size.
	if(!Count(text)) {
		std::cerr << "Error calculating vertex counts." << std::endl;
		return false;
	}

	// Initialize array of vertices and indices.
	position = new glm::vec3[m_vCount];
	normals = new glm::vec3[m_nCount];
	pIndices = new GLuint[m_iCount];
	nIndices = new GLuint[m_iCount];

	for(unsigned int i = 0; i < text.size(); i++)
	{
		// Line is blank, skip.
		if(text[i]->empty())
			continue;

		// Line is a comment, skip.
		if((*text[i])[0] == '#') 
			continue;

		// Line is a vertex.
		else if((sscanf_s(text[i]->c_str(), "v %f %f %f", &x, &y, &z) == 3))
			position[vIndex++] = glm::vec3(x, y, z);
		
		// Line is a normal.
		else if((sscanf_s(text[i]->c_str(), "vn %f %f %f", &nx, &ny, &nz) == 3))
			normals[nIndex++] = glm::vec3(nx, ny, nz);

		// Line is a face.
		else if((sscanf_s(text[i]->c_str(), "f %d//%d %d//%d %d//%d", &ix, &inx, &iy, &iny, &iz, &inz) == 6))
		{
			pIndices[fIndex] = ix - 1;
			pIndices[fIndex + 1] = iy - 1;
			pIndices[fIndex + 2] = iz - 1;

			nIndices[fIndex] = inx - 1;
			nIndices[fIndex + 1] = iny - 1;
			nIndices[fIndex + 2] = inz - 1;
			fIndex += 3;
		}
	}
	
	// delete file strings.
	for(unsigned int i = 0; i < text.size(); i++)
		delete text[i];
	text.clear();

	CreateData(); // Generate vertices and normals using data in the .obj file.

	return true;
}

bool Mesh::CreateData()
{
	Vertices.clear(); // Clear all data from the vertex list.
	Indices.clear(); // Clear all data from the index list
	vMap.clear();  // Clear the previous hash.

	for(GLuint i = 0; i < m_iCount; i++)
	{
		Vertex temp; // temporary vertex
		int pIndex = pIndices[i]; // get index of position for current vertex.
		int nIndex = nIndices[i]; // get index of normal for current vertex.

		GLuint key = pIndex; // initial key is the position index.
		temp.Position = position[pIndex]; //store position in temporary variable.
		temp.Normal = normals[nIndex]; // store normal in temporary variable.

		key += nIndex * 1000; // update key with normal index * the a constant value.

		// is vertex/normal combo unique?
		if(vMap.count(key) == 0)
		{
			// add index for current key (vertex/normal pair).
			vMap[key] = Vertices.size();
			// add new index into index list.
			Indices.push_back(Vertices.size());
			// add new vertex into vertex list.
			Vertices.push_back(temp);
		}

		else
			// pair already exists so just get index for it using key.
			Indices.push_back(vMap[key]);
	}

	CreateBuffers(); // Fill the buffers with new data.

	return true;
}

bool Mesh::CreateBuffers()
{
	// Bind vbo to the array buffer.
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	// Fill array buffer with vertex data.
	glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(Vertex), &Vertices[0], GL_STATIC_DRAW);
	
	// Bind ibo to the index buffer.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	// Fill index buffer with index data.
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size() * sizeof(GLuint), &Indices[0], GL_STATIC_DRAW);

	return true;
}

// Gets vertex, normal, and face counts to dynamically create arrays at the correct size.
bool Mesh::Count(std::vector<std::string*> text)
{	
	GLuint face = 0;

	for(unsigned int i = 0; i < text.size(); i++)
	{
		// Line is blank, skip.
		if(text[i]->empty())
			continue;

		// Line is a comment, skip.
		if((*text[i])[0] == '#')
			continue;
	
		// Line is a vertex, increment count.
		else if((*text[i])[0] == 'v' && ((*text[i])[1] == ' '))
			m_vCount++;
		
		// Line is a normal, increment count.
		else if((*text[i])[0] == 'v' && ((*text[i])[1] == 'n'))
			m_nCount++;
		
		// Line is a face, increment count.
		else if((*text[i])[0] == 'f')
			face++;
	}

	// For each face there are 3 indices.
	m_iCount = face * 3;

	return true;
}