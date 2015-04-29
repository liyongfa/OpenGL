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

float tx = 0.0f;
float ty = 0.0f;
float tz = -17.0f;
float ax = 41.0f;
float ay = 52.0f;
float az = 0.0f;
int mouseX = 0;
int mouseY = 0;
int mouseState = 0;
int width = WINDOW_WIDTH;
int height = WINDOW_HEIGHT;
bool showGrid = true;
bool enableRotate = false;
GLuint tex0 = 0;
GLuint tex1 = 0;
GLuint tex2 = 0;
GLuint bindTex = 0;
bool blendTex = true;
bool loaded = false;
int texMode = GL_MODULATE;
float alpha = 0.5f;

FPS fps;

void SetTo3D(void);
void Reshape(int w, int h);
void Keyboard(unsigned char code, int x, int y);
void Mouse(int button, int state, int x, int y);
void MouseWheel(int button, int dirction, int x, int y);
void Motion(int x, int y);
void LoadTex(void);
void ReleaseTex(void);

void SetupOpenGL(int width, int height)
{
	glShadeModel(GL_SMOOTH); //控制opengl中绘制指定两点间其他点颜色的过渡模式
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (float)width / height, 0.1, 100);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glViewport(0, 0, width, height);

	const char* version = (const char*)glGetString(GL_VERSION);
	printf("opengl version = %s\n", version);

	LoadTex();
}

void SetTo3D(void)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, (float)width / height, 0.1f, 100.0f);
	glMatrixMode(GL_MODELVIEW);
}

void LoadTex(void)
{
	//ReleaseTex();
	static int index = 0;

	if (!loaded)
	{
		Bitmap bitmap;
		bitmap.Load("e:\\images\\texture\\tex0.bmp", false);
		bitmap.ExtendAlpha(128);

		glGenTextures(1, &tex0);
		glBindTexture(GL_TEXTURE_2D, tex0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexEnvi(GL_TEXTURE_2D, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bitmap.biWidth, bitmap.biHeight, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, bitmap.bits);
		//bitmap.SwapChannel(0, 2);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bitmap.biWidth, bitmap.biHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, bitmap.bits);

		bitmap.Load("e:\\images\\texture\\tex1.bmp", false);
		bitmap.ExtendAlpha(200);

		glGenTextures(1, &tex1);
		glBindTexture(GL_TEXTURE_2D, tex1);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexEnvi(GL_TEXTURE_2D, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bitmap.biWidth, bitmap.biHeight, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, bitmap.bits);
		//bitmap.SwapChannel(0, 2);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bitmap.biWidth, bitmap.biHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, bitmap.bits);

		glGenTextures(1, &tex2);
		glBindTexture(GL_TEXTURE_2D, tex2);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexEnvi(GL_TEXTURE_2D, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		bitmap.Alloc(128, 128, 32);
		bitmap.SetColor32(0xff, 0xff, 0xff, 0xff);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bitmap.biWidth, bitmap.biHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, bitmap.bits);

		bindTex = tex0;

		loaded = true;
	}
	else
	{
		switch (index % 3)
		{
		case 0:
			bindTex = tex0;
			break;
		case 1:
			bindTex = tex1;
			break;
		case 2:
			bindTex = tex2;
			break;
		default:
			break;
		}
	}

	index++;
}

void ReleaseTex(void)
{
	if (tex0 != 0)
	{
		glDeleteTextures(1, &tex0);
		tex0 = 0;
	}

	if (tex1 != 0)
	{
		glDeleteTextures(1, &tex1);
		tex1 = 0;
	}

	if (tex2 != 0)
	{
		glDeleteTextures(1, &tex2);
		tex2 = 0;
	}

	bindTex = 0;

	loaded = false;
}

void Display(void)
{
	//printf("rfps=%lf, sfps=%lf, load=%lf\n", fps.GetRenderFPS(), fps.GetSkipedFPS(), fps.GetLoad());
}

void DrawGrid(float size, float step, float y)
{
	glBegin(GL_LINES);

	glColor3f(0.3f, 0.3f, 0.3f);
	for (float i = step; i <= size; i += step)
	{
		glVertex3f(-size, y, i);
		glVertex3f(size, y, i);
		glVertex3f(size, y, -i);
		glVertex3f(-size, y, -i);

		glVertex3f(i, y, size);
		glVertex3f(i, y, -size);
		glVertex3f(-i, y, size);
		glVertex3f(-i, y, -size);
	}

	//X-axis
	glColor3f(0.5f, 0.0f, 0.0f);
	glVertex3f(0, y, size);
	glVertex3f(0, y, -size);

	//Y-axis
	glColor3f(0.0f, 0.5f, 0.5f);
	glVertex3f(-size, y, 0);
	glVertex3f(size, y, 0);

	//Z-axis
	glColor3f(0.5f, 0.5f, 0.0f);
	glVertex3f(0, size / 2, 0);
	glVertex3f(0, -size / 2, 0);

	glEnd();

	//glColor3f(1.0f, 1.0f, 0.0f);
}

void DrawTex(void)
{
	//需要设置一下颜色，否则会收到上一次glColor设置的值影响
	glColor4f(1.0f, 1.0f, 1.0f, alpha);

	if (blendTex)
	{
		glBindTexture(GL_TEXTURE_2D, tex0);
		//如果设置为GL_REPLACE纹理不会受到glColor()所设置的颜色影响
		//如果设置为GL_MODULATE纹理会受到glColor()所设置的颜色影响，
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

		glBegin(GL_QUADS);
		glTexCoord2f(0, 0);	glVertex3f(-5, 0, 5);
		glTexCoord2f(0, 1);	glVertex3f(-5, 0, -5);
		glTexCoord2f(1, 1);	glVertex3f(5, 0, -5);
		glTexCoord2f(1, 0);	glVertex3f(5, 0, 5);
		glEnd();

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//关于blend中的src与dst，跟绘制先后有关系，先绘制的是dst，后绘制的是src
		//本例中tex0就是dst，tex1就是src

 		glBindTexture(GL_TEXTURE_2D, tex1);
		//如果设置为GL_REPLACE纹理不会受到glColor()所设置的颜色影响，包括alpha通道
		//如果设置为GL_MODULATE纹理会受到glColor()所设置的颜色影响，也受纹理本身的alpha影响。dst = color * tex;
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, texMode);

		glBegin(GL_QUADS);
		glTexCoord2f(0, 0);	glVertex3f(-5, 0, 5);
		glTexCoord2f(0, 1);	glVertex3f(-5, 0, -5);
		glTexCoord2f(1, 1);	glVertex3f(5, 0, -5);
		glTexCoord2f(1, 0);	glVertex3f(5, 0, 5);
		glEnd();

		glDisable(GL_BLEND);

		glBindTexture(GL_TEXTURE_2D, 0);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, bindTex);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, texMode);

		glBegin(GL_QUADS);
		glTexCoord2f(0, 0);	glVertex3f(-5, 0, 5);
		glTexCoord2f(0, 1);	glVertex3f(-5, 0, -5);
		glTexCoord2f(1, 1);	glVertex3f(5, 0, -5);
		glTexCoord2f(1, 0);	glVertex3f(5, 0, 5);
		glEnd();
	}
}

void Idle(void)
{
	if (fps.Step() != FPS::stateRender)
	{
		return;
	}

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Load identity matrix
	glLoadIdentity();
	// Multiply in translation matrix
	glTranslatef(tx, ty, tz);
	// Multiply in rotation matrix

	glRotatef(ax, 1, 0, 0);
	glRotatef(ay, 0, 1, 0);
	glRotatef(az, 0, 0, 1);

	// Render colored quad
	if (showGrid)
	{
		glDisable(GL_TEXTURE_2D);

		DrawGrid(10, 1, 0);

		glEnable(GL_TEXTURE_2D);
	}

	if (loaded)
	{
		glEnable(GL_TEXTURE_2D);

// 		glEnable(GL_FOG);
// 		float fogColors[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
// 		glFogf(GL_FOG_START, 0.1f);
// 		glFogf(GL_FOG_END, 100.0f);
// 		glFogfv(GL_FOG_COLOR, fogColors);
// 		glFogi(GL_FOG_MODE, GL_LINEAR);
// 		glHint(GL_FOG_HINT, GL_NICEST);

		DrawTex();

		//glDisable(GL_FOG);
		glDisable(GL_TEXTURE_2D);
	}

	// Swap buffers (color buffers, makes previous render visible)
	glutSwapBuffers();

	if (!enableRotate)
	{
		ay += 1.0f;

		if (ay >= 360.0f) ay = 0.0f;
	}
}

void Reshape(int w, int h)
{
	if (w == width && h == height)
	{
		return;
	}

	if (w == 0) w = 1;
	if (h == 0) h = 1;

	width = w;
	height = h;

	SetTo3D();

	glViewport(0, 0, w, h);
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
	case 'z':
		az += 5.0f;
		if (az > 360.0f) az -= 360.0f;
		break;
	case 'x':
		az -= 5.0f;
		if (az < 0.0f) az += 360.0f;
		break;
	case '-':
		alpha -= 0.01f;
		if (alpha < 0.0f) alpha = 0.0f;
		printf("alpha = %f\n", alpha);
		break;
	case '=':
		alpha += 0.01f;
		if (alpha > 1.0f) alpha = 1.0f;
		printf("alpha = %f\n", alpha);
		break;
	case 'c':
		enableRotate = !enableRotate;
		break;
	case 'g':
		showGrid = !showGrid;
		break;
	case 'r':
		LoadTex();
		break;
	case 'e':
		ReleaseTex();
		break;
	case 'm':
		texMode = (texMode == GL_MODULATE) ? GL_REPLACE : GL_MODULATE;
		printf("tex mode = %s\n", (texMode == GL_MODULATE) ? "GL_MODULATE" : "GL_REPLACE");
		break;
	case '1':
	{
		static bool enable = true;
		if (enable)
		{
			glEnable(GL_DEPTH_TEST);
		}
		else
		{
			glDisable(GL_DEPTH_TEST);
		}
		printf("depth test %s\n", enable ? "enable" : "disable");
		enable = !enable;
		break;
	}
	case '2':
	{
		static bool enable = true;
		if (enable)
		{
			glEnable(GL_CULL_FACE);
			glFrontFace(GL_CW);
			glCullFace(GL_BACK);
		}
		else
		{
			glDisable(GL_CULL_FACE);
		}
		printf("cull face %s\n", enable ? "enable" : "disable");
		enable = !enable;
		break;
	}
	case '3':
		blendTex = !blendTex;
		break;
	case '4':
	{
		static bool enable = true;
		if (enable)
		{
			glEnable(GL_FOG);
			float fogColors[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
			glFogf(GL_FOG_START, 0.1f);
			glFogf(GL_FOG_END, 100.0f);
			glFogfv(GL_FOG_COLOR, fogColors);
			glFogi(GL_FOG_MODE, GL_LINEAR);
			glHint(GL_FOG_HINT, GL_NICEST);
		}
		else
		{
			glDisable(GL_FOG);
		}
		printf("fog %s\n", enable ? "enable" : "disable");
		enable = !enable;
		break;
	}
	case '5':
		ay = 0.0f;
		break;
	case 'q':
		glutLeaveMainLoop();
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
			//printf("rfps=%lf, sfps=%lf, load=%lf\n", fps.GetRenderFPS(), fps.GetSkipedFPS(), fps.GetLoad());
		}
	}
}

void Motion(int x, int y)
{
	if (mouseState == 1)
	{
		ay += (x - mouseX) * 0.2f;
		ax += (y - mouseY) * 0.2f;

		if (ay < 0.0f) ay += 360.0f;
		if (ay > 360.0f) ay -= 360.0f;

		if (ax < 0.0f) ax += 360.0f;
		if (ax > 360.0f) ax -= 360.0f;

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
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_MULTISAMPLE);
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
	ReleaseTex();
	return 0;
}