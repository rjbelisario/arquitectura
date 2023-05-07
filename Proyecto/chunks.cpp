#include <iostream>
#include <vector>

using namespace std;

void divide(vector<int> v, int n, vector<vector<int>>& chunks) {
    int tamano_vector = v.size();
    int tamano_particion = tamano_vector / n;
    int r = tamano_vector % n;
    int aux = 0;

    for (int i = 0; i < n; i++) {
        int extra = (i < r) ? 1 : 0;
        int inicio = aux;
        int fin = aux + tamano_particion + extra;
        chunks.push_back(vector<int>(v.begin() + inicio, v.begin() + fin));
        aux = fin;
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
    int tamano, n=50;
    vector<int> v;
    vector<vector<int>> chunks;
    cargar_datos(v,tamano);
    divide(v, n, chunks);
    return 0;
}
