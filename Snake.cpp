#include <iostream>
#include <conio.h>
#include <windows.h>
#include <ctime>
using namespace std;

// Directions
const char UP = 'U', DOWN = 'D', LEFT = 'L', RIGHT = 'R';

HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
int width, height;

void gotoxy(int x, int y) {
    COORD c = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(console, c);
}

void setColor(int color) {
    SetConsoleTextAttribute(console, color);
}

void hideCursor() {
    CONSOLE_CURSOR_INFO info;
    GetConsoleCursorInfo(console, &info);
    info.bVisible = FALSE;
    SetConsoleCursorInfo(console, &info);
}

void initScreen() {
    CONSOLE_SCREEN_BUFFER_INFO info;
    GetConsoleScreenBufferInfo(console, &info);
    width = info.srWindow.Right - info.srWindow.Left + 1;
    height = info.srWindow.Bottom - info.srWindow.Top + 1;
}

void drawBorder() {
    setColor(11);
    for (int x = 0; x < width; ++x) {
        gotoxy(x, 1); cout << "#";
        gotoxy(x, height - 1); cout << "#";
    }
    for (int y = 1; y < height; ++y) {
        gotoxy(0, y); cout << "#";
        gotoxy(width - 1, y); cout << "#";
    }
    setColor(15);
}

struct Point {
    int x, y;
    Point(int a = 0, int b = 0) : x(a), y(b) {}
};

struct Node {
    Point pos;
    Node* next;
    Node(Point p) : pos(p), next(nullptr) {}
};

class Snake {
    Node* head;
    char dir;

public:
    Snake(int x, int y) {
        head = new Node(Point(x, y));
        dir = RIGHT;
    }

    Point getHead() { return head->pos; }

    void changeDir(char d) {
        if ((d == UP && dir != DOWN) || (d == DOWN && dir != UP) ||
            (d == LEFT && dir != RIGHT) || (d == RIGHT && dir != LEFT))
            dir = d;
    }

    bool move(Point food, bool &ate) {
        Point newPos = head->pos;
        if (dir == UP) newPos.y--;
        if (dir == DOWN) newPos.y++;
        if (dir == LEFT) newPos.x--;
        if (dir == RIGHT) newPos.x++;

        // Check wall collision
        if (newPos.x <= 0 || newPos.x >= width - 1 || newPos.y <= 1 || newPos.y >= height - 1)
            return false;

        // Check self-collision
        for (Node* temp = head; temp; temp = temp->next)
            if (temp->pos.x == newPos.x && temp->pos.y == newPos.y)
                return false;

        // Add new head
        Node* newHead = new Node(newPos);
        newHead->next = head;
        head = newHead;

        // Eat food
        if (newPos.x == food.x && newPos.y == food.y) {
            ate = true;
        } else {
            Node *prev = nullptr, *curr = head;
            while (curr->next) {
                prev = curr;
                curr = curr->next;
            }
            gotoxy(curr->pos.x, curr->pos.y); cout << " ";
            delete curr;
            prev->next = nullptr;
        }

        return true;
    }

    void draw() {
        setColor(10);
        for (Node* temp = head; temp; temp = temp->next) {
            gotoxy(temp->pos.x, temp->pos.y);
            cout << (temp == head ? '@' : 'O');
        }
        setColor(15);
    }

    ~Snake() {
        while (head) {
            Node* temp = head;
            head = head->next;
            delete temp;
        }
    }
};

class Game {
    Snake snake;
    Point food;
    int score;

public:
    Game() : snake(10, 10), score(0) { spawnFood(); }

    void spawnFood() {
        while (true) {
            int x = 2 + rand() % (width - 4);
            int y = 2 + rand() % (height - 4);
            food = Point(x, y);
            if (food.x != snake.getHead().x || food.y != snake.getHead().y) break;
        }
    }

    void input() {
        if (_kbhit()) {
            char key = _getch();
            if (key == 'w') snake.changeDir(UP);
            if (key == 's') snake.changeDir(DOWN);
            if (key == 'a') snake.changeDir(LEFT);
            if (key == 'd') snake.changeDir(RIGHT);
        }
    }

    bool update() {
        bool ate = false;
        bool alive = snake.move(food, ate);
        if (!alive) return false;
        if (ate) {
            score++;
            spawnFood();
        }
        return true;
    }

    void draw() {
        gotoxy(2, 0);
        setColor(14); cout << "Score: " << score;
        setColor(12);
        gotoxy(food.x, food.y); cout << 'o';
        setColor(15);
        snake.draw();
    }

    int getScore() const { return score; }
};

int main() {
    srand((unsigned)time(0));
    initScreen();
    hideCursor();
    system("cls");

    drawBorder();
    Game game;

    while (game.update()) {
        game.input();
        game.draw();
        Sleep(150);
    }

    gotoxy(width / 2 - 5, height / 2);
    setColor(12); cout << "GAME OVER!";
    gotoxy(width / 2 - 8, height / 2 + 1);
    cout << "Final Score: " << game.getScore();
    setColor(15);
    gotoxy(0, height);
    return 0;
}
