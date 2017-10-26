#include "OBJObject.h"
#include "Window.h"
using namespace std;

float Window::xMax = 0;
float Window::xMin = 0;
float Window::yMax = 0;
float Window::yMin = 0;
float Window::zMax = 0;
float Window::zMin = 0;

OBJObject::OBJObject(const char *filepath)
{
	toWorld = glm::mat4(1.0f);
	this->angle = 0.0f;
	parse(filepath);

	// Create array object and buffers. Remember to delete your buffers when the object is destroyed!
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// Bind the Vertex Array Object (VAO) first, then bind the associated buffers to it.
	// Consider the VAO as a container for all your buffers.
	glBindVertexArray(VAO);

	// Now bind a VBO to it as a GL_ARRAY_BUFFER. The GL_ARRAY_BUFFER is an array containing relevant data to what
	// you want to draw, such as vertices, normals, colors, etc.
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// glBufferData populates the most recently bound buffer with data starting at the 3rd argument and ending after
	// the 2nd argument number of indices. How does OpenGL know how long an index spans? Go to glVertexAttribPointer.
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * verticesOBJ.size(), verticesOBJ.data(), GL_STATIC_DRAW);
	// Enable the usage of layout location 0 (check the vertex shader to see what this is)
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,// This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 0. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
		3, // This second line tells us how any components there are per vertex. In this case, it's 3 (we have an x, y, and z component)
		GL_FLOAT, // What type these components are
		GL_FALSE, // GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't
		3 * sizeof(GLfloat), // Offset between consecutive indices. Since each of our vertices have 3 floats, they should have the size of 3 floats in between
		(GLvoid*)0); // Offset of the first vertex's component. In our case it's 0 since we don't pad the vertices array with anything.

					 // We've sent the vertex data over to OpenGL, but there's still something missing.
					 // In what order should it draw those vertices? That's why we'll need a GL_ELEMENT_ARRAY_BUFFER for this.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indicesOBJ.size(), indicesOBJ.data(), GL_STATIC_DRAW);

	// Unbind the currently bound buffer so that we don't accidentally make unwanted changes to it.
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// Unbind the VAO now so we don't accidentally tamper with it.
	// NOTE: You must NEVER unbind the element array buffer associated with a VAO!
	glBindVertexArray(0);
}


OBJObject::~OBJObject()
{
	// Delete previously generated buffers. Note that forgetting to do this can waste GPU memory in a 
	// large project! This could crash the graphics driver due to memory leaks, or slow down application performance!
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

// Copied parser from HW1
void OBJObject::parse(const char *filepath)
{
	// Populate the face indices, vertices, and normals vectors with the OBJ Object data
	FILE *fp;		// file pointer
	glm::vec3 point;

	float x, y, z;	// vertex coordinates
	float r, g, b;	// vertex color
	int c1, c2;		// characters read from file
	unsigned int f1, f2;		// face integers to be read
	int errorCheck;		// checks fscanf_s errors

					// make the file name configurable so you load form other files
	fopen_s(&fp, filepath, "rb");

	// ERROR CHECK: just in case the file can't be found or is corrupt
	if (fp == NULL) { cerr << "error loading file" << endl; exit(-1); }

	while (!feof(fp)) {
		c1 = fgetc(fp);
		c2 = fgetc(fp);

		// Vertex "v" case
		if (c1 == 'v' && c2 == ' ')
		{
			fscanf_s(fp, "%f %f %f %f %f %f", &x, &y, &z, &r, &g, &b);
			verticesOBJ.push_back(glm::vec3(x, y, z));
		}

		// Vertex normal "vn" case
		else if (c1 == 'v' && c2 == 'n')
		{
			fscanf_s(fp, "%f %f %f", &x, &y, &z);
			point = glm::normalize(glm::vec3(x, y, z));
			point.x = (point.x + 1) / 2;
			point.y = (point.y + 1) / 2;
			point.z = (point.z + 1) / 2;
			normalsOBJ.push_back(glm::vec3(x, y, z));
		}

		// NEW: Face "f" case
		// Piazza Post #246 stated that we only read in the vertex indices in face
		else if (c1 == 'f')
		{
			errorCheck = fscanf_s(fp, "%u//%u ", &f1, &f2);
			// If no error, store and get the next two indices
			if (errorCheck == 2)
			{
				indicesOBJ.push_back(f1 - 1);
				fscanf_s(fp, "%u//%u ", &f1, &f2);
				indicesOBJ.push_back(f1 - 1);
				fscanf_s(fp, "%u//%u", &f1, &f2);
				indicesOBJ.push_back(f1 - 1);
			}
		}
	}

	fclose(fp);   // close the file when done
}

void OBJObject::draw(GLuint shaderProgram)
{
	// Calculate the combination of the model and view (camera inverse) matrices
	glm::mat4 modelview = Window::V * toWorld;
	// We need to calculate this because modern OpenGL does not keep track of any matrix other than the viewport (D)
	// Consequently, we need to forward the projection, view, and model matrices to the shader programs
	// Get the location of the uniform variables "projection" and "modelview"
	uProjection = glGetUniformLocation(shaderProgram, "projection");
	uModelview = glGetUniformLocation(shaderProgram, "modelview");
	// Now send these values to the shader program
	glUniformMatrix4fv(uProjection, 1, GL_FALSE, &Window::P[0][0]);
	glUniformMatrix4fv(uModelview, 1, GL_FALSE, &modelview[0][0]);
	// Now draw the cube. We simply need to bind the VAO associated with it.
	glBindVertexArray(VAO);
	// Tell OpenGL to draw with triangles, using 36 indices, the type of the indices, and the offset to start from
	glDrawElements(GL_TRIANGLES, indicesOBJ.size(), GL_UNSIGNED_INT, 0);
	// Unbind the VAO when we're done so we don't accidentally draw extra stuff or tamper with its bound buffers
	glBindVertexArray(0);
}

// Taken from hw 1
void OBJObject::translate(glm::vec3 translateVector)
{
	this->toWorld = glm::translate(glm::mat4(1.0f), translateVector) * this->toWorld;
}

// Taken from hw 1
void OBJObject::scale(glm::vec3 scaleVector)
{
	this->toWorld = this->toWorld * glm::scale(glm::mat4(1.0f), scaleVector);
}

void OBJObject::update()
{
	spin(1.0f);
}

void OBJObject::spin(float deg)
{
	// If you haven't figured it out from the last project, this is how you fix spin's behavior
	toWorld = toWorld * glm::rotate(glm::mat4(1.0f), 1.0f / 180.0f * glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f));
}

void OBJObject::centerScale() 
{
	Window::xMax = verticesOBJ[0].x;
	Window::xMin = verticesOBJ[0].x;
	Window::yMax = verticesOBJ[0].y;
	Window::yMin = verticesOBJ[0].y;
	Window::zMax = verticesOBJ[0].z;
	Window::zMin = verticesOBJ[0].z;

	for (int i = 1; i < verticesOBJ.size(); i++)
	{
		if (verticesOBJ[i].x > Window::xMax) Window::xMax = verticesOBJ[i].x;
		if (verticesOBJ[i].x < Window::xMin) Window::xMin = verticesOBJ[i].x;
		if (verticesOBJ[i].y > Window::yMax) Window::yMax = verticesOBJ[i].y;
		if (verticesOBJ[i].y < Window::yMin) Window::yMin = verticesOBJ[i].y;
		if (verticesOBJ[i].z > Window::zMax) Window::zMax = verticesOBJ[i].z;
		if (verticesOBJ[i].z < Window::zMin) Window::zMin = verticesOBJ[i].z;
	}

	glm::vec3 midPoint(-1 * (Window::xMax + Window::xMin + (2*Window::Translation.x)) / 2,
		-1 * (Window::yMax + Window::yMin + (2 * Window::Translation.y)) / 2,
		-1 * (Window::zMax + Window::zMin + (2 * Window::Translation.z)) / 2);

	translate(midPoint);
	Window::Translation = glm::vec3(0, 0, 0);
}

