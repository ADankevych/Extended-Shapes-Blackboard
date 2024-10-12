#include <iostream>
#include <vector>
#include <map>
#include <fstream>
using namespace std;

const int BOARD_WIDTH = 90;
const int BOARD_HEIGHT = 30;

vector<vector<char>> grid(BOARD_HEIGHT, vector<char>(BOARD_WIDTH, ' '));
map<int, map<int, int>> countOfStars;
map<int, string> allShapes;
int currentID = 1;

void DeleteOrCreate(int x, int y, int index) {
    if (index == -1) {
        countOfStars[x][y]--;
        if (countOfStars[x][y] == 0) {
            grid[x][y] = ' ';
        }
    } else {
        countOfStars[x][y]++;
        grid[x][y] = '*';

    }
}


void lineParser(string &input, int &x, int &y, int &z) {
    int pos = input.find(' ');
    x = stoi(input.substr(0, pos));
    string remaining = input.substr(pos + 1);
    pos = remaining.find(' ');
    y = stoi(remaining.substr(0, pos));
    z = stoi(remaining.substr(pos + 1));
}

void lineParserFourElements(string &input, int &x, int &y, int &z, int &a) {
    int pos = input.find(' ');
    x = stoi(input.substr(0, pos));
    string remaining = input.substr(pos + 1);
    pos = remaining.find(' ');
    y = stoi(remaining.substr(0, pos));
    remaining = remaining.substr(pos + 1);
    pos = remaining.find(' ');
    z = stoi(remaining.substr(0, pos));
    a = stoi(remaining.substr(pos + 1));
}


struct Board {
    void print() {
        for (int i = 0; i < BOARD_WIDTH / 2; i++) {
            cout << " -";
        }
        cout << "\n";
        for (auto &row: grid) {
            cout << "|";
            for (char c: row) {
                cout << c;
            }
            cout << "|\n";
        }
        for (int i = 0; i < BOARD_WIDTH / 2; i++) {
            cout << " -";
        }
    }
};


class Figure {
public:
    virtual void draw(string input, int index) = 0;

};

class Triangle : public Figure {
public:
    void draw(string input, int deleteOrCreate) override {
        allShapes[currentID] = " triangle "+input;
        currentID++;

        int x, y, height;
        lineParser(input, x, y, height);

        if (height <= 0) {
            cout << "Invalid input" << endl;
            return;
        }

        for (int i = 0; i < height; i++) {
            int leftMost = x - i;
            int rightMost = x + i;
            int posY = y + i;

            if (posY < BOARD_HEIGHT) {
                if (leftMost >= 0 && leftMost < BOARD_WIDTH) {
                    DeleteOrCreate(posY, leftMost, deleteOrCreate);
                }
                if (rightMost >= 0 && rightMost < BOARD_WIDTH && leftMost != rightMost) {
                    DeleteOrCreate(posY, rightMost, deleteOrCreate);
                }
            }
        }
        for (int j = 0; j < 2 * height - 1; j++) {
            int baseX = x - height + 1 + j;
            int baseY = y + height - 1;

            if (baseX >= 0 && baseX < BOARD_WIDTH && baseY < BOARD_HEIGHT) {
                DeleteOrCreate(baseY, baseX, deleteOrCreate);
            }
        }
    }

};

class Rectangle : public Figure {
public:
    void draw(string input, int deleteOrCreate) override {
        allShapes[currentID] = " rectangle "+input;
        currentID++;

        int x, y, width, height;
        lineParserFourElements(input, x, y, width, height);

        if (width <= 0 || height <= 0) {
            cout << "Invalid input" << endl;
            return;
        }

        for(int i = y; i < y + height; i++) {
            for(int j = x; j < x + width; j++) {
                if(i == y || i == y + height - 1 || j == x || j == x + width - 1) {
                    DeleteOrCreate(i, j, deleteOrCreate);
                }
            }
        }

    }
};

class Circle : public Figure {
public:
    void draw(string input, int deleteOrCreate) override {
        allShapes[currentID] = " circle "+input;
        currentID++;

        int x, y, radius;
        lineParser(input, x, y, radius);

        if (radius <= 0) {
            cout << "Invalid input" << endl;
            return;
        }

        for(int i = y - radius; i <= y + radius; i++) {
            for(int j = x - radius; j <= x + radius; j++) {
                if((i - y) * (i - y) + (j - x) * (j - x) <= radius * radius &&
                   (i - y) * (i - y) + (j - x) * (j - x) >= (radius-0.5) * (radius-0.5) ) {
                    if(i >= 0 && i < BOARD_HEIGHT && j >= 0 && j < BOARD_WIDTH) {
                        DeleteOrCreate(i, j, deleteOrCreate);
                    }
                }
            }
        }
    }

};

class Square : public Figure {
public:
    void draw(string input, int deleteOrCreate) override {
        allShapes[currentID] = " square "+input;
        currentID++;

        int x, y, length;
        lineParser(input, x, y, length);

        if (length <= 0) {
            cout << "Invalid input" << endl;
            return;
        }

        for(int i = y; i < y + length; i++) {
            for(int j = x; j < x + length; j++) {
                if(i == y || i == y + length - 1 || j == x || j == x + length - 1) {
                    DeleteOrCreate(i, j, deleteOrCreate);
                }
            }
        }
    }
};

class Line : public Figure {
public:
    void draw(string input, int deleteOrCreate) override {
        allShapes[currentID] = " line "+input;
        currentID++;

        int xStart, yStart, xEnd, yEnd;
        lineParserFourElements(input, xStart, yStart, xEnd, yEnd);

        for (int i = 0; i < max(abs(xEnd - xStart), abs(yEnd - yStart)); i++) {
            int x = xStart + ((xEnd - xStart) * i) / max(abs(xEnd - xStart), abs(yEnd - yStart));
            int y = yStart + ((yEnd - yStart) * i) / max(abs(xEnd - xStart), abs(yEnd - yStart));
            if (x >= 0 && x < BOARD_WIDTH && y >= 0 && y < BOARD_HEIGHT) {
                DeleteOrCreate(y, x, deleteOrCreate);
            }
        }
    }

};


void findShape(string info, int index) {
    int pos = info.find(' ');
    string shape = info.substr(0, pos);

    unique_ptr<Figure> figure;

    if (shape == "triangle") {
        figure = make_unique<Triangle>();
    } else if (shape == "rectangle") {
        figure = make_unique<Rectangle>();
    } else if (shape == "circle") {
        figure = make_unique<Circle>();
    } else if (shape == "square") {
        figure = make_unique<Square>();
    } else if (shape == "line") {
        figure = make_unique<Line>();
    }

    if (figure) {
        figure->draw(info.substr(pos + 1), index);
    }
}

void printMenu() {
    cout << "Hello, welcome to shapes blackboard! \n"
         << "Enter one of the commands: \n"
         << "\t draw - Draw blackboard to the console \n"
         << "\t list - Print all added shapes with their IDs and parameters \n"
         << "\t shapes - Print a list of all available shapes and parameters for add call \n"
         << "\t add - Add shape with specified colour and fill mode to the blackboard \n"
         << "\t select - Select a shape with an ID or foreground shape by the selected coordinates \n"
         << "\t remove - Remove the selected shape from the blackboard \n"
         << "\t edit - Allows to modify the parameters of the selected figure \n"
         << "\t paint - Change the colour of the selected figure \n"
         << "\t move - Move the selected shape to the coordinates \n"
         << "\t clear - Remove all shapes from blackboard \n"
         << "\t save - Save the blackboard to the file \n"
         << "\t load - Load a blackboard from the file \n"
         << "\t exit - Exit the program \n";
}

void printShapes() {
    cout << "Before figure you should mention 'fill' or 'frame' mode, and colour of the figure\n"
         << "triangle - coordinates of the top(upper) point and height of the triangle\n"
         << "rectangle - coordinates of the upper left point and width height\n"
         << "circle - coordinates of the center and radius\n"
         << "square - coordinates of the upper left point and side length\n"
         << "line - coordinates of start and end points\n";
}

void printAllShapes() {
    for (auto &[id, shape] : allShapes) {
        cout << id << shape << "\n";
    }
}

void clearBoard() {
    grid = vector<vector<char>>(BOARD_HEIGHT, vector<char>(BOARD_WIDTH, ' '));
    countOfStars.clear();
    allShapes.clear();
    currentID = 1;
}

void saveBoard(const string &fileName) {
    ofstream file(fileName);
    for (int i = 0; i < BOARD_HEIGHT; i++) {
        for (int j = 0; j < BOARD_WIDTH; j++) {
            file << grid[i][j];
        }
        file << "\n";
    }
    file.close();
}

void loadBoard(const string &fileName) {
    clearBoard();
    ifstream file(fileName);
    string line;
    int i = 0;
    while (getline(file, line) && i < BOARD_HEIGHT) {
        for (int j = 0; j < min(static_cast<int>(line.size()), BOARD_WIDTH); j++) {
            grid[i][j] = line[j];
        }
        i++;
    }
    file.close();
}

int main() {
    printMenu();
    Board board;

    while (true) {
        cout << "\nEnter your command: ";
        string command;
        getline(cin, command);

        if (command == "exit") {
            break;
        } else if (command == "draw") {
            board.print();
        } else if (command == "list") {
            printAllShapes();
        } else if (command == "shapes") {
            printShapes();
        } else if (command == "add") {
            cout << "Enter the shape: ";
            string shapeInput;
            getline(cin, shapeInput);
            findShape(shapeInput, 1);
        } else if (command == "remove") {
            cout << "Enter shape id: ";
            int shapeID;
            cin >> shapeID;
            cin.ignore();
            findShape(allShapes[shapeID].substr(1), -1);
            allShapes.erase(shapeID);
        } else if (command == "clear") {
            clearBoard();
        } else if (command == "save") {
            cout << "Enter file name to save: ";
            string fileName;
            getline(cin, fileName);
            saveBoard(fileName);
        } else if (command == "load") {
            cout << "Enter file name to load: ";
            string fileName;
            getline(cin, fileName);
            loadBoard(fileName);
        }
    }

    return 0;
}
