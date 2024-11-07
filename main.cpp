#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <unordered_map>
#include <algorithm>
#include <stack>
#include <map>

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
    //vector<vector<int>> scc_adj;
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
        //scc_adj.clear();
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
        /*scc_adj.resize(centro_count);
        for (int i = 0; i < centro_count; i++) {
            for (auto vertice: adj[i]) {
                if (raizes[i] != raizes[vertice])
                    scc_adj[raizes[i]].push_back(raizes[vertice]);
            }
        }*/

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
        for (int i = 0; i < scc_componentes.size(); ++i) {
            if (scc_componentes[i].size() == 1) {
                continue; // Não há necessidade de patrulhamento para componentes com um único vértice
            }

            // Passo 1: Construir o subgrafo da componente e calcular graus de entrada e saída
            vector<vector<int>> subgraph_scc(centro_count);
            vector<int> grau_entrada(centro_count, 0);
            vector<int> grau_saida(centro_count, 0);

            for (int vertice : scc_componentes[i]) {
                for (int vizinho : adj[vertice]) {
                    // so adciona se estiver na lista de scc
                    if (find(scc_componentes[i].begin(), scc_componentes[i].end(), vizinho) != scc_componentes[i].end()) {
                        subgraph_scc[vertice].push_back(vizinho);
                        grau_entrada[vizinho]++;
                        grau_saida[vertice]++;
                    }
                }
            }

            // Passo 2: Verificar se o subgrafo é euleriano
            bool euleriano = true;
            for (int vertice : scc_componentes[i]) {
                if (grau_entrada[vertice] != grau_saida[vertice]) {
                    euleriano = false;
                    break;
                }
            }

            // Passo 3: Se o grafo direcionado e sem pesos nao for balanceado, balancear e tornar euleriano
            if (!euleriano) {
                // Identificar vértices desbalanceados
                vector<int> deficientes, excedentes;
                for (int vertice : scc_componentes[i]) {
                    if (grau_entrada[vertice] < grau_saida[vertice]) {
                        excedentes.push_back(vertice);
                    } else if (grau_entrada[vertice] > grau_saida[vertice]) {
                        deficientes.push_back(vertice);
                    }
                }

                // Emparelhamento de vértices deficientes e excedentes
                while (!deficientes.empty() && !excedentes.empty()) {
                    int d = deficientes.back(); // Vértice deficiente
                    int e = excedentes.back(); // Vértice excedente

                    // Verificar se d e e são vizinhos
                    if (find(subgraph_scc[d].begin(), subgraph_scc[d].end(), e) != subgraph_scc[d].end()) {
                        // Se são vizinhos, adicionar a aresta diretamente
                        subgraph_scc[d].push_back(e);
                        grau_entrada[e]++;
                        grau_saida[d]++;
                    } else {
                        // Se não são vizinhos, encontrar um caminho de d a e
                        vector<int> caminho; // Para armazenar o caminho encontrado
                        if (encontrar_caminho(d, e, subgraph_scc, caminho)) {
                            // Adicionar arestas do caminho ao subgrafo
                            for (size_t i = 0; i < caminho.size() - 1; ++i) {
                                subgraph_scc[caminho[i]].push_back(caminho[i + 1]);
                                grau_entrada[caminho[i + 1]]++;
                                grau_saida[caminho[i]]++;
                            }
                        }
                    }

                    // Remover do vetor de deficientes e excedentes
                    deficientes.pop_back();
                    excedentes.pop_back();
                }
            }

            // Passo 4: Algoritmo de Hierholzer para encontrar o caminho Euleriano
            stack<int> pilha;
            vector<int> rota_euleriana;
            int atual = batalhao_eleito_id[i];
            pilha.push(atual);

            while (!pilha.empty()) {
                atual = pilha.top();
                if (!subgraph_scc[atual].empty()) {
                    int proximo = subgraph_scc[atual].back();
                    subgraph_scc[atual].pop_back();
                    pilha.push(proximo);
                } else {
                    rota_euleriana.push_back(atual);
                    pilha.pop();
                }
            }

            // A rota_euleriana agora contém o caminho euleriano em ordem reversa
            reverse(rota_euleriana.begin(), rota_euleriana.end());

            // Adicionar e imprimir a rota encontrada
            rotas_para_imprimir.push_back(rota_euleriana);
        }

        // Imprimir todas as rotas
        cout << rotas_para_imprimir.size() << endl;
        for (const auto& rota : rotas_para_imprimir) {
            for (int v : rota) {
                cout << nomes_centros[v] << " ";
            }
            cout << endl;
        }
    }

    // Função para encontrar um caminho entre dois vértices usando BFS
    bool encontrar_caminho(int origem, int destino, const vector<vector<int>>& grafo, vector<int>& caminho) {
        vector<bool> visitado(grafo.size(), false);
        queue<int> fila;
        unordered_map<int, int> pai; // Para rastrear o caminho

        fila.push(origem);
        visitado[origem] = true;

        while (!fila.empty()) {
            int atual = fila.front();
            fila.pop();

            if (atual == destino) {
                // Reconstruir o caminho
                for (int v = destino; v != origem; v = pai[v]) {
                    caminho.push_back(v);
                }
                caminho.push_back(origem);
                reverse(caminho.begin(), caminho.end());
                return true;
            }

            for (int vizinho : grafo[atual]) {
                if (!visitado[vizinho]) {
                    visitado[vizinho] = true;
                    pai[vizinho] = atual;
                    fila.push(vizinho);
                }
            }
        }

        return false; // Caminho não encontrado
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
