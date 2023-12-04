#include <iostream>
#include <thread>
#include <immintrin.h>
#include <chrono>
#include <vector>

#define _THREADS _Thrd_hardware_concurrency()

using namespace std;
using namespace std::chrono;

const unsigned int msize = 128;

unsigned long long sum = 0;

void row_sum(unsigned long long start, unsigned long long step) {
    unsigned long long sumb = 0;
    for (unsigned long long i = start; i <= 10000000000; i += step) {
        //cout << i << endl;
        sumb += i;
    }

    sum += sumb;
}

void row_sum_with_threads() {
    vector<thread> threads(_THREADS);
    for (size_t i = 0; i < _THREADS; i++) {
        threads[i] = thread(row_sum, i + 1, _THREADS);
    }

    for (size_t i = 0; i < _THREADS; i++) {
        threads[i].join();
    }
}

void scalar_mul(int32_t** A, int32_t** B, int32_t** C, unsigned int N, int start, int step) {
    for (int i = start; i < N; i += step) {
        for (int j = 0; j < N; j++) {
            for (int k = 0; k < N; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

int vector_mul(int32_t** A, int32_t** TB, int32_t** C, unsigned int N, int start, int step) {
    if (N < 4) return 0;
    int32_t row[4] = { 0 };
    for (int i = start; i < N; i += step) {
        for (int j = 0; j < N; j++) {
            for (int k = 0; k < N; k += 4) {
                __m128i c_line = _mm_setzero_si128(); // заполняем результирующий SSE-регистр нулями
                __m128i a_line = _mm_load_si128((__m128i*) & A[i][k]); // загружаем 4 элемента int из массива A в SSE-регистр
                __m128i b_line = _mm_load_si128((__m128i*) & TB[j][k]); // загружаем 4 элемента int из массива B в SSE-регистр
                c_line = _mm_mullo_epi32(a_line, b_line); // Умножение двух SSE-регистров
                _mm_store_si128((__m128i*) & row, c_line);
                C[i][j] += row[0] + row[1] + row[2] + row[3];
            }
        }
        //cout << "and anoter one" << endl;
    }
}

void scalar_mul_with_threads(int32_t** A, int32_t** B, int32_t** C, unsigned int N) {
    vector<thread> threads(_THREADS);
    for (size_t i = 0; i < _THREADS; i++) {
        threads[i] = thread(scalar_mul, A, B, C, N, i, _THREADS);
    }

    for (size_t i = 0; i < _THREADS; i++) {
        threads[i].join();
    }
}

void vector_mul_with_threads(int32_t** A, int32_t** TB, int32_t** C, unsigned int N) {
    vector<thread> threads(_THREADS);
    for (size_t i = 0; i < _THREADS; i++) {
        threads[i] = thread(vector_mul, A, TB, C, N, i, _THREADS);
    }

    for (size_t i = 0; i < _THREADS; i++) {
        threads[i].join();
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
    
    double overall = 0.0;
    auto start = high_resolution_clock::now();
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start);

    for (int i = 0; i < 10; i++) {
        cout << endl << "------------------------ " << i + 1 << " ------------------------" << endl;
        cout << "sum row" << endl;
        overall = 0.0;
        start = high_resolution_clock::now();

        row_sum(0, 1);
        cout << sum << endl;

        end = high_resolution_clock::now();
        duration = duration_cast<microseconds>(end - start);
        cout << "time : " << duration.count() / 1000000 << " sec" << endl;

        cout << "sum row with threads" << endl;
        overall = 0.0;
        start = high_resolution_clock::now();
        sum = 0;

        row_sum_with_threads();
        cout << sum << endl;

        end = high_resolution_clock::now();
        duration = duration_cast<microseconds>(end - start);
        cout << "time : " << duration.count() / 1000000 << " sec" << endl;
    }

    cout << "Initialization" << endl;
    overall = 0.0;
    start = high_resolution_clock::now();

    alignas(16) int32_t** A = new int32_t * [msize];
    for (int32_t i = 0; i < msize; i++) {
        A[i] = new int32_t[msize];
    }
    alignas(16) int32_t** B = new int32_t * [msize];
    for (int32_t i = 0; i < msize; i++) {
        B[i] = new int32_t[msize];
    }
    alignas(16) int32_t** C_scalar = new int32_t * [msize];
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

    end = high_resolution_clock::now();
    duration = duration_cast<microseconds>(end - start);
    cout << "time : " << duration.count() / 1000000 << " sec" << endl;
    for (int i = 0; i < 10; i++) {
        cout << endl << "------------------------ " << i + 1 << " ------------------------" << endl;

        cout << "Scalar multiplication:" << endl;
        overall = 0.0;
        start = high_resolution_clock::now();
        scalar_mul(A, B, C_scalar, msize, 0, 1);
        end = high_resolution_clock::now();
        duration = duration_cast<microseconds>(end - start);
        cout << "time : " << duration.count() / 1000000 << " sec" << endl;

        cout << "SSE multiplication:" << endl;
        overall = 0.0;
        start = high_resolution_clock::now();
        vector_mul(A, TB, C_vector, msize, 0, 1);
        end = high_resolution_clock::now();
        duration = duration_cast<microseconds>(end - start);
        cout << "time : " << duration.count() / 1000000 << " sec" << endl;

        cout << "matrix equality : " << matrix_comparing(C_scalar, C_vector, msize) << endl;

        ///--------------

        cout << "Scalar multiplication with threads:" << endl;
        overall = 0.0;
        start = high_resolution_clock::now();

        scalar_mul_with_threads(A, B, C_scalar, msize);

        end = high_resolution_clock::now();
        duration = duration_cast<microseconds>(end - start);
        cout << "time : " << duration.count() / 1000000 << " sec" << endl;

        cout << "SSE multiplication with threads:" << endl;
        overall = 0.0;
        start = high_resolution_clock::now();

        vector_mul_with_threads(A, TB, C_vector, msize);

        end = high_resolution_clock::now();
        duration = duration_cast<microseconds>(end - start);
        cout << "time : " << duration.count() / 1000000 << " sec" << endl;

        cout << "matrix equality : " << matrix_comparing(C_scalar, C_vector, msize) << endl;
    }

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
