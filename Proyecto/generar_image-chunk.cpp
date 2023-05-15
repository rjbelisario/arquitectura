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
 
void unificar_mmap(int** sub_bloques, int num_sub_bloques, int n, int* result_ptr) {
    int pos = 0;
    for (int i = 0; i < num_sub_bloques; i++) {
        int* sub_bloque_ptr = *sub_bloques + i * n;
        for (int j = 0; j < n; j++) {
            *(result_ptr + pos++) = *(sub_bloque_ptr + j);
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
    int* memoria,sub_bloques,result_ptr;
    int tamano,n = 10,num_sub_bloques;
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
     
     
        // Dividir la memoria mapeada en sub-bloques
        start = chrono::high_resolution_clock::now();
        divide_mmap(memoria, tamano, n, &sub_bloques, num_sub_bloques);
        end = chrono::high_resolution_clock::now();
        auto duracion_divide_mmap = chrono::duration_cast<chrono::milliseconds>(end - start);
        cout << "Tiempo para dividir la memoria mapeada en sub-bloques: " << duracion_divide_mmap.count() << " ms" << endl;
     
        // Guardar los sub-bloques en un archivo
        start = chrono::high_resolution_clock::now();
        guardar_chunks_mmap(sub_bloques, num_sub_bloques * n, n, "chunks.txt");
        end = chrono::high_resolution_clock::now();
        auto duracion_guardar_chunks_mmap = chrono::duration_cast<chrono::milliseconds>(end - start);
        cout << "Tiempo para guardar los sub-bloques en un archivo: " << duracion_guardar_chunks_mmap.count() << " ms" << endl;
     
     
        // Cargar los sub-bloques desde el archivo y unificarlos en un solo bloque
        start = chrono::high_resolution_clock::now();
        result_ptr = static_cast<int*>(malloc(num_sub_bloques * n * sizeof(int)));
        cargar_datos_mmap(&sub_bloques, num_sub_bloques, n, result_ptr, "chunks.txt");
        end = chrono::high_resolution_clock::now();
        auto duracion_cargar_datos_mmap = chrono::duration_cast<chrono::milliseconds>(end - start);
        cout << "Tiempo para cargar los sub-bloques desde el archivo y unificarlos en un solo bloque: " << duracion_cargar_datos_mmap.count() << " ms" << endl;
     
        // Convertir el bloque unificado a una imagen en OpenCV
        int tamano_datos = num_sub_bloques * n * sizeof(int)/4;
        int filas = sqrt(tamano_datos / 3);
        int columnas = tamano_datos / (filas * 3);    
        Mat imagen_recuperada;
        start = chrono::high_resolution_clock::now();
        memoria_a_imagen(result_ptr, filas, columnas, imagen_recuperada);
        end = chrono::high_resolution_clock::now();
        auto duracion_memoria_a_imagen = chrono::duration_cast<chrono::milliseconds>(end - start);
        cout << "Tiempo para convertir el bloque unificado a una imagen en OpenCV: " << duracion_memoria_a_imagen.count() << " ms" << endl;
     
        // Mostrar la imagen recuperada en OpenCV
        namedWindow("Imagen recuperada", WINDOW_NORMAL);
        imshow("Imagen recuperada", imagen_recuperada);
        waitKey(0);
     
        // Liberar la memoria mapeada
        munmap(memoria, tamano);
        munmap(result_ptr, num_sub_bloques * n * sizeof(int));
    } else {
        cout << "No existe la imagen " << archivo_entrada << "." << endl;
    }
    return 0;
}
