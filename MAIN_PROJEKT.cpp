#include <iostream>
#include <vector>
#include <sstream>
using namespace std;

const int BOARD_SIZE = 10;

enum CellState {
    EMPTY,
    SHIP,
    HIT,
    MISS
};

class Ship {
public:
    string name;
    int size;
    int hits = 0;
    vector<pair<int, int>> positions;  // Pozycje wszystkich komórek statku

    Ship(string name, int size) : name(name), size(size) {}

    // Funkcja sprawdzająca, czy statek jest zatopiony
    bool isSunk() const {
        return hits == size;
    }

    // Funkcja dodająca pozycję do statku
    void addPosition(int x, int y) {
        positions.push_back({x, y});
    }
};

class Board {
private:
    vector<vector<CellState>> grid;
    vector<Ship> ships;  // Wektor przechowujący wszystkie statki

public:
    Board() : grid(BOARD_SIZE, vector<CellState>(BOARD_SIZE, EMPTY)) {}

    bool isValidPosition(int x, int y) const {
        return x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE;
    }

    bool canPlaceShip(int x, int y, int size, bool horizontal) const {
        int dx = horizontal ? 0 : 1;
        int dy = horizontal ? 1 : 0;

        for (int i = 0; i < size; i++) {
            int nx = x + i * dx;
            int ny = y + i * dy;
            
            if (!isValidPosition(nx, ny) || grid[nx][ny] != EMPTY)
                return false;

            for (int xx = nx - 1; xx <= nx + 1; xx++) {
                for (int yy = ny - 1; yy <= ny + 1; yy++) {
                    if (isValidPosition(xx, yy) && grid[xx][yy] == SHIP) {
                        return false;
                    }
                }
            }
        }
        return true;
    }

    bool placeShip(int x, int y, const Ship &ship, bool horizontal) {
        if (!canPlaceShip(x, y, ship.size, horizontal)) {
            return false;
        }

        int dx = horizontal ? 0 : 1;
        int dy = horizontal ? 1 : 0;
        Ship newShip = ship;

        for (int i = 0; i < ship.size; i++) {
            int nx = x + i * dx;
            int ny = y + i * dy;
            grid[nx][ny] = SHIP;
            newShip.addPosition(nx, ny);
        }

        ships.push_back(newShip);  // Dodajemy statek do listy statków
        return true;
    }

    bool shoot(int x, int y) {
        if (!isValidPosition(x, y)) {
            cout << "Nieprawidłowe współrzędne. Spróbuj ponownie." << endl;
            return false;
        }

        if (grid[x][y] == HIT || grid[x][y] == MISS) {
            cout << "Już strzelałeś w to miejsce. Wybierz inne pole." << endl;
            return false;
        }

        if (grid[x][y] == SHIP) {
            grid[x][y] = HIT;
            for (auto& ship : ships) {
                for (auto& pos : ship.positions) {
                    if (pos.first == x && pos.second == y) {
                        ship.hits++;
                        if (ship.isSunk()) {
                            cout << "Trafiony, zatopiony! (" << ship.name << ")" << endl;
                        } else {
                            cout << "Trafiony!" << endl;
                        }
                        return true;  // Trafienie, gracz może strzelać dalej
                    }
                }
            }
        } else {
            grid[x][y] = MISS;
            cout << "Pudło!" << endl;
            return false;  // Nietrafienie, zmiana gracza
        }
        return false;
    }

    bool allShipsSunk() const {
        for (const auto& ship : ships) {
            if (!ship.isSunk()) {
                return false;
            }
        }
        return true;
    }

    void display(bool showShips) const {
        cout << "  ";
        for (int i = 0; i < BOARD_SIZE; i++) cout << i << " ";
        cout << endl;

        for (int i = 0; i < BOARD_SIZE; i++) {
            cout << i << " ";
            for (int j = 0; j < BOARD_SIZE; j++) {
                if (grid[i][j] == HIT)
                    cout << "X ";
                else if (grid[i][j] == MISS)
                    cout << "O ";
                else if (showShips && grid[i][j] == SHIP)
                    cout << "S ";
                else
                    cout << ". ";
            }
            cout << endl;
        }
    }
};

class Player {
private:
    Board board;

public:
    void placeShips() {
        vector<Ship> ships = {
            {"Jednomasztowiec", 1}, {"Jednomasztowiec", 1}, {"Jednomasztowiec", 1}, {"Jednomasztowiec", 1},
            {"Dwumasztowiec", 2}, {"Dwumasztowiec", 2}, {"Dwumasztowiec", 2},
            {"Trzymasztowiec", 3}, {"Trzymasztowiec", 3},
            {"Czteromasztowiec", 4}
        };

        for (const auto& ship : ships) {
            int x, y;
            char orientation;
            bool placed = false;

            while (!placed) {
                cout << "Umieść statek " << ship.name << " (rozmiar " << ship.size << ").\n";
                cout << "Podaj współrzędne (x y) i orientację (h - poziomo, v - pionowo): ";
                
                string input;
                getline(cin, input);
                stringstream ss(input);
                
                if (ss >> x >> y >> orientation && (orientation == 'h' || orientation == 'v')) {
                    bool horizontal = (orientation == 'h');
                    placed = board.placeShip(x, y, ship, horizontal);

                    if (!placed) {
                        cout << "Nie można ustawić statku w tej pozycji. Spróbuj ponownie.\n";
                    } else {
                        cout << "Aktualna plansza po umieszczeniu statku:\n";
                        board.display(true);
                    }
                } else {
                    cout << "Niepoprawne dane wejściowe. Spróbuj ponownie.\n";
                }
            }
        }
    }

    void displayBoard(bool showShips = false) const {
        board.display(showShips);
    }

    bool attack(Player &opponent, int x, int y) {
        return opponent.board.shoot(x, y);
    }

    bool hasLost() const {
        return board.allShipsSunk();
    }
};

void clearScreen() {
    for (int i = 0; i < 30; ++i) {
        cout << endl;
    }
}

int main() {
    Player player1, player2;

    cout << "Gracz 1, ustaw swoje statki." << endl;
    player1.placeShips();
    clearScreen();

    cout << "Gracz 2, ustaw swoje statki." << endl;
    player2.placeShips();
    clearScreen();

    int x, y;
    bool player1Turn = true;
    while (true) {
        bool validShot;
        do {
            cout << (player1Turn ? "\nGracz 1" : "\nGracz 2") << ", wybierz pole do ataku (x y): ";
            cin >> x >> y;

            validShot = player1Turn ? player1.attack(player2, x, y) : player2.attack(player1, x, y);

            // Wyświetlenie plansz po strzale
            if (player1Turn) {
                cout << "\nPlansza Gracza 1 (Twoja):" << endl;
                player1.displayBoard(true);
                cout << "\nPlansza Gracza 2 (Trafienia/Pudła):" << endl;
                player2.displayBoard(false);
            } else {
                cout << "\nPlansza Gracza 2 (Twoja):" << endl;
                player2.displayBoard(true);
                cout << "\nPlansza Gracza 1 (Trafienia/Pudła):" << endl;
                player1.displayBoard(false);
            }

            // Sprawdzenie, czy przeciwnik stracił wszystkie statki
            if (player1Turn && player2.hasLost()) {
                cout << "Gracz 1 wygrał!" << endl;
                return 0;
            } else if (!player1Turn && player1.hasLost()) {
                cout << "Gracz 2 wygrał!" << endl;
                return 0;
            }

        } while (validShot);  // Gracz może strzelać ponownie, jeśli trafi

        // Zmiana tury po pudle
        cout << "\nNaciśnij Enter, aby przekazać turę przeciwnikowi...";
        cin.ignore();
        cin.get();
        clearScreen();

        player1Turn = !player1Turn;
    }

    return 0;
}
