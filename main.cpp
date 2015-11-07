#include <cstdint>
#include <ctime>
#include <iostream>
#include <algorithm>

using namespace std;

void prepareList(void **testList, const size_t testListSize) {
    vector<size_t> randomHelper;
    randomHelper.reserve(testListSize);
    for (size_t i = 0; i < testListSize; ++i) {
        randomHelper.push_back(i);
    }
    random_shuffle(randomHelper.begin(), randomHelper.end());
    for (size_t i = 0; i < testListSize; ++i) {
        testList[i] = (void *)(testList + randomHelper[i]);
    }
}

double measureRandomAccess(void **testList, const size_t testListSize) {
    double averageTime = 0;
    int iterationsCount = 10;
    for (size_t j = 0; j < iterationsCount; ++j) {
        prepareList(testList, testListSize);
        clock_t start = clock();
        void **currentPosition = testList;
        for (size_t i = 0; i < testListSize; ++i) {
            currentPosition = (void **) *currentPosition;
        }
        clock_t stop = clock();
        averageTime += (stop - start);
    }
    averageTime /= (iterationsCount * testListSize);
    return averageTime;
}

double measureDirectAccess(void **testList, const size_t testListSize) {
    double averageTime = 0;
    clock_t start = clock();
    void **currentPosition = testList;
    for (size_t i = 0; i < testListSize; ++i) {
        currentPosition = (void **) testList[i];
    }
    clock_t stop = clock();
    averageTime += (stop - start);
    return averageTime;
}


int main(int argc, char *argv[]) {
    cout << sizeof(void *) << endl;
    size_t initialListMemorySize = 1024;
    cout << "Random access" << endl;
    size_t initialListSize = initialListMemorySize / sizeof(void *);// size - number of elements, not memory size
    for (size_t testListSize = initialListSize; testListSize <= 65 * 1024 * initialListSize; testListSize *= 1.5) { //TODO
        void **testList = new void *[testListSize];
        void **trashList = new void *[testListSize];
        void **currentPosition = trashList;
        for (int i = 1; i <testListSize; i*=2) {
            currentPosition = (void **) trashList[i];
        }
        delete[] trashList;
        //cout << "List size: " << double(testListSize) * sizeof(void *) / 1024 << "Kb, Time: " << measureRandomAccess(testList, testListSize) << endl;
        cout << double(testListSize) * sizeof(void *) / 1024 << ',' << measureRandomAccess(testList, testListSize) << endl;
        delete[] testList;
    }
    /*
    cout << "Direct access" << endl;
    for (size_t testListSize = initialListSize; testListSize <= 65 * 1024 * initialListSize; testListSize *= 2) { //TODO
        void **testList = new void *[testListSize];
        cout << "List size: " << testListSize * sizeof(void *) / 1024 << "Kb, Time: " << measureDirectAccess(testList, testListSize) << endl;
        delete[] testList;
    }*/
    return 0;
}