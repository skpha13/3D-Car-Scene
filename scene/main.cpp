#include <windows.h>       
#include <stdlib.h>        
#include <stdio.h>
#include <math.h>		
#include <vector>

#include <GL/glew.h>        
#include <GL/freeglut.h>
#include "glm/glm.hpp"	
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "loadShaders.h"
#include "objloader.hpp"


GLuint
	VaoId,
	VboId,
	EboId,
	ProgramId,
	modelMatrixLocation,
	shadowMatrixLocation,
	viewLocation,
	projLocation,
	rotationMatrixLocation,
	lightColorLocation,
	lightPosLocation,
	viewPosLocation,
	codColLocation,
	colorLocation;


GLuint VaoIdGround, VaoIdMiddle, VaoIdOuter, VaoIdBirds, VaoIdCar, VaoIdRacer;
GLuint VboIdGround, VboIdMiddle, VboIdOuter, VboIdBirds, VboIdCar, VboIdRacer;

int codCol;
float PI = 3.141592;

// matrices
glm::mat4 modelMatrix, rotationMatrix;
glm::vec3 color;

// elements for view matrix
float Refx = 0.0f, Refy = 0.0f, Refz = 0.0f;
float alpha = PI / 8, beta = 0.0f, dist = 475.0f;
float Obsx, Obsy, Obsz;
float Vx = 0.0, Vy = 0.0, Vz = 500.0;
glm::mat4 view;

// elements for projection matrix
float width = 800, height = 600, xwmin = -800.f, xwmax = 800, ywmin = -600, ywmax = 600, znear = 0.1, zfar = 1, fov = 45, deltaY = ywmax - ywmin;
glm::mat4 projection;
// light source
float xL = 500.f, yL = 100.f, zL = 400.f;
// shadow matrix
float shadowMatrix[4][4];


std::vector<glm::vec3> verticesGround, verticesMiddle, verticesOuter, verticesBirds, verticesRacer, verticesCar;
std::vector<glm::vec2> uvsGround, uvsMiddle, uvsOuter, uvsBirds, uvsRacer, uvsCar;
std::vector<glm::vec3> normalsGround, normalsMiddle, normalsOuter, normalsBirds, normalsRacer, normalsCar;  


void processNormalKeys(unsigned char key, int x, int y)
{
	switch (key) 
	{
	case 'l':
		Vx -= 0.1;
		break;
	case 'r':
		Vx += 0.1;
		break;
	case '+':
		dist += 5;
		break;
	case '=':
		dist += 5;
		break;
	case '-':
		dist -= 5;
		break;
	}
	if (key == 27)
		exit(0);
}


void processSpecialKeys(int key, int xx, int yy) 
{
	switch (key) 
	{
	case GLUT_KEY_LEFT:
		beta -= 0.01;
		break;
	case GLUT_KEY_RIGHT:
		beta += 0.01;
		break;
	case GLUT_KEY_UP:
		alpha += 0.01;
		break;
	case GLUT_KEY_DOWN:
		alpha -= 0.01;
		break;
	}
}


void CreateVBO(void)
{
	GLfloat Vertices[] = 
	{
		// ground vertices
	   -1500.0f,  -1500.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		1500.0f,  -1500.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		1500.0f,  1500.0f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
	   -1500.0f,  1500.0f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
	};

	// vertices index
	GLubyte Indices[] = 
	{
		 // ground faces
		 1, 2, 0,   2, 0, 3,
	};

	glGenVertexArrays(1, &VaoId);
	glGenBuffers(1, &VboId);
	glGenBuffers(1, &EboId);
	glBindVertexArray(VaoId);

	glBindBuffer(GL_ARRAY_BUFFER, VboId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

	// 0: positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (GLvoid*)0);
	// 1: normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (GLvoid*)(4 * sizeof(GLfloat)));
}


void CreateVboObj(GLuint &VaoIdFunction, GLuint &VboIdFunction, std::vector<glm::vec3> &verticesFunction, std::vector<glm::vec3> &normalsFunction)
{
	glGenVertexArrays(1, &VaoIdFunction);
	glBindVertexArray(VaoIdFunction);
	
	glGenBuffers(1, &VboIdFunction);
	glBindBuffer(GL_ARRAY_BUFFER, VboIdFunction);
	glBufferData(GL_ARRAY_BUFFER, verticesFunction.size() * sizeof(glm::vec3) + normalsFunction.size() * sizeof(glm::vec3), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, verticesFunction.size() * sizeof(glm::vec3), &verticesFunction[0]);
	glBufferSubData(GL_ARRAY_BUFFER, verticesFunction.size() * sizeof(glm::vec3), normalsFunction.size() * sizeof(glm::vec3), &normalsFunction[0]);
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)(verticesFunction.size() * sizeof(glm::vec3)));
}

void CreateVboObjs(void)
{
	// ground.obj
	CreateVboObj(VaoIdGround, VboIdGround, verticesGround, normalsGround);
	
	// middle.obj
	CreateVboObj(VaoIdMiddle, VboIdMiddle, verticesMiddle, normalsMiddle);

	// outer.obj
	CreateVboObj(VaoIdOuter, VboIdOuter, verticesOuter, normalsOuter);

	// birds.obj
	CreateVboObj(VaoIdBirds, VboIdBirds, verticesBirds, normalsBirds);
	
	// car.obj
	CreateVboObj(VaoIdCar, VboIdCar, verticesCar, normalsCar);
	
	// racer.obj
	CreateVboObj(VaoIdRacer, VboIdRacer, verticesRacer, normalsRacer);
}


void DestroyVBO(void)
{
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &VboId);
	glDeleteBuffers(1, &EboId);
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VaoId);
}


void CreateShaders(void)
{
	ProgramId = LoadShaders("shader.vert", "shader.frag");
	glUseProgram(ProgramId);
}


void DestroyShaders(void)
{
	glDeleteProgram(ProgramId);
}


void Initialize(void)
{
	modelMatrix = glm::mat4(1.0f);
	rotationMatrix = glm::rotate(glm::mat4(1.0f), PI / 8, glm::vec3(0.0, 0.0, 1.0));

	loadOBJ("objs/ground.obj", verticesGround, uvsGround, normalsGround);
	loadOBJ("objs/middle.obj", verticesMiddle, uvsMiddle, normalsMiddle);
	loadOBJ("objs/outer.obj", verticesOuter, uvsOuter, normalsOuter);
	loadOBJ("objs/birds.obj", verticesBirds, uvsBirds, normalsBirds);
	loadOBJ("objs/racer.obj", verticesRacer, uvsRacer, normalsRacer);
	loadOBJ("objs/car.obj", verticesCar, uvsCar, normalsCar);
	
	glClearColor(1.0f, 0.85f, 0.75f, 0.0f);

	CreateVBO();
	CreateVboObjs();
	CreateShaders();
	
	// shader locations
	modelMatrixLocation = glGetUniformLocation(ProgramId, "myMatrix");
	shadowMatrixLocation = glGetUniformLocation(ProgramId, "matrUmbra");
	viewLocation = glGetUniformLocation(ProgramId, "view");
	projLocation = glGetUniformLocation(ProgramId, "projection");
	lightColorLocation = glGetUniformLocation(ProgramId, "lightColor");
	lightPosLocation = glGetUniformLocation(ProgramId, "lightPos");
	viewPosLocation = glGetUniformLocation(ProgramId, "viewPos");
	codColLocation = glGetUniformLocation(ProgramId, "codCol");
	colorLocation = glGetUniformLocation(ProgramId, "color");
}


void RenderFunction(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	
	// observer 
	Obsx = Refx + dist * cos(alpha) * cos(beta);
	Obsy = Refy + dist * cos(alpha) * sin(beta);
	Obsz = Refz + dist * sin(alpha);

	// view + projection
	glm::vec3 Obs = glm::vec3(Obsx, Obsy, Obsz);
	glm::vec3 PctRef = glm::vec3(Refx, Refy, Refz);
	glm::vec3 Vert = glm::vec3(Vx, Vy, Vz);
	
	view = glm::lookAt(Obs, PctRef, Vert);
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);
	
	projection = glm::infinitePerspective(fov, GLfloat(width) / GLfloat(height), znear);
	glUniformMatrix4fv(projLocation, 1, GL_FALSE, &projection[0][0]);

	// shadow matrix
	float D = -0.5f;
	shadowMatrix[0][0] = zL + D; shadowMatrix[0][1] = 0; shadowMatrix[0][2] = 0; shadowMatrix[0][3] = 0;
	shadowMatrix[1][0] = 0; shadowMatrix[1][1] = zL + D; shadowMatrix[1][2] = 0; shadowMatrix[1][3] = 0;
	shadowMatrix[2][0] = -xL; shadowMatrix[2][1] = -yL; shadowMatrix[2][2] = D; shadowMatrix[2][3] = -1;
	shadowMatrix[3][0] = -D * xL; shadowMatrix[3][1] = -D * yL; shadowMatrix[3][2] = -D * zL; shadowMatrix[3][3] = zL;
	glUniformMatrix4fv(shadowMatrixLocation, 1, GL_FALSE, &shadowMatrix[0][0]);

	// light variables
	glUniform3f(lightColorLocation, 1.0f, 1.0f, 1.0f);
	glUniform3f(lightPosLocation, xL, yL, zL);
	glUniform3f(viewPosLocation, Obsx, Obsy, Obsz);

	// ground
	glBindVertexArray(VaoId);

	codCol = 0;
	glUniform1i(codColLocation, codCol);
	
	modelMatrix = glm::mat4(1.0f);
	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);
	
	color = glm::vec3(0.5f, 0.5f, 0.5f);
	glUniform3fv(colorLocation, 1, &color[0]);
	
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, 0);

	// ground.obj
	glBindVertexArray(VaoIdGround);

	modelMatrix = glm::mat4(1.0f) * glm::scale(glm::mat4(1.0f), glm::vec3(deltaY, deltaY, deltaY));
	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);

	color = glm::vec3(0.165f, 0.001f, 0.063f);
	glUniform3fv(colorLocation, 1, &color[0]);
	
	glDrawArrays(GL_TRIANGLES, 0, verticesGround.size());

	// middle.obj
	glBindVertexArray(VaoIdMiddle);

	modelMatrix = glm::mat4(1.0f) * glm::scale(glm::mat4(1.0f), glm::vec3(deltaY, deltaY, deltaY));
	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);

	color = glm::vec3(1.0f, 0.037f, 0.091f);
	glUniform3fv(colorLocation, 1, &color[0]);
	
	glDrawArrays(GL_TRIANGLES, 0, verticesMiddle.size());

	// outer.obj
	glBindVertexArray(VaoIdOuter);
	
	modelMatrix = glm::mat4(1.0f) * glm::scale(glm::mat4(1.0f), glm::vec3(deltaY, deltaY, deltaY));
	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);
	
	color = glm::vec3(1.0f, 0.301f, 0.105f);
	glUniform3fv(colorLocation, 1, &color[0]);
	
	glDrawArrays(GL_TRIANGLES, 0, verticesOuter.size());

	// birds.obj
	glBindVertexArray(VaoIdBirds);
	
	modelMatrix = glm::mat4(1.0f) * glm::scale(glm::mat4(1.0f), glm::vec3(deltaY, deltaY, deltaY));
	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);
	
	color = glm::vec3(0.018f, 0.000392f, 0.008531f);
	glUniform3fv(colorLocation, 1, &color[0]);
	
	glDrawArrays(GL_TRIANGLES, 0, verticesBirds.size());

	// car.obj
	glBindVertexArray(VaoIdCar);
	
	modelMatrix = glm::mat4(1.0f) * glm::scale(glm::mat4(1.0f), glm::vec3(deltaY, deltaY, deltaY));
	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);
	
	color = glm::vec3(0.018f, 0.000392f, 0.008531f);
	glUniform3fv(colorLocation, 1, &color[0]);
	
	glDrawArrays(GL_TRIANGLES, 0, verticesCar.size());

	// racer.obj
	glBindVertexArray(VaoIdRacer);
	
	modelMatrix = glm::mat4(1.0f) * glm::scale(glm::mat4(1.0f), glm::vec3(deltaY, deltaY, deltaY));
	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);
	
	color = glm::vec3(1.0f, 0.301f, 0.105f);
	glUniform3fv(colorLocation, 1, &color[0]);
	
	glDrawArrays(GL_TRIANGLES, 0, verticesRacer.size());

	glutSwapBuffers();
	glFlush();
}


void Cleanup(void)
{
	DestroyShaders();
	DestroyVBO();
}


int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(1200, 900);
	glutCreateWindow("3D Car Scene");
	
	glewInit();
	Initialize();
	
	glutIdleFunc(RenderFunction);
	glutDisplayFunc(RenderFunction);
	glutKeyboardFunc(processNormalKeys);
	glutSpecialFunc(processSpecialKeys);
	glutCloseFunc(Cleanup);
	
	glutMainLoop();
}

