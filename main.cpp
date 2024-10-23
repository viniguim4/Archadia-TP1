#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <unordered_map>
#include <set>

using namespace std;

class Grafo {
public:
    int num_centros;
    vector<vector<int>> adj; // Lista de adjacência
    unordered_map<string, int> centro_index; // Mapeia nomes de centros urbanos para índices

    Grafo(int n) : num_centros(n) {
        adj.resize(n);
    }

    void adicionar_estrada(const string& origem, const string& destino) {
        int u = centro_index[origem];
        int v = centro_index[destino];
        adj[u].push_back(v);
    }

    void encontrar_capital() {
        // Implementar lógica para encontrar a capital
    }

    void contar_batalhoes_secundarios() {
        // Implementar lógica para contar batalhões secundários
    }

    void verificar_patrulhamento() {
        // Implementar lógica para verificar rotas de patrulhamento
    }
};

int main() {
    int num_centros, num_estradas;
    cin >> num_centros >> num_estradas;

    Grafo grafo(num_centros);
    string centro;

    // Lendo os centros urbanos
    for (int i = 0; i < num_centros; ++i) {
        cin >> centro;
        grafo.centro_index[centro] = i; // Mapeia o nome do centro para o índice
    }

    // Lendo as estradas
    for (int i = 0; i < num_estradas; ++i) {
        string origem, destino;
        cin >> origem >> destino;
        grafo.adicionar_estrada(origem, destino);
    }

    // Chamar as funções para resolver as partes do problema
    grafo.encontrar_capital();
    grafo.contar_batalhoes_secundarios();
    grafo.verificar_patrulhamento();

    return 0;
}

