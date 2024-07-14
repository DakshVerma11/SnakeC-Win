#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <thread>
#include <conio.h> // For kbhit() and getch()
#include <fstream> // For file handling

using namespace std;

const int width = 20;
const int height = 15;
const char emptyChar = ' ';
const char snakeChar = 'O';
const char fruitChar = '@';

class Point {
public:
    int x, y;
    Point(int xVal, int yVal) : x(xVal), y(yVal) {}
};

class Snake {
private:
    vector<Point> body;
    int direction; // 0 - Up, 1 - Right, 2 - Down, 3 - Left
    int fruitX, fruitY;
    int score; // Score to track the number of fruits eaten
    int highScore; // High score

public:
    Snake() : score(0), highScore(loadHighScore()) {
        body.push_back(Point(width / 2, height / 2));
        body.push_back(Point(width / 2 - 1, height / 2));
        direction = 1; // Start moving to the right
        placeFruit(); // Randomly place initial fruit
    }

    void move() {
        int newX = body[0].x;
        int newY = body[0].y;

        switch (direction) {
            case 0: newY--; break; // Up
            case 1: newX++; break; // Right
            case 2: newY++; break; // Down
            case 3: newX--; break; // Left
        }

        // Check for collision with walls
        if (newX < 0 || newX >= width || newY < 0 || newY >= height) {
            endGame();
        }

        // Check for collision with itself (except head)
        for (int i = 1; i < body.size(); i++) {
            if (body[i].x == newX && body[i].y == newY) {
                endGame();
            }
        }

        // Check for eating fruit
        bool isFruitEaten = newX == fruitX && newY == fruitY;

        if (isFruitEaten) {
            score++; // Increase score
            body.insert(body.begin(), Point(newX, newY)); // Add new head
            placeFruit(); // Place new fruit
        } else {
            body.insert(body.begin(), Point(newX, newY)); // Add new head
            body.pop_back(); // Remove tail
        }
    }

    void changeDirection(char key) {
        switch (key) {
            case 'w':
                if (direction != 2) direction = 0; // Up
                break;
            case 'd':
                if (direction != 3) direction = 1; // Right
                break;
            case 's':
                if (direction != 0) direction = 2; // Down
                break;
            case 'a':
                if (direction != 1) direction = 3; // Left
                break;
        }
    }

    void draw() {
        // Draw the top border
        cout << string(width + 2, '#') << endl;

        for (int y = 0; y < height; y++) {
            cout << '#'; // Left border
            for (int x = 0; x < width; x++) {
                bool isSnake = false;
                for (const Point& p : body) {
                    if (p.x == x && p.y == y) {
                        isSnake = true;
                        break;
                    }
                }
                if (x == fruitX && y == fruitY) {
                    cout << fruitChar;
                } else if (isSnake) {
                    cout << snakeChar;
                } else {
                    cout << emptyChar;
                }
            }
            cout << '#'; // Right border
            cout << endl;
        }

        // Draw the bottom border
        cout << string(width + 2, '#') << endl;

        // Display the score
        cout << "Score: " << score << endl;
        cout << "High Score: " << highScore << endl;
    }

private:
    void placeFruit() {
        default_random_engine generator(random_device{}());
        uniform_int_distribution<int> distributionX(0, width - 1);
        uniform_int_distribution<int> distributionY(0, height - 1);

        do {
            fruitX = distributionX(generator);
            fruitY = distributionY(generator);
        } while (isCollidingWithSnake(fruitX, fruitY)); // Ensure fruit doesn't spawn on the snake
    }

    bool isCollidingWithSnake(int x, int y) {
        for (const Point& p : body) {
            if (p.x == x && p.y == y) {
                return true;
            }
        }
        return false;
    }

    int loadHighScore() {
        ifstream infile("HighScore.txt");
        int highScore = 0;
        if (infile) {
            infile >> highScore;
        }
        return highScore;
    }

    void saveHighScore() {
        if (score > highScore) {
            ofstream outfile("HighScore.txt");
            if (outfile) {
                outfile << score;
            }
        }
    }

    void endGame() {
        cout << "Game Over!" << endl;
        saveHighScore();
        exit(0);
    }
};

int main() {
    Snake snake;

    while (true) {
        // Clear screen
        system("cls"); 

        snake.draw();

        // Non-blocking input handling
        if (_kbhit()) {
            char input = getch();
            snake.changeDirection(input);
        }

        snake.move();

        // Delay for smoother gameplay
        this_thread::sleep_for(chrono::milliseconds(100));
    }

    return 0;
}
