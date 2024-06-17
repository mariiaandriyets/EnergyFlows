#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <iostream>

const int GRID_SIZE = 8;
const int CELL_SIZE = 50;
const int WINDOW_SIZE = GRID_SIZE * CELL_SIZE;
const int INITIAL_ENERGY_SOURCE_TIME = 1000;
const int ENERGY_VISIBLE_TIME = 1000;
const int LEVEL_INCREASE_DIFFICULTY = 100;
const int MAX_LEVEL = 10;
const int DEVICES_TO_ACTIVATE = 5;
const int LEVEL_NOTIFICATION_TIME = 2000;

struct Cell {
    bool isDevice = false;
    bool isEnergySource = false;
    bool isProcessed = false;
    int energy = 0;
    sf::RectangleShape shape;
};

std::vector<std::vector<Cell>> grid(GRID_SIZE, std::vector<Cell>(GRID_SIZE));

void placeDevice() {
    int x = std::rand() % GRID_SIZE;
    int y = std::rand() % GRID_SIZE;
    if (!grid[x][y].isDevice && !grid[x][y].isProcessed) {
        grid[x][y].isDevice = true;
        grid[x][y].energy = 0;
        grid[x][y].shape.setFillColor(sf::Color::Blue);
    } else {
        placeDevice();
    }
}

void placeEnergySource() {
    int x = std::rand() % GRID_SIZE;
    int y = std::rand() % GRID_SIZE;
    if (!grid[x][y].isEnergySource && !grid[x][y].isProcessed) {
        grid[x][y].isEnergySource = true;
        grid[x][y].shape.setFillColor(sf::Color::Yellow);
    } else {
        placeEnergySource();
    }
}

void placeEnergySourceOnDevice() {
    std::vector<std::pair<int, int>> devices;
    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            if (grid[i][j].isDevice && !grid[i][j].isProcessed && !grid[i][j].isEnergySource) {
                devices.push_back({i, j});
            }
        }
    }
    if (!devices.empty()) {
        int index = std::rand() % devices.size();
        grid[devices[index].first][devices[index].second].isEnergySource = true;
        grid[devices[index].first][devices[index].second].shape.setFillColor(sf::Color::Yellow);
    }
}

enum class GameState {
    StartScreen,
    Playing
};

GameState gameState = GameState::StartScreen;
int currentLevel = 1;
int energySourceTime = INITIAL_ENERGY_SOURCE_TIME;
int devicesActivated = 0;
bool showLevelNotification = false;
sf::Clock levelNotificationClock;
std::chrono::steady_clock::time_point lastEnergySourceTime;
std::chrono::steady_clock::time_point energyVisibleStartTime;
bool energyVisible = false;

void nextLevel() {
    if (currentLevel < MAX_LEVEL) {
        currentLevel++;
        energySourceTime -= LEVEL_INCREASE_DIFFICULTY;
        devicesActivated = 0;
        for (int i = 0; i < GRID_SIZE; ++i) {
            for (int j = 0; j < GRID_SIZE; ++j) {
                grid[i][j].isDevice = false;
                grid[i][j].isEnergySource = false;
                grid[i][j].isProcessed = false;
                grid[i][j].energy = 0;
                grid[i][j].shape.setFillColor(sf::Color::White);
            }
        }
        for (int i = 0; i < 5; ++i) {
            placeDevice();
        }
        showLevelNotification = true;
        levelNotificationClock.restart();
        std::cout << "Перешли на уровень " << currentLevel << std::endl;
    }
}

bool isLevelComplete() {
    return devicesActivated >= DEVICES_TO_ACTIVATE;
}

int main() {
    std::srand(std::time(nullptr));
    sf::RenderWindow window(sf::VideoMode(WINDOW_SIZE, WINDOW_SIZE), "Energy Flows");
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        return -1;
    }
    sf::Text startText;
    startText.setFont(font);
    startText.setString("Start");
    startText.setCharacterSize(24);
    startText.setFillColor(sf::Color::White);
    startText.setStyle(sf::Text::Bold);
    sf::RectangleShape startButton;
    startButton.setSize(sf::Vector2f(200, 50));
    startButton.setFillColor(sf::Color(100, 100, 250));
    startButton.setOutlineColor(sf::Color::White);
    startButton.setOutlineThickness(3);
    startButton.setPosition(WINDOW_SIZE / 2 - startButton.getSize().x / 2, WINDOW_SIZE / 2 - startButton.getSize().y / 2);
    startText.setPosition(
        startButton.getPosition().x + (startButton.getSize().x - startText.getLocalBounds().width) / 2,
        startButton.getPosition().y + (startButton.getSize().y - startText.getLocalBounds().height) / 2 - 5
    );
    sf::Text levelText;
    levelText.setFont(font);
    levelText.setCharacterSize(24);
    levelText.setFillColor(sf::Color::Black);
    levelText.setStyle(sf::Text::Bold);
    levelText.setPosition(10, 10);
    sf::Text levelNotificationText;
    levelNotificationText.setFont(font);
    levelNotificationText.setCharacterSize(30);
    levelNotificationText.setFillColor(sf::Color::Green);
    levelNotificationText.setStyle(sf::Text::Bold);
    levelNotificationText.setString("New Level!");
    levelNotificationText.setPosition(WINDOW_SIZE / 2 - levelNotificationText.getLocalBounds().width / 2, WINDOW_SIZE / 2 - levelNotificationText.getLocalBounds().height / 2);
    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            grid[i][j].shape.setSize(sf::Vector2f(CELL_SIZE - 1, CELL_SIZE - 1));
            grid[i][j].shape.setPosition(i * CELL_SIZE, j * CELL_SIZE);
            grid[i][j].shape.setFillColor(sf::Color::White);
        }
    }
    lastEnergySourceTime = std::chrono::steady_clock::now();
    energyVisibleStartTime = std::chrono::steady_clock::now();
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (gameState == GameState::StartScreen && event.type == sf::Event::MouseButtonPressed) {
                if (startButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                    gameState = GameState::Playing;
                    lastEnergySourceTime = std::chrono::steady_clock::now();
                    energyVisibleStartTime = std::chrono::steady_clock::now();
                    energyVisible = false;
                    currentLevel = 1;
                    energySourceTime = INITIAL_ENERGY_SOURCE_TIME;
                    devicesActivated = 0;
                    for (int i = 0; i < GRID_SIZE; ++i) {
                        for (int j = 0; j < GRID_SIZE; ++j) {
                            grid[i][j].isDevice = false;
                            grid[i][j].isEnergySource = false;
                            grid[i][j].isProcessed = false;
                            grid[i][j].energy = 0;
                            grid[i][j].shape.setFillColor(sf::Color::White);
                        }
                    }
                    for (int i = 0; i < 5; ++i) {
                        placeDevice();
                    }
                }
            } else if (gameState == GameState::Playing && event.type == sf::Event::MouseButtonPressed) {
                int x = event.mouseButton.x / CELL_SIZE;
                int y = event.mouseButton.y / CELL_SIZE;
                if (grid[x][y].isEnergySource) {
                    if (grid[x][y].isDevice && !grid[x][y].isProcessed) {
                        grid[x][y].energy += 10;
                        devicesActivated++;
                        grid[x][y].isProcessed = true;
                        grid[x][y].shape.setFillColor(sf::Color::Green);
                        std::cout << "Активировано устройств: " << devicesActivated << std::endl;
                    }
                    grid[x][y].isEnergySource = false;
                    grid[x][y].shape.setFillColor(sf::Color::Green);
                    std::cout << "Энергия собрана в ячейке: (" << x << ", " << y << ")" << std::endl;
                    if (isLevelComplete()) {
                        nextLevel();
                    }
                } else if (grid[x][y].isDevice) {
                    grid[x][y].energy -= 5;
                }
            }
        }
        if (gameState == GameState::Playing) {
            auto now = std::chrono::steady_clock::now();
            if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastEnergySourceTime).count() > energySourceTime) {
                for (int i = 0; i < GRID_SIZE; ++i) {
                    for (int j = 0; j < GRID_SIZE; ++j) {
                        if (grid[i][j].isEnergySource) {
                            grid[i][j].isEnergySource = false;
                            if (!grid[i][j].isDevice && !grid[i][j].isProcessed) {
                                grid[i][j].shape.setFillColor(sf::Color::White);
                            } else if (grid[i][j].isDevice) {
                                grid[i][j].shape.setFillColor(sf::Color::Blue);
                            }
                        }
                    }
                }
                if (std::rand() % 2 == 0) {
                    placeEnergySourceOnDevice();
                } else {
                    placeEnergySource();
                }
                lastEnergySourceTime = now;
                energyVisibleStartTime = now;
                energyVisible = true;
            }
            if (energyVisible && std::chrono::duration_cast<std::chrono::milliseconds>(now - energyVisibleStartTime).count() > ENERGY_VISIBLE_TIME) {
                for (int i = 0; i < GRID_SIZE; ++i) {
                    for (int j = 0; j < GRID_SIZE; ++j) {
                        if (grid[i][j].isEnergySource) {
                            grid[i][j].isEnergySource = false;
                            if (!grid[i][j].isDevice && !grid[i][j].isProcessed) {
                                grid[i][j].shape.setFillColor(sf::Color::White);
                            } else if (grid[i][j].isDevice) {
                                grid[i][j].shape.setFillColor(sf::Color::Blue);
                            }
                        }
                    }
                }
                energyVisible = false;
            }
        }
        window.clear();
        if (gameState == GameState::StartScreen) {
            window.draw(startButton);
            window.draw(startText);
        } else if (gameState == GameState::Playing) {
            for (int i = 0; i < GRID_SIZE; ++i) {
                for (int j = 0; j < GRID_SIZE; ++j) {
                    window.draw(grid[i][j].shape);
                }
            }
            levelText.setString("Level: " + std::to_string(currentLevel));
            window.draw(levelText);
            if (showLevelNotification) {
                window.draw(levelNotificationText);
                if (levelNotificationClock.getElapsedTime().asMilliseconds() > LEVEL_NOTIFICATION_TIME) {
                    showLevelNotification = false;
                }
            }
        }
        window.display();
    }
    return 0;
}
