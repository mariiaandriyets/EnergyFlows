#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <chrono>

// Константы
const int GRID_SIZE = 12;
const int CELL_SIZE = 50;
const int WINDOW_SIZE = GRID_SIZE * CELL_SIZE;
const int ENERGY_SOURCE_TIME = 1000; // время появления источника энергии в миллисекундах

// Структуры данных
struct Cell {
    bool isDevice = false;
    bool isEnergySource = false;
    int energy = 0;
    sf::RectangleShape shape;
};

std::vector<std::vector<Cell>> grid(GRID_SIZE, std::vector<Cell>(GRID_SIZE));

// Функция для случайного размещения устройства на сетке
void placeDevice() {
    int x = std::rand() % GRID_SIZE;
    int y = std::rand() % GRID_SIZE;
    grid[x][y].isDevice = true;
    grid[x][y].energy = 0;
    grid[x][y].shape.setFillColor(sf::Color::Blue);
}

// Функция для случайного появления источника энергии на сетке
void placeEnergySource() {
    int x = std::rand() % GRID_SIZE;
    int y = std::rand() % GRID_SIZE;
    grid[x][y].isEnergySource = true;
    grid[x][y].shape.setFillColor(sf::Color::Yellow);
}

// Перечисление для хранения состояния игры
enum class GameState {
    StartScreen,
    Playing,
    GameOver
};

GameState gameState = GameState::StartScreen;

// Основная функция
int main() {
    std::srand(std::time(nullptr));

    // Создаем окно
    sf::RenderWindow window(sf::VideoMode(WINDOW_SIZE, WINDOW_SIZE), "Energy Flows");

    // Шрифт для отображения текста
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        return -1; // убедитесь, что у вас есть шрифт arial.ttf в папке с проектом
    }

    // Текст для кнопки "Старт"
    sf::Text startText;
    startText.setFont(font);
    startText.setString("Start");
    startText.setCharacterSize(24);
    startText.setFillColor(sf::Color::White);
    startText.setStyle(sf::Text::Bold);

    // Кнопка "Старт" - фон
    sf::RectangleShape startButton;
    startButton.setSize(sf::Vector2f(200, 50));
    startButton.setFillColor(sf::Color(100, 100, 250));
    startButton.setOutlineColor(sf::Color::White);
    startButton.setOutlineThickness(3);
    startButton.setPosition(WINDOW_SIZE / 2 - startButton.getSize().x / 2, WINDOW_SIZE / 2 - startButton.getSize().y / 2);

    // Позиционируем текст в центре кнопки
    startText.setPosition(
        startButton.getPosition().x + (startButton.getSize().x - startText.getLocalBounds().width) / 2,
        startButton.getPosition().y + (startButton.getSize().y - startText.getLocalBounds().height) / 2 - 5
    );

    // Заполняем сетку устройствами
    for (int i = 0; i < 10; ++i) {
        placeDevice();
    }

    // Инициализация сетки
    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            grid[i][j].shape.setSize(sf::Vector2f(CELL_SIZE - 1, CELL_SIZE - 1));
            grid[i][j].shape.setPosition(i * CELL_SIZE, j * CELL_SIZE);
            if (!grid[i][j].isDevice) {
                grid[i][j].shape.setFillColor(sf::Color::White);
            }
        }
    }

    // Основной игровой цикл
    auto lastEnergySourceTime = std::chrono::steady_clock::now();
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
                }
            } else if (gameState == GameState::Playing && event.type == sf::Event::MouseButtonPressed) {
                int x = event.mouseButton.x / CELL_SIZE;
                int y = event.mouseButton.y / CELL_SIZE;

                if (grid[x][y].isEnergySource) {
                    if (grid[x][y].isDevice) {
                        grid[x][y].energy += 10;
                        grid[x][y].shape.setFillColor(sf::Color::Blue);
                    }
                    grid[x][y].isEnergySource = false;
                } else if (grid[x][y].isDevice) {
                    grid[x][y].energy -= 5;
                }
            }
        }

        if (gameState == GameState::Playing) {
            auto now = std::chrono::steady_clock::now();
            if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastEnergySourceTime).count() > ENERGY_SOURCE_TIME) {
                for (int i = 0; i < GRID_SIZE; ++i) {
                    for (int j = 0; j < GRID_SIZE; ++j) {
                        if (grid[i][j].isEnergySource) {
                            grid[i][j].isEnergySource = false;
                            if (!grid[i][j].isDevice) {
                                grid[i][j].shape.setFillColor(sf::Color::White);
                            } else {
                                grid[i][j].shape.setFillColor(sf::Color::Blue);
                            }
                        }
                    }
                }
                placeEnergySource();
                lastEnergySourceTime = now;
            }
        }

        // Отрисовка
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
        }

        window.display();
    }

    return 0;
}
