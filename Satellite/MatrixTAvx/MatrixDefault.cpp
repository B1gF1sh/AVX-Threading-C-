#include <iostream>
#include <fstream>
#include <chrono>

const int N = 500;

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

void MultiplyMatrix(float* A, float* B, float* C, int N) {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            float sum = 0.0f;
            for (int k = 0; k < N; ++k) {
                sum += A[i * N + k] * B[k * N + j];
            }
            C[i * N + j] = sum;
        }
    }
}

int main() {

    auto start = std::chrono::high_resolution_clock::now();


    A = new float[N * N];
    B = new float[N * N];
    C = new float[N * N];

    ReadMatrix("matrix_a.txt", A, N);
    ReadMatrix("matrix_b.txt", B, N);

    MultiplyMatrix(A, B, C, N);

    WriteMatrix("matrix_product_default.txt", C, N);

    delete[] A;
    delete[] B;
    delete[] C;

    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> elapsed = end - start;

    std::cout << "Time Elapsed: " << elapsed.count() << " ms" << std::endl;
    std::cout << "Default matrix multiplication completed.\n";
    return 0;
}
