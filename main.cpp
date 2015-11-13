#include <ctime>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <vector>
#include <cmath>

#define ONE(x) x x
#define FIVE(x) ONE(x) ONE(x) ONE(x) ONE(x) ONE(x)
#define TEN(x) FIVE(x) FIVE(x)
#define FIFTY(x) TEN(x) TEN(x) TEN(x) TEN(x) TEN(x)
#define HUNDRED(x) FIFTY(x) FIFTY(x)
#define FIVE_HUNDRED(x) HUNDRED(x) HUNDRED(x) HUNDRED(x) HUNDRED(x) HUNDRED(x)
#define THOUSAND(x) FIVE_HUNDRED(x) FIVE_HUNDRED(x)
#define CACHE_CLEAR_LIST_SIZE (8 * 1024 * 1024)
#define CYCLES 10000

using namespace std;

struct Pointer {
    struct Pointer *next;
    long int pad[31];
};

typedef Pointer testType;

struct TestParams {
    const char *testName;
    const char *resultsFileName;
    size_t initialListSize;
    size_t maxListSize;

    void (*prepareList)(testType *testList, const size_t testListSize);
};

void clearCache() {
    size_t trashListSize = CACHE_CLEAR_LIST_SIZE / sizeof(testType *);
    testType *trashList = new testType[trashListSize];
    for (size_t i = 0; i < trashListSize; ++i) {
        trashList[i].next = trashList + ((i + 1) % trashListSize);
    }
    testType *currentPosition = trashList;
    for (int i = 1; i < trashListSize; i *= 2) {
        currentPosition = currentPosition->next;
    }
    delete[] trashList;
}

void prepareSequentialList(testType *testList, const size_t testListSize) {
    for (size_t i = 0; i < testListSize; ++i) {
        testList[i].next = testList + ((i + 1) % testListSize);
    }
    clearCache();
}

void prepareRandomList(testType *testList, const size_t testListSize) {
    vector<size_t> randomHelper;
    randomHelper.reserve(testListSize);
    for (size_t i = 0; i < testListSize; ++i) {
        randomHelper.push_back(i);
    }
    random_shuffle(randomHelper.begin(), randomHelper.end());
    size_t previous = randomHelper[0];
    size_t current;
    for (size_t i = 1; i < testListSize; ++i) {
        current = randomHelper[i];
        testList[previous].next = testList + current;
        previous = current;
    }
    testList[randomHelper.back()].next = testList + randomHelper[0];
    clearCache();
}

double measure(testType *testList) {
    testType *currentPosition = testList;
    double averageTime = 0;
    for (int count = 0; count < CYCLES; count++) {
        clock_t start = clock();
        THOUSAND(currentPosition = currentPosition->next;)
        clock_t stop = clock();
        averageTime += stop - start;
    }
    return averageTime / 1000 / CYCLES;
}

void measurement(TestParams &testParams) {
    cout << testParams.testName << endl;
    ofstream resultsFile;
    resultsFile.open(testParams.resultsFileName);
    for (size_t testListSize = testParams.initialListSize;
         testListSize <= testParams.maxListSize; testListSize = (size_t) ceil(1.1 * testListSize)) {
        testType *testList = new testType[testListSize];
        testParams.prepareList(testList, testListSize);
        double measuredTime = measure(testList);
        cout << "List size: " << double(testListSize) * sizeof(testType) / 1024 << "Kb, Time: " << measuredTime << endl;
        resultsFile << double(testListSize) * sizeof(testType) / 1024 << ',' << measuredTime << endl;
        delete[] testList;
    }
    resultsFile.close();
}

int main(int argc, char *argv[]) {
    cout << "Struct size: " << sizeof(testType) << endl;
    size_t initialListMemorySize = 1024;
    size_t maxListMemorySize = 8 * 1024 * initialListMemorySize;
    size_t initialListSize = initialListMemorySize / sizeof(testType);// size - number of elements, not memory size
    size_t maxListSize = maxListMemorySize / sizeof(testType);

    TestParams sequentialAccessParams;
    sequentialAccessParams.testName = "Sequential access";
    sequentialAccessParams.initialListSize = initialListSize;
    sequentialAccessParams.maxListSize = maxListSize;
    sequentialAccessParams.prepareList = prepareSequentialList;
    sequentialAccessParams.resultsFileName = "sequentialAccessCacheMeasuring.csv";

    TestParams randomAccessParams;
    randomAccessParams.testName = "Random access";
    randomAccessParams.initialListSize = initialListSize;
    randomAccessParams.maxListSize = maxListSize;
    randomAccessParams.prepareList = prepareRandomList;
    randomAccessParams.resultsFileName = "randomAccessCacheMeasuring.csv";

    measurement(sequentialAccessParams);
    measurement(randomAccessParams);
    system("./plot.py &");
    return 0;
}