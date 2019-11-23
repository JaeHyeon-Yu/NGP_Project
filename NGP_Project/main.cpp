#pragma comment(lib,"ws2_32")
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <GL/freeglut.h>
#include <WinSock2.h>
#include <iostream>
#include <string>
#include <cmath>
#include <stdlib.h>
#include <time.h>
#include "main.h"
#include "tower.h"
#include "globals.h"

//엄 그러니까 지금 openGL좌표계로 그릴 수 밖에 없는 이유는 단위를 따로 정하지 않아서인데,
//단위를 어떤 것을 써야할지가 문제인데, 이게 좌표계 변환을 안 하다보니 생긴 일 인것 같다.
//그러나 역시 골때리게도 그리기 위해선 -1.0에서 1.0사이의 값을 제시하게 된다.
//이걸 어떻게 변환해먹어줘야 하냐

//일단 월드의 크기가 어느 정도인지를 정해주고
//그 다음에 타워의 크기와 공의 크기를 월드 좌표계 기준으로 정해주고
//그 다음에 그리는게 문제네 이거참;;


// -1~0 1P, 0~1 2
#define DISTANCE_OF_FLOOR 2

int level = 1;
int Game_state = TITLE_STATE;	//0 = 타이틀 1 = 고르기 2 = 줌인 3 = 게임 4 = 패배 5 = 승리
Tower tower;
mouse_ first_click;
mouse_ moved_mouse;
bool drag = false;
double zoom = 0;
int timer = 0;
int rotate_timer = 0;

WSADATA wsa;
SOCKET sock;

GLubyte *pBytes;
BITMAPINFO *info;
GLuint textures[2];

bool bmp_load = false;

void InitConnect(WSADATA wsa, SOCKET sock, int GameState);

// For Network
Tower g_towers[MAX_USERS];
int g_myIdx = NULL;
// Ball g_balls[MAX_USERS]

//소켓 함수 오류 출력 후 종료
void err_quit(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

//소켓 함수 오류 출력
void err_display(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	std::cout << "[" << msg << "] " << (char*)lpMsgBuf << std::endl;
	LocalFree(lpMsgBuf);
}


void main(int argc, char *argv[])
{
	//InitConnect(wsa, sock, Game_state);
	//초기화 함수들
	// tower.Initialize(EASY_1);
	// Test Code
	for (int i = 0; i < MAX_USERS; ++i)
		g_towers[i].Initialize(HARD_2);
	Sound_SetUp();
	Play_Sound();
	srand(time(NULL));
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH); // 디스플레이 모드 설정	// 더블버퍼링 && 3차원으로 그린다.
	glutInitWindowPosition(100, 100); // 윈도우의 위치지정
	glutInitWindowSize(1600, 800); // 윈도우의 크기 지정
	glutCreateWindow("Spyral Helix"); // 윈도우 생성 (윈도우 이름)
	glutDisplayFunc(drawScene); // 출력 함수의 지정
	glutReshapeFunc(Reshape); // 다시 그리기 함수의 지정
	glutMouseFunc(Mouse);	// 마우스 입력
	glutMotionFunc(Motion);
	glutKeyboardFunc(Keyboard);		// 키보드 입력
	glutTimerFunc(100/3, Timerfunction, 1);
	glutMainLoop();
}
// 윈도우 출력 함수
GLvoid drawScene(GLvoid)
{
	//main이 아니라 여기서 다 한다. 그리기나 연산 모두.
	//Game_state에 따라서 그려지는게 달라지는 형식.
	//여기선 단순히 그려지는게 달라지지만 서버가 들어감에 따라서 주고받는 데이터가 바뀔 필요성이 있다.
	//데이터 전송이 필요하기 때문에 Game_state함수는 다른 함수에서 따로 관리할 필요가 있다.
	//반드시 데이터 전송->랜더링 순이어야 하기 때문에 순서제어가 필요하다.
	//반드시 쓰레드로 나눌 필요가 있는가?
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // 바탕색 깜장색! 깜장색!
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 설정된 색으로 전체를 칠하기, 깊이버퍼 클리어

	if(!bmp_load)	
		Init_Texture(pBytes, info, textures);
	bmp_load = true;

	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	GLfloat specref[] = { 1.0f,1.0f,1.0f,1.0f };

	glMaterialfv(GL_FRONT, GL_SPECULAR, specref);
	glMateriali(GL_FRONT, GL_SHININESS, 64);

	glEnable(GL_AUTO_NORMAL);

	if (Game_state == TITLE_STATE)
		Draw_Title();
	// else if (Game_state == STANDBY_STATE)
	// 	InitConnect(wsa, sock, Game_state);
	else
	{
		//여기도 -1.0과 1.0사이의 좌표계를 이용한다.
		Draw_Background();
		glPushMatrix();
		{
			glTranslated(0, 0.5, -1);
			glRotated(30, 1, 0, 0);
			// 1인용일때는 상관이 없는데 2인용으로 양쪽에 그려지게끔 하니까 좀 이상하게 그려진다.
			// 일단 주석처리하고 나중에 생각해야지

			
			if (g_towers[g_myIdx].Get_ball_camera_follow() == true)
			{
				glTranslated(0, -g_towers[g_myIdx].Get_ball_y(), 0);
			}
			else
			{
				//????? 이거 뭐야
				//카메라 따라가는 거다.
				glTranslated(0, -g_towers[g_myIdx].Get_ball_floor()*-DISTANCE_OF_FLOOR, 0);
			}
			if (Game_state == SELECT_STATE || Game_state == ZOOM_IN_STATE)
			{
				//확대하는 건 어떻게 처리해야 할까....
				glRotated(rotate_timer - zoom * rotate_timer/20, 0, 1, 0);
				glRotated(15 - zoom*15/20, 0, 0, 1);
				glTranslated(0, 1.5 - zoom * 0.075, 0);
				glScaled(0.2 + zoom * 0.04, 0.05 + zoom * 0.0475, 0.2 + zoom * 0.04);
			}
			
			g_towers[0].SetXpos(-2.f);
			g_towers[1].SetXpos(2.f);

			for (int i = 0; i < MAX_USERS; ++i) {
				Control_light();
				g_towers[i].Draw_Tower();
			}
		}
		glPopMatrix();

		Draw_Level();
		if (Game_state == DEFEAT_STATE)
		{
			Draw_defeat_state();
		}
		else if (Game_state == WIN_STATE)
		{
			Draw_win_state();
		}
		else if (Game_state == SELECT_STATE)
		{
			Draw_select_state();
		}
	}
	glutSwapBuffers();
}

GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(60.0, w / h, 1, 200.0);
	glTranslated(0.0, -1.0, -3);

	glEnable(GL_TEXTURE_2D);
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_DEPTH_TEST);	// 은면제거!
	glEnable(GL_AUTO_NORMAL);
	glLoadIdentity();
}

void Timerfunction(int value)
{
	if (Game_state == TITLE_STATE) //타이틀 상태는 시간이 지나면 전환된다.
	{
		timer++;
		if (timer == 20)
		{
			Game_state = STANDBY_STATE;
			timer = 0;
		}
	}
	if (Game_state == ZOOM_IN_STATE) //타이머 변수 대신에 zoom변수를 이용해 확대시킨 후 메인 상태로 변경.
	{
		zoom += 1;
		if (zoom > 19)
		{
			Game_state = MAIN_STATE;
			zoom = 0;
		}
	}

	if (Game_state == SELECT_STATE) //선택 창에서 전체 맵을 회전시키기 위해 rotate_timer를 사용했다.
	{
		rotate_timer += 3;
		rotate_timer = rotate_timer % 360;
	}

	if (Game_state == MAIN_STATE) //메인 상태에선 타워의 각도를 갱신해준다.
	{
		tower.Update();
	}

	glutPostRedisplay();	// 화면 재출력
	glutTimerFunc(100/3, Timerfunction, 1);
}

void Keyboard(unsigned char key, int x, int y)
{
	//여기선 Game_state를 바꿀 수 있지만, 어차피 서버에서 관리할 것이기에 여기도 제거한다.
	//잠깐 근데 그렇게 되면 서버로 전송해야할 변수가 생각보다 많아진다!
	switch (key)
	{
	case '1':
		if (Game_state == SELECT_STATE)
		{
			level = EASY_1;
			tower.Initialize(EASY_1);
		}
		break;
	case'2':
		if (Game_state == SELECT_STATE)
		{
			level = EASY_2;
			tower.Initialize(EASY_2);
		}
		break;
	case'3':
		if (Game_state == SELECT_STATE)
		{
			level = NORMAL_1;
			tower.Initialize(NORMAL_1);
		}
		break;
	case'4':
		if (Game_state == SELECT_STATE)
		{
			level = NORMAL_2;
			tower.Initialize(NORMAL_2);
		}
		break;
	case'5':
		if (Game_state == SELECT_STATE)
		{
			level = HARD_1;
			tower.Initialize(HARD_1);
		}
		break;
	case'6':
		if (Game_state == SELECT_STATE)
		{
			level = HARD_2;
			tower.Initialize(HARD_2);
		}
		break;
	case's':
		if (Game_state == SELECT_STATE)
		{
			Game_state = ZOOM_IN_STATE; //이 때 zoom변수를 이용한다. 확대에 사용.
		}
		break;
	case'x':
		if (Game_state == DEFEAT_STATE || Game_state == WIN_STATE || Game_state == MAIN_STATE)
		{
			Game_state = SELECT_STATE;
			tower.Initialize(level);
		}
		break;
	case'r':
		if (Game_state == DEFEAT_STATE || Game_state == WIN_STATE || Game_state == MAIN_STATE)
		{
			tower.Initialize(level);
			Game_state = ZOOM_IN_STATE;
		}
		break;
	case'p':
		tower.Power_overwhelming();
		break;
	}
	glutPostRedisplay();	// 화면 재출력!
}

void Mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		drag = true;
		first_click.x = x;
	}

	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		drag = false;
		tower.Fix_degree();
	}
}

void Motion(int xp, int yp)
{
	moved_mouse.x = xp;

	if (drag == true)
	{
		int moving = moved_mouse.x - first_click.x;
		g_towers[g_myIdx].Rotate_by_mouse(-moving);
	}

}

void Control_light()
{
	GLfloat AmbientLight[] = { 0.2f,  0.2f,  0.2f,  1.0f };
	GLfloat DiffuseLight[] = { 1.0, 1 , 1, 1.0 };
	GLfloat SpecularLight[] = { 0.5,0.5,0.5,1.0 };
	GLfloat lightPos_1[] = { 1, 1, 1, 1.0 };

	GLfloat AmbientLight2[] = { 0.2f,  0.2f ,  0.2f,  1.0f };
	GLfloat DiffuseLight2[] = { 1 , 1 , 1.0 , 1.0 };
	GLfloat SpecularLight2[] = { 0.5 ,0.5 ,0.5 ,1.0 };
	GLfloat lightPos_2[] = { -1, 1, 1, 1.0 };

	

	GLfloat gray[] = { 0.2f,0.2f,0.2f,1.0f };


	glEnable(GL_LIGHTING);

	glLightfv(GL_LIGHT0, GL_AMBIENT, AmbientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, DiffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, SpecularLight);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos_1);
	glEnable(GL_LIGHT0);

	glLightfv(GL_LIGHT1, GL_AMBIENT, AmbientLight2);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, DiffuseLight2);
	glLightfv(GL_LIGHT1, GL_SPECULAR, SpecularLight2);
	glLightfv(GL_LIGHT1, GL_POSITION, lightPos_2);
	glEnable(GL_LIGHT1);

	
	
}

void Draw_Background()
{
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	glTexGeni(GL_S, GL_TEXTURE_ENV_COLOR, GL_MODULATE);
	glPushMatrix();
	glTranslated(0, 0, -10);

	glBegin(GL_POLYGON);
	glNormal3f(0, 0, 1);
		glColor3f(1, 1, 1);
		glTexCoord2i(0, 1);
		glVertex3f(-10, 10, 0);

		glTexCoord2i(1, 1);
		glVertex3f(10, 10, 0);

		glTexCoord2i(1, 0);
		glVertex3f(10, -10, 0);

		glTexCoord2i(0, 0);
		glVertex3f(-10, -10, 0);
	glEnd();

	glPopMatrix();
}

void Draw_Level()
{
	char str[20];

	switch (level)
	{
	case EASY_1:
		strcpy_s(str, "Level : EASY_1");
		break;
	case EASY_2:
		strcpy_s(str, "Level : EASY_2");
		break;
	case NORMAL_1:
		strcpy_s(str, "Level : NORMAL_1");
		break;
	case NORMAL_2:
		strcpy_s(str, "Level : NORMAL_2");
		break;
	case HARD_1:
		strcpy_s(str, "Level : HARD_1");
		break;
	case HARD_2:
		strcpy_s(str, "Level : HARD_2");
		break;
	}

	glEnable(GL_COLOR_MATERIAL);
	glColor3f(0, 0, 0);
	glRasterPos2f(1, 2.5);//아니 이건 또 뭔 함수야

	int len = (int)strlen(str);
	for (int i = 0; i < len; i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, str[i]);
}

void Draw_Title()
{
	//요것도 그리는 거니까 -1.0과 1.0 사이
	glBindTexture(GL_TEXTURE_2D, textures[1]);
	glTexGeni(GL_S, GL_TEXTURE_ENV_COLOR, GL_MODULATE);

	glPushMatrix();
	glTranslatef(0, 1, 0);
	glScaled(2, 2, 2);
		glBegin(GL_POLYGON);
			glColor3f(1, 1, 1);
			glTexCoord2i(0, 1);
			glVertex3f(-1, 1, 0);

			glTexCoord2i(1, 1);
			glVertex3f(1, 1, 0);

			glTexCoord2i(1, 0);
			glVertex3f(1, -1, 0);

			glTexCoord2i(0, 0);
			glVertex3f(-1, -1, 0);

		glEnd();
	glPopMatrix();
}

void Draw_defeat_state()
{
	char str[] ="Press 'r' To Restart";
	//strcpy_s(str, "");
	

	glEnable(GL_COLOR_MATERIAL);
	glColor3f(1, 0, 0);
	glRasterPos2f(-0.5, 1);

	int len = (int)strlen(str);
	for (int i = 0; i < len; i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, str[i]);

	char str2[] = "Hold 'x' to Pay Respects";
	//strcpy_s(str, "");


	glEnable(GL_COLOR_MATERIAL);
	glColor3f(1, 0, 0);
	glRasterPos2f(-0.5, 1.2);

	int len2 = (int)strlen(str2);
	for (int i = 0; i < len2; i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, str2[i]);
}

void Draw_win_state()
{
	char str[] = "Press 'r' To Restart";
	//strcpy_s(str, "");

	glEnable(GL_COLOR_MATERIAL);
	glColor3f(1, 0, 0);
	glRasterPos2f(-0.5, 1.2);

	int len = (int)strlen(str);
	for (int i = 0; i < len; i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, str[i]);


	char str2[] = "Press 'x' To Title";
	//strcpy_s(str, "");

	glEnable(GL_COLOR_MATERIAL);
	glColor3f(1, 0, 0);
	glRasterPos2f(-0.5, 1.4);

	int len2 = (int)strlen(str2);
	for (int i = 0; i < len2; i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, str2[i]);

}

void Draw_select_state()
{
	char str[] = "Press Number To Select Level";
	//strcpy_s(str, "");

	glEnable(GL_COLOR_MATERIAL);
	glColor3f(1, 0, 0);
	glRasterPos2f(-0.5, 2.2);

	int len = (int)strlen(str);
	for (int i = 0; i < len; i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, str[i]);

	char str2[] = "Press 's' To Start";
	//strcpy_s(str, "");

	glEnable(GL_COLOR_MATERIAL);
	glColor3f(1, 0, 0);
	glRasterPos2f(-0.5, 2.1);

	int len2 = (int)strlen(str2);
	for (int i = 0; i < len2; i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, str2[i]);
}

void InitConnect(WSADATA wsa, SOCKET sock, int GameState)
{
	int retval;
	std::string serverip;

	std::cout << "서버의 IP주소를 입력하세요. : ";
	std::getline(std::cin, serverip);

	//윈속 초기화
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)return;

	//socket()
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)err_quit("socket()");

	//connect()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(serverip.c_str());
	serveraddr.sin_port = htons(SERVERPORT);
	retval = connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)err_quit("connect()");

	// 초기 데이터를 클래스 단위로 받아온다.
	retval = recv(sock, (char*)& g_towers, sizeof(g_towers), 0);
	if (retval == SOCKET_ERROR) err_display("recv()");
	retval = recv(sock, (char*)& g_myIdx, sizeof(int), 0);
	if (retval == SOCKET_ERROR) err_display("recv()");

	Game_state = EASY_1;
}

//template<class T>
//bool recvn(SOCKET client_sock, const char* name, T File)//탬플릿으로 편하게 받아온다
//{
//	int retval;	//return value
//	char buf[1025];	//버퍼[크기]
//	size_t len{ 0 };	//파일의 총 크기를 저장하는 변수
//
//	//고정 길이 수신
//	retval = recv(client_sock, (char*)&len, sizeof(size_t), 0);
//
//	if (retval == SOCKET_ERROR)
//	{
//		err_quit("recv()");
//		return false;
//	}
//	else if (retval == 0)
//		return false;
//
//	size_t remainData{ len };	//남아있는 데이터의 크기(초기화 시에는 len과 같은 크기로)
//	size_t readsize{ 0 };	//읽어오는 크기
//	size_t sentData{ 0 };
//
//	int prev_per{ 0 };	//업데이트 전의 percentage
//	int curr_per{ 0 };	//현재 percentage
//
//	while (remainData)
//	{
//		if (remainData > BUFSIZE)
//			readsize = BUFSIZE;
//		else
//			readsize = remainData;
//
//		//가변 길이 수신
//		retval = recv(client_sock, buf, readsize, 0);
//		if (retval == SOCKET_ERROR)
//		{
//			err_quit("recv()");
//			return false;
//		}
//		else if (retval == 0)
//			return false;
//
//		//std::copy(buffer, &buffer[읽어 온 바이트 수], File_pos)가 람다식으로 들어간다.
//		File(buf, &buf[readsize]);
//
//		remainData -= readsize;
//		sentData += readsize;
//		curr_per = 100 * ((len * 1.f - remainData * 1.f) / len);
//		if (name == "Getting file name.....")
//			std::cout << name << std::endl;
//		else if (prev_per != curr_per)
//		{
//			prev_per = curr_per;
//			std::cout << "\r" << "[" << name << " : " << curr_per << "%]";
//		}
//	}
//
//	std::cout << "Complete." << std::endl;
//
//	return true;
//}