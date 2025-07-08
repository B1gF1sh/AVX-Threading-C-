#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>


const int N =10000;

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

int main(){
    
    auto a = read_vector_from_file("Vector_a.txt");
    auto b = read_vector_from_file("Vector_b.txt");

    float result = 0.0f;
    auto start = std::chrono::high_resolution_clock::now();

    for(int i=0; i< a.size(); i++){
        result += a[i] * b[i];
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;

    std::cout << "Dot Product: " << result <<std::endl;
    std::cout << "Time: " << elapsed.count() << " ms" << std::endl;

    //std::ofstream out("SingleThreadResult.txt");
    //out << "Dot: " << result << "\n";
    //out << "Time: " << elapsed.count() << " ms\n";
    //out.close();

    return 0;
}
