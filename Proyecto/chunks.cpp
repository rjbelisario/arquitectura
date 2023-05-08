#include <fstream>
#include <vector>
#include <iostream>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

using namespace std;

void divide(vector<int> v, int n, vector<vector<int>>& chunks) {
    int tamano_vector = v.size();

    for (int i = 0; i < tamano_vector; i += n) {
        chunks.push_back(vector<int>(v.begin() + i, v.begin() + i + n));
    }
}
void cargar_datos(vector<int>& v,int& tamano, const string& filename){
    ifstream archivo("archivo.txt");
    int valor;
    archivo >> tamano;
    for (int i = 0; i < tamano; i++) {
        archivo >> valor;
        v.push_back(valor);
    }
    archivo.close();
}
void guardar_chunks(const std::vector<std::vector<int>>& chunks, const std::string& filename) {
    std::ofstream archivo(filename);
    for (const std::vector<int>& chunk : chunks) {
        for (int i : chunk) {
            archivo << i << " ";
        }
        archivo << std::endl;
    }
    archivo.close();
}
void cargar_datos_mmap(std::vector<int>& v, int& tamano, const std::string& filename) {
    int fd = open(filename.c_str(), O_RDONLY);
    struct stat st;
    fstat(fd, &st);
    tamano = st.st_size / sizeof(int);
    int* data = static_cast<int*>(mmap(nullptr, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0));
    for (int i = 1; i <= tamano; i++) {
        v.push_back(data[i]);
    }
    munmap(data, st.st_size);
    close(fd);
}
int main() {
    int tamano, n=3;
    vector<int> v;
    vector<vector<int>> chunks;
    cargar_datos(v,tamano, "archivo.txt");
    cout << v[0];
    divide(v, n, chunks);
    guardar_chunks(chunks, "chunks.txt");
    return 0;
}
