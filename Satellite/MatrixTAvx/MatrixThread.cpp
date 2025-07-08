#include <iostream>
#include <fstream>
#include <pthread.h>
#include <vector>
#include <chrono>


const int N = 1000;
const int NUM_THREADS = 4;

float* A;
float* B;
float* C;

void ReadMatrix(const std::string& filename, float* matrix, int N) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "File open failed: " << filename << "\n";
        exit(1);
    }

    for (int i = 0; i < N * N; ++i) {
        file >> matrix[i];
    }
}

void WriteMatrix(const std::string& filename, float* matrix, int N) {
    std::ofstream file(filename);
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            file << matrix[i * N + j] << " ";
        }
        file << "\n";
    }
}

struct ThreadData {
    int start_row;
    int end_row;
};

void* MultiplyPart(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    for (int i = data->start_row; i < data->end_row; ++i) {
        for (int j = 0; j < N; ++j) {
            float sum = 0.0f;
            for (int k = 0; k < N; ++k) {
                sum += A[i * N + k] * B[k * N + j];
            }
            C[i * N + j] = sum;
        }
    }
    pthread_exit(nullptr);
    return nullptr;
}

int main() {

    auto start = std::chrono::high_resolution_clock::now();

    A = new float[N * N];
    B = new float[N * N];
    C = new float[N * N];

    ReadMatrix("matrix_a.txt", A, N);
    ReadMatrix("matrix_b.txt", B, N);

    pthread_t threads[NUM_THREADS];
    ThreadData threadData[NUM_THREADS];
    int rows_per_thread = N / NUM_THREADS;

    for (int t = 0; t < NUM_THREADS; ++t) {
        threadData[t].start_row = t * rows_per_thread;
        threadData[t].end_row = (t == NUM_THREADS - 1) ? N : (t + 1) * rows_per_thread;
        pthread_create(&threads[t], nullptr, MultiplyPart, (void*)&threadData[t]);
    }

    for (int t = 0; t < NUM_THREADS; ++t) {
        pthread_join(threads[t], nullptr);
    }

    WriteMatrix("matrix_product_threads.txt", C, N);

    delete[] A;
    delete[] B;
    delete[] C;

    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> elapsed = end-start;

    std::cout << "Time Elapsed With 4 Thread: " << elapsed.count() << " ms" << std::endl;
    std::cout << "Threaded matrix multiplication completed.\n";

    return 0;
}
