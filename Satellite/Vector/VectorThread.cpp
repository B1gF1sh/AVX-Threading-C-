#include<vector>
#include<iostream>
#include<pthread.h>
#include<chrono>
#include<fstream>


const int N = 10000;

std::vector<float> a(N);
std::vector<float> b(N);

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


struct ThreadData{

    int start, end;
    const std::vector<float>* a;
    const std::vector<float>* b;
    float result;
};


void* ComputeDotProduct(void* args){

    ThreadData* data = (ThreadData*) args;
    data -> result = 0;

    for(int i= data->start; i<data->end;i++){
        data -> result += (*(data->a))[i]* (*(data->b))[i];
    }

    return NULL;

}


int main(){
    auto start = std::chrono::high_resolution_clock::now();

    const int N = 10000;

    auto a = read_vector_from_file("Vector_a.txt");
    auto b = read_vector_from_file("Vector_b.txt");

    pthread_t threads[2];
    ThreadData data[2];

    int mid = N/2;

 
    data[0] = {0,mid,&a,&b,0};
    pthread_create(&threads[0],NULL, ComputeDotProduct, &data[0]);
 
    data[1] = {mid,N,&a,&b,0};
    pthread_create(&threads[1],NULL, ComputeDotProduct, (void*)&data[1]);


    pthread_join(threads[0],NULL);
    pthread_join(threads[1],NULL);



    float total = data[0].result + data[1].result;

    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> elapsed = end -start;

    std::cout << "Dot Product: " << total << std:: endl;
    std::cout << "Time Elapsed (with 2 threads): " << elapsed.count() << " ms" << std::endl;

    //std::ofstream out("ThreadResult.txt");
    //out << "result: " << total << "\n";
    //out << "Time Taken: " << elapsed.count() << " ms\n";
    //out.close();






    return 0;

}



