#include <GL/glew.h>
#include <GL/freeglut.h>

//glm library
#include <glm/glm.hpp>
#include <glm/gtx/transform2.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Mesh.h"
#include "Shader.h"
#include "OrbitCamera.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <chrono>
#include <time.h>


using namespace glm;

Shader shader;
Mesh *mesh;
Mesh* mesh2;
Mesh cube;
Mesh torus;
Mesh sphere;
Mesh cylinder;
Mesh plane;
glm::mat4 projection;
glm::mat4 view;
glm::mat4 model;
bool drawTorus = true;
bool spin = false;
float rotation = 0.0f;
glm::vec4 lightPosition;
glm::mat4 dotTrans; // where the model is located wrt the camera

vec4 up = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
float cameraRot = 0;

int frame = 0; // sets frame

OrbitCamera* cameraO;

int lives = 3;
int waves = 0;

vec3 asteroids[3][3];
int maxAst = 0;
int ast = 0;
int astCount = 0;

std::chrono::time_point<std::chrono::steady_clock> startTime;
float elpTime;


/* report GL errors, if any, to stderr */
void checkError(const char *functionName)
{
	GLenum error;
	while (( error = glGetError() ) != GL_NO_ERROR) {
	  std::cerr << "GL error " << error << " detected in " << functionName << std::endl;
	}
}

void initShader(void)
{
	shader.InitializeFromFile("shaders/phong3.vert", "shaders/phong3.frag");
	//shader.AddAttribute("vertexPosition");
	//shader.AddAttribute("vertexNormal");


	checkError ("initShader");
}

void initRendering(void)
{
	glClearColor (0.05f, 0.05f, 0.1f, 0.0f); // Dodger Blue
	checkError ("initRendering");
}

void init(void) 
{	
	
	// Perspective projection matrix.
	projection = glm::perspective(45.0f, 800.0f/600.0f, 1.0f, 1000.0f);

	// View  positioned back -10 on the z axis, looking into the screen.
	glm::vec3 initpos = glm::vec3(0.0f, 0.0f, -15.0f);
	glm::vec3 initlookatpnt = glm::vec3(.0f, .0f, 0.0f);
	// orbit view
	cameraO = new OrbitCamera(initlookatpnt, vec3(0.0f, 1.0f, 0.0f), 15.0f, 5.0f, 0.0f, 0.0f, view, initpos);
	
	frame = -1;
	lives = 3;
	waves = 0;
	astCount = 0;
	lightPosition = glm::vec4(50.0f, 1.0f, 0.0f, 1.0f);

	// Load identity matrix into model matrix.
	model = glm::mat4();

	initShader ();
	initRendering ();
}

/* This defines the triangle(s) in our mesh*/
void createMesh() {
	// draw a square with 2 triangles but only 4 vertices
	mesh2 = new Mesh(&shader);
	struct Mesh::Vertex v1, v2, v3, v4, v5, v6;
	v1.Position = glm::vec3(1, 1.0f, 1.0);
	v2.Position = glm::vec3(2.0f, 2.0f, 2.0);
	v3.Position = glm::vec3(3.0f, 3.0f, 3.0);
	v4.Position = glm::vec3(1.0f, 2.0f, 3.0);
	v5.Position = glm::vec3(3.0f, 2.0f, 1.0);
	v6.Position = glm::vec3(1.0f, 2.0f, 1.0);
	GLuint i_v1 = mesh2->addNewVertex(v1);
	mesh2->addVertexIndex(i_v1);
	GLuint i_v2 = mesh2->addNewVertex(v2);
	mesh2->addVertexIndex(i_v2);
	GLuint i_v3 = mesh2->addNewVertex(v3);
	mesh2->addVertexIndex(i_v3);
	GLuint i_v4 = mesh2->addNewVertex(v4);

	mesh2->addVertexIndex(i_v3);
	mesh2->addVertexIndex(i_v2);
	mesh2->addVertexIndex(i_v4);
	GLuint i_v5 = mesh2->addNewVertex(v5);

	mesh2->addVertexIndex(i_v5);
	mesh2->addVertexIndex(i_v3);
	mesh2->addVertexIndex(i_v4);

	GLuint i_v6 = mesh2->addNewVertex(v6);
	mesh2->addVertexIndex(i_v6);
	mesh2->addVertexIndex(i_v4);
	mesh2->addVertexIndex(i_v2);

}

void dumpInfo(void)
{
	printf ("Vendor: %s\n", glGetString (GL_VENDOR));
	printf ("Renderer: %s\n", glGetString (GL_RENDERER));
	printf ("Version: %s\n", glGetString (GL_VERSION));
	printf ("GLSL: %s\n", glGetString (GL_SHADING_LANGUAGE_VERSION));
	checkError ("dumpInfo");
}


void intro() {
	printf("               Welcom to P.O.D.\n");
	printf("                     #####\n");
	printf("                   ##     ##\n");
	printf("                 ##  \\   /  ##\n");
	printf("                #     \\ /     #\n");
	printf("                #      Y      #\n");
	printf("                 ##    |    ##\n");
	printf("                   ##  |  ##\n");
	printf("                     #####\n");
	printf("          Planetary Orbital Defense\n\n");
	printf("Waves of Asteroids are hurtling towards Earth.\n");
	printf("The defense grid will change shape every wave.\n");
	printf("       Use WASD to orbit around Earth.\n");
	printf("Use SPACE to activate a grip piece temporarily.\n");
	printf("             Use R to start over.\n");
	printf("   Use the scroll wheel to zoom in and out.\n");
	printf("Earth can withstand 2 hits before it explodes.\n\n");
	char blank[2];
	printf("           Press ENTER to begin.");
	fgets(blank, sizeof blank, stdin);
	startTime = std::chrono::steady_clock::now();
}

void gameOver() {
	std::chrono::time_point<std::chrono::steady_clock> curTime = std::chrono::steady_clock::now();
	elpTime = (float)((curTime - startTime).count() / 1000000000.0f);
	printf("GAME OVER\n\n");
	printf("Live = %d\n", lives);
	printf("Waves = %d\n", waves);
	printf("Time = %f\n\n", elpTime);
}

void fillAstroids(int count) {
	srand((unsigned int)time(NULL));
	float a = 15.0;
	for (int i = 0; i < count; i++) {
		float x = ((float)rand() / (float)(RAND_MAX)) * a + 15.0;
		if ((int)x % 2 == 0) {
			x *= -1;
		}
		float y = ((float)rand() / (float)(RAND_MAX)) * a + 15.0;
		if ((int)y % 2 == 0) {
			y *= -1;
		}
		float z = ((float)rand() / (float)(RAND_MAX)) * a + 15.0;
		if ((int)z % 2 == 0) {
			z *= -1;
		}
		asteroids[i][0] = vec3(x, y, z);
		asteroids[i][1] = vec3(abs(x) / (120 * 15), abs(y) / (120 * 15), abs(z) / (120 * 15));
	}
}

void display(void)
{
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// change frame
	if (astCount <= 0) {
		waves += 1;
		frame += 1;
		astCount = frame * frame + waves;
		if (frame > 3)
			frame = 0;
	}

	//Initial camera: positioned at 15 on the z axis, looking into the screen down -z.
	view = glm::lookAt(cameraO->getEye(), vec3(0.0f, 0.0f, 0.0f), cameraO->getUpVector());


	lightPosition = glm::rotate(0.001f, 0.0f, 1.0f, 0.0f) * lightPosition;

	cameraO->zoom(5.0f);
	dotTrans = glm::translate(cameraO->getEye()) * glm::scale(0.02f, 0.02f, 0.02f);
	cameraO->zoom(-5.0f);

	sphere.render(view * dotTrans, projection, -1); // Render the reticle

	srand((unsigned int)time(NULL));
	if (ast == 0) {
		maxAst = rand() % 3 + 1;
		ast = 1;
		fillAstroids(maxAst);
	}

	for (int i = 0; i < maxAst; i++) {
		sphere.render(view * glm::translate(asteroids[i][0]) * glm::scale(0.3f, 0.3f, 0.3f), projection, -1);
		float x = asteroids[i][0].x;
		float y = asteroids[i][0].y;
		float z = asteroids[i][0].z;
		if (x > 0) {
			x -= asteroids[i][1].x;
			if (x < 0) {
				x = 0;
			}
		}
		if (x < 0) {
			x += asteroids[i][1].x;
			if (x > 0) {
				x = 0;
			}
		}
		if (y > 0) {
			y -= asteroids[i][1].y;
			if (y < 0) {
				y = 0;
			}
		}
		if (y < 0) {
			y += asteroids[i][1].y;
			if (y > 0) {
				y = 0;
			}
		}
		if (z > 0) {
			z -= asteroids[i][1].z;
			if (z < 0) {
				z = 0;
			}
		}
		if (z < 0) {
			z += asteroids[i][1].z;
			if (z > 0) {
				z = 0;
			}
		}
		if (x == 0 && y == 0 && z == 0) {
			ast += 1;
		}
		asteroids[i][0] = vec3(x, y, z);
	}
	if (ast == maxAst + 1) {
		ast = 0;
		astCount -= maxAst;
	}
	else {
		ast = 1;
	}
	
	//rotation += 0.5f; // Update rotation angle if rotation is enabled.
	
	model = glm::rotate(0.0f, 0.0f, 1.0f, 0.0f); // rotates the model by the current rotation angle.
	mesh2->render(view * model, projection, -1); // Render current active model.
	
	shader.Activate(); // Bind shader.
	shader.SetUniform("lightPosition", view*lightPosition);
	shader.SetUniform("lightDiffuse", glm::vec4(1.0, 1.0, 1.0, 1.0));
	shader.SetUniform("lightSpecular", glm::vec4(1.0, 1.0, 1.0, 1.0));
	shader.SetUniform("lightAmbient", glm::vec4(0.0, 0.0, 0.0, 1.0));

	shader.SetUniform("surfaceDiffuse", glm::vec4(0.0, 0.1, 1.0, 1.0));
	shader.SetUniform("surfaceSpecular", glm::vec4(1.0, 1.0, 1.0, 1.0));
	shader.SetUniform("surfaceAmbient", glm::vec4(1.0, 0.0, 0.0, 1.0));
	shader.SetUniform("shininess", 90.0f);
	shader.SetUniform("surfaceEmissive", glm::vec4(0.0, 0.0, 0.0, 1.0));


	//draw a shiny red sphere
	mesh->render(view * model,projection, frame); // Render current active model.
	shader.Activate();
	//draw the light!
	shader.SetUniform("surfaceEmissive", glm::vec4(1.0, 1.0, 1.0, 1.0));
	mesh->render(view * glm::translate(lightPosition.x,lightPosition.y, lightPosition.z)*glm::scale(.5f,.5f,.5f), projection, -1);


	shader.DeActivate(); // Unbind shader.

	glutSwapBuffers(); // Swap the buffers.

	checkError ("display");
}

void idle()
{
	glutPostRedisplay();
}


void reshape (int w, int h)
{
	glViewport (0, 0, (GLsizei) w, (GLsizei) h);
	checkError ("reshape");
}

void keyboard(unsigned char key, int x, int y)
{
	
	switch (key) {
	case 27:
		exit(0);
		break;
	case 'r': // reset game
		
		system("cls");
		gameOver();
		init();
		intro();
		break;
	case 'w':
		// orbit up
		cameraO->rotatePolar(0.1f);
		break;

	case 's':
		// orbit down
		cameraO->rotatePolar(-0.1f);
		break;

	case 'a':
		// orbit left
		cameraO->rotateAzimuth(0.1f);
		break;

	case 'd':
		// orbit right
		cameraO->rotateAzimuth(-0.1f);
		break;
   }
}

void mouse(int button, int state, int x, int y)
{
	// Wheel reports as button 3(scroll up) and button 4(scroll down)
	if ((button == 3) || (button == 4)) // It's a wheel event
	{
		// Each wheel event reports like a button click, GLUT_DOWN then GLUT_UP
		if (state == GLUT_UP) return; // Disregard redundant GLUT_UP events
		if (button == 3) {
			cameraO->zoom(0.1);
		}
		else {
			cameraO->zoom(-0.1);
		}
	}
}


int main(int argc, char** argv)
{
	intro();
	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_DOUBLE| GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize (800, 600); 
	glutInitWindowPosition (100, 100);
	glutCreateWindow("Lighting Demo");
	//glutGameModeString("800x600:8");
	//glutEnterGameMode();
	//dumpInfo ();
	glewInit();
	init ();

	glutDisplayFunc(display); 
	glutIdleFunc(idle); 
	glutReshapeFunc(reshape);
	glutKeyboardFunc (keyboard);
	glutMouseFunc(mouse);

	
	glEnable(GL_DEPTH_TEST);

	createMesh(); // make sure to create the mesh before trying to render it!

	sphere = Mesh("models/sphere.obj", &shader);
	torus = Mesh("models/torus.obj", &shader);
	cube = Mesh("models/cube.obj", &shader);
	plane = Mesh("models/plane.obj", &shader);
	cylinder = Mesh("models/cylinder.obj", &shader);
	mesh = &sphere;
	sphere.changeNormals();


   glutMainLoop();

   return 0;
}