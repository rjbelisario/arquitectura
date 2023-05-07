#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

void divide(vector<int> v, int size, vector<vector<int>>& chunks) {
    int tamano_vector = v.size();
    for (int i = 0; i < tamano_vector; i += size) {
        chunks.push_back(vector<int>(v.begin() + i, v.begin() + i + size));
    }
}

void cargar_datos(vector<int>& v,int& tamano){
    int valor;
    FILE* archivo = fopen("archivo.txt", "r");
    fscanf(archivo, "%d", &tamano);
    for (int i = 0; i < tamano; i++) {
        fscanf(archivo, "%d", &valor);
        v.push_back(valor);
    }
    fclose(archivo);
}

int main() {
    int tamano, chunk_size = 3;
    vector<int> v;
    vector<vector<int>> chunks;
    cargar_datos(v, tamano);
    divide(v, chunk_size, chunks);

    // Crear la imagen a partir de los datos del vector
    int width = (v.size() / chunk_size);
    Mat image(width, 1, CV_8UC3, Scalar(0, 0, 0));
    for (int i = 0; i < width; i++) {
        Vec3b pixel_value(chunks[i][2], chunks[i][1], chunks[i][0]);
        image.at<Vec3b>(i, 0) = pixel_value;
    }

    // Mostrar la imagen en una ventana
    namedWindow("Imagen RGB generada", WINDOW_NORMAL);
    imshow("Imagen RGB generada", image);
    waitKey(0);

    // Guardar la imagen en un archivo
    imwrite("imagen_rgb_generada.png", image);

    return 0;
}
