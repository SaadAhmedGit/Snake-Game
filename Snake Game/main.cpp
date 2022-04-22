#include <iostream>
#include <conio.h>
#include <SFML/Graphics.hpp>
#include <fstream>
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

char input();
void draw_snake(const Player& P);
void gotoRowCol(const Coord& pos);
void init_snake(Player& P, const Board& B);
void change_direction(Player& P, const char c);
//bool WallTouch(const Snake& S, const Board& B);
bool all_snakes_alive(const Player* Ps, const int p_count);
void init_player(Player& P, const char* name, const Board& B);
Player* init_game(Board& B, const int p_count);

void init_snake(Player& P, const Board& B) {
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

void init_player(Player& P, const Board& B) {
	P.score = 0;
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
	init_snake(P, B);
}
void initBoard(Board& B) {
	B.start.x = 5;
	B.start.y = 5;
	B.W = 110;
	B.H = 20;
	B.bMap = new char[B.W * B.H]{};
	ifstream file("level-1.txt");
	if (file.is_open()) {
		for (int i = 0; i < B.H; i++) {
			for (int j = 0; j < B.W; j++) {
				file >> B.bMap[i * B.W + j];
			}
		}
		file.close();
	}
	else {
		cout << "Unable to open file";
	}

}
Player* init_game(Board& B, const int p_count) {
	initBoard(B);
	const auto Ps = new Player[p_count]{};
	for (int i = 0; i < p_count; i++) {
		Ps[i].pID = i + 1;
		init_player(Ps[i], B);
	}
	return Ps;
}
void display_board(const Board& B) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 3);
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

void draw_snake(const Player& P) {
	int color = FOREGROUND_RED;
	switch (P.pID) {
	case 1:
		color = FOREGROUND_RED;
		break;
	case 2:
		color = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
		break;
	default:
		break;
	}
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);
	gotoRowCol(P.pSnake.body[0]);
	cout << P.pSnake.headSym;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
	for (int i = 1; i < P.pSnake.len; i++) {
		gotoRowCol(P.pSnake.body[i]);
		cout << P.pSnake.sym;
	}
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
}
void erase_snake(const Snake& S, const Board& B) {
	gotoRowCol(S.body[0]);
	char c = B.bMap[((S.body[0].y - B.start.y) * B.W) + S.body[0].x - B.start.x];
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 3);
	cout << (c == 's' ? char(219) : ' ');
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	for (int i = 1; i < S.len; i++) {
		gotoRowCol(S.body[i]);
		cout << ' ';
	}
}
void shift_snake_body(const Snake& S) {
	for (int i = S.len - 1; i > 0; i--) {
		S.body[i] = S.body[i - 1];
	}
}
void move_snake(const Snake& S, const Board& B) {
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
		S.body[0].x = B.start.x;
	}
	if (S.body[0].y < B.start.y) {
		S.body[0].y = B.start.y + B.H - 1;
	}
	else if (S.body[0].y > B.start.y + B.H - 1) {
		S.body[0].y = B.start.y;
	}
}

void generate_food(const Player* Ps, const Board& B, Food& f) {
	if (!f.spawned && time(0) % 4 == 0) {
		srand(static_cast<u_int>(time(nullptr)));  // NOLINT(cert-msc51-cpp)
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
		if (B.bMap[f.fPos.y * B.W + f.fPos.x] != '.') {
			f.spawned = false;
		}
	}
}

bool check_food_eaten(const Snake& S, Food& f) {
	if (S.body[0].x == f.fPos.x && S.body[0].y == f.fPos.y) {
		f.spawned = false;
		return true;
	}
	return false;
}
void draw_food(Food& f) {
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
	const int new_size = S.len + 1;
	const auto temp = new Coord[new_size]{};
	for (int i = 0; i < S.len && i < new_size; i++) {
		temp[i] = S.body[i];
	}
	switch (S.direction) {
	case UP:
		temp[new_size - 1].x = S.body[0].x;
		temp[new_size - 1].y = S.body[0].y - 1;
		break;
	case DOWN:
		temp[new_size - 1].x = S.body[0].x;
		temp[new_size - 1].y = S.body[0].y + 1;
		break;
	case LEFT:
		temp[new_size - 1].x = S.body[0].x - 1;
		temp[new_size - 1].y = S.body[0].y;
		break;
	case RIGHT:
		temp[new_size - 1].x = S.body[0].x + 1;
		temp[new_size - 1].y = S.body[0].y;
		break;
	default:
		break;
	}
	delete[] S.body;
	S.body = temp;
	S.len++;
}

bool check_snake_killed(const Snake& S) {
	for (int i = 1; i < S.len; i++) {
		if (S.body[0].x == S.body[i].x && S.body[0].y == S.body[i].y) {
			return true;
		}
	}
	return false;
}

bool check_snake_touch(const Player* Ps, const int mySnakeID, const int pCount) {
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

void kill_snake(Player& P, const Board& B) {
	erase_snake(P.pSnake, B);
	for (int i = 0; i < P.pSnake.len; i++) {
		P.pSnake.body[i] = {};
	}
	P.alive = false;
}

void play_music(sf::Music& msc, const char* fileName) {
	if (!msc.openFromFile(fileName)) {
		return;
	}
	msc.play();
	msc.setLoop(true);
}
void play_sound(sf::SoundBuffer& buffer, sf::Sound& snd, const char* fileName, const float vol, const float pitch) {
	if (!buffer.loadFromFile(fileName)) return;
	snd.setPitch(pitch);
	snd.setVolume(vol);
	snd.play();
}
void display_scores(const Player* Ps, const Board& B, const int pCount) {
	for (int i = 0; i < pCount; i++) {
		gotoRowCol({ B.start.x, B.start.y + B.H + (i + 4) });
		cout << "Player " << i + 1 << ": " << Ps[i].score;
	}
}
char check_wall_hit(const Snake& S, const Board& B) {
	return B.bMap[((S.body[0].y - B.start.y) * B.W) + S.body[0].x - B.start.x];
}
bool deduct_score(Player& P) {
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
	Player* Ps = init_game(B, pCount);
	gotoRowCol({ B.W / 2 - (35 / 2), B.H / 2 });
	cout << "PRESS ANY KEY TO START THE GAME..."; (void)_getch();
	system("cls");
	display_board(B);
	sf::Music bg_music;
	bg_music.setVolume(20);
	play_music(bg_music, "bg_music.wav");
	while (all_snakes_alive(Ps, pCount)) {
		generate_food(Ps, B, f);
		display_scores(Ps, B, pCount);
		for (int i = 0; i < pCount; i++) {
			change_direction(Ps[i], toupper(input()));
			if (Ps[i].alive) {
				erase_snake(Ps[i].pSnake, B);
				shift_snake_body(Ps[i].pSnake);
				move_snake(Ps[i].pSnake, B);
				switch (check_wall_hit(Ps[i].pSnake, B)) {
				case 's':
					play_sound(sfx, sfx_snd, "sfx_hit.wav", 20, 1.0f);
					kill_snake(Ps[i], B);
					continue;
					break;
				case 'p':
					play_sound(sfx, sfx_snd, "sfx_hit.wav", 20, 1.0f);
					B.bMap[((Ps[i].pSnake.body[0].y - B.start.y) * B.W) + Ps[i].pSnake.body[0].x - B.start.x] = '.';
					if (!deduct_score(Ps[i])) {
						kill_snake(Ps[i], B);
						play_sound(sfx, sfx_snd, "sfx_snake_death.wav", 40, 1);
						continue;
					}
					break;
				default:
					break;
				}
				if (check_snake_killed(Ps[i].pSnake)) {
					kill_snake(Ps[i], B);
					play_sound(sfx, sfx_snd, "sfx_snake_death.wav", 40, 1);
					continue;
				}
				else if (check_snake_touch(Ps, i, pCount)) {
					if (!deduct_score(Ps[i])) {
						kill_snake(Ps[i], B);
						play_sound(sfx, sfx_snd, "sfx_snake_death.wav", 40, 1);
						continue;
					}
				}
				else if (check_food_eaten(Ps[i].pSnake, f)) {
					play_sound(sfx, sfx_snd, "sfx-pop.wav", 40, 1);
					Ps[i].score += 10;
					ExpandSnake(Ps[i].pSnake);
				}
				draw_snake(Ps[i]);
				FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
			}
		}
		if (f.spawned && !f.drawn) {
			play_sound(sfx, sfx_snd, "sfx-pop.wav", 30, 5);
		}
		draw_food(f);
		Sleep(100);
	}
	bg_music.stop();
	delete[] Ps;
	system("cls");
	gotoRowCol({ B.W / 2 - (13 / 2), B.H / 2 });
	cout << "GAME OVER..."; (void)_getch();
	return 0;

}
bool all_snakes_alive(const Player* Ps, const int pCount) {
	for (int i = 0; i < pCount; i++) {
		if (Ps[i].alive) {
			return true;
		}
	}
	return false;
}

char input() {
	char c = 0;
	if (_kbhit()) {
		c = _getch();
		if (c == 224)
			return _getch();
		return c;
	}
	return c;
}

void change_direction(Player& p, const char c) {
	if (c == p.kUP && p.pSnake.direction != DOWN)
		p.pSnake.direction = UP;
	else if (c == p.kDOWN && p.pSnake.direction != UP)
		p.pSnake.direction = DOWN;
	else if (c == p.kLEFT && p.pSnake.direction != RIGHT)
		p.pSnake.direction = LEFT;
	else if (c == p.kRIGHT && p.pSnake.direction != LEFT)
		p.pSnake.direction = RIGHT;
}

void gotoRowCol(const Coord& pos) {
	COORD scrn{};
	scrn.X = pos.x;
	scrn.Y = pos.y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), scrn);
}