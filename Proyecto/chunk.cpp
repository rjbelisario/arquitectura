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
    FILE* archivo = fopen("vector.txt", "r");
    fscanf(archivo, "%d", &tamano);
    for (int i = 0; i < tamano; i++) {
        fscanf(archivo, "%d", &valor);
        v.push_back(valor);
    }
    fclose(archivo);
}

int main() {
    int tamano, chunk_size = 4;
    vector<int> v;
    vector<vector<int>> chunks;
    cargar_datos(v, tamano);
    divide(v, chunk_size, chunks);

    // Crear la imagen a partir de los datos del vector
    int width = chunk_size;
    int height = (chunks.size() + width - 1) / width;
    Mat image(height * 100, width * 100, CV_8UC1, Scalar(0));
    int k = 0;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (k < chunks.size()) {
                int pixel_value = chunks[k++][0];
                rectangle(image, Point(x * 100 + 10, y * 100 + 10), Point(x * 100 + 90, y * 100 + 90), Scalar(pixel_value), -1);
            }
        }
    }

    // Mostrar la imagen en una ventana
    namedWindow("Figura generada", WINDOW_NORMAL);
    imshow("Figura generada", image);
    waitKey(0);

    // Guardar la imagen en un archivo
    imwrite("figura_generada.png", image);

    return 0;
}
