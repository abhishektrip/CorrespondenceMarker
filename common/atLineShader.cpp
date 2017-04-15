#include "atLineShader.h"
using namespace atRenderEngine::Utility;


atLineShader::atLineShader()
{
	mProgramId = Utility::atLoadShaders("../shaders/SimpleColor.vs", "../shaders/SimpleColor.fs");
}


atLineShader::~atLineShader()
{
}

void atLineShader::Draw(std::vector<Vector3f>& vertexArray, std::vector<Vector3f>& colorArray, Matrix4f & MVPMatrix)
{
	//GLfloat g_vertex_buffer_data[] =
	//{
	//	source.x(), source.y(),source.z(),
	//	target.x(), target.y(),target.z()
	//};
	//// One color for each vertex. They were generated randomly.
	//static const GLfloat g_color_buffer_data[] = {
	//	1.0,0.0,0.0,
	//	0.0,1.0,0.0,
	//};
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);


	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(mProgramId, "MVP");

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vector3f)* vertexArray.size(), vertexArray.data(), GL_STATIC_DRAW);

	GLuint colorbuffer;
	glGenBuffers(1, &colorbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vector3f)* colorArray.size(), colorArray.data(), GL_STATIC_DRAW);

	glUseProgram(mProgramId);

	// Send our transformation to the currently bound shader, 
	// in the "MVP" uniform
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, MVPMatrix.data());

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(
		0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	// 2nd attribute buffer : colors
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glVertexAttribPointer(
		1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);
	glLineWidth(3);
	// Draw the triangle !
	glDrawArrays(GL_LINES, 0, vertexArray.size()); // 12*3 indices starting at 0 -> 12 triangles
	glLineWidth(1);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

}

void atRenderEngine::Utility::atLineShader::DrawRay(RayData & ray, Matrix4f & MVP)
{
	Vector3f source = ray.rayOrigin;
	Vector3f target = ray.rayOrigin + 5 * ray.rayDirection;

	std::vector<Vector3f> vertexBuffer;
	vertexBuffer.push_back(source);
	vertexBuffer.push_back(target);

	std::vector<Vector3f> colorBuffer;
	colorBuffer.push_back(Vector3f(1.0, 0.0, 0.0));
	colorBuffer.push_back(Vector3f(0.0, 1.0, 0.0));

	Draw(vertexBuffer, colorBuffer, MVP);
	//GLfloat g_vertex_buffer_data[] =
	//{
	//	source.x(), source.y(),source.z(),
	//	target.x(), target.y(),target.z()
	//};
	//// One color for each vertex. They were generated randomly.
	//static const GLfloat g_color_buffer_data[] = {
	//	1.0,0.0,0.0,
	//	0.0,1.0,0.0,
	//};


	//GLuint VertexArrayID;
	//glGenVertexArrays(1, &VertexArrayID);
	//glBindVertexArray(VertexArrayID);

	//GLuint programID = Utility::atLoadShaders("../shaders/SimpleColor.vs", "../shaders/SimpleColor.fs");
	//// Get a handle for our "MVP" uniform
	//GLuint MatrixID = glGetUniformLocation(programID, "MVP");

	//GLuint vertexbuffer;
	//glGenBuffers(1, &vertexbuffer);
	//glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	//GLuint colorbuffer;
	//glGenBuffers(1, &colorbuffer);
	//glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);

	//glUseProgram(programID);

	//// Send our transformation to the currently bound shader, 
	//// in the "MVP" uniform
	//glUniformMatrix4fv(MatrixID, 1, GL_FALSE, MVP.data());

	//// 1rst attribute buffer : vertices
	//glEnableVertexAttribArray(0);
	//glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	//glVertexAttribPointer(
	//	0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
	//	3,                  // size
	//	GL_FLOAT,           // type
	//	GL_FALSE,           // normalized?
	//	0,                  // stride
	//	(void*)0            // array buffer offset
	//);

	//// 2nd attribute buffer : colors
	//glEnableVertexAttribArray(1);
	//glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	//glVertexAttribPointer(
	//	1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
	//	3,                                // size
	//	GL_FLOAT,                         // type
	//	GL_FALSE,                         // normalized?
	//	0,                                // stride
	//	(void*)0                          // array buffer offset
	//);

	//// Draw the triangle !
	//glDrawArrays(GL_LINES, 0, 2); // 12*3 indices starting at 0 -> 12 triangles

	//glDisableVertexAttribArray(0);
	//glDisableVertexAttribArray(1);
}
void atLineShader::DrawPoints(std::vector<Vector3f>& vertexArray, std::vector<Vector3f>& colorArray, Matrix4f & MVPMatrix)
{
	//GLfloat g_vertex_buffer_data[] =
	//{
	//	source.x(), source.y(),source.z(),
	//	target.x(), target.y(),target.z()
	//};
	//// One color for each vertex. They were generated randomly.
	//static const GLfloat g_color_buffer_data[] = {
	//	1.0,0.0,0.0,
	//	0.0,1.0,0.0,
	//};
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);


	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(mProgramId, "MVP");

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vector3f)* vertexArray.size(), vertexArray.data(), GL_STATIC_DRAW);

	GLuint colorbuffer;
	glGenBuffers(1, &colorbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vector3f)* colorArray.size(), colorArray.data(), GL_STATIC_DRAW);

	glUseProgram(mProgramId);

	// Send our transformation to the currently bound shader, 
	// in the "MVP" uniform
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, MVPMatrix.data());

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(
		0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	// 2nd attribute buffer : colors
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glVertexAttribPointer(
		1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);
	glPointSize(4);
	// Draw the triangle !
	glDrawArrays(GL_POINTS, 0, vertexArray.size()); // 12*3 indices starting at 0 -> 12 triangles
	glPointSize(1);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

}
