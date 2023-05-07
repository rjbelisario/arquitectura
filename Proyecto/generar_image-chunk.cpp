#include <iostream>
#include <vector>
#include <fstream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

void dividir_vector(const vector<int>& v, int n, vector<vector<int>>& chunks) {
    int tamano_vector = v.size();
    for (int i = 0; i < tamano_vector; i += n) {
        chunks.push_back(vector<int>(v.begin() + i, v.begin() + i + n));
    }
}

void guardar_chunks(const vector<vector<int>>& chunks, const string& filename) {
    ofstream archivo(filename);
    for (const vector<int>& chunk : chunks) {
        for (int i : chunk) {
            archivo << i << " ";
        }
    }
    archivo.close();
}

int main() {
    // Leer la imagen y convertirla en un vector
    Mat imagen = imread("imagen.png");
    vector<int> v(imagen.rows * imagen.cols * 3);
    int k = 0;
    for (int y = 0; y < imagen.rows; y++) {
        for (int x = 0; x < imagen.cols; x++) {
            Vec3b pixel = imagen.at<Vec3b>(y, x);
            v[k++] = pixel[0];
            v[k++] = pixel[1];
            v[k++] = pixel[2];
        }
    }

    // Dividir el vector en chunks de tamaño n
    int n = 9;
    vector<vector<int>> chunks;
    dividir_vector(v, n, chunks);

    // Guardar los chunks en un archivo de texto lineal
    guardar_chunks(chunks, "chunks.txt");

    return 0;
}
