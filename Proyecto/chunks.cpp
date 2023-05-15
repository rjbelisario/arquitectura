#include <fstream>
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

void divide_mmap(char* data, int &tamano, int n, int* chunks) { // Divide los chunks directamente en memoria
    char* endptr;
    for (int i = 0; i < tamano; i++) {
        *(chunks + i) = strtol(data, &endptr, 10);
        data = endptr;
    }
    int num_chunks = tamano / n + (tamano % n != 0); // Calcular el numero de chunks

    // Este for hara que todos los chunks sean pares en caso de que sea necesario multiplicar matrices.
    // Solo ajusta el ultimo chunk para que este se rellene con ceros y hacerlos todos del mismo tamano
    for (int i = tamano; i < num_chunks*n; i++) {
        *(chunks + i) = 0; // Inicializar los elementos restantes a cero
    }
    tamano=num_chunks*n; // ajusta el nuevo tamanno si el chunk es impar
}

void guardar_chunks_mmap(int* chunks, int tamano, int n, const string& filename) { // Guarda los chunks en el archivo
    ofstream archivo(filename);
    int num_chunks = tamano / n + (tamano % n != 0); // Calcular el numero de chunks
    int k = 0;
    while (k < tamano) {
        for (int j = 0; j < n; j++) {
            archivo << to_string(*(chunks + k)) << " ";
            k++;
            if (k >= tamano) {
                break;
            }
        }
        archivo << endl;
    }
    archivo.close();
}

void cargar_datos_mmap(int& tamano, char** data, const string& filename) { // carga toda la informacion desde el archivo de entrada
    int fd = open(filename.c_str(), O_RDONLY);
    if (fd < 0) {
        cerr << "Error: no se pudo abrir el archivo de entrada." << endl;
        exit(1); // verificamos que realmente exista el archivo. Esto en caso de error o corrupcion en el algoritmo principal
    }
    struct stat st;
    if (fstat(fd, &st) < 0) { // funcion del mmap para obtener la informacion del archivo_entrada
        cerr << "Error: no se pudo obtener el tamaño del archivo de entrada." << endl;
        exit(1);
    }
    *data = reinterpret_cast<char*>(mmap(nullptr, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0));
    if (*data == MAP_FAILED) { //funcion del mmap para verificar que se mapeo exitosamente en memoria
        cerr << "Error: no se pudo mapear el archivo en memoria." << endl;
        exit(1);
    }
    char* endptr;
    tamano = strtol(*data, &endptr, 10); // el strtol es para convertir el numero en entero
    char* p = endptr;
    *data = p;
    close(fd);
}

int main() {
    string archivo_entrada;
    char* data;
    int tamano, n;
    cout << "Ingrese el nombre del archivo incluyendo la extension: ";
    cin >> archivo_entrada;
    if (ifstream(archivo_entrada)) {
        do {
            cout << "Tamano de los chunks: ";
            cin >> n;
            if (cin.fail() || n <= 0) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Debe ingresar un numero entero positivo." << endl;
            }
        } while (n <= 0);

        auto start_cargar_datos = high_resolution_clock::now();
        cargar_datos_mmap(tamano, &data, archivo_entrada);
        auto end_cargar_datos = high_resolution_clock::now();
        duration<double> tiempo_cargar_datos = end_cargar_datos - start_cargar_datos;
        cout << "Archivo mapeado en memoria exitosamente. Tiempo de ejecución: " << tiempo_cargar_datos.count() << " segundos" << endl;

        auto start_divide_mmap = high_resolution_clock::now();
        int num_chunks = tamano / n + (tamano % n != 0); // Calcular el número de chunks
        int* chunks = new int[num_chunks * n];
        divide_mmap(data, tamano, n, chunks);
        auto end_divide_mmap = high_resolution_clock::now();
        duration<double> tiempo_divide_mmap = end_divide_mmap - start_divide_mmap;
        cout << "Datos divididos en " << n << " chunks en memoria. Tiempo de ejecución: " << tiempo_divide_mmap.count() << " segundos" << endl;

        auto start_guardar_chunks_mmap = high_resolution_clock::now();        
        guardar_chunks_mmap(chunks, tamano, n, "chunks.txt");
        auto end_guardar_chunks_mmap = high_resolution_clock::now();
        duration<double> tiempo_guardar_chunks_mmap = end_guardar_chunks_mmap - start_guardar_chunks_mmap;        
        cout << "Datos guardados en chunks.txt. Tiempo de ejecución: " << tiempo_guardar_chunks_mmap.count() << " segundos" << endl;

        auto end_total = high_resolution_clock::now();
        duration<double> tiempo_total = end_total - start_cargar_datos;
        cout << "Tiempo total de ejecución: " << tiempo_total.count() << " segundos" << endl;

        munmap(data, tamano * sizeof(int) + sizeof(int));
        delete[] chunks;
        return 0;
    } else {
        cout << "No ha generado el archivo de entrada " << archivo_entrada << "." << endl;
        return 1;
    }
}
