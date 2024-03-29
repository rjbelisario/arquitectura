#include <iostream>
#include <fstream>
#include <random>
#define N 9

using namespace std;

int main() {
    int tamano_vector, aleatorio;
    random_device n_aleatorio;
    mt19937 mt(n_aleatorio());
    uniform_int_distribution<int> dist(1, N);
    cout << "Ingrese el tamaño en numeros entero: ";
    cin >> tamano_vector;
    // Genera el archivo
    ofstream archivo("archivo.txt");
    archivo << tamano_vector << ' ' <<endl; // tamaño en la primera posicion
    // llena el archivo
    for (int i = 0; i < tamano_vector; ++i) {
        aleatorio = dist(n_aleatorio);
        archivo << aleatorio << " ";
    }
    archivo << endl;
    archivo.close();//cierra el archivo
    getchar();
    return 0;
}
