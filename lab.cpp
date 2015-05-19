#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sstream>
//#define PRINT_TEMP_FIELD
using namespace std;

double maxTemp;
double minTemp;
double dx;
double dy;
double dt;
size_t x;
size_t y;
size_t size;
size_t iterationAmount;
double defTemp = 20;

class Edge {
    public:
        int type;
        double temp;
};

class Node {
    public:
        Node() { id = -1; temp = defTemp; };
        int& operator () () { return id; };
        bool operator == (Node A) { return temp = A.temp; };
        bool operator < (Node A) { return temp < A.temp; };
        bool operator > (Node A) { return temp > A.temp; };
        void setTemp(double ttemp) { temp = ttemp; };
        double getTemp() { return temp; };
    private:
        int id;
        double temp;
};

vector < vector < Node > > index(0);
vector < vector < double > > AMatrix(0);
vector < double > B(0);
vector < Edge > firstLevel(3);

void initializing(int argc, char *argv[]) {
    if (argc != 7) {
        printf("Lack of arguments!\n");
        exit(-1);
    }
    double sizeX = atof(argv[1]);
    double sizeY = atof(argv[2]);

    x = atoi(argv[3]);
    y = atoi(argv[4]);
    cout << "x = " << x << " | y = " << y << endl;
    dx = sizeX / (x - 1);
    dy = sizeY / (y - 1);

    int timeToCount = atoi(argv[5]);
    dt = atof(argv[6]);
    iterationAmount = timeToCount / dt + 1;
    minTemp = defTemp;
    maxTemp = 100;
}

void getEdge(vector < Edge > &mas) {
    FILE* fp = fopen("settings.txt", "r");
    float temp;
    for (size_t i = 0; i < mas.size(); ++i) {
        fscanf(fp, "%d %f\n", &(mas[i].type), &temp);
        mas[i].temp = temp;
        cout << mas[i].type << endl;
    }
    fclose(fp);
}

double getB(size_t i) {
    if (firstLevel[i].type == 1) {
        return firstLevel[i].temp;
    } else {
        return dx * firstLevel[i].temp;
    }
}

void createBMas() {

    size_t i, j;
    
    for (i = 1; i < index.size() - 1; ++i) {
        for (j = 1; j < index[i].size() - 1; ++j) {
            B[index[i][j]()] = -1 * index[i][j].getTemp() * dx * dx * dy * dy;
        }
    }

    // Right
    double right = getB(0);
    for (i = x / 2 + 1; i < x; ++i) {
        B[index[i][ index[i].size() - 1 ]()] = right;
    }

    // Bottom
    double bottom = getB(1);
    for (i = 1; i < x - 1; ++i) {
        B[index[i][0]()] = bottom;
    }

    // Left
    double left = getB(2);
    for (i = 0; i < (x / 2 + 1); ++i) {
        B[index[i][ index[i].size() - 1 ]()] = left;
    }
}

void createAMas() {

    size_t i = 0, j = 0;

    AMatrix.resize(size);
    B.resize(size, 0);

    for (i = 0; i < size; ++i) {
        AMatrix[i].resize(size, 0);
    }
    // Make matrix for computations
    for (i = 1; i < x - 1; ++i) {
        for (j = 1; j < index[i].size() - 1; ++j) {
            AMatrix[index[i][j]()][index[i][j]()] = 
                - 2 * dx * dx * dt 
                - 2 * dy * dy * dt
                - dx * dx * dy * dy;
            AMatrix[index[i][j]()][index[i - 1][j]()] = dy * dy * dt;
            AMatrix[index[i][j]()][index[i + 1][j]()] = dy * dy * dt;
            AMatrix[index[i][j]()][index[i][j + 1]()] = dx * dx * dt;
            AMatrix[index[i][j]()][index[i][j - 1]()] = dx * dx * dt;
        }
    }

    getEdge(firstLevel);

    printf("Right:  type: %d, value: %6.2f\n", firstLevel[1].type, firstLevel[1].temp);
    printf("Bottom: type: %d, value: %6.2f\n", firstLevel[2].type, firstLevel[2].temp);
    printf("Left:   type: %d, value: %6.2f\n", firstLevel[3].type, firstLevel[3].temp);

    if (firstLevel[0].type == 1) {
        // Right
        for (i = x / 2 + 1; i < x; ++i) {
            AMatrix[ index[i][ index[i].size() - 1 ]() ][ index[i][ index[i].size() - 1 ]() ] = 1;
        }
    } else {
        for (i = x / 2 + 1; i < x; ++i) {
            AMatrix[ index[i][ index[i].size() - 1 ]() ][ index[i][ index[i].size() - 1 ]() ] = 1;
            if (index[i - 1].size() >= index[i].size())
                AMatrix[ index[i][ index[i].size() - 1 ]() ][ index[i - 1][ index[i].size() - 1 ]() ] = -1;
        }
    }
    
    if (firstLevel[1].type == 1) {
        // Bottom
        for (i = 1; i < x - 1; ++i) {
            AMatrix[index[i][0]()][index[i][0]()] = 1;
        }
    } else {
        for (i = 1; i < x - 1; ++i) {
            AMatrix[index[i][0]()][index[i][0]()] = 1;
            AMatrix[index[i][0]()][index[i][1]()] = -1;
        }
    }

    if (firstLevel[2].type == 1) {
        // Left
        for (i = 0; i <  (x / 2 + 1); ++i) {
            AMatrix[ index[i][ index[i].size() - 1 ]() ][ index[i][ index[i].size() - 1 ]() ] = 1;
        }
    } else {
        for (i = 0; i <  (x / 2 + 1); ++i) {
            AMatrix[ index[i][ index[i].size() - 1 ]() ][ index[i][ index[i].size() - 1 ]() ] = 1;
            if (index[i + 1].size() >= index[i].size())
                AMatrix[ index[i][ index[i].size() - 1 ]() ][ index[i + 1][ index[i].size() - 1 ]() ] = -1;
        }
    }
}

void createIndex() {
    index.resize(x);
    int count = 0;
    for (size_t i = 0; i < x; ++i) {
        size_t height = (i < x / 2) ? (i + 1) : (x - i);
        if (height > y) {
            height = y;
        }
        index[i].resize(height);
        for (size_t j = 0; j < index[i].size(); ++j) {
            index[i][j]() = count++;
        }
    }
    size = count;
    cout << "Size: " << size << endl;
}


void toDefault(vector < double > &mas, size_t num, double &Bb) {
    double temp =  mas[num];
    if (temp == 0) {
        return;
    }
    size_t i;
    for (i = 0; i < mas.size(); ++i) {
        mas[i] = mas[i] / temp;
    }
    Bb /= temp;
}

void backStep(vector < vector < double > > &mas, vector < double > &b) {
    size_t i, z;
    for (i = mas.size() - 1; i > 0; --i)
        for (z = 0; z < i; ++z) {
            b[z] -= mas[z][i] * b[i] / mas[i][i];
            mas[z][i] = 0;
        }
}

void differ(const vector < double > &base,
        vector < double > &toEdit, 
        size_t num, double b, double &Bb) {
    double temp = toEdit[num];
    size_t i;
    for (i = num; i < toEdit.size(); ++i) {
        toEdit[i] -= base[i] * temp;
    }
    Bb -= b * temp;
}

void setTemprature() {
    for (size_t i = 0; i < index.size(); ++i) {
        for (size_t j = 0; j < index[i].size(); ++j) {
            double temp = B[index[i][j]()];
            if (minTemp > temp)
                minTemp = temp;
            if (maxTemp < temp)
                maxTemp = temp;
            index[i][j].setTemp(temp);
        }
    }
}

void printToFile(int countIteration) {
    stringstream stream;
    stream << "out/step" << countIteration;
    ofstream file;
    file.open(stream.str().c_str(), ios::out | ios::trunc);
    for (size_t i = 0; i < index.size(); ++i) {
        for (size_t j = 0; j < index[i].size(); ++j) {
            file << i * dx << ' ' << j * dy << ' ' << index[i][j].getTemp() << endl;
        }
        file << endl;
    }
    file.close();
}

void printResultFile() {
    stringstream pause;
    pause << "pause " << dt;
    ofstream file;
    file.open("result", ios::out | ios::trunc);
    file << "set cbrange [" << (int)minTemp << ':' << (int)maxTemp + 1 << ']' << endl;
    file << "set view map" << endl;
    for (size_t i = 0; i < iterationAmount; ++i) {
        file << "splot 'out/step" << i << "' with pm3d" << endl << pause.str() << endl;
    }
    file << "pause -1" << endl;
    file.close();
}

void printTempField() {
    for (size_t j = index[0].size(); j > 0; --j) {
        for (size_t i = 0; i < index.size(); ++i) {
            std::cout.width(4);
            std::cout << (int)index[i][j - 1].getTemp();
        }
        cout << endl;
    }
}

void printAMasAndB(vector < vector < double > > &A, vector < double > &b) {
    const size_t width = 4;
    for (size_t i = 0; i < A.size(); ++i) {
        cout.width(width);
        cout << i << ") ";
        for (size_t j = 0; j < A[i].size(); ++j) {
            std::cout.width(width);
            std::cout << A[i][j];
        }
        cout << "| " << setw(width) << b[i] << endl;
    }
}

void printIndexes() {
    for (size_t i = 0; i < index.size(); ++i) {
        for (size_t j = 0; j < index[i].size(); ++j) {
            std::cout.width(3);
            std::cout << index[i][j]();
        }
        cout << endl;
    }
}

void iteration(vector < vector < double > > A) {
    for (size_t i = 0; i < A.size(); ++i) {
        toDefault(A[i], i, B[i]);
        for (size_t j = i + 1; j < A.size(); ++j) {
            differ(A[i], A[j], i, B[i], B[j]);
        }
    }
    backStep(A, B);
}

int main(int argc, char *argv[]) {

    initializing(argc, argv);

    printf("dx = %f; dy = %f;\n", dx, dy);

    createIndex();
    createAMas();
    createBMas();

    
#ifdef PRINT
    printIndexes();
    printAMasAndB(AMatrix, B);
#endif

    for (size_t iterNum = 0; iterNum < iterationAmount; ++iterNum) {
        cout << "Iteration num: " << iterNum << endl;
        createBMas();
        iteration(AMatrix);
        setTemprature();
#ifdef PRINT_TEMP_FIELD
        printTempField();
#endif
        printToFile(iterNum);
    }

    printResultFile();

    exit(0);
}
