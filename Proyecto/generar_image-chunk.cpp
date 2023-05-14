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

void unificar_vector(const vector<vector<int>>& chunks, vector<int>& resultado) {
    for (const auto& subvector : chunks) { // Recorre cada subvector en chunks
        resultado.insert(resultado.end(), subvector.begin(), subvector.end()); // Agrega los elementos del subvector al resultado
    }
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
    int* memoria;
    int tamano;
    // Cargar la imagen en OpenCV
    Mat imagen = imread("imagen.jpg", IMREAD_COLOR);
    if (imagen.empty()) {
        cerr << "Error al cargar la imagen" << endl;
        return -1;
    }

    // Convertir la imagen a memoria mapeada
    imagen_a_memoria(imagen, memoria, tamano);

    // ... Hacer algo con la memoria mapeada ...

    // Convertir la memoria mapeada a una imagen en OpenCV
    Mat imagen_recuperada;
    memoria_a_imagen(memoria, imagen.rows, imagen.cols, imagen_recuperada);

    // Mostrar la imagen recuperada en OpenCV
    namedWindow("Imagen recuperada", WINDOW_NORMAL);
    imshow("Imagen recuperada", imagen_recuperada);
    waitKey(0);

    // Liberar la memoria mapeada
    munmap(memoria, tamano);

    return 0;
}
