#include <sstream>
#include <fstream>
#include <vector>
#include <string>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <iostream>
#include <chrono>
#include <limits>
#include <opencv2/opencv.hpp>
 
using namespace std;
using namespace cv;
 
void unificar_mmap(int** chunks, int n_chunks, int n, int* result_ptr) {
    int pos = 0;
    for (int i = 0; i < n_chunks; i++) {
        int* chunk_ptr = *chunks + i * n;
        for (int j = 0; j < n; j++) {
            *(result_ptr + pos++) = *(chunk_ptr + j);
        }
    }
}
 
void cargar_datos_mmap(int** chunks, int& num_chunks, int n, int* result_ptr, const string& filename) {
    ifstream archivo(filename);
    vector<int> datos;
    int valor;
    while (archivo >> valor) {
        datos.push_back(valor);
    }
    archivo.close();
 
    num_chunks = datos.size() / n + (datos.size() % n != 0);
    *chunks = new int[num_chunks * n];
 
    int* p = *chunks;
    for (int i = 0; i < datos.size(); i++) {
        *p++ = datos[i];
    }
 
    unificar_mmap(chunks, num_chunks, n, result_ptr);
}
 
void divide_mmap(int* data, int tamano, int n, int** chunks, int& num_chunks) {
    tamano/=4;
    num_chunks = tamano / n + (tamano % n != 0);
    *chunks = new int[num_chunks * n*2];
 
    int* p = *chunks;
 
    for (int i = 0; i < tamano; i++) {
        *p++ = *data++;    
    }
 
    cout << endl;
}
 
void guardar_chunks_mmap(int* chunks, int tamano, int n, const string& filename) {
    ofstream archivo(filename);
    int num_chunks = tamano / n + (tamano % n != 0);
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
 
void memoria_a_imagen(const int* memoria, int filas, int columnas, Mat& imagen) {
    imagen.create(filas, columnas, CV_8UC3);
    int k = 0;
    for (int y = 0; y < filas; y++) {
        for (int x = 0; x < columnas; x++) {
            Vec3b& pixel = imagen.at<Vec3b>(y, x);
            pixel[0] = static_cast<uchar>(memoria[k++]);
            pixel[1] = static_cast<uchar>(memoria[k++]);
            pixel[2] = static_cast<uchar>(memoria[k++]);
        }
    }
}
 
void imagen_a_memoria(const Mat& imagen, int*& memoria, int& tamano) {
    tamano = imagen.rows * imagen.cols * 3 * sizeof(int);
    memoria = reinterpret_cast<int*>(mmap(nullptr, tamano, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0));
    int k = 0;
    for (int y = 0; y < imagen.rows; y++) {
        for (int x = 0; x < imagen.cols; x++) {
            Vec3b pixel = imagen.at<Vec3b>(y, x);
            memoria[k++] = static_cast<int>(pixel[0]);
            memoria[k++] = static_cast<int>(pixel[1]);
            memoria[k++] = static_cast<int>(pixel[2]);
        }
    }
}
 
int main() {
    string archivo_entrada;
    int* memoria;
    int tamano;
    int n;
    cout << "Ingrese el nombre de la imagen incluyendo la extension: ";
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
 
        // Cargar la imagen en OpenCV
        Mat imagen = imread(archivo_entrada, IMREAD_COLOR);
        if (imagen.empty()) {
            cerr << "Error al cargar la imagen" << endl;
            return -1;
        }
     
        // Convertir la imagen a memoria mapeada
        auto start = chrono::high_resolution_clock::now();
        imagen_a_memoria(imagen, memoria, tamano);
        auto end = chrono::high_resolution_clock::now();
        auto duracion_imagen_a_memoria = chrono::duration_cast<chrono::milliseconds>(end - start);
        cout << "Tiempo para convertir la imagen a memoria mapeada: " << duracion_imagen_a_memoria.count() << " ms" << endl;
        int* chunks;
        int n_chunks;
     
        // Dividir la memoria mapeada en chunks
        start = chrono::high_resolution_clock::now();
        divide_mmap(memoria, tamano, n, &chunks, n_chunks);
        end = chrono::high_resolution_clock::now();
        auto duracion_divide_mmap = chrono::duration_cast<chrono::milliseconds>(end - start);
        cout << "Tiempo para dividir la memoria mapeada en chunks: " << duracion_divide_mmap.count() << " ms" << endl;
     
        // Guardar los chunks en un archivo
        start = chrono::high_resolution_clock::now();
        guardar_chunks_mmap(chunks, n_chunks * n, n, "chunks.txt");
        end = chrono::high_resolution_clock::now();
        auto duracion_guardar_chunks_mmap = chrono::duration_cast<chrono::milliseconds>(end - start);
        cout << "Tiempo para guardar los chunks en un archivo: " << duracion_guardar_chunks_mmap.count() << " ms" << endl;
     
        int* result_ptr;
     
        // Cargar los chunks desde el archivo chunks y unificamos en uno solo
        start = chrono::high_resolution_clock::now();
        result_ptr = static_cast<int*>(malloc(n_chunks * n * sizeof(int)));
        cargar_datos_mmap(&chunks, n_chunks, n, result_ptr, "chunks.txt");
        end = chrono::high_resolution_clock::now();
        auto duracion_cargar_datos_mmap = chrono::duration_cast<chrono::milliseconds>(end - start);
        cout << "Tiempo para cargar los chunks desde el archivo y unificarlos en un solo bloque: " << duracion_cargar_datos_mmap.count() << " ms" << endl;
     
        // Convertimos los datos obtenidos de chunks a una imagen
        int tamano_datos = n_chunks * n * sizeof(int);
        Mat imagen_recuperada;
        start = chrono::high_resolution_clock::now();
        memoria_a_imagen(result_ptr, imagen.rows, imagen.cols, imagen_recuperada);
        end = chrono::high_resolution_clock::now();
        auto duracion_memoria_a_imagen = chrono::duration_cast<chrono::milliseconds>(end - start);
        cout << "Tiempo para convertir los datos cargados nuevamente en memoria en una imagen por OpenCV: " << duracion_memoria_a_imagen.count() << " ms" << endl;
        vector<int> parametros;
        parametros.push_back(IMWRITE_JPEG_QUALITY);
        parametros.push_back(100);
     
       // Guardamos la imagen
       // verificamos si es jpeg para que no pierda tanta calidad
       string extension = archivo_entrada.substr(archivo_entrada.find_last_of(".") + 1);
       if(extension == "jpg" || extension == "jpeg")
            imwrite("recuperacion_" + archivo_entrada, imagen_recuperada, parametros);
       else 
            imwrite("recuperacion_" + archivo_entrada, imagen_recuperada);
     
        // liberamos la memoria mapeada
        munmap(memoria, tamano);
        munmap(result_ptr, n_chunks * n * sizeof(int));
    } else {
        cout << "No existe la imagen de entrada " << archivo_entrada << "." << endl;
    }
    return 0;
}
