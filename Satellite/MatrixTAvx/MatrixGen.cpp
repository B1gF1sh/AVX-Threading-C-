#include <iostream>
#include <fstream>
#include <random>


void Generate_Matrix_File(const std::string &filename, int rows, int columns, float min_value, float max_value){

    std::ofstream file(filename);
    if(!file){
        std::cerr << "File cannot open: " << filename << std::endl;
        return;
    }


    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(min_value,max_value);

    for(int i=0; i< rows; i++){
        for(int j=0; j<columns;j++){
            file << dist(gen);
            if (j < columns-1)
            file << " ";
        }
        file << " \n";
    }

    file.close();

    std::cout << " Matrix File Generated: " << filename << std::endl;

}


int main(){

    Generate_Matrix_File("matrix_a.txt",1000,1000,0.0f,5.0f);
    Generate_Matrix_File("matrix_b.txt",1000,1000,0.0f,5.0f);

    return 0; 




}