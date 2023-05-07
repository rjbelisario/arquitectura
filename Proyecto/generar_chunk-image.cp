#include <iostream>
#include <vector>
#include <fstream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

void cargar_chunks(const string& filename, vector<vector<int>>& chunks) {
    ifstream archivo(filename);
    string linea;
    while (getline(archivo, linea)) {
        stringstream ss(linea);
        int valor;
        vector<int> chunk;
        while (ss >> valor) {
            chunk.push_back(valor);
        }
        chunks.push_back(chunk);
    }
    archivo.close();
}

int main() {
    // Cargar los chunks desde el archivo de texto
    vector<vector<int>> chunks;
    cargar_chunks("chunks.txt", chunks);

    // Crear la imagen a partir de los datos del vector
    int width = chunks.size(); // El ancho de la imagen es igual al número de chunks
    int height = chunks[0].size() / 3; // La altura de la imagen es igual al tamaño de cada chunk dividido por 3
    Mat imagen(height, width, CV_8UC3, Scalar(0, 0, 0));
    int k = 0;
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            int i = y * 3;
            Vec3b pixel_value(chunks[x][i+2], chunks[x][i+1], chunks[x][i]);
            imagen.at<Vec3b>(y, x) = pixel_value;
        }
    }

    // Mostrar la imagen en una ventana
    namedWindow("Imagen RGB generada", WINDOW_NORMAL);
    imshow("Imagen RGB generada", imagen);
    waitKey(0);

    // Guardar la imagen en un archivo
    imwrite("imagen_generada.png", imagen);

    return 0;
}
