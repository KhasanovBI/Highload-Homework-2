#include <cstdint>
#include <ctime>
#include <iostream>
#include <algorithm>
#include <fstream>

using namespace std;

void prepareList(volatile void **testList, const size_t testListSize) {
    vector<size_t> randomHelper;
    randomHelper.reserve(testListSize);
    for (size_t i = 0; i < testListSize; ++i) {
        randomHelper.push_back(i);
    }
    random_shuffle(randomHelper.begin(), randomHelper.end());
    for (size_t i = 0; i < testListSize; ++i) {
        testList[i] = (void *)(testList + randomHelper[i]);
    }

    volatile void **trashList = new volatile void *[testListSize];
    volatile void **currentPosition = trashList;
    for (int i = 0; i <testListSize; i+=64) {
        currentPosition = (volatile void **) trashList[i];
    }
    delete[] trashList;
}

double measureRandomAccess(volatile void **testList, const size_t testListSize) {
    double averageTime = 0;
    int iterationsCount = 10;
    int cyclesCount = 1000000;
    for (size_t j = 0; j < iterationsCount; ++j) {
        prepareList(testList, testListSize);
        clock_t start = clock();
        volatile void **currentPosition = testList;
        for (size_t i = 0; i < cyclesCount; ++i) {
            currentPosition = (volatile void **) *currentPosition;
        }
        clock_t stop = clock();
        averageTime += (stop - start);
    }
    averageTime /= (iterationsCount * cyclesCount);
    return averageTime;
}

double measureDirectAccess(volatile void **testList, const size_t testListSize) {
    double averageTime = 0;
    clock_t start = clock();
    volatile void **currentPosition = testList;
    for (size_t i = 0; i < testListSize; ++i) {
        currentPosition = (volatile void **) testList[i];
    }
    clock_t stop = clock();
    averageTime += (stop - start);
    return double(averageTime) / testListSize;
}


int main(int argc, char *argv[]) {
    ofstream resultsFile;
    resultsFile.open("cache.csv");
    cout << sizeof(void *) << endl;
    size_t initialListMemorySize = 1024;
    size_t maxListMemorySize = 8 * 1024 * initialListMemorySize;
    cout << "Random access" << endl;
    size_t initialListSize = initialListMemorySize / sizeof(void *);// size - number of elements, not memory size
    size_t maxListSize = maxListMemorySize / sizeof(void *);
    for (size_t testListSize = initialListSize; testListSize <= maxListSize; testListSize *= 1.5) {
        volatile void **testList = new volatile void *[testListSize];
        double measuredTime = measureRandomAccess(testList, testListSize);
        cout << "List size: " << double(testListSize) * sizeof(void *) / 1024 << "Kb, Time: " << measuredTime << endl;
        resultsFile << double(testListSize) * sizeof(void *) / 1024 << ',' << measuredTime << endl;
        delete[] testList;
    }
    /*
    cout << "Direct access" << endl;
    for (size_t testListSize = initialListSize; testListSize <= 65 * 1024 * initialListSize; testListSize *= 2) { //TODO
        void **testList = new void *[testListSize];
        cout << "List size: " << testListSize * sizeof(void *) / 1024 << "Kb, Time: " << measureDirectAccess(testList, testListSize) << endl;
        delete[] testList;
    }*/
    resultsFile.close();
    system("./plot.py");
    return 0;
}