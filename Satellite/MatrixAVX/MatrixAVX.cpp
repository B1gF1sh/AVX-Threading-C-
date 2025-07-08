#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <immintrin.h>

std::vector<std::vector<float>> Read_Matrix(const std::string &filename, int rows, int cols) {
    std::vector<std::vector<float>> matrix(rows, std::vector<float>(cols));
    std::ifstream file(filename);
    std::string line;

    if(!file.is_open()){
        std::cerr << "File could not open: " << filename << "\n";
        std::exit(1);
    }

    for (int i = 0; i < rows && std::getline(file, line); ++i) {
        std::stringstream ss(line);
        for (int j = 0; j < cols; ++j) {
            ss >> matrix[i][j];
        }
    }
    return matrix;
}

void Write_Matrix(const std::string &filename, const std::vector<std::vector<float>> &matrix) {
    std::ofstream file(filename);
    for (const auto &row : matrix) {
        for (float val : row) {
            file << val << " ";
        }
        file << "\n";
    }
}

std::vector<std::vector<float>> Multiply_AVX(const std::vector<std::vector<float>> &A, const std::vector<std::vector<float>> &B) {
   
    int N = A.size();
    std::vector<std::vector<float>> C(N, std::vector<float>(N, 0.0f));

    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            __m256 sum = _mm256_setzero_ps();

            int k = 0;
            for (; k + 7 < N; k += 8) {
                __m256 vecA = _mm256_loadu_ps(&A[i][k]);

                float bvals[8];
                for (int t = 0; t < 8; ++t) {
                    bvals[t] = B[k + t][j];
                }

                __m256 vecB = _mm256_loadu_ps(bvals);
                sum = _mm256_add_ps(sum, _mm256_mul_ps(vecA, vecB));
            }

            float temp[8];
            _mm256_storeu_ps(temp, sum);
            for (int t = 0; t < 8; ++t) {
                C[i][j] += temp[t];
            }

            for (; k < N; ++k) {
                C[i][j] += A[i][k] * B[k][j];  
            }
        }
    }

    return C;
}

int main() {
    int N = 500;

    auto A = Read_Matrix("matrix_a.txt", N, N);
    auto B = Read_Matrix("matrix_b.txt", N, N);
    auto C = Multiply_AVX(A, B);

    Write_Matrix("matrix_avx.txt", C);

    std::cout << "AVX Matrix computation completed successfully.\n";
    return 0;
}
