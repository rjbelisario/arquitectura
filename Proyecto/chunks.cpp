#include <sstream>
#include <fstream>
#include <vector>
#include <string>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <iostream>
using namespace std;

void divide(const vector<int>& v, int n, vector<vector<int>>& chunks) {
    int tamano_vector = v.size();
    for (int i = 0; i < tamano_vector; i += n) {
        vector<int> chunk(v.begin() + i, v.begin() + i + n);
        if (chunk.size() < n) {
            chunk.resize(n, 0);
        }
        chunks.push_back(chunk);
    }
}
void divide_mmap(char* data, int tamano, int n, vector<vector<int>>& chunks) {
    char* endptr;
    for (int i = 0; i < tamano; i += n) {
        vector<int> chunk;
        for (int j = i; j < i + n; j++) {
            chunk.push_back(strtol(data, &endptr, 10));
            data = endptr;
        }
        if (chunk.size() < n) {
            chunk.resize(n, 0);
        }
        chunks.push_back(chunk);
    }
}
void unificar_vector(const vector<vector<int>>& chunks, vector<int>& resultado) {
    for (const auto& subvector : chunks) { // Recorre cada subvector en chunks
        resultado.insert(resultado.end(), subvector.begin(), subvector.end()); // Agrega los elementos del subvector al resultado
    }
}
void cargar_datos(vector<int>& v,int& tamano, const string& filename){
    ifstream archivo(filename);
    int valor;
    archivo >> tamano;
    for (int i = 0; i < tamano; i++) {
        archivo >> valor;
        v.push_back(valor);
    }
    archivo.close();
}
void guardar_chunks(const vector<vector<int>>& chunks, const string& filename) {
    ofstream archivo(filename);
    for (const vector<int>& chunk : chunks) {
        for (int i : chunk) {
            archivo << i << " ";
        }
        archivo << endl;
    }
    archivo.close();
}
void cargar_datos_mmap(vector<int>& v, int& tamano, char** data, const string& filename) {
    int fd = open(filename.c_str(), O_RDONLY);
    struct stat st;
    fstat(fd, &st);
    *data = static_cast<char*>(mmap(nullptr, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0));
    char* endptr;
    tamano = strtol(*data, &endptr, 10);
    v.resize(tamano);
    char* p = endptr;
    *data = p; 
    for (int i = 0; i < tamano; i++) {
        v[i] = strtol(p, &endptr, 10);
        p = endptr;
    }
    close(fd);
}
int main() {
    char* data;
    int tamano, n=10;
    vector<int> v,v2;
    vector<vector<int>> chunks;
    cargar_datos_mmap(v,tamano, &data, "archivo.txt");
    divide_mmap(data, tamano, n, chunks);
    //divide(v, n, chunks);
    //unificar_vector(chunks,v2);
    //for (int i = 0; i < tamano; ++i) {
    //    printf("%d ", strtol(p, &endptr, 10));
    //    p = endptr;
    //}
    guardar_chunks(chunks, "chunks.txt");
    munmap(data, tamano * sizeof(int) + sizeof(int));
    return 0;
}
