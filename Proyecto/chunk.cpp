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
    int tamano,chunk_size=4;
    vector<int> v;
    vector<vector<int>> chunks;
    cargar_datos(v,tamano);
    divide(v, chunk_size, chunks);

   // Unir los chunks en una nueva imagen
    int width = chunk_size;
    int height = (chunks.size() + width - 1) / width;
    Mat result(height, width, CV_32SC1);
    int k = 0;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (k < chunks.size()) {
                result.at<int>(y, x) = chunks[k++][0];
            } else {
                result.at<int>(y, x) = 0;
            }
        }
    }

    // Escalar los valores de la imagen para mostrarla correctamente
    double min_val, max_val;
    minMaxLoc(result, &min_val, &max_val);
    result.convertTo(result, CV_8UC1, 255.0 / (max_val - min_val), -255.0 * min_val / (max_val - min_val));

    // Mostrar la imagen resultante
    namedWindow("Resultado", WINDOW_NORMAL);
    imshow("Resultado", result);
    waitKey(0);

    // Guardar la imagen resultante en un archivo
    imwrite("resultado.png", result);

    return 0;
}
