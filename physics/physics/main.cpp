#define GLFW_DLL
#define GLFW_INCLUDE_GLU

#define Length 10
#define Interval 0.25f
#define PI 3.14159265358

#include<stdlib.h>
#include<GL/glut.h>
#include<gl\glu.h>
#include<gl\gl.h>
#include<math.h>
#include<stdio.h>
#include<iostream>
#include<thread>

using std::thread;

float U_time = 0;

bool Grid_State;
float Grid_R;
float Grid_G;
float Grid_B;
bool HeightColor;
float maxHeight;

typedef struct expression
{
	float Cycle;
	float Amplitude;
	float Degree;
	float x, y;
	int type;
}Expression;

Expression list[200];
int num;
float LookDegree;
float LookUp;

void GetData()
{
	maxHeight = 0;
	FILE* asd;
	fopen_s(&asd,"FuncList.txt", "r");
	if (asd == NULL)
	{
		//fclose(asd);
		exit(2);
	}
	fscanf_s(asd, "%d", &num);
	for (int i = 0; i < num; i++)
	{
		fscanf_s(asd, "%d %f %f %f %f %f", &list[i].type, &list[i].Cycle, &list[i].Amplitude, &list[i].Degree, &list[i].x, &list[i].y);
		maxHeight += list[i].Amplitude;
	}
	fclose(asd);
}
float GetZ(float x, float y)
{
	int type;
	float degree, rad;
	float z = 0, X, Y;
	for (int i = 0; i < num; i++)
	{
		type = list[i].type;
		if (type % 2 == 0)
		{
			degree = list[i].Degree;
			rad = degree * PI / 180;
			z += sin(((y + U_time) * cos(rad) + (x + U_time) * sin(rad)) / list[i].Cycle) * list[i].Amplitude;
		}
		else if (type % 2 == 1)
		{
			X = x - list[i].x;
			Y = y - list[i].y;
			z += cos((sqrt(X * X + Y * Y) - U_time) / list[i].Cycle) * list[i].Amplitude;
		}
	}
	return z;
}
void DrawWave1()
{
	float x, y, z, zTemp;
	for (x = -Length; x <= Length; x += Interval)
	{
		y = -Length;
		z = GetZ(x, y);
		zTemp = GetZ(x, y + Interval);
		if (HeightColor)
		{
			float t = GetZ(x, y) + maxHeight;
			glColor3f(t / (maxHeight * 2), 0.2f, 1 - t / (maxHeight * 2));
		}
		glBegin(GL_LINES);
		glVertex3d(x, y, z);
		glVertex3d(x, y + Interval, zTemp);
		glEnd();
		for (y = -Length + Interval; y < Length; y += Interval)
		{
			z = zTemp;
			zTemp = GetZ(x, y + Interval);
			if (HeightColor)
			{
				float t = z + maxHeight;
				glColor3f(t / (maxHeight * 2), 0.2f, 1 - t / (maxHeight * 2));
			}
			glBegin(GL_LINES);
			glVertex3d(x, y, z);
			glVertex3d(x, y + Interval, zTemp);
			glEnd();
		}
	}
	return;
}
void DrawWave2()
{
	float x, y, z, zTemp;
	for (y = -Length; y <= Length; y += Interval)
	{
		x = -Length;
		z = GetZ(x, y);
		zTemp = GetZ(x + Interval, y);
		if (HeightColor)
		{
			float t = z + maxHeight;
			glColor3f(t / (maxHeight * 2), 0.2f, 1 - t / (maxHeight * 2));
		}
		glBegin(GL_LINES);
		glVertex3d(x, y, z);
		glVertex3d(x + Interval, y, zTemp);
		glEnd();
		for (x = -Length + Interval; x < Length; x += Interval)
		{
			z = zTemp;
			zTemp = GetZ(x + Interval, y);
			if (HeightColor)
			{
				float t = z + maxHeight;
				glColor3f(t / (maxHeight * 2), 0.2f, 1 - t / (maxHeight * 2));
			}
			glBegin(GL_LINES);
			glVertex3d(x, y, z);
			glVertex3d(x + Interval, y, zTemp);
			glEnd();
		}
	}
	return;
}
void myDisplay() {
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);             //시점(VIEW) 좌표계를 지금부터 변경시키겠다는 뜻입니다.
	glLoadIdentity();                                     //단위행렬로 초기화시킵니다. [4x4]
	glFrustum(-1, 1, -1, 1, 1, 35);
	float z = LookUp;
	float x = sin(LookDegree * PI / 180) * 15;
	float y = cos(LookDegree * PI / 180) * 15;

	gluLookAt(x, y, z, 0, 0, 0, 0, 0, 1);
	glColor3f(Grid_R, Grid_G, Grid_B);
	if (Grid_State)
	{
		for (int i = -Length; i <= Length; i++)
		{
			glBegin(GL_LINES);
			glVertex3d(-Length, i, 0);
			glVertex3d(Length, i, 0);
			glEnd();
			glBegin(GL_LINES);
			glVertex3d(i, -Length, 0);
			glVertex3d(i, Length, 0);
			glEnd();
		}
	}
	glColor3f(0.5f, 0.5f, 1);
	DrawWave1();
	DrawWave2();
	U_time += 0.05f;
	glutSwapBuffers();

	return;
}
void timer(int value) {
	glutPostRedisplay();      //윈도우를 다시 그리도록 요청
	glutTimerFunc(30, timer, 0); //다음 타이머 이벤트는 30밀리세컨트 후  호출됨.
}

void keyboard(unsigned char key, int x, int y)
{
	if (key == 27)
	{
		exit(0);
	}
	else if (key == (int)'a' || key == (int)'A')
	{
		LookDegree += 1;
	}
	else if (key == (int)'d' || key == (int)'D')
	{
		LookDegree -= 1;
	}
	else if (key == (int)'w' || key == (int)'W')
	{
		LookUp += 0.5f;
		if (LookUp >= 15) LookUp = 15;
	}
	else if (key == (int)'s' || key == (int)'S')
	{
		LookUp -= 0.5f;
		if (LookUp <= -15) LookUp = -15;
	}
}

void Menu(int value)
{
	switch (value)
	{
	case 1:
		break;
	case 2:
		Grid_State = true;
		break;
	case 3:
		Grid_State = false;
		break;
	case 4:
		Grid_R = 0.5f;
		Grid_G = 1;
		Grid_B = 0.5f;
		break;
	case 5:
		Grid_R = 1;
		Grid_G = 1;
		Grid_B = 1;
		break;
	case 6:
		GetData();
		break;
	case 7:
		HeightColor = !HeightColor;
	default:
		break;
	}
	glutPostRedisplay();
}
void CreateMenu()
{
	GLint GridSubMenu = glutCreateMenu(Menu);
	glutAddMenuEntry("Green", 4);
	glutAddMenuEntry("White", 5);

	GLint SubMenu = glutCreateMenu(Menu);
	glutAddMenuEntry("On", 2);
	glutAddMenuEntry("Off", 3);
	glutAddSubMenu("Color", GridSubMenu);

	glutCreateMenu(Menu);
	glutAddMenuEntry("Setting", 1);
	glutAddMenuEntry("Data Refresh", 6);
	glutAddMenuEntry("Height Color", 7);

	glutAddSubMenu("Grid", SubMenu);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}
int main() {
	HeightColor = false;
	Grid_R = 0.5f;
	Grid_G = 1;
	Grid_B = 0.5f;
	Grid_State = true;

	LookDegree = 0;
	LookUp = 15;
	GetData();
	glutInitDisplayMode(GLUT_DOUBLE);
	glutInitWindowSize(1000, 1000);
	glutCreateWindow("gluLookAt");
	CreateMenu();
	glutDisplayFunc(myDisplay);
	glutKeyboardFunc(keyboard);
	glutTimerFunc(0, timer, 0);
	glutMainLoop();
}