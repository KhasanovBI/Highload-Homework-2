#include <cstdint>
#include <ctime>
#include <iostream>
#include <algorithm>
#include <fstream>

using namespace std;
#define CACHE_CLEAR_LIST_SIZE (8 * 1024 * 1024)
#define RANDOM_ACCESS_ITERATIONS_COUNT 10

void clearCache() {
    size_t trashListSize = CACHE_CLEAR_LIST_SIZE / sizeof(void *);
    volatile void **trashList = new volatile void *[trashListSize];
    volatile void **currentPosition = trashList;
    for (int i = 1; i < trashListSize; i *= 2) {
        currentPosition = (volatile void **) trashList[i];
    }
    delete[] trashList;
}

void prepareDirectList(volatile void **testList, const size_t testListSize, const size_t step) {
    for (size_t i = 0; i < testListSize; i += step) {
        testList[i] = (void *) (testList + i + step);
    }
    clearCache();
}

void directAccessMeasurement(size_t initialListSize, size_t maxListSize) {
    cout << "Direct access" << endl;
    ofstream resultsFile;
    resultsFile.open("directAccessCacheMeasuring.csv");
    volatile void **testList = new volatile void *[maxListSize];
    for (double step = initialListSize; step < maxListSize; step *= 1.15) {
        double measuredTime = 0;
        size_t DIRECT_ACCESS_ITERATIONS_COUNT = (size_t) floor(step / initialListSize * 100);
        for (size_t j = 0; j < DIRECT_ACCESS_ITERATIONS_COUNT; ++j) {
            prepareDirectList(testList, maxListSize, (const size_t) floor(step));
            clock_t start = clock();
            volatile void **currentPosition = testList;
            for (size_t i = 0; i < maxListSize; i += step) {
                currentPosition = (volatile void **) *currentPosition;
            }
            clock_t stop = clock();
            measuredTime += (stop - start);
        }
        measuredTime /= (DIRECT_ACCESS_ITERATIONS_COUNT * maxListSize / step);
        cout << "Step size: " << step * sizeof(void *) / 1024 << "Kb, Time: " << measuredTime << endl;
        resultsFile << step * sizeof(void *) / 1024 << ',' << measuredTime << endl;
    }
    delete[] testList;
    resultsFile.close();
}


void prepareRandomList(volatile void **testList, const size_t testListSize) {
    vector<size_t> randomHelper;
    randomHelper.reserve(testListSize);
    for (size_t i = 0; i < testListSize; ++i) {
        randomHelper.push_back(i);
    }
    random_shuffle(randomHelper.begin(), randomHelper.end());
    for (size_t i = 0; i < testListSize; ++i) {
        testList[i] = (void *) (testList + randomHelper[i]);
    }
    clearCache();
}

double measureRandomAccess(volatile void **testList, const size_t testListSize) {
    double averageTime = 0;
    for (size_t j = 0; j < RANDOM_ACCESS_ITERATIONS_COUNT; ++j) {
        prepareRandomList(testList, testListSize);
        clock_t start = clock();
        volatile void **currentPosition = testList;
        for (size_t i = 0; i < testListSize; ++i) {
            currentPosition = (volatile void **) *currentPosition;
        }
        clock_t stop = clock();
        averageTime += (stop - start);
    }
    averageTime /= (RANDOM_ACCESS_ITERATIONS_COUNT * testListSize);
    return averageTime;
}

void randomAccessMeasurement(size_t initialListSize, size_t maxListSize) {
    cout << "Random access" << endl;
    ofstream resultsFile;
    resultsFile.open("randomAccessCacheMeasuring.csv");
    for (size_t testListSize = initialListSize; testListSize <= maxListSize; testListSize *= 1.15) {
        volatile void **testList = new volatile void *[testListSize];
        double measuredTime = measureRandomAccess(testList, testListSize);
        cout << "List size: " << double(testListSize) * sizeof(void *) / 1024 << "Kb, Time: " << measuredTime << endl;
        resultsFile << double(testListSize) * sizeof(void *) / 1024 << ',' << measuredTime << endl;
        delete[] testList;
    }
    resultsFile.close();
}

int main(int argc, char *argv[]) {
    size_t initialListMemorySize = 1024;
    size_t maxListMemorySize = 8 * 1024 * initialListMemorySize;
    size_t initialListSize = initialListMemorySize / sizeof(void *);// size - number of elements, not memory size
    size_t maxListSize = maxListMemorySize / sizeof(void *);
    randomAccessMeasurement(initialListSize, maxListSize);
    directAccessMeasurement(initialListSize, maxListSize);

    system("./plot.py &");
    return 0;
}