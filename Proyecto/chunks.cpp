#include <iostream>
#include <vector>
#include <fstream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

void dividir_vector(const vector<int>& v, int n, vector<vector<int>>& chunks) {
    int tamano_vector = v.size();
    int tamano_chunk = static_cast<int>(ceil(static_cast<double>(n) / 3) * 3); // Ajustar el tamaño del chunk para que sea un múltiplo de 3
    for (int i = 0; i < tamano_vector; i += tamano_chunk) {
        chunks.push_back(vector<int>(v.begin() + i, v.begin() + i + tamano_chunk));
    }
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

    // Crear la imagen a partir de los datos del vector
    int width = chunks.size(); // El ancho de la imagen es igual al número de chunks
    int height = chunks[0].size() / 3; // La altura de la imagen es igual al tamaño de cada chunk dividido por 3
    Mat imagen2(height, width, CV_8U);
    k = 0;
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            int i = y * 3;
            imagen2.at<uchar>(y, x) = chunks[x][i+2]; // Canal rojo
            imagen2.at<uchar>(y, x+width) = chunks[x][i+1]; // Canal verde
            imagen2.at<uchar>(y, x+2*width) = chunks[x][i]; // Canal azul
        }
    }
    cvtColor(imagen2, imagen2, COLOR_BGR2RGB); // Convertir de BGR a RGB

    // Mostrar la imagen en una ventana
    namedWindow("Imagen RGB generada", WINDOW_NORMAL);
    imshow("Imagen RGB generada", imagen2);
    waitKey(0);

    // Guardar la imagen en un archivo
    imwrite("imagen_generada.png", imagen2);
    // Guardar los chunks en un archivo de texto
    guardar_chunks(chunks, "chunks.txt");

    return 0;
}
