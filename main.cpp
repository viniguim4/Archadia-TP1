#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <unordered_map>

#define INT_MAX 2147483647

using namespace std; // Adicionando o namespace std

class Grafo {
public:
    int centro_count; // Contador de centros urbanos
    vector<vector<int>> adj; // Lista de adjacência
    unordered_map<string, int> centro_index; // Mapeia nomes de centros urbanos para índices
    vector<string> nomes_centros; // Armazena os nomes dos centros urbanos

    Grafo() : centro_count(0) {}

    void adicionar_estrada(const string& origem, const string& destino) {
        // Adiciona ou mapeia a cidade de origem
        if (centro_index.find(origem) == centro_index.end()) {
            centro_index[origem] = centro_count++;
            nomes_centros.push_back(origem); // Armazena o nome do centro
            adj.resize(centro_count); // Redimensiona a lista de adjacência
        }
        // Adiciona ou mapeia a cidade de destino
        if (centro_index.find(destino) == centro_index.end()) {
            centro_index[destino] = centro_count++;
            nomes_centros.push_back(destino); // Armazena o nome do centro
            adj.resize(centro_count); // Redimensiona a lista de adjacência
        }

        // Adiciona a estrada (grafo direcionado)
        int u = centro_index[origem];
        int v = centro_index[destino];
        adj[u].push_back(v); // Apenas adiciona a direção de origem para destino
    }

    string encontrar_capital() {
        int melhor_centro = -1;
        int max_alcançaveis = -1;
        int menor_distancia_maxima = INT_MAX;

        for (int i = 0; i < centro_count; ++i) {
            vector<int> distancias(centro_count, -1);
            queue<int> fila;
            fila.push(i);
            distancias[i] = 0;

            int cidades_alcançaveis = 0;
            int distancia_maxima = 0;

            // BFS para calcular distâncias e contar cidades alcançáveis
            while (!fila.empty()) {
                int atual = fila.front();
                fila.pop();
                cidades_alcançaveis++;

                for (int vizinho : adj[atual]) {
                    if (distancias[vizinho] == -1) { // Se não visitado
                        distancias[vizinho] = distancias[atual] + 1;
                        fila.push(vizinho);
                    }
                }
            }

            // Calcular a distância máxima a partir do centro atual
            for (int d : distancias) {
                if (d > distancia_maxima) {
                    distancia_maxima = d;
                }
            }

            // Verificar se este centro é melhor
            if (cidades_alcançaveis > max_alcançaveis ||
                (cidades_alcançaveis == max_alcançaveis && distancia_maxima < menor_distancia_maxima)) {
                max_alcançaveis = cidades_alcançaveis;
                menor_distancia_maxima = distancia_maxima;
                melhor_centro = i;
            }
        }

        // Retornar o nome do centro que será a capital
        return nomes_centros[melhor_centro]; // Nome da capital
    }

    void contar_batalhoes_secundarios() {
        // Implementar lógica para contar batalhões secundários
    }

    void verificar_patrulhamento() {
        // Implementar lógica para verificar rotas de patrulhamento
    }
};

int main() {
    int centros, num_estradas;
    cin >> centros >> num_estradas;

    Grafo grafo;
    string origem, destino;

    // Lendo as estradas
    for (int i = 0; i < num_estradas; ++i) {
        cin >> origem >> destino;
        grafo.adicionar_estrada(origem, destino);
    }

    // Chamar a função para encontrar e imprimir a capital
    string capital = grafo.encontrar_capital();
    cout << "A capital é: " << capital << endl;

    // Chame as outras funções aqui

    return 0;
}
