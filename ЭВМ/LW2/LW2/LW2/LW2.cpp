#include <iostream>
#include <immintrin.h>
#include <chrono>

using namespace std;
using namespace std::chrono;

const unsigned int msize = 4096;

void scalar_mul(int32_t** A, int32_t** B, int32_t** C, unsigned int N) {
    cout << "Scalar multiplication:" << endl;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            for (int k = 0; k < N; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

int vector_mul(int32_t** A, int32_t** TB,int32_t** C, unsigned int N) {
    cout << "SSE multiplication:" << endl;
    if (N < 4) return 0;
    int32_t row[4] = { 0 };
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            for (int k = 0; k < N; k += 4) {
                __m128i c_line = _mm_setzero_si128(); // заполняем результирующий SSE-регистр нулями
                __m128i a_line = _mm_load_si128((__m128i*) & A[i][k]); // загружаем 4 элемента int из массива A в SSE-регистр
                __m128i b_line = _mm_load_si128((__m128i*) & TB[j][k]); // загружаем 4 элемента int из массива B в SSE-регистр
                c_line = _mm_mullo_epi32(a_line, b_line); // Умножение двух SSE-регистров
                _mm_store_si128((__m128i*) &row, c_line);
                C[i][j] += row[0] + row[1] + row[2] + row[3];
            }
        }
        //cout << "and anoter one" << endl;
    }
}

string matrix_comparing(int32_t** A, int32_t** B, unsigned int N) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (A[i][j] != B[i][j]) {
                return "false";
            }
        }
    }
    return "true";
}

void show_m(int32_t** m, unsigned int N) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            cout << m[i][j] << " ";
        }
        cout << endl;
    }
}

int main()
{
    srand(time(NULL));

    cout << "Initialization" << endl;
    double overall = 0.0;
    auto start = high_resolution_clock::now();

    alignas(16) int32_t **A = new int32_t*[msize];
    for (int32_t i = 0; i < msize; i++) {
        A[i] = new int32_t[msize];
    }
    alignas(16) int32_t **B = new int32_t*[msize];
    for (int32_t i = 0; i < msize; i++) {
        B[i] = new int32_t[msize];
    }
    alignas(16) int32_t **C_scalar = new int32_t*[msize];
    for (int32_t i = 0; i < msize; i++) {
        C_scalar[i] = new int32_t[msize];
    }
    alignas(16) int32_t** C_vector = new int32_t * [msize];
    for (int32_t i = 0; i < msize; i++) {
        C_vector[i] = new int32_t[msize];
        for (int32_t j = 0; j < msize; j++) {
            A[i][j] = (uint32_t)rand() % 100;
            B[i][j] = (uint32_t)rand() % 100;
            C_scalar[i][j] = 0;
            C_vector[i][j] = 0;
        }
    }
    alignas(16) int32_t** TB = new int32_t * [msize];
    for (int32_t i = 0; i < msize; i++) {
        TB[i] = new int32_t[msize];
        for (int32_t j = 0; j < msize; j++) {
            TB[i][j] = B[j][i];
        }
    }
    
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start);
    cout << "time : " << duration.count() << endl;
    
    overall = 0.0;
    start = high_resolution_clock::now();
    scalar_mul(A, B, C_scalar, msize);
    end = high_resolution_clock::now();
    duration = duration_cast<microseconds>(end - start);
    cout << "time : " << duration.count() << endl;

    overall = 0.0;
    start = high_resolution_clock::now();
    vector_mul(A, TB, C_vector, msize);
    end = high_resolution_clock::now();
    duration = duration_cast<microseconds>(end - start);
    cout << "time : " << duration.count() << endl;
    
    /*
    show_m(C_scalar, msize);
    cout << endl;
    show_m(C_vector, msize);
    cout << endl;
    */
    cout << "matrix equality : " << matrix_comparing(C_scalar, C_vector, msize) << endl;

    for (int i = 0; i < msize; i++) {
        delete[] A[i];
    }
    delete[] A;
    for (int i = 0; i < msize; i++) {
        delete[] B[i];
    }
    delete[] B;
    for (int i = 0; i < msize; i++) {
        delete[] C_scalar[i];
    }
    delete[] C_scalar;
    for (int i = 0; i < msize; i++) {
        delete[] C_vector[i];
    }
    delete[] C_vector;

    return 0;
}
