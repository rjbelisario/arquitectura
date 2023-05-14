#include <iostream>
#include <fstream>
#include <unistd.h>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

void imagen_a_memoria(const Mat& imagen, int*& memoria, int& tamano) {
    tamano = imagen.rows * imagen.cols * 3;
    memoria = new int[tamano];
    int k = 0;
    for (int y = 0; y < imagen.rows; y++) {
        for (int x = 0; x < imagen.cols; x++) {
            Vec3b pixel = imagen.at<Vec3b>(y, x);
            memoria[k++] = pixel[0];
            memoria[k++] = pixel[1];
            memoria[k++] = pixel[2];
        }
    }
}

void dividir_memoria(int* memoria, int tamano, int n, int** subvectores, int& tamano_subvector) {
    int tamano_subvector_int = tamano / n;
    tamano_subvector = tamano_subvector_int * sizeof(int);
    *subvectores = new int[tamano_subvector_int * n];
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < tamano_subvector_int; j++) {
            (*subvectores)[i * tamano_subvector_int + j] = memoria[i * tamano_subvector_int + j];
        }
    }
}

void unificar_memoria(int* memoria, int tamano, int n, int* resultado) {
    int tamano_subvector_int = tamano / n;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < tamano_subvector_int; j++) {
            resultado[i * tamano_subvector_int + j] = memoria[i * tamano_subvector_int + j];
        }
    }
}

void memoria_a_imagen(int* memoria, int filas, int columnas, Mat& imagen) {
    int k = 0;
    for (int y = 0; y < filas; y++) {
        for (int x = 0; x < columnas; x++) {
            Vec3b& pixel = imagen.at<Vec3b>(y, x);
            pixel[0] = memoria[k++];
            pixel[1] = memoria[k++];
            pixel[2] = memoria[k++];
        }
    }
}

void guardar_subvectores(int* subvectores, int tamano_subvector, int n, const char* archivo) {
    ofstream ofs(archivo, ios::out | ios::binary);
    ofs.write(reinterpret_cast<const char*>(&n), sizeof(n));
    ofs.write(reinterpret_cast<const char*>(&tamano_subvector), sizeof(tamano_subvector));
    for (int i = 0; i < n; i++) {
        ofs.write(reinterpret_cast<const char*>(subvectores + i * tamano_subvector / sizeof(int)), tamano_subvector);
    }
    ofs.close();
}

void cargar_subvectores(int** subvectores, int& tamano_subvector, int& n, const char* archivo) {
    ifstream ifs(archivo, ios::in | ios::binary);
    ifs.read(reinterpret_cast<char*>(&n), sizeof(n));
    ifs.read(reinterpret_cast<char*>(&tamano_subvector), sizeof(tamano_subvector));
    *subvectores = new int[tamano_subvector / sizeof(int) * n];
    for (int i = 0; i < n; i++) {
        ifs.read(reinterpret_cast<char*>(*subvectores + i * tamano_subvector / sizeof(int)), tamano_subvector);
    }
    ifs.close();
}

int main() {
    int* memoria;
    int* subvectores;
    int tamano, tamano_subvector, n = 10;

    // imagen a memoria
    Mat imagen = imread("imagen.png");
    imagen.convertTo(imagen, CV_8UC3);
    int filas = imagen.rows;
    int columnas = imagen.cols;
    imagen_a_memoria(imagen, memoria, tamano);

    // divide memoria y guarda subvectores
    dividir_memoria(memoria, tamano, n, &subvectores, tamano_subvector);
    guardar_subvectores(subvectores, tamano_subvector, n, "subvectores.bin");

    // unificar memoria
    int* resultado = new int[tamano];
    unificar_memoria(subvectores, tamano_subvector * n, n, resultado);

    // memoria a imagen
    Mat imagen_resultante(filas, columnas, CV_8UC3);
    memoria_a_imagen(resultado, filas, columnas, imagen_resultante);
    imwrite("imagen_resultante.png", imagen_resultante);

    // limpiar memoria
    delete[] memoria;
    delete[] subvectores;
    delete[] resultado;

    return 0;
}
