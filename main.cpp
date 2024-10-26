#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <unordered_map>
#include <algorithm>

#define INT_MAX 2147483647

using namespace std; // Adicionando o namespace std

class Grafo {
public:
    int centro_count; // Contador de centros urbanos
    vector<vector<int>> adj; // Lista de adjacência
    unordered_map<string, int> centro_index; // Mapeia nomes de centros urbanos para índices
    vector<string> nomes_centros; // Armazena os nomes dos centros urbanos
    string capital; // Nome da capital

    // para a DFS e SCC
    vector<bool> visited; // Vetor de visitados
    vector<vector<int>> scc_componentes; // Componentes fortemente conexas
    vector<vector<int>> scc_adj; // Lista de adjacência das componentes fortemente conexas


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

    void encontrar_capital() {
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

        // nome do centro que será a capital
        capital = nomes_centros[melhor_centro]; // Nome da capital
    }

    void dfs_batalhao(int vertice, vector<int> &output, vector<vector<int>> &adj_ref) {
        visited[vertice] = true;
        for (auto vertice_visita : adj_ref[vertice])
            if (!visited[vertice_visita])
                dfs_batalhao(vertice_visita, output, adj_ref);
        output.push_back(vertice);
    }

    void contar_batalhoes_secundarios() {
        vector<int> ordenador;
        visited.assign(centro_count, false);
        scc_adj.clear();
        scc_componentes.clear();

        // kosaraju alg
        for ( int i = 0; i < centro_count; i++)
            if (!visited[i])
                dfs_batalhao(i, ordenador, this->adj);

        // Transpor o grafo
        vector<vector<int>> transposto(centro_count);
        for (int i = 0; i < centro_count; i++) {
            for (auto vertice: adj[i]) {
                transposto[vertice].push_back(i);
            }
        }

        visited.assign(centro_count, false);
        reverse(ordenador.begin(), ordenador.end());

        vector<int> raizes(centro_count, -1);
        // Segunda DFS
        for (auto vertice : ordenador) {
            if (!visited[vertice]) {
                vector<int> componente;
                dfs_batalhao(vertice, componente, transposto);
                scc_componentes.push_back(componente);
                int raiz = *min_element(componente.begin(), componente.end());
                for (auto inner_vertice: componente)
                    raizes[inner_vertice] = raiz;
            }
        }

        // Construir o grafo das componentes
        scc_adj.resize(scc_componentes.size());
        for (int i = 0; i < centro_count; i++) {
            for (auto vertice: adj[i]) {
                if (raizes[i] != raizes[vertice])
                    scc_adj[raizes[i]].push_back(raizes[vertice]);
            }
        }

        // printar o grafo scc
        for (int i = 0; i < scc_componentes.size(); i++) {
            cout << "Componente " << i << ": ";
            for (auto vertice : scc_componentes[i])
                cout << nomes_centros[vertice] << " ";
            cout << endl;
        }

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
    grafo.encontrar_capital();
    cout << "A capital é: " << grafo.capital << endl;

    // Chame as outras funções aqui
    grafo.contar_batalhoes_secundarios();
    return 0;
}
