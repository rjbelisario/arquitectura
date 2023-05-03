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

int main() {
    FILE* archivo = fopen("archivo.txt", "r");
    int tamano,valor;
    fscanf(archivo, "%d", &tamano);
    vector<int> v;
    for (int i = 0; i < tamano; i++) {
        fscanf(archivo, "%d", &valor);
        v.push_back(valor);
    }

    for (int i=0; i < tamano; i++)
        cout << v[i] << " ";
    cout << endl;
    int n,elem1,elem2, band=1;
    while(band<0 or band>0){
        cout << "Tamano del Vector " << v.size() << endl;
        cout << "Ingrese cuantas veces desea partir el vector: ";
        cin >> n;
        vector<vector<int>> chunks;
        divide(v, n, chunks);
        for (int i = 0; i < chunks.size(); i++) {
            cout << "[";
            for (int j = 0; j < chunks[i].size(); j++) {
                cout << chunks[i][j];
                cout << (chunks[i].size() > j+1 ? "," : "");
            }
            cout << "]"<< endl;
        }
        cout << "Divisiones totales: " << n << " particiones" << endl;
        cout << "Buscar elemento: ";
        cin >> elem1;
        for (int i = 0; i < chunks.size(); i++) {
            for (int j = 0; j < chunks[i].size(); j++) {
                if(chunks[i][j]==elem1)
                    cout <<"Posicion: Vector:"<<i+1<<" Pos:"<<j+1 <<endl;
            }
        }
        cout << endl;

        cout << "Consultar datos (VECTOR): ";
        cin >> elem1;
        cout << "Consultar datos (POS): ";
        cin >> elem2;
        cout << ((chunks[elem1-1].size() > 0 && chunks.size()>=elem1-1 && elem2-1<=chunks[elem1-1].size()) ? to_string(chunks[elem1-1][elem2-1]) : "No se encuentra dentro del vector") << endl;
        cout << "Ingrese 0 para salir: ";
        cin >> band;
    }
    return 0;
}
