/*
  ==============================================================================

    MultiArray.h
    Created: 29 Dec 2020 6:13:13pm
    Author:  ryan

  ==============================================================================
*/

#pragma once

/**
 * Simple multi-dimensional array.
 */
template <typename T>
class MultiArray {
public:
    /**
     * Create array of size [n][m]
     */
    MultiArray(int n, int m)
        : n(n), m(m) {
        data = new T[n * m];
    }

    ~MultiArray() {
        delete[] data;
    }

    void set(int a, int b, T value) {
        data[n * a + b] = value;
    }

    T get(int a, int b) {
        return data[n * a + b];
    }

    /**
     * Sets all elements to zero
     */
    void clear() {
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j) {
                set(i, j, 0);
            }
        }
    }

private:
    int n;
    int m;
    T *data;
};