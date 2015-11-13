#!/usr/bin/python
import matplotlib.pyplot as plt
import numpy as np


def main():
    memorySize, time = np.loadtxt('sequentialAccessCacheMeasuring.csv', delimiter=',', unpack=True, dtype=float)
    fig = plt.figure()
    fig.canvas.set_window_title('Sequential access cache size measuring')
    ax = fig.add_subplot(1, 1, 1, axisbg='white')
    plt.plot(memorySize, time)
    plt.title("Cache size measuring")
    plt.xlabel('Memory, kb')
    plt.ylabel('Access time, s')
    plt.show(block=False)

    memorySize, time = np.loadtxt('randomAccessCacheMeasuring.csv', delimiter=',', unpack=True, dtype=float)
    fig = plt.figure()
    fig.canvas.set_window_title('Random access cache size measuring')
    ax = fig.add_subplot(1, 1, 1, axisbg='white')
    plt.plot(memorySize, time)
    plt.title("Cache size measuring")
    plt.xlabel('Memory, kb')
    plt.ylabel('Access time, s')
    plt.show()

if __name__ == '__main__':
    main()
