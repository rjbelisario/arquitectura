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
void divide_mmap(char* data, int tamano, int n, vector<vector<int>>& chunks) {
    int elementos_faltantes = n - (tamano % n);
    if (tamano % n != 0) {
        for (int i = 0; i < elementos_faltantes; i++) {
            // agregar elementos faltantes con valor cero
            chunks.push_back(vector<int>(n, 0));
        }
    }
 
    char* endptr;
    int* p_data = reinterpret_cast<int*>(data); // convertir el puntero a int*
 
    for (int i = 0; i < tamano; i += n) {
        vector<int> subvector;
        for (int j = i + 1; j <= i + n; j++) {
            subvector.push_back(strtol(data, &endptr, 10));
            data = endptr + 1;
        }
        chunks.push_back(subvector);
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
 
void vector_a_imagen(const vector<int>& v, int filas, int columnas, Mat& imagen) {
    // Copiar los valores del vector en los píxeles de la imagen
    int k = 0;
    for (int y = 0; y < filas; y++) {
        for (int x = 0; x < columnas; x++) {
            Vec3b& pixel = imagen.at<Vec3b>(y, x);
            pixel[0] = v[k++];
            pixel[1] = v[k++];
            pixel[2] = v[k++];
        }
    }
}
 
void imagen_a_vector(const Mat& imagen, vector<int>& v) {
    v.resize(imagen.rows * imagen.cols * 3);
    int k = 0;
    for (int y = 0; y < imagen.rows; y++) {
        for (int x = 0; x < imagen.cols; x++) {
            Vec3b pixel = imagen.at<Vec3b>(y, x);
            v[k++] = pixel[0];
            v[k++] = pixel[1];
            v[k++] = pixel[2];
        }
    }
}
 
int main() {
    char* data;
    int tamano, n=10;
    vector<int> v,v2;
    vector<vector<int>> chunks;
    cargar_datos(v,tamano, "archivo.txt");
    //divide(v, n, chunks);
    //for (int i = 0; i < tamano; ++i) {
    //    printf("%d ", strtol(p, &endptr, 10));
    //    p = endptr;
    //}
 
    // imagen a vector
    Mat imagen = imread("imagen.png");
    imagen_a_vector(imagen, v);
 
    //chunks y unificar chunks
    divide(v, n, chunks);
    guardar_chunks(chunks, "chunks.txt");
    unificar_vector(chunks,v2);
 
    // vector a imagen
    int tamano_vector = v2.size();
    int filas = sqrt(tamano_vector / 3);
    int columnas = tamano_vector / (filas * 3);
    Mat imagen_resultante(filas, columnas, CV_8UC3);
    vector_a_imagen(v2, filas, columnas, imagen_resultante);
    imwrite("imagen_resultante.png", imagen_resultante);
 
    //munmap(data, tamano * sizeof(int) + sizeof(int));
    return 0;
}
