#include <iostream>
#include <vector>
#include <map>
#include <fstream>
using namespace std;

const int BOARD_WIDTH = 90;
const int BOARD_HEIGHT = 30;

map<string, string> colorDictionary = { // taken from AI
        {"reset", "\033[0m"},
        {"black", "\033[30m"},
        {"red", "\033[31m"},
        {"green", "\033[32m"},
        {"yellow", "\033[33m"},
        {"blue", "\033[34m"},
        {"magenta", "\033[35m"},
        {"cyan", "\033[36m"},
        {"white", "\033[37m"}
};

vector<vector<string>> grid(BOARD_HEIGHT, vector<string>(BOARD_WIDTH, " "));
map<int, string> allShapes;
int currentID = 1;
int selectedID = 0;
int selectedX = 0;
int selectedY = 0;


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
            for (string c: row) {
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
    virtual void draw(string input, string fillOrFrame, string color) = 0;

};

class Triangle : public Figure {
public:
    void draw(string input, string fillOrFrame, string color) override {
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
                    grid[posY][leftMost] = colorDictionary[color] + "*" + colorDictionary["reset"];
                }
                if (rightMost >= 0 && rightMost < BOARD_WIDTH && leftMost != rightMost) {
                    grid[posY][rightMost] = colorDictionary[color] + "*" + colorDictionary["reset"];
                }
            }
        }
        for (int j = 0; j < 2 * height - 1; j++) {
            int baseX = x - height + 1 + j;
            int baseY = y + height - 1;

            if (baseX >= 0 && baseX < BOARD_WIDTH && baseY < BOARD_HEIGHT) {
                grid[baseY][baseX] = colorDictionary[color] + "*" + colorDictionary["reset"];
            }
        }

        if (fillOrFrame == "fill") {
            for (int i = 1; i < height; i++) {
                for (int j = x - i + 1; j < x + i; j++) {
                    if (i>=0 && j >= 0 && j < BOARD_WIDTH && y + i < BOARD_HEIGHT) {
                        grid[y + i][j] = colorDictionary[color] + "*" + colorDictionary["reset"];
                    }
                }
            }
        }
    }

};

class Rectangle : public Figure {
public:
    void draw(string input, string fillOrFrame, string color) override {
        int x, y, width, height;
        lineParserFourElements(input, x, y, width, height);

        if (width <= 0 || height <= 0) {
            cout << "Invalid input" << endl;
            return;
        }

        for(int i = y; i < y + height; i++) {
            for(int j = x; j < x + width; j++) {
                if(i == y || i == y + height - 1 || j == x || j == x + width - 1) {
                    grid[i][j] = colorDictionary[color] + "*" + colorDictionary["reset"];
                }
            }
        }

        if (fillOrFrame == "fill") {
            for (int i = y + 1; i < y + height - 1; i++) {
                for (int j = x + 1; j < x + width - 1; j++) {
                    if (i>=0 && j >= 0 && j < BOARD_WIDTH && i < BOARD_HEIGHT) {
                        grid[i][j] = colorDictionary[color] + "*" + colorDictionary["reset"];
                    }
                }
            }
        }
    }
};

class Circle : public Figure {
public:
    void draw(string input, string fillOrFrame, string color) override {
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
                        grid[i][j] = colorDictionary[color] + "*" + colorDictionary["reset"];
                    }
                }
            }
        }

        if (fillOrFrame == "fill") {
            for (int i = y - radius + 1; i < y + radius; i++) {
                for (int j = x - radius + 1; j < x + radius; j++) {
                    if ((i - y) * (i - y) + (j - x) * (j - x) <= radius * radius) {
                        if (i >= 0 && i < BOARD_HEIGHT && j >= 0 && j < BOARD_WIDTH) {
                            grid[i][j] = colorDictionary[color] + "*" + colorDictionary["reset"];
                        }
                    }
                }
            }
        }
    }

};

class Square : public Figure {
public:
    void draw(string input, string fillOrFrame, string color) override {
        int x, y, length;
        lineParser(input, x, y, length);

        if (length <= 0) {
            cout << "Invalid input" << endl;
            return;
        }

        for(int i = y; i < y + length; i++) {
            for(int j = x; j < x + length; j++) {
                if(i == y || i == y + length - 1 || j == x || j == x + length - 1) {
                    grid[i][j] = colorDictionary[color] + "*" + colorDictionary["reset"];
                }
            }
        }

        if (fillOrFrame == "fill") {
            for (int i = y + 1; i < y + length - 1; i++) {
                for (int j = x + 1; j < x + length - 1; j++) {
                    if (i >= 0 && i < BOARD_HEIGHT && j >= 0 && j < BOARD_WIDTH) {
                        grid[i][j] = colorDictionary[color] + "*" + colorDictionary["reset"];
                    }
                }
            }
        }
    }
};

class Line : public Figure {
public:
    void draw(string input, string fillOrFrame, string color) override {
        int xStart, yStart, xEnd, yEnd;
        lineParserFourElements(input, xStart, yStart, xEnd, yEnd);

        for (int i = 0; i < max(abs(xEnd - xStart), abs(yEnd - yStart)); i++) {
            int x = xStart + ((xEnd - xStart) * i) / max(abs(xEnd - xStart), abs(yEnd - yStart));
            int y = yStart + ((yEnd - yStart) * i) / max(abs(xEnd - xStart), abs(yEnd - yStart));
            if (x >= 0 && x < BOARD_WIDTH && y >= 0 && y < BOARD_HEIGHT) {
                grid[y][x] = colorDictionary[color] + "*" + colorDictionary["reset"];
            }
        }

        if (fillOrFrame == "fill") {
            for (int i = 0; i < max(abs(xEnd - xStart), abs(yEnd - yStart)); i++) {
                int x = xStart + ((xEnd - xStart) * i) / max(abs(xEnd - xStart), abs(yEnd - yStart));
                int y = yStart + ((yEnd - yStart) * i) / max(abs(xEnd - xStart), abs(yEnd - yStart));
                if (x >= 0 && x < BOARD_WIDTH && y >= 0 && y < BOARD_HEIGHT) {
                    grid[y][x] = colorDictionary[color] + "*" + colorDictionary["reset"];
                }
            }
        }
    }
};


void findShape(string info, int createOrRewrite) {
    int pos = info.find(' ');
    string fillOrFrame = info.substr(0, pos);
    string remaining = info.substr(pos + 1);
    pos = remaining.find(' ');
    string color = remaining.substr(0, pos);
    remaining = remaining.substr(pos + 1);
    pos = remaining.find(' ');
    string shape = remaining.substr(0, pos);

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

    if (createOrRewrite == 1) {
        allShapes[currentID] = info;
        currentID++;
    }

    if (figure) {
        figure->draw(remaining.substr(pos + 1), fillOrFrame, color);
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
        cout << id << " " << shape << "\n";
    }
}

void clearBoard() {
    grid = vector<vector<string>>(BOARD_HEIGHT, vector<string>(BOARD_WIDTH, " "));
    allShapes.clear();
    currentID = 1;
}

void removeShape(string info) {
    int id = stoi(info);
    if (allShapes.find(id) == allShapes.end()) {
        cout << "Shape with ID " << id << " does not exist." << endl;
        return;
    }
    allShapes.erase(id);
}

void saveBoard(const string &fileName) {
    ofstream file(fileName);
    for (auto &[id, shape] : allShapes) {
        file << id << shape << "\n";
    }
    file.close();
}

void loadBoard(const string &fileName) {
    clearBoard();
    ifstream file(fileName);
    try {
        string line;
        while (getline(file, line)) {
            int pos = line.find(' ');
            int id = stoi(line.substr(0, pos));
            allShapes[id] = line.substr(pos + 1);
        }
    }
    catch (exception &error) {
        cout << "Invalid file format" << endl;
    }
    file.close();
}

bool pointsInside( int x, int y){
    for (auto &[id, shape] : allShapes) {
        int pos = shape.find(' ');
        string fillOrFrame = shape.substr(0, pos);
        string remaining = shape.substr(pos + 1);
        pos = remaining.find(' ');
        string color = remaining.substr(0, pos);
        remaining = remaining.substr(pos + 1);
        pos = remaining.find(' ');
        string shapeType = remaining.substr(0, pos);
        remaining = remaining.substr(pos + 1);
        int x1, y1;

        if (shapeType == "triangle") {
            int height;
            lineParser(remaining, x1, y1, height);

            if (x >= x1 - height && x <= x1 + height && y >= y1 && y <= y1 + height) {
                selectedID = id;
                return true;
            }
        } else if (shapeType == "rectangle") {
            int width, height;
            lineParserFourElements(remaining, x1, y1, width, height);

            if (x >= x1 && x <= x1 + width && y >= y1 && y <= y1 + height) {
                selectedID = id;
                return true;
            }
        } else if (shapeType == "circle") {
            int radius;
            lineParser(remaining, x1, y1, radius);

            if ((x - x1) * (x - x1) + (y - y1) * (y - y1) <= radius * radius) {
                selectedID = id;
                return true;
            }
        } else if (shapeType == "square") {
            int length;
            lineParser(remaining, x1, y1, length);

            if (x >= x1 && x <= x1 + length && y >= y1 && y <= y1 + length) {
                selectedID = id;
                return true;
            }
        } else if (shapeType == "line") {
            int x2, y2;
            lineParserFourElements(remaining, x1, y1, x2, y2);

            if (x >= min(x1, x2) && x <= max(x1, x2) && y >= min(y1, y2) && y <= max(y1, y2)) {
                selectedID = id;
                return true;
            }
        }
    }

    return false;
}

void selectShape(string info) {
    int pos = info.find(' ');
    if (pos == -1) {
        int id = stoi(info);
        if (allShapes.find(id) == allShapes.end()) {
            cout << "Shape with ID " << id << " does not exist." << endl;
            return;
        }
        selectedID = id;
    } else {
        int x = stoi(info.substr(0, pos));
        int y = stoi(info.substr(pos + 1));

        if (x >= 0 && x < BOARD_WIDTH && y >= 0 && y < BOARD_HEIGHT) {
            selectedID = 0;
            selectedX = x;
            selectedY = y;
            if (pointsInside(x, y)) {
                cout << "Selected: " << selectedID << " " << allShapes[selectedID] << endl;
            } else {
                cout << "No shape selected" << endl;
            }
        } else {
            cout << "Invalid coordinates" << endl;
        }
    }
}

void editShape(string info) {
    int positionOfParam = stoi(info.substr(0, info.find(' ')));
    string newParam = info.substr(info.find(' ') + 1);
    if (positionOfParam == 3){
        cout << "You can't change the shape type" << endl;
    } else {
        int i = 0;
        int pos = 0;
        string substring = "";
        string remaindering = allShapes[selectedID];
        while (i < positionOfParam - 1){
            i++;
            pos = remaindering.find(' ');
            substring += remaindering.substr(0, pos + 1);
            remaindering = remaindering.substr(pos + 1);

        }
        substring = substring + newParam + " " + remaindering.substr(remaindering.find(' ') + 1);
        allShapes[selectedID] = substring;

        grid = vector<vector<string>>(BOARD_HEIGHT, vector<string>(BOARD_WIDTH, " "));
        for (auto &[id, shape] : allShapes) {
            findShape(shape, 0);
        }
    }
}


int main() {
    printMenu();
    Board board;
    string input;

    while (true) {
        cout << "\nEnter the command: ";
        getline(cin, input);

        if (input == "exit") {
            break;
        } else if (input == "draw") {
            board.print();
        } else if (input == "list") {
            printAllShapes();
        } else if (input == "shapes") {
            printShapes();
        } else if (input == "clear") {
            clearBoard();
        } else {
            int pos = input.find(' ');
            string command = input.substr(0, pos);
            string info = input.substr(pos + 1);

            if (command == "add") {
                findShape(info, 1);
            }
            else if (command == "remove") {
                removeShape(allShapes[selectedID]);
                 grid = vector<vector<string>>(BOARD_HEIGHT, vector<string>(BOARD_WIDTH, " "));
                for (auto &[id, shape] : allShapes) {
                    findShape(shape, 0);
                }

            }
            else if (command == "save") {
                saveBoard(info);
            }
            else if (command == "load") {
                loadBoard(info);
            }
            else if (command == "select") {
                selectShape(info);
            }
            else if (command == "paint") {
                if (selectedID == 0) {
                    cout << "No shape selected" << endl;
                } else {
                    int pos = info.find(' ');
                    string color = info.substr(0, pos);
                    allShapes[selectedID] = color + allShapes[selectedID].substr(allShapes[selectedID].find(' '));
                    grid = vector<vector<string>>(BOARD_HEIGHT, vector<string>(BOARD_WIDTH, " "));
                    for (auto &[id, shape] : allShapes) {
                        findShape(shape, 0);
                    }
                }
            }
            else if (command == "edit") {
                if (selectedID == 0) {
                    cout << "No shape selected" << endl;
                } else {
                    editShape(info);
                }
            }
        }
    }

    return 0;
}
