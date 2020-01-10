#include "Model_3DS.h"
//#include "TextureBuilder.h"
#include "GLTexture.h"
#include <iostream>
#include <ctime>
#include <vector>
#include <glut.h>
#include <stdio.h>
#include <windows.h>
#include <string.h>
#include <math.h>
using namespace std;

//environment/lane variables
int WIDTH = 1280;
int HEIGHT = 720;
bool mode2 = false, mode1 = true;
char title[] = "3D Model Loader Sample";
//game variables
bool start = false;
bool jumping = false;
int track = 1;
float xoffset = 0;
float camera = -2.0;
float cameraC = 0.1;
float offsetCounter = 0.1;
float limitOffset = 0;
float yJump = 4;
float jumpOffset = 0.0;
float jumpCounter = 1.0;
bool lane1 = true;
bool gameover = false;
// 3D Projection Options
GLdouble fovy = 45.0;
GLdouble aspectRatio = (GLdouble)WIDTH / (GLdouble)HEIGHT;
GLdouble zNear = 0.1;
GLdouble zFar = 1000;

//move arms & legs
int angleArms = 0;
bool upArms = true;
int angleLegs = 0;
bool upLegs = true;

class Vector
{
public:
	GLdouble x, y, z;
	Vector() {}
	Vector(GLdouble _x, GLdouble _y, GLdouble _z) : x(_x), y(_y), z(_z) {}
	//================================================================================================//
	// Operator Overloading; In C++ you can override the behavior of operators for you class objects. //
	// Here we are overloading the += operator to add a given value to all vector coordinates.        //
	//================================================================================================//
	void operator +=(float value)
	{
		x += value;
		y += value;
		z += value;
	}
};

// Camera variables
Vector Eye3rd(0, 5, 20);
Vector At3rd(0, 0, 0);
Vector Up3rd(0, 1, 0);
Vector EyeFps(0, 2.5, 14.5);
Vector AtFps(0, 0, 0);
Vector UpFps(0, 1, 0);

bool firstPerson = true;
int cameraZoom = 0;

// Model Variables
Model_3DS model_house;
Model_3DS model_tree;

// Textures
GLTexture tex_groundMode1;
GLTexture tex_groundMode2;
GLTexture tex_jeans;
GLTexture tex_jeansMode2;
//GLuint tex;
//=======================================================================
// Lighting Configuration Function
//=======================================================================
void InitLightSource()
{
	// Enable Lighting for this OpenGL Program
	glEnable(GL_LIGHTING);

	// Enable Light Source number 0
	// OpengL has 8 light sources

	// Define Light source 0 ambient light
	GLfloat ambient[] = { 0.2f, 0.2f, 0.2f, 0.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);

	//// Define Light source 0 diffuse light
	GLfloat diffuse[] = { 0.7f, 0.7f, 0.7f, 1.0f };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);

	//// Define Light source 0 Specular light
	/*GLfloat specular[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);*/

	//// Finally, define light source 0 position in World Space
	GLfloat light_position[] = { xoffset, 0.6f, -track, 0.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	/*GLfloat ambient[] = { 0.7f, 0.7f, 0.7, 1.0f };
	GLfloat diffuse[] = { 0.6f, 0.6f, 0.6, 1.0f };
	GLfloat specular[] = { 1.0f, 1.0f, 1.0, 1.0f };
	GLfloat shininess[] = { 60 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
	GLfloat light_position[] = { 0.2f, 0.2f, 0.2f, 1.0f };

	GLfloat lightIntensity[] = { 0.7f, 0.7f, 1, 1.0f };

	glLightfv(GL_FRONT, GL_POSITION, light_position);
	glLightfv(GL_FRONT, GL_DIFFUSE, lightIntensity);
	glEnable(GL_LIGHT0);*/

}

//=======================================================================
// Material Configuration Function
//======================================================================
void InitMaterial()
{
	// Enable Material Tracking
	glEnable(GL_COLOR_MATERIAL);

	// Sich will be assigneet Material Properties whd by glColor
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	// Set Material's Specular Color
	// Will be applied to all objects
	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);

	// Set Material's Shine value (0->128)
	GLfloat shininess[] = { 96.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
}

//=======================================================================
// OpengGL Configuration Function
//=======================================================================
void myInit(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);

	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();

	gluPerspective(fovy, aspectRatio, zNear, zFar);
	//*******************************************************************************************//
	// fovy:			Angle between the bottom and top of the projectors, in degrees.			 //
	// aspectRatio:		Ratio of width to height of the clipping plane.							 //
	// zNear and zFar:	Specify the front and back clipping planes distances from camera.		 //
	//*******************************************************************************************//

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
	if (!firstPerson) {
		gluLookAt(Eye3rd.x, Eye3rd.y, Eye3rd.z, At3rd.x, At3rd.y, At3rd.z, Up3rd.x, Up3rd.y, Up3rd.z);
	}
	else {
		gluLookAt(EyeFps.x, EyeFps.y, EyeFps.z, AtFps.x, AtFps.y, AtFps.z, UpFps.x, UpFps.y, UpFps.z);
	}
	//*******************************************************************************************//
	// EYE (ex, ey, ez): defines the location of the camera.									 //
	// AT (ax, ay, az):	 denotes the direction where the camera is aiming at.					 //
	// UP (ux, uy, uz):  denotes the upward orientation of the camera.							 //
	//*******************************************************************************************//


	InitMaterial();

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_NORMALIZE);
}

//=======================================================================
// Render Ground Function
//=======================================================================
void RenderGround()
{
	//glDisable(GL_LIGHTING);	// Disable lighting 

	if (mode1 && !mode2) glColor3f(0.6, 0.6, 0.6);	// Dim the ground texture a bit
	else glColor3f(0.50, 0.72, 0.20);
	glEnable(GL_TEXTURE_2D);	// Enable 2D texturing

	if (mode1 && !mode2) {
		glBindTexture(GL_TEXTURE_2D, tex_groundMode1.texture[0]);	// Bind the ground texture
	}
	else glBindTexture(GL_TEXTURE_2D, tex_groundMode2.texture[0]);
	if (start) {
		track += 1;
		if (mode1) {
			if (track % 7 == 0) {
				int obstaclePos = track;
				if (track % 2 == 0 && !lane1 && track >= obstaclePos && track <= obstaclePos + 1) {
					cout << "here here" << endl;
					start = false;
					gameover = true;
				}
				if (track % 2 != 0 && lane1 && track >= obstaclePos && track <= obstaclePos + 1) {
					cout << "here here 2" << endl;
					start = false;
					gameover = true;
				}


			}

		}
		else {
			if (track % 5 == 0) {
				int obstaclePos = track;
				if (track % 2 == 0 && !lane1 && track >= obstaclePos && track <= obstaclePos + 1) {
					cout << "here" << endl;
					start = false;
					gameover = true;
				}
				if (track % 2 != 0 && lane1 && track >= obstaclePos && track <= obstaclePos + 1) {
					cout << "here2" << endl;
					gameover = true;
					start = false;
				}


			}


		}
	}
	if (track > 70 && !mode2 && mode1) {
		track = 0;
		start = false;
		mode2 = true;
		mode1 = false;

	}
	else {
		if (track > 70 && mode2 && !mode1) {
			cout << "hena ya behyma" << endl;
			gameover = true;
		}
	}



	glPushMatrix();

	//Part 1 of lane 1
	glTranslatef(0.0, 0.0, track);
	glTranslatef(xoffset, 0.0, 0.0);
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);	// Set quad normal direction.
	glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
	glVertex3f(-2, 0, -50);
	glTexCoord2f(0.5, 0);
	glVertex3f(2, 0, -50);
	glTexCoord2f(0.85, 0.85);
	glVertex3f(2, 0, 20);
	glTexCoord2f(0, 0.85);
	glVertex3f(-2, 0, 20);
	glEnd();
	if (mode1 & !mode2) {
		//obstacles
		for (int i = -7; i < 63; i += 7) {
			float x1 = -2;
			float x2 = 2;
			float y1 = 0;
			float y2 = 2;
			float z1 = 0;
			if (i % 2 == 0) {
				x1 += 6;
				x2 += 6;
			}

			//x1 = -2 x2 = 2 y1=0 y2 =2 z1=0 z2 =1
			glBegin(GL_QUADS);
			glNormal3f(0, 1, 0);	// Set quad normal direction.
			glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
			glVertex3f(x1, y1, z1 - i);
			glTexCoord2f(0, 0.85);
			glVertex3f(x2, y1, z1 - i);
			glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
			glVertex3f(x2, y2, z1 - i);
			glTexCoord2f(0.5, 0);
			glTexCoord2f(0, 0.85);
			glVertex3f(x1, y2, z1 - i);
			glEnd();
		}
	}
	else {
		//obstacles
		for (int i = -7; i < 63; i += 5) {
			float x1 = -2;
			float x2 = 2;
			float y1 = 0;
			float y2 = 2;
			float z1 = 0;
			if (i % 2 == 0) {
				x1 += 6;
				x2 += 6;
			}

			//x1 = -2 x2 = 2 y1=0 y2 =2 z1=0 z2 =1
			glBegin(GL_QUADS);
			glNormal3f(0, 1, 0);	// Set quad normal direction.
			glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
			glVertex3f(x1, y1, z1 - i);
			glTexCoord2f(0, 0.85);
			glVertex3f(x2, y1, z1 - i);
			glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
			glVertex3f(x2, y2, z1 - i);
			glTexCoord2f(0.5, 0);
			glTexCoord2f(0, 0.85);
			glVertex3f(x1, y2, z1 - i);
			glEnd();
		}
	}




	//Part 1 of lane 2
	glTranslatef(6.0, 0.0, 0.0);

	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);	// Set quad normal direction.
	glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
	glVertex3f(-2, 0, -50);
	glTexCoord2f(0.85, 0);
	glVertex3f(2, 0, -50);
	glTexCoord2f(0.85, 0.85);
	glVertex3f(2, 0, 20);
	glTexCoord2f(0, 0.85);
	glVertex3f(-2, 0, 20);
	glEnd();

	//Part 2 of lane 1
	glTranslatef(-6.0, 0.0, -70.0);

	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);	// Set quad normal direction.
	glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
	glVertex3f(-2, 0, -50);
	glTexCoord2f(0.85, 0);
	glVertex3f(2, 0, -50);
	glTexCoord2f(0.85, 0.85);
	glVertex3f(2, 0, 20);
	glTexCoord2f(0, 0.85);
	glVertex3f(-2, 0, 20);
	glEnd();

	//Part 2 of lane 2
	glTranslatef(6.0, 0.0, 0.0);

	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);	// Set quad normal direction.
	glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
	glVertex3f(-2, 0, -50);
	glTexCoord2f(0.85, 0);
	glVertex3f(2, 0, -50);
	glTexCoord2f(0.85, 0.85);
	glVertex3f(2, 0, 20);
	glTexCoord2f(0, 0.85);
	glVertex3f(-2, 0, 20);
	glEnd();

	glPopMatrix();


	glEnable(GL_LIGHTING);	// Enable lighting again for other entites coming throung the pipeline.
	glColor3f(1, 1, 1);	// Set material back to white instead of grey used for the ground texture.
	glDisable(GL_TEXTURE_2D);

}


void timerMoveArms(int val)
{
	int x = angleArms;
	if (x == -85) upArms = true;
	if (x < 85 && upArms) {
		angleArms += 5;
	}
	if (x >= 85 || !upArms) {
		upArms = false;
		if (x > -85)
		{
			angleArms -= 5;
		}
	}
	glutPostRedisplay();
	glutTimerFunc(30, timerMoveArms, 1);
}



void timerMoveLegs(int val)
{
	int x = angleLegs;
	if (x == -50) upLegs = true;
	if (x < 50 && upLegs) {
		angleLegs += 5;
	}
	if (x >= 50 || !upLegs) {
		upLegs = false;
		if (x > -50)
		{
			angleLegs -= 5;
		}
	}
	glutPostRedisplay();
	glutTimerFunc(30, timerMoveLegs, 1);
}

void drawMinion()
{
	//glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);	// Enable 2D texturing



	GLUquadricObj* quadratic;
	quadratic = gluNewQuadric();

	GLUquadricObj* quadraticJeans;
	quadraticJeans = gluNewQuadric();
	gluQuadricTexture(quadraticJeans, GL_TRUE);




	glPushMatrix();
	//editing
	glTranslated(0, -10, 0);
	glScaled(400, 150, 400);
	glRotated(-45, 0, 1, 0);
	//CLOTHES
	if (mode1 && !mode2) glBindTexture(GL_TEXTURE_2D, tex_jeans.texture[0]);
	else glBindTexture(GL_TEXTURE_2D, tex_jeansMode2.texture[0]);
	glPushMatrix();
	glTranslated(0, -0.017, 0);
	glRotated(90, 0, 1, 0);
	glRotated(90, 1, 0, 0);
	if (mode1 && !mode2) glColor3f(0.33, 0.33, 0.33);
	else glColor3f(0.45, 0.63, 0.76);
	gluCylinder(quadraticJeans, 0.005, 0.005, 0.02, 300, 300);
	glPopMatrix();
	glPushMatrix();
	glTranslated(0, -0.037, 0);
	gluSphere(quadraticJeans, 0.005, 300, 300);
	glPopMatrix();

	//LEGS
	glPushMatrix();//left
	glTranslated(-0.002, -0.043, 0);
	glRotated(-45, 0, 1, 0);
	glRotated(angleLegs, 0, 0, 1);
	glRotated(90, 1, 0, 0);
	gluCylinder(quadraticJeans, 0.001, 0.001, 0.01, 300, 300);
	glPopMatrix();
	glPushMatrix();//right
	if (mode1 && !mode2) glColor3f(0.33, 0.33, 0.33);
	else glColor3f(0.45, 0.63, 0.76);
	glTranslated(0.0008, -0.043, -0.0017);
	glRotated(-45, 0, 1, 0);
	glRotated(-angleLegs, 0, 0, 1);
	glRotated(90, 1, 0, 0);
	gluCylinder(quadraticJeans, 0.001, 0.001, 0.01, 300, 300);
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
	//HEAD
	glPushMatrix();
	if (mode1 && !mode2) glColor3ub(144, 94, 173);
	else glColor3ub(252, 224, 41);
	gluSphere(quadratic, 0.0005, 30, 30);
	glPopMatrix();
	//FACE
	glPushMatrix();
	glRotated(90, 0, 1, 0);
	glRotated(90, 1, 0, 0);
	gluCylinder(quadratic, 0.005, 0.005, 0.017, 300, 300);
	glPopMatrix();
	//ARMS
	glPushMatrix();//left
	glTranslated(0, -0.012, 0.008);
	glRotated(-45, 0, 1, 0);
	glRotated(-angleArms, 0, 0, 1);
	glRotated(90, 1, 0, 0);
	gluCylinder(quadratic, 0.001, 0.001, 0.01, 300, 300);
	glPopMatrix();
	glPushMatrix();//right
	glTranslated(0.008, -0.012, 0);
	glRotated(-45, 0, 1, 0);
	glRotated(angleArms, 0, 0, 1);
	glRotated(90, 1, 0, 0);
	gluCylinder(quadratic, 0.001, 0.001, 0.01, 300, 300);
	glPopMatrix();
	//GOGGLES
	glPushMatrix();
	if (mode1 && !mode2) glColor3f(0, 0, 0);
	else glColor3ub(10, 117, 188);
	glTranslated(0, -0.0045, 0);
	glRotated(90, 0, 1, 0);
	glRotated(90, 1, 0, 0);
	gluCylinder(quadratic, 0.005005, 0.005005, 0.0025, 300, 300);
	glPopMatrix();
	//HAIR
	glColor3f(0, 0, 0);
	glPushMatrix();
	glBegin(GL_LINES);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0.01, 0);
	glVertex3f(0.002, 0, 0);
	glVertex3f(0.001, 0.01, 0);
	glVertex3f(-0.002, 0, 0);
	glVertex3f(-0.001, 0.01, 0);
	glVertex3f(0, 0, 0.001);
	glVertex3f(0, 0.01, 0.002);
	glVertex3f(0.002, 0, 0);
	glVertex3f(0.002, 0.01, 0);
	glEnd();
	glPopMatrix();

	glPopMatrix();

	glEnable(GL_LIGHTING);
}
//=======================================================================
// Display Function
//=======================================================================
void myDisplay(void)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	/*cout << xoffset;*/
		//animation between lanes
	if ((xoffset < limitOffset && offsetCounter>0) || (xoffset > limitOffset&& offsetCounter < 0)) {
		xoffset += offsetCounter;
	}
	camera += cameraC;
	if (camera > 2) {
		cameraC = -0.1;
	}
	else {
		cameraC = 0.1;

	}
	// Draw Ground
	RenderGround();
	//Draw Character
	glPushMatrix();
	glTranslated(0, 4 + jumpOffset, 15);
	glScaled(0.15, 0.15, 0.15);
	drawMinion();
	glPopMatrix();

	// Draw Tree Model
	//glPushMatrix();
	//glTranslatef(10, 0, 0);
	//glScalef(0.7, 0.7, 0.7);
	//model_tree.Draw();
	//glPopMatrix();

	//// Draw House Model
	//glPushMatrix();
	//glRotatef(90.f, 1, 0, 0);
	//model_house.Draw();
	//glPopMatrix();
//	yJump = 4;

	glPushMatrix();
	GLUquadricObj* qobj;
	qobj = gluNewQuadric();
	glTranslated(50, 0, 0);
	glRotated(90, 1, 0, 1);
	//glBindTexture(GL_TEXTURE_2D, tex);
	gluQuadricTexture(qobj, true);
	gluQuadricNormals(qobj, GL_SMOOTH);
	gluSphere(qobj, 100, 100, 100);
	gluDeleteQuadric(qobj);
	glPopMatrix();
	InitLightSource();

	glutSwapBuffers();

}

//=======================================================================
// Keyboard Function
//=======================================================================
void myKeyboard(unsigned char button, int x, int y)
{
	switch (button)
	{
	case 'u': {
		jumping = true;
	}
			break;

	case 'a': {
		offsetCounter = 0.1;

		//xoffset = 0.28;
		limitOffset = 0.28;
		lane1 = true;
		jumping = true;
	}
			break;

	case 'd':
	{
		limitOffset = -6;
		offsetCounter = -0.1;
		jumping = true;
		//xoffset =-6;
		lane1 = false;
	}
	break;

	case 'h': {
		glLoadIdentity();	//Clear Model_View Matrix
		firstPerson = !firstPerson;
		if (!firstPerson) {
			gluLookAt(Eye3rd.x, Eye3rd.y, Eye3rd.z, At3rd.x, At3rd.y, At3rd.z, Up3rd.x, Up3rd.y, Up3rd.z);
			glutPostRedisplay();
		}
		else {
			gluLookAt(EyeFps.x, EyeFps.y, EyeFps.z, AtFps.x, AtFps.y, AtFps.z, UpFps.x, UpFps.y, UpFps.z);
			glutPostRedisplay();
		}
	}
			break;
	case 'j': {
	}
			break;

	case ' ':
		start = true;
		break;

	case 'w':
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;

	case 'r':
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	case 'n':
		gameover = false;
		track = 0;
		mode2 = false;
		mode1 = true;
		start = false;
		break;

	case 27:
		exit(0);
		break;

	default:
		break;
	}
	glutPostRedisplay();
}

//=======================================================================
// Anim Function
//=======================================================================
void anim() {
	if (jumping) {
		/*cout << "Jumping";*/
		jumpOffset = 3 * jumpCounter - jumpCounter * jumpCounter;
		jumpCounter += 0.07;
		if (jumpOffset < 0) {
			jumping = false;
			jumpOffset = 0.0;
			jumpCounter = 0.0;
		}
	}
}


//=======================================================================
// Motion Function
//=======================================================================
void myMotion(int x, int y)
{
	y = HEIGHT - y;

	if (cameraZoom - y > 0)
	{
		//	Eye.x += -0.1;
		Eye3rd.z += -0.1;
		EyeFps.z += -0.1;
	}
	else
	{
		//	Eye.x += 0.1;
		Eye3rd.z += 0.1;
		EyeFps.z += 0.1;
	}

	cameraZoom = y;

	glLoadIdentity();	//Clear Model_View Matrix
	if (!firstPerson) {
		gluLookAt(Eye3rd.x, Eye3rd.y, Eye3rd.z, At3rd.x, At3rd.y, At3rd.z, Up3rd.x, Up3rd.y, Up3rd.z);	//Setup Camera with modified paramters

	}
	else {
		gluLookAt(EyeFps.x, EyeFps.y, EyeFps.z, AtFps.x, AtFps.y, AtFps.z, UpFps.x, UpFps.y, UpFps.z);
	}

	GLfloat light_position[] = { 0.0f, 10.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glutPostRedisplay();	//Re-draw scene 
}

//=======================================================================
// Mouse Function
//=======================================================================
void myMouse(int button, int state, int x, int y)
{
	y = HEIGHT - y;

	if (state == GLUT_DOWN)
	{
		cameraZoom = y;
	}
}

//=======================================================================
// Reshape Function
//=======================================================================
void myReshape(int w, int h)
{
	if (h == 0) {
		h = 1;
	}

	WIDTH = w;
	HEIGHT = h;

	// set the drawable region of the window
	glViewport(0, 0, w, h);

	// set up the projection matrix 
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fovy, (GLdouble)WIDTH / (GLdouble)HEIGHT, zNear, zFar);

	// go back to modelview matrix so we can move the objects about
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	if (!firstPerson) {
		gluLookAt(Eye3rd.x, Eye3rd.y, Eye3rd.z, At3rd.x, At3rd.y, At3rd.z, Up3rd.x, Up3rd.y, Up3rd.z);

	}
	else {
		gluLookAt(EyeFps.x, EyeFps.y, EyeFps.z, AtFps.x, AtFps.y, AtFps.z, UpFps.x, UpFps.y, UpFps.z);
	}
}

//=======================================================================
// Assets Loading Function
//=======================================================================
void LoadAssets()
{
	// Loading Model files
	//model_house.Load("Models/house/house.3ds");
	//model_tree.Load("Models/tree/tree1.3ds");

	// Loading texture files
	tex_groundMode1.Load("Textures/ground.bmp");
	tex_groundMode2.Load("Textures/grass.bmp");
	tex_jeans.Load("Textures/jeans.bmp");
	tex_jeansMode2.Load("Textures/jeansMode2.bmp");
	/*	loadBMP(&tex, "Textures/sky.bmp", true)*/;

}


//=======================================================================
// Main Function
//=======================================================================
void main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(WIDTH, HEIGHT);

	glutInitWindowSize(1200, 600);
	glutInitWindowPosition(50, 50);

	glutCreateWindow(title);

	glutDisplayFunc(myDisplay);

	glutIdleFunc(anim);

	glutKeyboardFunc(myKeyboard);

	glutMotionFunc(myMotion);

	glutMouseFunc(myMouse);

	glutReshapeFunc(myReshape);

	glutTimerFunc(0, timerMoveArms, 1);

	glutTimerFunc(0, timerMoveLegs, 1);

	myInit();


	LoadAssets();
	glClearColor(1.0, 1.0, 0.0, 0.0);

	glEnable(GL_LIGHT0);
	//glEnable(GL_DEPTH_TEST);
	//glEnable(GL_LIGHTING);

	//glShadeModel(GL_SMOOTH);

	glutMainLoop();
}