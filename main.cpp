#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <unordered_map>
#include <algorithm>
#include <stack>

#define INT_MAX 2147483647

using namespace std;

class Grafo {
public:
    int centro_count;
    vector<vector<int>> adj; // Lista de adjacência estruturada para manter informaçao do grafo
    unordered_map<string, int> centro_index; // Mapeia nomes de centros urbanos para índices
    vector<string> nomes_centros; // Armazena os nomes dos centros urbanos para garantir a ordenaçao do mapeamento
    string capital;
    vector<int> distancias_eleitas; // Vetor de distâncias da capital para cada centro urbano

    // para a DFS e SCC
    vector<bool> visited; // Vetor de visitados para a DFS
    vector<vector<int>> scc_componentes;
    vector<vector<int>> scc_adj;
    vector<int> batalhao_eleito_id;

    // vetor de rotas
    vector<vector<int>> rotas_para_imprimir;

    Grafo() : centro_count(0) {}

    void adicionar_estrada(const string& origem, const string& destino) {

        // Adiciona ou mapeia a cidade de origem
        if (centro_index.find(origem) == centro_index.end()) {
            centro_index[origem] = centro_count++;
            nomes_centros.push_back(origem);
            adj.resize(centro_count);
        }

        // Adiciona ou mapeia a cidade de destino
        if (centro_index.find(destino) == centro_index.end()) {
            centro_index[destino] = centro_count++;
            nomes_centros.push_back(destino);
            adj.resize(centro_count);
        }

        // Adiciona a estrada (grafo direcionado)
        int u = centro_index[origem];
        int v = centro_index[destino];
        adj[u].push_back(v);
    }

    void encontrar_capital() {
        int melhor_centro = -1;
        int max_alcançaveis = -1;
        int menor_distancia_maxima = INT_MAX;

        // Para cada centro urbano, calcular a quantidade de cidades alcançáveis e a distância máxima
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

            // Verificar se este centro é melhor do que o eleito anteriormente
            if (cidades_alcançaveis > max_alcançaveis ||
                (cidades_alcançaveis == max_alcançaveis && distancia_maxima < menor_distancia_maxima)) {
                max_alcançaveis = cidades_alcançaveis;
                menor_distancia_maxima = distancia_maxima;
                melhor_centro = i;
                distancias_eleitas = distancias;
            }
        }

        // nome do centro que será a capital
        capital = nomes_centros[melhor_centro];
        cout << capital << endl;
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
        // Primeira DFS
        for ( int i = 0; i < centro_count; i++) {
            if (!visited[i]) { dfs_batalhao(i, ordenador, this->adj); }
        }

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
        scc_adj.resize(centro_count);
        for (int i = 0; i < centro_count; i++) {
            for (auto vertice: adj[i]) {
                if (raizes[i] != raizes[vertice])
                    scc_adj[raizes[i]].push_back(raizes[vertice]);
            }
        }

        // printar o numero de batalhoes secundarios
        cout << scc_componentes.size()-1 << endl;

        batalhao_eleito_id.resize(scc_componentes.size());
        // para cada componente fortemente conexa eleger um vertice original para se tornar um batalhao com base na proximidade da capital olhar o vetor de distancias eleitas se a componente possuir uma capital apenas continue
         for (int i = 0; i < scc_componentes.size(); i++) {

            // verifica a presença da capital na componente
            static bool capital_scc_encontrada = false;
            if (!capital_scc_encontrada) {
                for (auto vertice : scc_componentes[i]) {
                    if (nomes_centros[vertice] == capital) {
                        capital_scc_encontrada = true;
                        batalhao_eleito_id[i] = vertice;
                        goto just_continue_outer;
                    }
                }
            }

            // se a componente possuir apenas um vertice eleger ele como batalhao senao eleger o vertice mais proximo da capital
            if (scc_componentes[i].size() == 1) {
                cout <<  nomes_centros[scc_componentes[i][0]] << endl;
                batalhao_eleito_id[i] = scc_componentes[i][0];
            } else {
                int melhor_vertice = scc_componentes[i][0];
                for (int j = 1; j < scc_componentes[i].size(); j++) {
                    if (distancias_eleitas[scc_componentes[i][j]] < distancias_eleitas[melhor_vertice]) {
                        melhor_vertice = scc_componentes[i][j];
                    }
                }
                cout <<  nomes_centros[melhor_vertice] << endl;
                batalhao_eleito_id[i] = melhor_vertice;
            }

            // label para dar um continue outer se achou a capital na componente
            just_continue_outer: ;
        }
    }

    void verificar_patrulhamento() {

        // para cada componente fortemente conexa verificar se o subgrafo é euleriano e se for encontrar o ciclo euleriano
        for (int i = 0; i < scc_componentes.size(); ++i) {
            // se a componente possuir mais de um vertice medir esforço para encontrar ciclo euleriano
            if (scc_componentes[i].size() > 1) {
                vector<int> grau_entrada(centro_count, 0);
                vector<int> grau_saida(centro_count, 0);

                // Criar subgrafo para a SCC atual
                vector<vector<int>> subgrafo(centro_count);
                for (int u : scc_componentes[i]) {
                    for (int v : adj[u]) {
                        if (find(scc_componentes[i].begin(), scc_componentes[i].end(), v) != scc_componentes[i].end()) {
                            subgrafo[u].push_back(v);
                            grau_saida[u]++;
                            grau_entrada[v]++;
                        }
                    }
                }

                // Verificar se o subgrafo é euleriano
                bool euleriano = true;
                for (int u : scc_componentes[i]) {
                    if (grau_entrada[u] != grau_saida[u]) {
                        euleriano = false;
                        break;
                    }
                }

                // Encontrar ciclo euleriano
                if (euleriano) {
                    vector<int> rota;
                    encontrar_ciclo_euleriano(batalhao_eleito_id[i], rota, subgrafo);
                    if (rota.size() > 1) {
                        rota.pop_back();
                        rotas_para_imprimir.push_back(rota);
                    }
                }
            }
        }


        cout << rotas_para_imprimir.size() << endl;
        for (vector<int> rota : rotas_para_imprimir) {
            for (int v : rota) {
                cout << nomes_centros[v] << " ";
            }
            cout << endl;
        }
    }


    void encontrar_ciclo_euleriano(int u, vector<int>& rota, vector<vector<int>>& subgrafo) {
        stack<int> pilha;
        pilha.push(u);

        // DFS para encontrar o ciclo euleriano
        while (!pilha.empty()) {
            int v = pilha.top();
            if (!subgrafo[v].empty()) {
                int w = subgrafo[v].back();
                subgrafo[v].pop_back();
                pilha.push(w);
            } else {
                rota.push_back(v);
                pilha.pop();
            }
        }
        reverse(rota.begin(), rota.end());
    }
};


int main() {
    // Ler o número de centros urbanos e numero estradas
    int centros, num_estradas;
    cin >> centros >> num_estradas;

    Grafo grafo;
    string origem, destino;

    // Lendo as arestas
    for (int i = 0; i < num_estradas; ++i) {
        cin >> origem >> destino;
        grafo.adicionar_estrada(origem, destino);
    }

    grafo.encontrar_capital();
    grafo.contar_batalhoes_secundarios();
    grafo.verificar_patrulhamento();

    return 0;
}
