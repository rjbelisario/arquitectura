#include <iostream>
#include <fstream>
#include <random>

using namespace std;

int main() {
    int tamano, aleatorio;
    random_device n_aleatorio;
    mt19937 mt(n_aleatorio());
    uniform_int_distribution<int> dist(1, 100);
    cout << "Ingrese el tamaño entero: ";
    cin >> tamano;
    // Genera el archivo
    ofstream archivo("archivo.txt");
    archivo << tamano << ' '; // tamaño en la primera posicion
    // llena el archivo
    for (int i = 0; i < tamano; ++i) {
        aleatorio = dist(n_aleatorio);
        archivo << aleatorio;
        if (i != tamano-1)
            archivo << " ";
    }
    archivo << endl;
    archivo.close();//cierra el archivo
    getchar();
    return 0;
}
