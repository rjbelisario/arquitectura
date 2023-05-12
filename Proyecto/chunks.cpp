#include <fstream>
#include <vector>
#include <string>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <iostream>
#include <limits>
#include <chrono>
using namespace std;
using namespace chrono;

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
void divide_mmap(char* data, int tamano, int n, int** chunks, int& num_chunks) {
    char* endptr;
    num_chunks = tamano / n + (tamano % n != 0); // Calcular el número de subvectores
    *chunks = new int[num_chunks*n]; // Reservar memoria para todos los subvectores
    int* p = *chunks;
    for (int i = 0; i < tamano; i++) {
        *p = strtol(data, &endptr, 10);
        p++;
        data = endptr;
    }
    for (int i = tamano; i < num_chunks*n; i++) {
        *(*chunks + i) = 0; // Inicializar los elementos restantes a cero
    }
}
void guardar_chunks_mmap(int* chunks, int tamano, int n, const string& filename) {
    ofstream archivo(filename);
    int num_chunks = tamano / n + (tamano % n != 0); // Calcular el número de subvectores
    int k = 0;
    for (int i = 0; i < num_chunks; i++) {
        for (int j = 0; j < n; j++) {
            archivo << to_string(*(chunks + k)) << " ";
            k++;
        }
        archivo << endl;
        if (k >= tamano) {
            break;
        }
    }
    archivo.close();
}
void cargar_datos_mmap(int& tamano, char** data, const string& filename) {
    int fd = open(filename.c_str(), O_RDONLY);
    struct stat st;
    fstat(fd, &st);
    *data = static_cast<char*>(mmap(nullptr, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0));
    char* endptr;
    tamano = strtol(*data, &endptr, 10);
    char* p = endptr;
    *data = p; 
    close(fd);
}
int main() {
    string archivo_entrada;
    char* data;
    int* chunks;
    int tamano, n, num_chunks;
    vector<int> v,v2;
    cout << "Ingrese el nombre del archivo incluyendo la extension: ";
    cin >> archivo_entrada;
    if (ifstream(archivo_entrada)) {
        do {
            cout << "Tamano de los subvectores: ";
            cin >> n;
            if (cin.fail()) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Debe ingresar un numero entero." << endl;
                n = 0;
            }else if (n==0)
                cout << "Debe ser mayor a cero." << endl;
            else if (n < 0) {
                cout << "Debe ser un numero positivo." << endl;
            }
        } while (n < 1 );

        auto start_cargar_datos = high_resolution_clock::now();
        cargar_datos_mmap(tamano, &data, archivo_entrada);
        auto end_cargar_datos = high_resolution_clock::now();
        duration<double> tiempo_cargar_datos = end_cargar_datos - start_cargar_datos;
        cout << "Archivo mapeado en memoria exitosamente. Tiempo de ejecución: " << tiempo_cargar_datos.count() << " segundos" << endl;

        auto start_divide_mmap = high_resolution_clock::now();
        divide_mmap(data, tamano, n, &chunks, num_chunks);
        auto end_divide_mmap = high_resolution_clock::now();
        duration<double> tiempo_divide_mmap = end_divide_mmap - start_divide_mmap;
        cout << "Datos divididos en " << n << " chunks en memoria. Tiempo de ejecución: " << tiempo_divide_mmap.count() << " segundos" << endl;


        //divide(v, n, chunks);
        //unificar_vector(chunks,v2);
        //for (int i = 0; i < tamano; ++i) {
        //    printf("%d ", strtol(p, &endptr, 10));
        //    p = endptr;
        //}
        //guardar_chunks(chunks, "chunks.txt");


        auto start_guardar_chunks_mmap = high_resolution_clock::now();        
        guardar_chunks_mmap(chunks, tamano, n, "chunks.txt");
        auto end_guardar_chunks_mmap = high_resolution_clock::now();
        duration<double> tiempo_guardar_chunks_mmap = end_guardar_chunks_mmap - start_guardar_chunks_mmap;        
        cout << "Datos guardados en chunks.txt. Tiempo de ejecución: " << tiempo_guardar_chunks_mmap.count() << " segundos" << endl;

        auto end_total = high_resolution_clock::now();
        duration<double> tiempo_total = end_total - start_cargar_datos;
        cout << "Tiempo total de ejecución: " << tiempo_total.count() << " segundos" << endl;

        munmap(data, tamano * sizeof(int) + sizeof(int));
        return 0;
    } else {
        cout << "No ha generado el archivo de entrada " << archivo_entrada << "." << endl;
    }
}