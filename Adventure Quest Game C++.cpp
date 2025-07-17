#define _CRT_SECURE_NO_WARNINGS //there was sme unsafe alert in code so i add this to run code smoothly
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <ctime>
using namespace std;

const int MAX_SIZE = 11;
const int MIN_SIZE = 5;

class Player;

class GameItem {
protected:
    char name[20];
    int row, col;
public:
    GameItem(const char* n = "", int r = -1, int c = -1) {
        strcpy(name, n);
        row = r;
        col = c;
    }
    virtual void interact(Player& p) = 0;  //this is ploymorphism pure virutal
    const char* getName() { return name; }
    int getRow() { return row; }
    int getCol() { return col; }
    void setPosition(int r, int c) { row = r; col = c; }
};

class Coin : public GameItem {
public:
    Coin(const char* type, int r, int c) : GameItem(type, r, c) {}
    void interact(Player& p) override;  //already virtual
};

class Hurdle : public GameItem {
    int effectTurns;
    char type;
public:
    Hurdle(const char* n, int r, int c, int turns, char t)
        : GameItem(n, r, c), effectTurns(turns), type(t) {
    }
    void interact(Player& p) override;  //virtyal
};

class Player {
    char name[20];
    int row, col;
    int gold, silver, points;
    int skipTurns;
    int sword, shield, water, key;
public:
    Player(const char* n = "Player", int r = 0, int c = 0) {
        strcpy(name, n);
        row = r; col = c;
        gold = 10; silver = 20; points = 0; skipTurns = 0;
        sword = shield = water = key = 0;
    }
    void move(int dr, int dc, int size) {
        if (skipTurns > 0) {
            cout << "\033[33m" << name << " is skipping a turn.\033[0m\n";
            skipTurns--;
            return;
        }
        int newRow = row + dr;
        int newCol = col + dc;
        if (newRow >= 0 && newRow < size && newCol >= 0 && newCol < size) {
            row = newRow; col = newCol;
            if (dr == -1) cout << "\033[33m" << name << " moved UP.\033[0m\n";
            else if (dr == 1) cout << "\033[33m" << name << " moved DOWN.\033[0m\n";
            else if (dc == -1) cout << "\033[33m" << name << " moved LEFT.\033[0m\n";
            else if (dc == 1) cout << "\033[33m" << name << " moved RIGHT.\033[0m\n";
        }
    }
    void collectCoin(Coin* coin) {
        if (!coin) return;
        if (strcmp(coin->getName(), "Gold") == 0) { gold++; points += 10; }
        else { silver++; points += 5; }
    }
    void printStatus() {
        cout << "\033[36m" << name << " at (" << row << "," << col << ") | Gold: " << gold
            << ", Silver: " << silver << ", Points: " << points << ", Skips: " << skipTurns << "\033[0m" << endl;
        cout << " \033[35m Inventory -> Sword: " << sword << ", Shield: " << shield
            << ", Water: " << water << ", Key: " << key << "\033[0m"<<endl;
    }
    void addItem(const char* item) {
        if (strcmp(item, "Sword") == 0) { sword++; points -= 40; }
        else if (strcmp(item, "Shield") == 0) { shield++; points -= 30; }
        else if (strcmp(item, "Water") == 0) { water++; points -= 50; }
        else if (strcmp(item, "Key") == 0) { key++; points -= 70; }
        cout << "\033[32m" << name << " bought a " << item << "!\033[0m\n";
    }
    bool deductForHurdle(const char* type) {
        if (strcmp(type, "Fire") == 0 && silver >= 12) { silver -= 12; return true; }
        if (strcmp(type, "Snake") == 0 && points >= 20) { points -= 20; return true; }
        if (strcmp(type, "Ghost") == 0 && points >= 10) { points -= 10; return true; }
        if (strcmp(type, "Lion") == 0 && gold >= 15) { gold -= 15; return true; }
        if (strcmp(type, "Lock") == 0 && points >= 25) { points -= 25; return true; }
        return false;
    }
    void setSkip(int t) { skipTurns = t; }
    int getRow() { return row; }
    int getCol() { return col; }
    int getPoints() { return points; }
    const char* getName() { return name; }
    bool useHelper(char h) {
        if (h == 'S' && sword > 0) { sword--; cout << "\033[32m" << name << " used Sword.\033[0m\n"; return true; }
        if (h == 'D' && shield > 0) { shield--; cout << "\033[32m" << name << " used Shield.\033[0m\n"; return true; }
        if (h == 'W' && water > 0) { water--; cout << "\033[32m" << name << " used Water.\033[0m\n"; return true; }
        if (h == 'K' && key > 0) { key--; cout << "\033[32m" << name << " used Key.\033[0m\n"; return true; }
        return false;
    }
};

void Coin::interact(Player& p) { p.collectCoin(this); }

void Hurdle::interact(Player& p) {
    bool handled = false;
    if (type == 'F') handled = p.useHelper('W');
    else if (type == 'L') handled = p.useHelper('S');
    else if (type == 'G') handled = p.useHelper('D');
    else if (type == 'K') handled = p.useHelper('K');
    else if (type == 'S') handled = p.useHelper('S');
    if (!handled) {
        cout << "\033[31m" << p.getName() << " could not resolve hurdle and will skip turns.\033[0m\n";
        p.setSkip(effectTurns);
    }
}


class Board {
    int size;
    GameItem* cells[121];
public:
    Board(int s = 9) {
        size = s;
        memset(cells, 0, sizeof(cells));
    }
    ~Board() {
        for (int i = 0; i < size * size; i++) delete cells[i];
    }
    void placeItems() {
        srand((unsigned)time(NULL));
        int total = 15 + rand() % 6;
        for (int i = 0; i < total; i++) {
            int r = rand() % size;
            int c = rand() % size;
            if (r == size / 2 && c == size / 2) continue;
            int idx = r * size + c;
            if (!cells[idx]) {
                if (i % 4 == 0) cells[idx] = new Coin("Gold", r, c);
                else if (i % 4 == 1) cells[idx] = new Coin("Silver", r, c);
                else if (i % 4 == 2) cells[idx] = new Hurdle("Fire", r, c, 2, 'F');
                else cells[idx] = new Hurdle("Snake", r, c, 3, 'S');
            }
        }
    }
    void placeHurdle(int r, int c, const char* type, int effect, char sym) {
        int idx = r * size + c;
        if (!cells[idx]) cells[idx] = new Hurdle(type, r, c, effect, sym);
    }
    GameItem* getItemAt(int r, int c) { return cells[r * size + c]; }
    void removeItem(int r, int c) {
        int idx = r * size + c;
        delete cells[idx]; cells[idx] = NULL;
    }
    void draw(Player& p1, Player& p2) {
        cout << "\n\033[35m========= Game Board =============\n";
        for (int i = 0; i < size; i++) {
            cout << "||  ";
            for (int j = 0; j < size; j++) {
                if (p1.getRow() == i && p1.getCol() == j) cout << "\033[36mP1 ";
                else if (p2.getRow() == i && p2.getCol() == j) cout << "\033[32mP2 ";
                else if (i == size / 2 && j == size / 2) cout << "\033[34mW  ";
                else {
                    GameItem* g = getItemAt(i, j);
                    if (!g) cout << "\033[90m.  ";
                    else if (strcmp(g->getName(), "Gold") == 0) cout << "\033[33mG  ";
                    else if (strcmp(g->getName(), "Silver") == 0) cout << "\033[37mS  ";
                    else cout << "\033[31mH  ";
                }
            }
            cout << " \033[35m||\n";
        }
        cout << "==================================\n\033[0m";
    }
    int getSize() { return size; }
};

class AdventureQuest {
    Board board;
    Player player1, player2;
    int turn;
public:
    AdventureQuest() : board(9), player1("Player 1", 0, 0), player2("Player 2", 8, 0), turn(0) {
        board.placeItems();
    }
    void playGame() {
        while (true) {
            board.draw(player1, player2);
            player1.printStatus(); player2.printStatus();
            Player* curr = (turn % 2 == 0) ? &player1 : &player2;

            int choice;
            while (true) {
                cout << "\n" << curr->getName() << "'s turn. Choose option:\n";
                cout << "\033[32m1. Move\n2. Buy Helper\n3. Place Hurdle\n0. Exit Game\nChoice: \033[0m";
                if (!(cin >> choice) ||choice < 0 || choice > 3) {
                    cin.clear(); cin.ignore(1000, '\n');
                    cout << "\033[31mInvalid choice!\033[30m\n";
                }
                else break;
            }  if (choice == 0) {
                cout << "\033[31mYou Exit from Game!!!!\n\033[30m";
                return ;
            }

            if (choice == 2) {
                char item[10];
                cout << "Buy what? (Sword/Shield/Water/Key): "; cin >> item;
                curr->addItem(item);
                continue;
            }
            else if (choice == 3) {
                int r, c; char t;
                while (true) {
                    cout << "Enter cell (row col): ";
                    if (!(cin >> r >> c)) { cin.clear(); cin.ignore(1000, '\n'); cout << "Invalid coordinates!\n"; continue; }
                    break;
                }
                while (true) {
                    cout << "Type: Fire(12S),Snake(20P),Ghost(10P),Lion(15G),Lock(25P) (F/S/G/L/K): ";
                    if (!(cin >> t) || strchr("FSGLKfsglk", t) == NULL) { cin.clear(); cin.ignore(1000, '\n'); cout << "\033[31mInvalid hurdle type!\033[37m\n"; continue; }
                    break;
                }
                const char* name = ""; int eff = 0; char sym = ' ';
                if (t == 'F' || t == 'f') { name = "Fire"; eff = 2; sym = 'F'; }
                else if (t == 'S' || t == 's') { name = "Snake"; eff = 3; sym = 'S'; }
                else if (t == 'G' || t == 'g') { name = "Ghost"; eff = 1; sym = 'G'; }
                else if (t == 'L' || t == 'l') { name = "Lion"; eff = 4; sym = 'L'; }
                else if (t == 'K' || t == 'k') { name = "Lock"; eff = 99; sym = 'K'; }

                if (curr->deductForHurdle(name)) board.placeHurdle(r, c, name, eff, sym);
                else cout << curr->getName() << " does not have enough points/gold to place " << name << "!\n";
                continue;
            }

            char mv;
            while (true) {
                cout << "\033[32mMove ^ (UP (U/u)/ Left <- (L/l)/ Down ! (D/d)/ Right -> (R/r) )\033[0m\n ";
                cout << "Enter your move Please : ";
                if (!(cin >> mv) || strchr("ULDRuldr", mv) == NULL) { cin.clear(); cin.ignore(1000, '\n'); cout << "\033[31mInvalid input!! you can reEnter\033[37m\n"; continue; }
                break;
            }
            int dr = 0, dc = 0;
            if (mv == 'U' || mv == 'u') dr = -1;
            else if (mv == 'D' || mv == 'd') dr = 1;
            else if (mv == 'L' || mv == 'l') dc = -1;
            else if (mv == 'R' || mv == 'r') dc = 1;

            curr->move(dr, dc, board.getSize());
            GameItem* gi = board.getItemAt(curr->getRow(), curr->getCol());
            if (gi) {
                if (strcmp(gi->getName(), "Gold") == 0 || strcmp(gi->getName(), "Silver") == 0)
                    cout << "\033[33m" << curr->getName() << " collected a " << gi->getName() << " coin!\033[0m\n";
                else
                    cout << "\033[31m" << curr->getName() << " encountered a " << gi->getName() << " hurdle!\033[0m\n";
                gi->interact(*curr);
                board.removeItem(curr->getRow(), curr->getCol());
            }

            if (curr->getRow() == board.getSize() / 2 && curr->getCol() == board.getSize() / 2) {
                cout << curr->getName() << " reached the goal!\n";
                break;
            }
            turn++;
        }
        int p1s = player1.getPoints(), p2s = player2.getPoints();
        cout << "\n\033[31mGame Over!\n";
        if (p1s > p2s) cout << "\033[32mPlayer 1 wins!\n";
        else if (p2s > p1s) cout << "\033[32mPlayer 2 wins!\n";
        else cout << "\033[34mIt's a draw!\n";
    }
};

int main() {
    cout << "\033[36m\n----------------------------------\033[30m\n\n";
    srand((unsigned)time(NULL));
    AdventureQuest game;
    game.playGame();
    cout << "\033[37m";
    return 0;
}
