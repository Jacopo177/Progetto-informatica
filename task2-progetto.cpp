#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

// Struttura per memorizzare i dati di ogni singolo nodo letto da coords.txt
struct Nodo {
    int id_originale;
    int i, j;
    double x, y;
};

// Funzione ricorsiva per il partizionamento "nested dissection"
void nested_dissection(std::vector<Nodo>& nodi, int asse, std::vector<Nodo>& ordinamento_finale) {
    // Condizione di uscita dalla ricorsione
    if (nodi.empty()) return;
    if (nodi.size() == 1) {
        ordinamento_finale.push_back(nodi[0]);
        return;
    }

    // asse: 0 significa taglio sull'asse X, 1 significa taglio sull'asse Y
    // Ordiniamo i nodi in base all'asse corrente per trovare facilmente il punto medio
    if (asse == 0) {
        std::sort(nodi.begin(), nodi.end(), [](const Nodo& a, const Nodo& b) { return a.x < b.x; });
    }
    else {
        std::sort(nodi.begin(), nodi.end(), [](const Nodo& a, const Nodo& b) { return a.y < b.y; });
    }

    // Troviamo il valore mediano che farà da separatore
    int mid = nodi.size() / 2;
    double coord_separatore = (asse == 0) ? nodi[mid].x : nodi[mid].y;

    std::vector<Nodo> v1, v2, vS;

    // Distribuiamo i nodi nei tre insiemi V_1, V_2, V_S
    for (const auto& n : nodi) {
        double coord = (asse == 0) ? n.x : n.y;

        // Per precisione dei numeri in virgola mobile (double), usiamo una tolleranza minima
        if (coord < coord_separatore - 1e-6) {
            v1.push_back(n);
        }
        else if (coord > coord_separatore + 1e-6) {
            v2.push_back(n);
        }
        else {
            vS.push_back(n); // Nodi esattamente sulla linea di taglio
        }
    }

    // Chiamate ricorsive sui sotto-insiemi V_1 e V_2, invertendo l'asse di taglio (1 - asse)
    nested_dissection(v1, 1 - asse, ordinamento_finale);
    nested_dissection(v2, 1 - asse, ordinamento_finale);

    // L'ordinamento richiede di accodare V_S alla fine
    for (const auto& n : vS) {
        ordinamento_finale.push_back(n);
    }
}

int main() {
    std::vector<Nodo> tutti_i_nodi;
    std::ifstream file_in("coords.txt");
    if (!file_in) {
        std::cerr << "File non trovato." << std::endl;
        return 1;
    }

    Nodo temp;
    // Leggiamo i dati generati nel Task 1
    while (file_in >> temp.id_originale >> temp.i >> temp.j >> temp.x >> temp.y) {
        tutti_i_nodi.push_back(temp);
    }
    file_in.close();

    std::vector<Nodo> ordinamento_finale;
    // Avviamo la ricorsione partendo dall'asse X (asse = 0)
    nested_dissection(tutti_i_nodi, 0, ordinamento_finale);

    // Scriviamo il risultato nel file ordering.txt
    std::ofstream file_out("ordering.txt");
    for (size_t m = 0; m < ordinamento_finale.size(); ++m) {
        // m è il nuovo indice progressivo, mentre id_originale è il vecchio "n"
        file_out << m << " " << ordinamento_finale[m].id_originale << "\n";
    }
    file_out.close();

    std::cout << "File ordering.txt generato." << std::endl;
    return 0;
}