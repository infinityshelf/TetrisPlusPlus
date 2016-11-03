#include "GridController.hpp"
#include "TetrisPiece.hpp"
#include <iostream>
#include <cassert>
#include <cstdlib>

const int windowWidth = pixels * COLUMNS * 2;
const int windowHeight = pixels * ROWS;
const bool debug = true;
const int pixels = 32.0f;
const char fileName[] = "grid.out";
int totalLinesCleared = 0;
int score = 0;
int level = 0;
bool last_clear_was_a_tetris = false;

// typedef enum TETROMINO_TYPES TetrominoType;

GridController * GridController::s_instance = nullptr;
sf::RenderWindow * GridController::m_windowref = nullptr;


GridController* GridController::instance() {
    static GridController instance = GridController();
    return &instance;
}

GridController::GridController() {
    totalLinesCleared = 0;
    for (int y = 0; y < ROWS; y++) {
        for (int x = 0; x < COLUMNS; x++) {
            this->setSpaceOccupied(x, y, TETROMINO_TYPE_NONE);
        }
    }
}

GridController::~GridController() {}

bool GridController::isSpaceOccupied(int x, int y) {
    bool occupado = false;
    if (x >= 0 && x < COLUMNS && y >= 0 && y < ROWS) {
        occupado = (this->grid[y][x] != TETROMINO_TYPE_NONE);
    }
    return occupado;
}

void GridController::setSpaceOccupied(int x, int y, TetrominoType type) {
    if (x >= 0 && x < COLUMNS && y >= 0 && y < ROWS) {
        this->grid[y][x] = type;
    }
}

char GridController::characterForType(TetrominoType type) {
    char place;
    switch(type) {
        case TETROMINO_TYPE_O: {
            place = 'O';
            break;
        }
        case TETROMINO_TYPE_I: {
            place = 'I';
            break;
        }
        case TETROMINO_TYPE_L: {
            place = 'L';
            break;
        }
        case TETROMINO_TYPE_J: {
            place = 'J';
            break;
        }
        case TETROMINO_TYPE_S: {
            place = 'S';
            break;
        }
        case TETROMINO_TYPE_Z: {
            place = 'Z';
            break;
        }
        case TETROMINO_TYPE_T: {
            place = 'T';
            break;
        }
        case TETROMINO_TYPE_MAX:{}
        case TETROMINO_TYPE_NONE: {
            place = '_';
            break;
        }
        default: {
            place = '#';
            break;
        }
    }
    return place;

}

TetrominoType GridController::typeForCharacter(char place) {
    TetrominoType type = TETROMINO_TYPE_NONE;
    switch(place) {
        case 'O': {
            type = TETROMINO_TYPE_O;
            break;
        }
        case 'I': {
            type = TETROMINO_TYPE_I;
            break;
        }
        case 'L': {
            type = TETROMINO_TYPE_L;
            break;
        }
        case 'J': {
            type = TETROMINO_TYPE_J;
            break;
        }
        case 'S': {
            type = TETROMINO_TYPE_S;
            break;
        }
        case 'Z': {
            type = TETROMINO_TYPE_Z;
            break;
        }
        case 'T': {
            type = TETROMINO_TYPE_T;
            break;
        }
        case '_': {
            type = TETROMINO_TYPE_NONE;
            break;
        }
        default: {
            type = TETROMINO_TYPE_NONE;
            break;
        }
    }
    return type;

}

void GridController::printGrid() {
    for (int y = 0; y < ROWS; y++) {
        for (int x = 0; x < COLUMNS; x++) {
            TetrominoType tetrominoType = this->grid[y][x];
            char place = GridController::characterForType(tetrominoType);
            std::cout << place;
            if (x == 9) {
                std::cout << std::endl;
            }
        }
    }
    std::cout << std::endl;
}

void GridController::saveGridToFile() {
    FILE *fp = fopen(fileName, "w");
    int length = ROWS * COLUMNS;
    char *gridString = (char *)malloc(sizeof(char) * length);
    for (int i = 0; i < length; i++) {
        gridString[i] = GridController::characterForType(this->grid[0][i]);
    }
    if (fp != NULL) {
        fwrite(gridString, sizeof(char), length, fp);
        fclose(fp);
    } else {
        std::cout << "COULD NOT OPEN FILE: " << fileName << "\007" << std::endl;
    }
    free(gridString);
}

void GridController::loadGridFromFile() {
    FILE *fp = fopen(fileName, "r");
    int length = ROWS * COLUMNS;
    char *gridString = (char *)malloc(sizeof(char) * length);
    if (fp != NULL) {
        fread(gridString, sizeof(char), length, fp);
        for (int i = 0; i < length; i++) {
            this->grid[0][i] = GridController::typeForCharacter(gridString[i]);
        }
        //this->printGrid();
        fclose(fp);
    } else {
        std::cout << "COULD NOT OPEN FILE: " << fileName <<  "\007" << std::endl;
    }
    free(gridString);
}

void GridController::provideWindow(sf::RenderWindow * window) {
    this->m_windowref = window;
}

sf::RenderWindow * GridController::window_instance() {
    return this->m_windowref;
}

void GridController::draw() {
    this->checkRows();
    for (int y = 0; y < ROWS; ++y) {
        for (int x = 0; x < COLUMNS; ++x) {
            sf::RectangleShape shape = Monomino::rectangleShapeForType(this->grid[y][x]);
            shape.setPosition(x*pixels, y*pixels);
            if (this->grid[y][x] != TETROMINO_TYPE_NONE) {
                this->m_windowref->draw(shape);
            }
        }
    }
    sf::RectangleShape bbox;
    sf::Vector2f size = sf::Vector2f((COLUMNS * pixels), (ROWS * pixels));
    bbox.setSize(size);
    bbox.setPosition(0,0);
    bbox.setFillColor(sf::Color(0x000000));
    bbox.setOutlineColor(sf::Color::White);
    bbox.setOutlineThickness(1);

    sf::Font font;
    font.loadFromFile("sansation.ttf");
    char scoreString[100];
    sprintf(scoreString, "LEVEL: %d\nLINES: %d\nSCORE: %d", level, totalLinesCleared, score);
    sf::Text text(scoreString, font, 40);
    text.setPosition((COLUMNS * 1.5 * pixels)-(text.getLocalBounds().width/2), ROWS * pixels *0.25);
    text.setFillColor(sf::Color::White);
    this->m_windowref->draw(text);
    this->m_windowref->draw(bbox);
}

void GridController::checkRows() {
    int linesCleared = 0;
    level = totalLinesCleared / 10;
    for (int row = 0; row < ROWS; row++) {
        bool cleared = true;
        for (int col = 0; col < COLUMNS; col++) {
            if (!this->isSpaceOccupied(col, row)) {
                cleared = false;
            }
        }
        if (cleared == true) {
            linesCleared++;
            this->clearRow(row);
            this->shiftRowsAbove(row);
        }
    }
    if (linesCleared == 4 && last_clear_was_a_tetris == true) {
        score += 2000 * (level + 1);
        last_clear_was_a_tetris = true;
    } else if (linesCleared == 4 && last_clear_was_a_tetris == false) {
        score += 1000 * (level + 1);
        last_clear_was_a_tetris = true;
    } else if (linesCleared == 2) {
        score += 150 * (level + 1);
        last_clear_was_a_tetris = false;
    } else if (linesCleared == 3) {
        score += 350 * (level + 1);
        last_clear_was_a_tetris = false;
    } else if (linesCleared == 1) {
        score += 50 * (level + 1);
        last_clear_was_a_tetris = false;
    }
}

void GridController::clearRow(int rowToClear) {
    for (int col = 0; col < COLUMNS; col++) {
        this->grid[rowToClear][col] = TETROMINO_TYPE_NONE;
    }
    totalLinesCleared++;
}

void GridController::shiftRowsAbove(int clearedRow) {
    if (clearedRow < ROWS) {
        for (int row = clearedRow; row > 0; row--) {
            for (int col = 0; col < COLUMNS; col++) {
                this->grid[row][col] = this->grid[row-1][col];
            }
        }
    }
}



