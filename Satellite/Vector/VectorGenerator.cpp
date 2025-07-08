#include<iostream>
#include<fstream>
#include<random>

const int N = 10000;



void generate_vector_file(const std::string& filename, float min_value, float max_value){

    std::ofstream file(filename);
    if(!file){
        std::cerr << "Cannot open file: " << filename << std::endl;
        return;
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(min_value,max_value);

    for(int i=0;i<N; ++i){
        file << dist(gen) << "\n";
    }

    file.close();
    std::cout << "Generated " << filename << std::endl;
 }

 int main(){
    generate_vector_file("Vector_a.txt",0.0f, 5.0f);
    generate_vector_file("Vector_b.txt",0.0f, 5.0f);
    return 0;
 }