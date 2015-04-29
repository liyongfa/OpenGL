#include <stdio.h>
#include <gl/freeglut.h>
#include <math.h>
#include "common.h"
using namespace e;

#pragma comment(lib, "common.lib")

#define FREEGLUT_PRINT_ERRORS

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define A2R(x) ((float)x/180.0f * 3.141592654f)
#define __TEST_2D__

float tx = 0.0f;
float ty = 0.0f;
float tz = -17.0f;
float ax = 41.0f;
float ay = 52.0f;
float az = 0.0f;
int mouseX = 0;
int mouseY = 0;
int mouseState = 0;
bool perspective = true;
int width = WINDOW_WIDTH;
int height = WINDOW_HEIGHT;
bool stopRotate = false;
FPS fps;

//在一次渲染总正视投影和透视投影可以串行使用
void SetTo2D(void);
void SetTo3D(void);

void SetupOpenGL(int width, int height)
{
	glViewport(0, 0, width, height);
	glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (float)width / height, 0.1, 100);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void Display(void)
{
	//printf("rfps=%lf, sfps=%lf, load=%lf\n", fps.GetRenderFPS(), fps.GetSkipedFPS(), fps.GetLoad());
}

void DrawGrid(float size, float step)
{
	glColor3f(0.3f, 0.3f, 0.3f);

	glBegin(GL_LINES);

	for (float i = step; i <= size; i += step)
	{
		glVertex3f(-size, 0, i);
		glVertex3f( size, 0, i);
		glVertex3f( size, 0, -i);
		glVertex3f(-size, 0, -i);

		glVertex3f(i, 0, size);
		glVertex3f(i, 0, -size);
		glVertex3f(-i, 0, size);
		glVertex3f(-i, 0, -size);
	}

	//X-axis
	glColor3f(0.5f, 0.0f, 0.0f);
	glVertex3f(0, 0, size);
	glVertex3f(0, 0, -size);

	//Y-axis
	glColor3f(0.0f, 0.5f, 0.5f);
	glVertex3f(-size, 0, 0);
	glVertex3f(size, 0, 0);

	//Z-axis
	glColor3f(0.5f, 0.5f, 0.0f);
	glVertex3f(0, size/2, 0);
	glVertex3f(0, -size/2, 0);

	glEnd();
}

void DrawPoints(int step)
{
	static bool flag = false;
	static float dx = (float)(rand() % 12 - 6);
	static float dy = (float)(rand() % 12 - 6);

	int count = 60;
	float a = 360.0f / count;
	float r = 0.0f;

	if (!flag)
	{
		r = 1.0f + step / 30.0f * 5.0f;
	}
	else
	{
		r = 6.0f - step / 30.0f * 5.0f;
	}

	for (int i = 0; i < count; i++)
	{
		float x = r * cos(A2R(a * i));
		float y = r * sin(A2R(a * i));

		glBegin(GL_POINTS);
		
		for (int j = 0; j < 5; j++)
		{
			glColor3f(0.0f, 1.0f, 0.0f);
			glVertex3f(x * (1.0f + j*0.1f) + dx, 0, y * (1.0f + j*0.1f) + dy);
		}
		glEnd();
	}

	if (step == 29)
	{
		flag = !flag;
		if (!flag)
		{
			dx = (float)(rand() % 15 - 6);
			dy = (float)(rand() % 15 - 6);
		}
	}
}

void Idle(void)
{
	if (fps.Step() != FPS::stateRender)
	{
		return;
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Load identity matrix

	glLoadIdentity();
	// Multiply in translation matrix
	glTranslatef(tx, ty, tz);
	// Multiply in rotation matrix

	glRotatef(ax, 1, 0, 0);
	glRotatef(ay, 0, 1, 0);
	glRotatef(az, 0, 0, 1);

#ifdef __TEST_2D__

	SetTo2D();
	glBegin(GL_QUADS);
	glColor3f(1.0, 0.0f, 0.0f);
	glVertex3f(100, 200, 0);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(200, 200, 0);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(200, 100, 0);
	glColor3f(1.0f, 1.0f, 0.0f);
	glVertex3f(100, 100, 0);

	glColor3f(1.0, 0.0f, 0.0f);
	glVertex3f(100, 200, -100);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(200, 200, -100);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(200, 100, -100);
	glColor3f(1.0f, 1.0f, 0.0f);
	glVertex3f(100, 100, -100);
	glEnd();

	if (perspective)
	{
		SetTo3D();
	}

#endif

	// Render colored quad

	if (perspective)
	{
		DrawGrid(10, 1);

		glBegin(GL_QUADS);
		glColor3f(1.0, 0.0f, 0.0f);
		glVertex2f(-1, 1);
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex2f(1, 1);
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex2f(1, -1);
		glColor3f(1.0f, 1.0f, 0.0f);
		glVertex2f(-1, -1);
		glEnd();

		DrawPoints(fps.GetThisCycleFrameCount());
	}
	else
	{
		int x = width < height ? height : width;
		x = x / 60 * 60;
		DrawGrid((float)(x/2), 30);

		glBegin(GL_QUADS);
		glColor3f(1.0, 0.0f, 0.0f);
		glVertex2f(-50, 50);
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex2f(50, 50);
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex2f(50, -50);
		glColor3f(1.0f, 1.0f, 0.0f);
		glVertex2f(-50, -50);
		glEnd();
	}

	// Swap buffers (color buffers, makes previous render visible)
	glutSwapBuffers();

	if (!stopRotate)
	{
		ay += 1.0f;
	}
}

void SetTo2D(void)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-width / 2, width / 2, -height / 2, height / 2, -1000, 1000);
	glMatrixMode(GL_MODELVIEW);
}

void SetTo3D(void)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, (float)width / height, 0.1f, 100.0f);
	glMatrixMode(GL_MODELVIEW);
}

void Reshape(int w, int h)
{
	if (w == width && h == height)
	{
		return;
	}

	width = w;
	height = h;

	if (perspective)
	{
		SetTo3D();
	}
	else
	{
		SetTo2D();
	}

// 	float ovw = 640;
// 	float ovh = 480;
// 	float s0 = ovw / ovh;
// 	float s1 = (float)w / h;
// 
// 	if (s1 > s0)
// 	{
// 		float zoom = w / ovw;
// 		if (zoom >= 0.5f && zoom <= 0.55f)
// 		{
// 			zoom = 0.5f;
// 		}
// 		if (zoom >= 1.0f && zoom <= 1.1f)
// 		{
// 			zoom = 1.0f;
// 		}
// 		if (zoom >= 2.0f && zoom <= 2.2f)
// 		{
// 			zoom = 2.0f;
// 		}
// 		float _w = ovw * zoom;
// 		float _h = ovh * zoom;
// 		float _x = (w - _w) / 2;
// 		float _y = (h - _h) / 2;
// 
// 		glViewport((int)_x, (int)_y, (int)_w, (int)_h);
// 	}
// 	else
// 	{
// 		float zoom = h / ovh;
// 		if (zoom >= 0.5f && zoom <= 0.55f)
// 		{
// 			zoom = 0.5f;
// 		}
// 		if (zoom >= 1.0f && zoom <= 1.1f)
// 		{
// 			zoom = 1.0f;
// 		}
// 		if (zoom >= 2.0f && zoom <= 2.2f)
// 		{
// 			zoom = 2.0f;
// 		}
// 		float _w = ovw * zoom;
// 		float _h = ovh * zoom;
// 		float _x = (w - _w) / 2;
// 		float _y = (h - _h) / 2;
// 
// 		glViewport((int)_x, (int)_y, (int)_w, (int)_h);
// 	}


	glViewport(0, 0, w, h);

	printf("zoom = %f\n", (float)w / h);
}

void Keyboard(unsigned char code, int x, int y)
{
	switch (code)
	{
	case 'a':
		tx -= 1.0f;
		break;
	case 'd':
		tx += 1.0f;
		break;
	case 'w':
		ty += 1.0f;
		break;
	case 's':
		ty -= 1.0f;
		break;
	case 'f':
		tz -= 1.0f;
		break;
	case 'n':
		tz += 1.0f;
		break;
	case 'p':
		perspective = !perspective;

		if (perspective)
		{
			SetTo3D();
		}
		else
		{
			SetTo2D();
		}

		break;
	case 'r':
		tx = 0.0f;
		ty = 0.0f;
		tz = -17.f;
		ax = 41.0f;
		ay = 52.0f;
		az = 0.0f;
		break;
	case 'z':
		az += 5.0f;
		break;
	case 'x':
		az -= 5.0f;
		break;
	case 'c':
		stopRotate = !stopRotate;
		break;
	default:

		break;
	}
}

void Mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			mouseX = x;
			mouseY = y;
			mouseState = 1;
		}
		else
		{
			mouseState = 0;
		}
	}
	else if (button == GLUT_RIGHT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			//printf("(%0.2f, %.02f, %.02f) (%.02f, %.02f, %.02f)\n", tx, ty, tz, ax, ay, az);
			printf("rfps=%lf, sfps=%lf, load=%lf\n", fps.GetRenderFPS(), fps.GetSkipedFPS(), fps.GetLoad());
		}
	}
}

void Motion(int x, int y)
{
	if (mouseState == 1)
	{
		ay += (x - mouseX) * 0.2f;
		ax += (y - mouseY) * 0.2f;

		mouseX = x;
		mouseY = y;
	}
}

void MouseWheel(int button, int dirction, int x, int y)
{
	tz += dirction;
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("opengl example");
	glutDisplayFunc(Display);
	glutIdleFunc(Idle);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	glutMouseWheelFunc(MouseWheel);
	SetupOpenGL(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutMainLoop();

	return 0;
}