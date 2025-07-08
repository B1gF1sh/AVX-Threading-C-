#include <iostream>
#include <fstream>
#include <pthread.h>
#include <immintrin.h>
#include <vector>
#include <cstring>
#include <thread>
#include <chrono>




int N = 5000;
int NUM_THREADS = 4;//std::min(NUM_THREADS, N);

float* A;
float* B;
float* B_T;
float* C;

struct ThreadData{
    int start_row;
    int end_row;
};

void ReadMatrix(const std::string &filename, float* matrix, int N){
    std::ifstream file(filename);
    if(!file.is_open()){
        std::cerr << "File could not open: " << filename << std::endl;
        exit(1);
    }

    for(int i=0; i<N*N;++i){
        file >> matrix[i];
    }
}


void WriteMatrix(const std::string &filename, float* matrix, int N){
    std::ofstream file(filename);
    for(int i=0; i< N; ++i){
        for(int j=0; j <N; j++){
            file << matrix[i*N+j] << " ";
        }
        file << "\n";
    }
}

void TransposeMatrix(const float* input, float* output, int N){
    for(int i=0; i<N; ++i){
        for(int j =0; j<N; ++j){
            output[j*N+i] = input[i*N+j];
        }
    }
}

void* MultiplyWorker(void* arg){
    ThreadData* data = (ThreadData*)arg;
    int start = data-> start_row;
    int end = data->end_row;

    for(int i=start; i<end; ++i){
        for(int j= 0; j<N; ++j){
            __m256 sum = _mm256_setzero_ps();

            int k = 0;

            for(; k+7<N; k+= 8){
                __m256 a = _mm256_loadu_ps(&A[i*N +k]);
                __m256 b = _mm256_loadu_ps(&B_T[j*N +k]);
                sum = _mm256_add_ps(sum, _mm256_mul_ps(a,b));
            }

            float temp[8];
            _mm256_storeu_ps(temp,sum);

            float total = 0.0f;

            for(int t=0; t<8; ++t){
                total += temp[t];
            }

            for(; k<N; ++k){
                total += A[i*N+k] * B_T[j*N+k];
            }
            C[i*N+j] = total;
        }
    }

    pthread_exit(nullptr);
    return nullptr;

}

int main(){

    auto start = std::chrono::high_resolution_clock::now();
    
    N=5000;
    // Intel Core I9 12900h 6p core + 8e core 6x2+8=20 thread
    NUM_THREADS = 4; //std::min(static_cast<int>(std::thread::hardware_concurrency()),N);

    A = new float[N*N]();
    B = new float[N*N]();
    B_T = new float[N*N]();
    C = new float[N*N]();



    ReadMatrix("matrix_a.txt", A, N);
    ReadMatrix("matrix_b.txt", B, N);

    TransposeMatrix(B, B_T, N);

    pthread_t threads[NUM_THREADS];
    ThreadData thread_data[NUM_THREADS];


    int rows_per_thread = N / NUM_THREADS;

    for(int t=0; t< NUM_THREADS; ++t){
        
        thread_data[t].start_row = t*rows_per_thread;
        thread_data[t].end_row = (t == NUM_THREADS -1) ? N : (t + 1) * rows_per_thread;

        pthread_create(&threads[t],nullptr,MultiplyWorker,(void*) &thread_data[t]);

    }

    for(int t=0; t<NUM_THREADS;++t){
        pthread_join(threads[t],nullptr);
    }


    WriteMatrix("matrix_avx_threads.txt", C, N);

    delete[] A;
    delete[] B;
    delete[] B_T;
    delete[] C;

    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> elapsed = end-start;

    std::cout << "Time: " << elapsed.count() << " ms" << std::endl;
    std::cout << "AVX + Threads matrix multiplication complete.\n";
    return 0;

}