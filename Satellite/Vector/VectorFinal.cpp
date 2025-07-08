#include<iostream>
#include<fstream>
#include<vector>
#include<pthread.h>
#include<xmmintrin.h>
#include<chrono>
//#include<cstdlib>

const int N = 10000;
const int NUM_THREADS = 2;

std::vector<float> a(N);
std::vector<float> b(N);

float results[NUM_THREADS];


struct ThreadData{
    int id;
    int start;
    int end;
};

std::vector<float> read_vector_from_file(const std::string &filename){
    std::vector<float> vec(N);
    std::ifstream file(filename);

    if(!file){
        std::cerr<< "Error: Cannot open file " << filename << std::endl;
        exit(1);
    }
    for(int i=0; i<N && file >> vec[i];i++);
    return vec;
}

void write_result_to_file(const std::string& filename, float result){

    std::ofstream file(filename);
    if(file)
        file << result << std::endl;
    else
        std::cerr << "Error writing to file " << filename << std::endl;

}

void* ComputeDotProduct(void* args){

    ThreadData* data = (ThreadData*)args;
    int start = data->start;
    int end = data->end;

    __m128 sum = _mm_setzero_ps();
    int i = start;

    for(; i<=end-4; i+=4){
        __m128 vector_a = _mm_loadu_ps(&a[i]);
        __m128 vector_b = _mm_loadu_ps(&b[i]);
        __m128 prod = _mm_mul_ps(vector_a,vector_b);

        sum = _mm_add_ps(sum,prod);

    }

    float temp[4];
    _mm_storeu_ps(temp,sum);
    
    float local_sum = temp[0] + temp[1] + temp[2] + temp[3];

    for(; i<end; i++){
        local_sum += a[i]*b[i];
    }

    results[data->id] = local_sum;
    pthread_exit(NULL);
    return NULL;

}

int main(){

    auto start = std::chrono::high_resolution_clock::now();

    a = read_vector_from_file("Vector_a.txt");
    b = read_vector_from_file("Vector_b.txt");

    pthread_t threads[NUM_THREADS];
    ThreadData threadData[NUM_THREADS];
    int chunk = N / NUM_THREADS;

    for(int i=0; i < NUM_THREADS; i++){
        threadData[i].id = i;
        threadData[i].start = i * chunk;
        threadData[i].end = (i== NUM_THREADS-1) ? N : (i+1)*chunk;

        pthread_create(&threads[i],NULL,ComputeDotProduct,(void*)&threadData[i]);


    }

    for(int i=0; i<NUM_THREADS;i++){
        pthread_join(threads[i],NULL);
    }

    float final_result = 0.0f;
    for(int i=0; i< NUM_THREADS; ++i){
        final_result += results[i];
    }

    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> elapsed = end - start;
    
    std::cout << "Time (SSE + Threads):" << elapsed.count() << " ms" << std::endl;
    std::cout << "Final Dot Product: " << final_result << std::endl;

    //write_result_to_file("Result.txt",final_result);
}