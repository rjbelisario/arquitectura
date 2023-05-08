#include <sstream>
#include <fstream>
#include <vector>
#include <string>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

using namespace std;

void divide(vector<int> v, int n, vector<vector<int>>& chunks) {
    int tamano_vector = v.size();
    int elementos_faltantes = n - (tamano_vector % n);
    if (tamano_vector % n != 0) {
        for (int i = 0; i < elementos_faltantes; i++) {
            v.push_back(0);
        }
    }

    for (int i = 0; i < v.size(); i += n) {
        chunks.push_back(vector<int>(v.begin() + i, v.begin() + i + n));
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
void cargar_datos_mmap(vector<int>& v, int& tamano, const string& filename) {
    int fd = open(filename.c_str(), O_RDONLY);
    struct stat st;
    fstat(fd, &st);
    const char* data = static_cast<const char*>(mmap(nullptr, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0));
    string str(data, st.st_size);
    istringstream iss(str);
    int valor;
    iss >> tamano;
    while (iss >> valor) {
        v.push_back(valor);
    }
    tamano = v.size();
    close(fd);
}
int main() {
    int tamano, n=3;
    vector<int> v;
    vector<vector<int>> chunks;
    cargar_datos_mmap(v,tamano, "archivo.txt");
    divide(v, n, chunks);
    guardar_chunks(chunks, "chunks.txt");
    return 0;
}
