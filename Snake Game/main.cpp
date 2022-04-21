#include <iostream>
#include <conio.h>
#include <SFML/Graphics.hpp>
#include <Windows.h>
#include <SFML/Audio.hpp>
using namespace std;

#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4

struct Coord
{
	int x, y;
};
struct Food
{
	Coord fPos;
	char fSym;
	bool spawned;
	bool drawn;
};
struct Snake
{
	Coord* body;
	int direction;
	int len;
	char sym;
	char headSym;
};
struct Player
{
	Snake pSnake;
	string name;
	int score;
	int pID;
	bool alive;

	char kUP;
	char kDOWN;
	char kLEFT;
	char kRIGHT;
};
struct Board
{
	Coord start;
	char* bMap;
	int W, H;
};

char Input();
void DrawSnake(const Snake& S);
void gotoRowCol(const Coord& pos);
void initSnake(Player& P, const Board& B);
void changeDirection(Player& P, const char c);
//bool WallTouch(const Snake& S, const Board& B);
bool AllSnakesAlive(const Player* Ps, const int pCount);
void initPlayer(Player& P, const char* name, const Board& B);
Player* initGame(Board& B, const int pCount, const char* name);

void initSnake(Player& P, const Board& B) {
	P.pSnake.len = 7;
	P.pSnake.sym = char(178);
	P.pSnake.headSym = char(153);
	P.alive = true;
	P.pSnake.body = new Coord[P.pSnake.len]{};
	Coord start{};
	switch (P.pID) {
	case 1:
		P.pSnake.direction = RIGHT;
		start.x = B.W / 2;
		start.y = B.H / 2;
		break;
	case 2:
		P.pSnake.direction = RIGHT;
		start.x = B.W - (B.W / 2);
		start.y = B.H - (B.H / 3);
		break;
	default:
		break;
	}
	P.pSnake.body[0] = start;
	for (int i = 1; i < P.pSnake.len; i++) {
		P.pSnake.body[i].x = P.pSnake.body[0].x - i;
		P.pSnake.body[i].y = P.pSnake.body[0].y;
	}
}

void initPlayer(Player& P, const char* name, const Board& B) {
	P.score = 0;
	P.name = name;

	switch (P.pID) {
	case 1:
		P.kUP = 'W';
		P.kDOWN = 'S';
		P.kLEFT = 'A';
		P.kRIGHT = 'D';
		break;
	case 2:
		P.kUP = 72;
		P.kDOWN = 80;
		P.kLEFT = 75;
		P.kRIGHT = 77;
		break;
	default:
		break;
	}
	initSnake(P, B);
}
void initBoard(Board& B) {
	B.start.x = 5;
	B.start.y = 5;
	B.W = 110;
	B.H = 20;
	B.bMap = new char[B.W * B.H]{};
	//Add 's' meaning solid wall, at the borders of board
	for (int i = 0; i < B.W; i++) {
		B.bMap[i] = 's';
		B.bMap[i + (B.H - 1) * B.W] = 's';
	}
	for (int i = 0; i < B.H; i++) {
		B.bMap[i * B.W] = 's';
		B.bMap[i * B.W + B.W - 1] = 's';
	}
	//Add 'p' chars in the shape of an empty 8x8 square near the middle
	for (int i = B.start.x; i < B.start.x + 8; i++) {
		for (int j = B.start.y; j < B.start.y + 8; j++) {
			B.bMap[i + j * B.W] = 'p';
		}
	}

}

Player* initGame(Board& B, const int pCount, const char* name) {
	initBoard(B);
	Player* Ps = new Player[pCount]{};
	for (int i = 0; i < pCount; i++) {
		Ps[i].pID = i + 1;
		initPlayer(Ps[i], name, B);
	}
	return Ps;
}



void DisplayBoard(const Board& B) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 3);
	//Display B.bMap
	for (int i = 0; i < B.H; i++) {
		for (int j = 0; j < B.W; j++) {
			switch (B.bMap[i * B.W + j]) {
			case 's':
				gotoRowCol({ j + B.start.y, i + B.start.x });
				cout << char(219);
				break;
			case 'p':
				gotoRowCol({ j + B.start.y, i + B.start.x });
				cout << char(176);
				break;
			default:
				break;
			}
		}
	}
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
}

void DrawSnake(const Snake& P) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);
	gotoRowCol(P.body[0]);
	cout << P.headSym;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
	for (int i = 1; i < P.len; i++) {
		gotoRowCol(P.body[i]);
		cout << P.sym;
	}
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
}
void EraseSnake(const Snake& S) {
	for (int i = 0; i < S.len; i++) {
		gotoRowCol(S.body[i]);
		cout << ' ';
	}
}
void ShiftSnakeBody(const Snake& S) {
	for (int i = S.len - 1; i > 0; i--) {
		S.body[i] = S.body[i - 1];
	}
}
void MoveSnake(const Snake& S, const Board& B) {
	switch (S.direction) {
	case UP:
		S.body[0].y--;
		break;
	case DOWN:
		S.body[0].y++;
		break;
	case LEFT:
		S.body[0].x--;
		break;
	case RIGHT:
		S.body[0].x++;
		break;
	default:
		break;
	}
	if (S.body[0].x < B.start.x) {
		S.body[0].x = B.start.x + B.W - 1;
	}
	else if (S.body[0].x > B.start.x + B.W - 1) {
		S.body[0].x = B.start.x;
	}
	if (S.body[0].y < B.start.y) {
		S.body[0].y = B.start.y + B.H - 1;
	}
	else if (S.body[0].y > B.start.y + B.H - 1) {
		S.body[0].y = B.start.y;
	}
}

void GenerateFood(const Player* Ps, const Board& B, Food& f) {
	if (!f.spawned && time(0) % 4 == 0) {
		srand((u_int)time(0));
		f.fPos.x = (rand() % B.W) + B.start.x;
		f.fPos.y = (rand() % B.H) + B.start.y;
		f.spawned = true;
		for (int i = 0; i < B.W; i++) {
			for (int j = 0; j < B.H; j++) {
				if (f.fPos.x == Ps[0].pSnake.body[i].x && f.fPos.y == Ps[0].pSnake.body[i].y) {
					f.spawned = false;
					return;
				}
				if (f.fPos.x == Ps[1].pSnake.body[i].x && f.fPos.y == Ps[1].pSnake.body[i].y) {
					f.spawned = false;
					return;
				}
			}
		}
	}
}

bool CheckFoodEaten(const Snake& S, Food& f) {
	if (S.body[0].x == f.fPos.x && S.body[0].y == f.fPos.y) {
		f.spawned = false;
		return true;
	}
	return false;
}
void DrawFood(Food& f) {
	if (f.spawned && !f.drawn) {
		gotoRowCol(f.fPos);
		cout << f.fSym;
		f.drawn = true;
	}
	else if (!f.spawned && f.drawn) {
		gotoRowCol(f.fPos);
		cout << ' ';
		f.drawn = false;
	}
}
void ExpandSnake(Snake& S) {
	const int newSize = S.len + 1;
	Coord* temp = new Coord[newSize]{};
	for (int i = 0; i < S.len && i < newSize; i++) {
		temp[i] = S.body[i];
	}
	switch (S.direction) {
	case UP:
		temp[newSize - 1].x = S.body[0].x;
		temp[newSize - 1].y = S.body[0].y - 1;
		break;
	case DOWN:
		temp[newSize - 1].x = S.body[0].x;
		temp[newSize - 1].y = S.body[0].y + 1;
		break;
	case LEFT:
		temp[newSize - 1].x = S.body[0].x - 1;
		temp[newSize - 1].y = S.body[0].y;
		break;
	case RIGHT:
		temp[newSize - 1].x = S.body[0].x + 1;
		temp[newSize - 1].y = S.body[0].y;
		break;
	default:
		break;
	}
	delete[] S.body;
	S.body = temp;
	S.len++;
}

bool CheckSnakeKilled(const Snake& S) {
	for (int i = 1; i < S.len; i++) {
		if (S.body[0].x == S.body[i].x && S.body[0].y == S.body[i].y) {
			return true;
		}
	}
	return false;
}

bool CheckSnakeTouch(const Player* Ps, const int mySnakeID, const int pCount) {
	for (int i = 0; i < pCount; i++) {
		if (i == mySnakeID || !Ps[i].alive) {
			continue;
		}
		for (int j = 0; j < Ps[i].pSnake.len; j++) {
			if (Ps[i].pSnake.body[j].x == Ps[mySnakeID].pSnake.body[0].x && Ps[i].pSnake.body[j].y == Ps[mySnakeID].pSnake.body[0].y) {
				return true;
			}
		}
	}
	return false;
}

void KillSnake(Player& P) {
	EraseSnake(P.pSnake);
	for (int i = 0; i < P.pSnake.len; i++) {
		P.pSnake.body[i] = {};
	}
	P.alive = false;
}

void PlayMusic(sf::Music& msc, const char* fileName) {
	if (!msc.openFromFile(fileName)) {
		return;
	}
	msc.play();
	msc.setLoop(true);
}
void PlaySound(sf::SoundBuffer& buffer, sf::Sound& snd, const char* fileName, const float vol, const float pitch) {
	if (!buffer.loadFromFile(fileName)) return;
	snd.setPitch(pitch);
	snd.setVolume(vol);
	snd.play();
}
void DisplayScores(const Player* Ps, const Board& B, const int pCount) {
	for (int i = 0; i < pCount; i++) {
		gotoRowCol({ B.start.x, B.start.y + B.H + (i + 4) });
		cout << "Player " << i + 1 << ": " << Ps[i].score;
	}
}
char CheckWallHit(const Snake& S, const Board& B) {
	return B.bMap[((S.body[0].y-B.start.y) * B.W) + S.body[0].x - B.start.x];
}
bool deductScore(Player& P) {
	if (P.score - 10 >= 0) {
		P.score -= 10;
		return true;
	}
	if (P.pSnake.len - 2 > 1) {
		P.pSnake.len -= 2;
		return true;
	}
	else {
		return false;
	}
}
int main() {
	int pCount = 2;
	Board B{};
	Food f{};
	sf::SoundBuffer sfx;
	sf::Sound sfx_snd(sfx);
	f.fSym = '*';
	Player* Ps = initGame(B, pCount, "Saad Ahmed");
	gotoRowCol({ B.W / 2 - (35 / 2), B.H / 2 });
	cout << "PRESS ANY KEY TO START THE GAME..."; (void)_getch();
	system("cls");
	DisplayBoard(B);
	sf::Music bg_music;
	bg_music.setVolume(20);
	PlayMusic(bg_music, "bg_music.wav");
	while (AllSnakesAlive(Ps, pCount)) {
		GenerateFood(Ps, B, f);
		DisplayScores(Ps, B, pCount);
		for (int i = 0; i < pCount; i++) {
			changeDirection(Ps[i], toupper(Input()));
			if (Ps[i].alive) {
				EraseSnake(Ps[i].pSnake);
				ShiftSnakeBody(Ps[i].pSnake);
				MoveSnake(Ps[i].pSnake, B);
				switch (CheckWallHit(Ps[i].pSnake, B)) {
				case 's':
					PlaySound(sfx, sfx_snd, "sfx_hit.wav", 20, 1.0f);
					KillSnake(Ps[i]);
					break;
				case 'p':
					PlaySound(sfx, sfx_snd, "sfx_hit.wav", 20, 1.0f);
					if (!deductScore(Ps[i])) {
						KillSnake(Ps[i]);
						PlaySound(sfx, sfx_snd, "sfx_snake_death.wav", 40, 1);
						continue;
					}
					break;
				default:
					break;
				}
				if (CheckSnakeKilled(Ps[i].pSnake)) {
					KillSnake(Ps[i]);
					PlaySound(sfx, sfx_snd, "sfx_snake_death.wav", 40, 1);
					continue;
				}
				else if (CheckSnakeTouch(Ps, i, pCount)) {
					if (!deductScore(Ps[i])) {
						KillSnake(Ps[i]);
						PlaySound(sfx, sfx_snd, "sfx_snake_death.wav", 40, 1);
						continue;
					}
				}
				else if (CheckFoodEaten(Ps[i].pSnake, f)) {
					PlaySound(sfx, sfx_snd, "sfx-pop.wav", 40, 1);
					Ps[i].score += 10;
					ExpandSnake(Ps[i].pSnake);
				}
				DrawSnake(Ps[i].pSnake);
				FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
			}
		}
		if (f.spawned && !f.drawn) {
			PlaySound(sfx, sfx_snd, "sfx-pop.wav", 30, 5);
		}
		DrawFood(f);
		Sleep(100);
	}
	bg_music.stop();
	delete[] Ps;
	system("cls");
	gotoRowCol({ B.W / 2 - (13 / 2), B.H / 2 });
	cout << "GAME OVER..."; (void)_getch();
	return 0;

}
bool AllSnakesAlive(const Player* Ps, const int pCount) {
	for (int i = 0; i < pCount; i++) {
		if (Ps[i].alive) {
			return true;
		}
	}
	return false;
}

char Input() {
	char c = 0;
	if (_kbhit()) {
		c = _getch();
		if (c == 224)
			return _getch();
		return c;
	}
	return c;
}

void changeDirection(Player& P, const char c) {
	if (c == P.kUP && P.pSnake.direction != DOWN)
		P.pSnake.direction = UP;
	else if (c == P.kDOWN && P.pSnake.direction != UP)
		P.pSnake.direction = DOWN;
	else if (c == P.kLEFT && P.pSnake.direction != RIGHT)
		P.pSnake.direction = LEFT;
	else if (c == P.kRIGHT && P.pSnake.direction != LEFT)
		P.pSnake.direction = RIGHT;
}

void gotoRowCol(const Coord& pos) {
	COORD scrn{};
	scrn.X = pos.x;
	scrn.Y = pos.y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), scrn);
}