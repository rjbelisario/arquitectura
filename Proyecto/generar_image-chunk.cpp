#include <sstream>
#include <fstream>
#include <vector>
#include <string>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <iostream>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

void divide_mmap(char* data, int tamano, int n, int** chunks) {
    char* endptr;
    int num_chunks;
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
void mmap_a_imagen(int* data, int filas, int columnas, Mat& imagen) {
    int k = 0;
    for (int y = 0; y < filas; y++) {
        for (int x = 0; x < columnas; x++) {
            Vec3b& pixel = imagen.at<Vec3b>(y, x);
            pixel[0] = *(data + k++);
            pixel[1] = *(data + k++);
            pixel[2] = *(data + k++);
        }
    }
}

void imagen_a_mmap(const Mat& imagen, int* data, int& tamano) {
    tamano = 0;
    int k = 0;
    for (int y = 0; y < imagen.rows; y++) {
        for (int x = 0; x < imagen.cols; x++) {
            Vec3b pixel = imagen.at<Vec3b>(y, x);
            *(data + k++) = pixel[0];
            *(data + k++) = pixel[1];
            *(data + k++) = pixel[2];
            tamano++;
        }
    }
}

void unificar_mmap(char* data, int tamano, int n, char* resultado) {
    int num_chunks = tamano / n + (tamano % n != 0);
    int offset = 0;
    for (int i = 0; i < num_chunks; i++) {
        int chunk_size = min(n, tamano - i * n);
        memcpy(resultado + offset, data + i * n, chunk_size * sizeof(int));
        offset += chunk_size * sizeof(int);
    }
}

int main() {
    char* data;
    int* chunks;
    int tamano, n = 10;

    // imagen a mmap
    Mat imagen = imread("imagen.png");
    imagen.convertTo(imagen, CV_8UC3);
    int filas = imagen.rows;
    int columnas = imagen.cols;
    imagen_a_mmap(imagen, reinterpret_cast<int**>(&data), tamano);

    // divide mmap y guarda chunks
    divide_mmap(data, tamano, n, &chunks);
    guardar_chunks_mmap(chunks, tamano, n, "chunks.txt");

    // unificar mmap
    char* resultado = new char[tamano * sizeof(int)];
    unificar_mmap(data, tamano * sizeof(int), n, resultado);

    // mmap a imagen
    Mat imagen_resultante(filas, columnas, CV_8UC3);
    mmap_a_imagen(reinterpret_cast<int*>(resultado), filas, columnas, imagen_resultante);
    imwrite("imagen_resultante.png", imagen_resultante);
 
    munmap(data, tamano * sizeof(int));
    return 0;
}
