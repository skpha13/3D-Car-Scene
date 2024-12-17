#include <windows.h>        //	Utilizarea functiilor de sistem Windows (crearea de ferestre, manipularea fisierelor si directoarelor);
#include <stdlib.h>         //  Biblioteci necesare pentru citirea shaderelor;
#include <stdio.h>
#include <math.h>			//	Biblioteca pentru calcule matematice;
#include <GL/glew.h>        //  Definește prototipurile functiilor OpenGL si constantele necesare pentru programarea OpenGL moderna; 
#include <GL/freeglut.h>    //	Include functii pentru: 
							//	- gestionarea ferestrelor si evenimentelor de tastatura si mouse, 
							//  - desenarea de primitive grafice precum dreptunghiuri, cercuri sau linii, 
							//  - crearea de meniuri si submeniuri;
#include "loadShaders.h"	//	Fisierul care face legatura intre program si shadere;
#include "glm/glm.hpp"		//	Bibloteci utilizate pentru transformari grafice;
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"


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
	codColLocation;

int codCol;
float PI = 3.141592;

// matrices
glm::mat4 modelMatrix, rotationMatrix;

// elements for view matrix
float Refx = 0.0f, Refy = 0.0f, Refz = 0.0f;
float alpha = PI / 8, beta = 0.0f, dist = 400.0f;
float Obsx, Obsy, Obsz;
float Vx = 0.0, Vy = 0.0, Vz = 1.0;
glm::mat4 view;

// elements for projection matrix
float width = 800, height = 600, xwmin = -800.f, xwmax = 800, ywmin = -600, ywmax = 600, znear = 0.1, zfar = 1, fov = 45;
glm::mat4 projection;
// light source
float xL = 500.f, yL = 100.f, zL = 400.f;
// shadow matrix
float shadowMatrix[4][4];

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
	// varfurile 
	GLfloat Vertices[] = 
	{
		// coordonate                   // culori			// normale
		// varfuri "ground"
	   -1500.0f,  -1500.0f, 0.0f, 1.0f,  1.0f, 1.0f, 0.9f,  0.0f, 0.0f, 1.0f,
		1500.0f,  -1500.0f, 0.0f, 1.0f,  1.0f, 1.0f, 0.9f,  0.0f, 0.0f, 1.0f,
		1500.0f,  1500.0f,  0.0f, 1.0f,  1.0f, 1.0f, 0.9f,  0.0f, 0.0f, 1.0f,
	   -1500.0f,  1500.0f,  0.0f, 1.0f,  1.0f, 1.0f, 0.9f,  0.0f, 0.0f, 1.0f,
	   // varfuri cub
		-50.0f,  -50.0f, 50.0f, 1.0f,   1.0f, 0.5f, 0.2f,  -1.0f, -1.0f, -1.0f,
		 50.0f,  -50.0f,  50.0f, 1.0f,  1.0f, 0.5f, 0.2f,  1.0f, -1.0f, -1.0f,
		 50.0f,  50.0f,  50.0f, 1.0f,   1.0f, 0.5f, 0.2f,  1.0f, 1.0f, -1.0f,
		-50.0f,  50.0f, 50.0f, 1.0f,    1.0f, 0.5f, 0.2f,  -1.0f, 1.0f, -1.0f,
		-50.0f,  -50.0f, 150.0f, 1.0f,  1.0f, 0.5f, 0.2f,  -1.0f, -1.0f, 1.0f,
		 50.0f,  -50.0f,  150.0f, 1.0f, 1.0f, 0.5f, 0.2f,  1.0f, -1.0f, 1.0f,
		 50.0f,  50.0f,  150.0f, 1.0f,  1.0f, 0.5f, 0.2f,  1.0f, 1.0f, 1.0f,
		-50.0f,  50.0f, 150.0f, 1.0f,   1.0f, 0.5f, 0.2f,  -1.0f, 1.0f, 1.0f,
		// varfuri con
		 -40.0f, -69.28f, 0.0f, 1.0f,   0.1f, 1.0f, 0.2f, -40.0f, -69.28f, 80.0f,
		 40.0f, -69.28f, 0.0f, 1.0f,    0.1f, 1.0f, 0.2f, 40.0f, -69.28f, 80.0f,
		 80.0f, 0.0f, 0.0f, 1.0f,       0.1f, 1.0f, 0.2f, 80.0f, 0.0f, 80.0f,
		 40.0f, 69.28f, 0.0f, 1.0f,     0.1f, 1.0f, 0.2f, 40.0f, 69.28f, 80.0f,
		-40.0f, 69.28f, 0.0f, 1.0f,     0.1f, 1.0f, 0.2f, -40.0f, 69.28f, 80.0f,
		-80.0f, 0.0f,  0.0f, 1.0f,      0.1f, 1.0f, 0.2f, -80.0f, 0.0f, 80.0f,
		  0.0f, 0.0f, 100.0f, 1.0f,     0.3f, 1.0f, 0.2f, 0.0f, 0.0f, 1.0f,
	};

	// indicii pentru varfuri
	GLubyte Indices[] = 
	{
		// fetele "ground"
		 1, 2, 0,   2, 0, 3,
		 // fetele cubului
		 5, 6, 4,   6, 4, 7,
		 6, 7, 10, 10, 7, 11,
		 11, 7, 8,   8, 7, 4,
		 8, 4, 9,   9, 4, 5,
		 5, 6, 9,   9, 6, 10,
		 9, 10, 8,  8, 10, 11,
		 // fetele conului
		 12, 13, 18,
		 13, 14, 18,
		 14, 15, 18,
		 15, 16, 18,
		 16, 17, 18,
		 17, 12, 18
	};

	glGenVertexArrays(1, &VaoId);
	glGenBuffers(1, &VboId);
	glGenBuffers(1, &EboId);
	glBindVertexArray(VaoId);

	glBindBuffer(GL_ARRAY_BUFFER, VboId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

	// atributul 0 = pozitie
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 10 * sizeof(GLfloat), (GLvoid*)0);
	// atributul 1 = culoare
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(GLfloat), (GLvoid*)(4 * sizeof(GLfloat)));
	// atributul 2 = normale
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(GLfloat), (GLvoid*)(7 * sizeof(GLfloat)));
}


void DestroyVBO(void)
{
	glDisableVertexAttribArray(2);
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
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f); 
	CreateVBO();
	CreateShaders();
	// locatii pentru shader-e
	modelMatrixLocation = glGetUniformLocation(ProgramId, "myMatrix");
	shadowMatrixLocation = glGetUniformLocation(ProgramId, "matrUmbra");
	viewLocation = glGetUniformLocation(ProgramId, "view");
	projLocation = glGetUniformLocation(ProgramId, "projection");
	lightColorLocation = glGetUniformLocation(ProgramId, "lightColor");
	lightPosLocation = glGetUniformLocation(ProgramId, "lightPos");
	viewPosLocation = glGetUniformLocation(ProgramId, "viewPos");
	codColLocation = glGetUniformLocation(ProgramId, "codCol");
}


void RenderFunction(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	
	//pozitia observatorului
	Obsx = Refx + dist * cos(alpha) * cos(beta);
	Obsy = Refy + dist * cos(alpha) * sin(beta);
	Obsz = Refz + dist * sin(alpha);

	// matrice de vizualizare + proiectie
	glm::vec3 Obs = glm::vec3(Obsx, Obsy, Obsz);   // se schimba pozitia observatorului	
	glm::vec3 PctRef = glm::vec3(Refx, Refy, Refz); // pozitia punctului de referinta
	glm::vec3 Vert = glm::vec3(Vx, Vy, Vz); // verticala din planul de vizualizare 
	view = glm::lookAt(Obs, PctRef, Vert);
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);
	projection = glm::infinitePerspective(fov, GLfloat(width) / GLfloat(height), znear);
	glUniformMatrix4fv(projLocation, 1, GL_FALSE, &projection[0][0]);

	// matricea pentru umbra
	float D = -0.5f;
	shadowMatrix[0][0] = zL + D; shadowMatrix[0][1] = 0; shadowMatrix[0][2] = 0; shadowMatrix[0][3] = 0;
	shadowMatrix[1][0] = 0; shadowMatrix[1][1] = zL + D; shadowMatrix[1][2] = 0; shadowMatrix[1][3] = 0;
	shadowMatrix[2][0] = -xL; shadowMatrix[2][1] = -yL; shadowMatrix[2][2] = D; shadowMatrix[2][3] = -1;
	shadowMatrix[3][0] = -D * xL; shadowMatrix[3][1] = -D * yL; shadowMatrix[3][2] = -D * zL; shadowMatrix[3][3] = zL;
	glUniformMatrix4fv(shadowMatrixLocation, 1, GL_FALSE, &shadowMatrix[0][0]);

	// Variabile uniforme pentru iluminare
	glUniform3f(lightColorLocation, 1.0f, 1.0f, 1.0f);
	glUniform3f(lightPosLocation, xL, yL, zL);
	glUniform3f(viewPosLocation, Obsx, Obsy, Obsz);

	// desenare cub
	codCol = 0;
	glUniform1i(codColLocation, codCol);
	modelMatrix = glm::mat4(1.0f);
	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, 0);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, (void*)(6));

	// desenare con
	modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.f, 0.f, 150.0));
	modelMatrixLocation = glGetUniformLocation(ProgramId, "myMatrix");
	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);
	glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_BYTE, (void*)(42));

	// desenare umbra cub
	codCol = 1;
	glUniform1i(codColLocation, codCol);
	modelMatrix = glm::mat4(1.0f);
	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, (void*)(6));

	// desenare umbra con
	modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.f, 0.f, 150.0));
	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);
	glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_BYTE, (void*)(42));

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

