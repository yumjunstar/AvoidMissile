//오락실 우주선 게임
/*
* © 2021 염준우 <yumjunstar@naver.com>
*/
#define MAX_COL 120
#define MAX_ROW 30
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <conio.h>
struct Ship_Location {//플레이어
	int health;//현재 체력
	int x;
	int y;
}ship;

const int max_health = 3;
int score = 0;//현재 점수
int max_score = 0;//최고 점수
int** background;//물체 위치 저장 장애물 아이템
int bottom_size = 5;//밑에 표시창이 차지하는 크기
int level = 1;//레벨

int bottom_explain = 0;//임시 변수

void init();//초기 설정 함수
void gotoxy(int x, int y);//좌표 이동 함수
void color(int code);//색깔 변경 함수

void ship_move();//플레이어 함선이 움직일 수 있도록
void obstacle(int difficulty);//미사일의 위치를 저장하는 함수
void cure(int dif);//체력 회복 아이템 위치를 저장하는 함수
void object_down();//미사일과 아이템들의 위치를 옮기는 함수
void print_game();//게임 화면 출력 함수
void bottom();//하단 체력, 점수, 최고 점수 표시하는 함수

void clearbottom();
void reset_arr_1(int* arr, int size, int value);//1차원 배열 초기화 함수
void reset_arr_2(int** arr, int row, int column, int value);//2차원 배열 초기화 함수
void print_arr();//디버그용 함수
int gameover();//게임이 끝날때 화면
void load();//역대 최고 점수 불러오기
void save();//최고 점수 저장하기
void mainmenu();//메인 메뉴 함수

int main(void) {
	int delay = 20;
	mainmenu();
	do {//게임을 계속 할 것인지 확인 하는 do ~ while 문
		init();//초기 설정
		delay = 20;
		while (ship.health >= 1) {
			
			ship_move();//우주선 움직임
			
			obstacle(level);//할당 및 움직임
			cure(2);
			object_down();

			print_game();
			score++;
			level = score / 100 + 1;//레벨 관련 식 100 점마다 한레벨씩 올라가도록

			{
				if (score >= max_score) {//최대 점수 지정
					max_score = score;
				}
				if (20 - score / 100 > 0) {//딜레이 점점 줄이기
					delay = 20 - score / 100;
				}
				else {//0보다 줄어드면 delay 1로 변경
					delay = 1;
				}
				Sleep(delay);//속도 조절
			}
		}
		save();
		system("cls");
	} while (gameover() == 1);

	return 0;
}

void mainmenu() {
	system("mode con:cols=100 lines=20");
	system("title 메인 메뉴");
	gotoxy(50 - 13, 15);
	color(7);
	printf("아무키나 눌러 게임 시작");
	while (1) {
		gotoxy(50 - 10, MAX_ROW / 2 - 9);
		color(14);
		printf("★미사일 피하기★\n");

		Sleep(100);
		color(3);
		gotoxy(50 - 10, MAX_ROW / 2 - 9);
		printf("★미사일 피하기★\n");
		Sleep(100);
		if (_kbhit()) break;//키가 눌렀는지 확인
	}
}

void init() {
	load();//최고 점수 불러오기
	score = 0;//점수 초기화
	srand(time(NULL));//시간에 따라 랜덤 시드 설정
	background = (int**)malloc(sizeof(int*) * MAX_ROW);//배경 행 (배경에 있는 오브젝트들 저장 플레이어 우주선 제외)
	for (int i = 0; i < MAX_ROW; i++) {
		background[i] = (int*)malloc(sizeof(int) * MAX_COL);//배경 열
	}

	ship.health = max_health;//우주선 처음 체력을 설정
	reset_arr_2(background, MAX_ROW, MAX_COL, 0);//0으로 초기화
	ship.x = (MAX_COL - bottom_size) / 2;//우주선의 초기 x좌표
	ship.y = MAX_ROW / 2;//우주선의 초기 y좌표 설정

	char* temp[30];
	sprintf_s(temp, sizeof(temp), "mode con:cols=%d lines=%d", MAX_COL, MAX_ROW);//문자열 형식 출력 함수
	system(temp);//창 크기 설정
	system("title 미사일 피하기");
	

	gotoxy(0, MAX_ROW - 5);//밑줄
	color(7);
	for (int i = 0; i < MAX_COL; i++) {
		printf("─");
	}
	bottom();
	clearbottom();
}
void ship_move() {
	if (GetAsyncKeyState(VK_LEFT) || GetAsyncKeyState(0x41)) {//왼쪽 또는 a키
		if (ship.x > 0) {
			gotoxy(ship.x, ship.y);
			printf(" ");//이전 잔상을 지우고
			ship.x--;//왼쪽으로 이동

		}
	}
	if (GetAsyncKeyState(VK_RIGHT) || GetAsyncKeyState(0x44)) {//오른쪽 또는 d키
		if (ship.x < MAX_COL - 1) {
			gotoxy(ship.x, ship.y);
			printf(" ");
			ship.x++;
		}
	}
	if (GetAsyncKeyState(VK_UP) || GetAsyncKeyState(0x57)) {//윗방향 또는 w키
		if (ship.y > 1) {
			gotoxy(ship.x, ship.y);
			printf(" ");
			ship.y--;
		}
	}
	if (GetAsyncKeyState(VK_DOWN) || GetAsyncKeyState(0x53)) {//윗방향 또는 s키
		if (ship.y < MAX_ROW - bottom_size - 1) {
			gotoxy(ship.x, ship.y);
			printf(" ");//이전 잔상 지우기
			ship.y++;
		}
	}
	gotoxy(ship.x, ship.y);
	color(11);
	printf("A");
	color(7);
}

void obstacle(int dif) {
	if (100 / dif > 0) {//0으로 나눌수는 없으므로
		if (score % (100 / dif) == 0) {//점점 장애물이 많아짐
			background[0][ship.x] = -1;//체력을 -1 깎는 장애물
		}
	}
	else {
		background[0][ship.x] = -1;
	}
}
void cure(int dif) {
	static int prv_dif = 0;//함수가 꺼져도 원형 유지
	int random = 0;//장애물 위치
	if (level != prv_dif) {//단계가 바뀔때마다
		for (int i = 0; i < dif; i++) {
			random = rand() % MAX_COL;
			if (background[0][random] == 0) {
				background[0][random] = 1;//치료제
			}
			else i--;
		}
	}
	prv_dif = level;
}
void object_down() {

	/*한 행에서 한열씩 옮기면 문제가 생김
	한줄 읽고 내리면 다음줄에도 그 오브젝트가 있게 되고
	for문이 다 끝날때 쯤이면 결국엔 한번에 주르륵 내려감
	*/

	//따라서 한 행씩 한번에 내리는 방법 사용
	int* temp;
	for (int i = MAX_ROW - 1; i >= 1; i--) {//모든 배경 물체 아래로 내리기
		reset_arr_1(background[MAX_ROW - 1], MAX_COL, 0);//맨 마지막줄 0으로 초기화
		temp = background[i - 1];
		background[i - 1] = background[i];
		background[i] = temp;
	}

	if (background[ship.y][ship.x] != 0) {//미사일과 우주선이 충돌하는 경우 또는 체력 회복 아이템을 먹는 경우
		gotoxy(ship.x, ship.y - 1);//한칸 위의 y좌표로 가서 그 미사일을 지우고
		printf(" ");

		ship.health += background[ship.y][ship.x];//체력을 회복하거나 데미지를 입는다.
		background[ship.y][ship.x] = 0;//그 위치의 오브젝트를 배열상에서 지운다.
	}

}

void print_game() {
	gotoxy(0, 0);//0,0위치로 이동
	for (int i = 0; i < MAX_ROW - bottom_size; i++) {
		for (int j = 0; j < MAX_COL; j++) {
			if (background[i][j] < 0) {//0보다 작으면 미사일 0보다 크면 치료제
				if (i - 1 >= 0 && background[i - 1][j] == 0) {
					gotoxy(j, i - 1);
					printf(" ");//미사일의 이전 위치는 지움
				}
				gotoxy(j, i);
				printf("M");//미사일
				if (i == MAX_ROW - bottom_size - 1) {//맨 아래에서도 지움
					gotoxy(j, i);
					printf(" ");//미사일의 이전 위치 지움
				}

			}
			else if (background[i][j] > 0) {
				if (i - 1 >= 0 && background[i - 1][j] == 0) {
					gotoxy(j, i - 1);
					printf(" ");//이전 위치는 지움
				}
				gotoxy(j, i);
				color(12);
				printf("+");//HealPack
				color(7);
				if (i == MAX_ROW - bottom_size - 1) {//맨 아래에서도 지움
					gotoxy(j, i);
					printf(" ");//미사일 이전 위치 지움
				}
			}
		}
	}

	bottom();
}
void bottom() {
	static int prev_health = 0;//이전 체력을 저장하는 곳
	static int prev_level = 0;

	if (prev_health != ship.health) {//체력 정보가 업데이트 됬을때만 밑에 표시창을 지우고 업데이트
		clearbottom();
	}prev_health = ship.health;

	if (prev_level != level) {//단계 정보가 업데이트 됬을때만
		gotoxy(0, MAX_ROW - 4);
		printf("%d 단계", level);
	}

	if (bottom_explain <= 1) {
		printf("  (방향키로 이동    하얀색을 피하세요    빨간색은 회복약입니다.)");
	}bottom_explain++;


	gotoxy(0, MAX_ROW - 2);
	printf("체력 : ");
	color(4);//어두운 빨강
	for (int i = 0; i < ship.health; i++) {
		printf("♥");
	}
	for (int i = 0; i < max_health - ship.health; i++) {
		printf("♡");
	}


	color(7);//하얀색
	int a = 0;
	if (ship.health - max_health > 0) {
		a = ship.health - max_health;
	}
	else a = 0;
	gotoxy(20 + a * 2, MAX_ROW - 2);
	printf("점수 : ");
	color(14);//밝은 노랑
	printf("%d", score);

	gotoxy(40 + a * 2, MAX_ROW - 2);
	color(7);//하얀색
	printf("최고 점수 : ");
	color(14);//밝은 노랑

	printf("%d", max_score);
	color(7);//하얀색


}

int gameover() {
	int i = 0;
	while (1) {
		color(12);
		printf(" _____                           _____                      _\n");
		printf("|  __ \\                         |  _  |                    | |\n");
		printf("| |  \\/  __ _  _ __ ___    ___  | | | |__   __  ___  _ __  | |\n");
		printf("| | __  / _` || '_ ` _ \\  / _ \\ | | | |\\ \\ / / / _ \\| '__| | |\n");
		printf("| |_\\ \\| (_| || | | | | ||  __/ \\ \\_/ / \\ V / |  __/| |    |_|\n");
		printf(" \\____/ \\__,_||_| |_| |_| \\___|  \\___/   \\_/   \\___||_|    (_)\n");
		printf("\n");
		color(7);
		printf("현재 점수 : ");
		color(14);
		printf("%d\n", score); 
		color(7);
		printf("최고 점수 : ");
		color(14);
		printf ("%d\n", max_score);
		color(7);
		
		printf("\n다시 하시겠습니까?\n");
		printf("1. 예\n");
		printf("2. 아니오\n");
		system("pause");
		printf("\n입력:");

		if (scanf_s("%d", &i) == 0) {
			rewind(stdin);
			i = 0;
			printf("문자 입력은 허용하지 않습니다.\n");
			Sleep(1000);
		}
		else if (!(i >= 1 && i <= 2)) {//1과 2말고 다른 숫자라면
			i = 0;
			printf("1과 2사이의 숫자로 입력해주세요.\n");
			Sleep(1000);
		}
		else {
			break;
		}
		system("cls");
	}
	return i;
}


void color(int code) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), code);//windows.h에 정의된 함수, 표준 출력 핸들, 숫자로 색깔 설정
}
void gotoxy(int x, int y) {
	COORD Pos;//좌표 구조체
	Pos.X = x;
	Pos.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos);//windows.h에 정의된 함수, 표준 출력 핸들, 좌표는 구조체로
}

void clearbottom() {
	gotoxy(0, MAX_ROW - 4);
	for (int i = 0; i < MAX_COL; i++) {
		printf(" ");
	}
	gotoxy(0, MAX_ROW - 2);
	for (int i = 0; i < MAX_COL; i++) {
		printf(" ");
	}
}
void reset_arr_2(int** arr, int row, int column, int value) {
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < column; j++) {
			arr[i][j] = value;
		}
	}
}
void reset_arr_1(int* arr, int size, int value) {
	for (int i = 0; i < size; i++) {
		arr[i] = value;
	}
}
void load() {
	FILE* fp;
	fopen_s(&fp, ".Game_Info.txt", "a+");//읽기 및 추가 모드
	if (fp != NULL) {
		fscanf_s(fp, "%d", &max_score);
		fclose(fp);
	}
	else printf("\n파일 읽기 실패\n");
}
void save() {
	FILE* fp;
	fopen_s(&fp, ".Game_Info.txt", "w");
	if (fp != NULL) {
		fprintf(fp, "%d\n", max_score);//최고 점수 저장
		fclose(fp);
	}
	else printf("\n파일 쓰기 실패\n");
}
void print_arr() {//디버그용
	printf("--------------------------------------");
	for (int i = 0; i < MAX_ROW; i++) {
		for (int j = 0; j < MAX_COL; j++) {
			if (background[i][j] != 0) {
				printf("%d", background[i][j]);
			}
			else {
				printf(" ");
			}
		}
		printf("\n");
	}
}