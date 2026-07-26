#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cmath>
#include <algorithm>

using namespace std;

 
// STRUTTURE DATI E FUNZIONI MATEMATICHE (Fuori dal main!)


struct Nodo {
    int id_originale;
    int i, j;
    double x, y;
};

int calcola_n(int i, int j, int N) {
    return (j - 1) * N + (i - 1);
}

double f(double x, double y) {
    return std::exp(-10.0 * (x * x + y * y));
}

double u0(double x, double y) {
    return 0.0; // Condizione di Dirichlet omogenea
}

// Funzione ricorsiva per il partizionamento "nested dissection"
void nested_dissection(std::vector<Nodo>& nodi, int asse, std::vector<Nodo>& ordinamento_finale) {
    if (nodi.empty()) return;
    if (nodi.size() == 1) {
        ordinamento_finale.push_back(nodi[0]);
        return;
    }

    if (asse == 0) {
        std::sort(nodi.begin(), nodi.end(), [](const Nodo& a, const Nodo& b) { return a.x < b.x; });
    }
    else {
        std::sort(nodi.begin(), nodi.end(), [](const Nodo& a, const Nodo& b) { return a.y < b.y; });
    }

    int mid = nodi.size() / 2;
    double coord_separatore = (asse == 0) ? nodi[mid].x : nodi[mid].y;

    std::vector<Nodo> v1, v2, vS;
    for (const auto& n : nodi) {
        double coord = (asse == 0) ? n.x : n.y;
        if (coord < coord_separatore - 1e-6) {
            v1.push_back(n);
        }
        else if (coord > coord_separatore + 1e-6) {
            v2.push_back(n);
        }
        else {
            vS.push_back(n);
        }
    }

    nested_dissection(v1, 1 - asse, ordinamento_finale);
    nested_dissection(v2, 1 - asse, ordinamento_finale);

    for (const auto& n : vS) {
        ordinamento_finale.push_back(n);
    }
}


// PROGRAMMA PRINCIPALE


int main() {
    // 1. Definiamo i valori di N per il benchmark (Lista completa ripristinata)
    std::vector<int> N_values = { 32, 64, 128, 256, 512, 1024 };
    double kappa = 0.01;

    for (int N : N_values) {
        std::cout << "\n---> Avvio generazione dati per N = " << N << " <---" << std::endl;

        double h = 1.0 / (N + 1);
        double coeff = kappa / (h * h);
        int num_nodi = N * N;

        
        // FASE 1: Generazione Griglia e Coordinate
        
        std::vector<Nodo> nodi_naturali;
        std::string nome_coords = "Dati/coords_" + std::to_string(N) + ".txt";
        std::ofstream file_coords(nome_coords);

        for (int j = 1; j <= N; ++j) {
            for (int i = 1; i <= N; ++i) {
                Nodo n;
                n.id_originale = calcola_n(i, j, N);
                n.i = i;
                n.j = j;
                n.x = i * h;
                n.y = j * h;
                nodi_naturali.push_back(n);

                // Salviamo coords_N.txt
                file_coords << n.id_originale << " " << n.i << " " << n.j << " " << n.x << " " << n.y << "\n";
            }
        }
        file_coords.close();

        
        // FASE 2: Calcolo Ordinamento Nested Dissection
        
        std::vector<Nodo> nodi_per_nd = nodi_naturali; // Copia per non rovinare l'originale
        std::vector<Nodo> ordinamento_finale;
        nested_dissection(nodi_per_nd, 0, ordinamento_finale);

        // Mappe di traduzione
        std::vector<int> id_nat_to_nat(num_nodi); // Identità (0->0, 1->1, ecc)
        std::vector<int> id_nat_to_nd(num_nodi);  // Traduttore Nested Dissection

        std::string nome_ordering = "Dati/ordering_" + std::to_string(N) + ".txt";
        std::ofstream file_ordering(nome_ordering);

        for (size_t m = 0; m < ordinamento_finale.size(); ++m) {
            int old_id = ordinamento_finale[m].id_originale;
            id_nat_to_nd[old_id] = m;      // Popoliamo il dizionario ND
            id_nat_to_nat[m] = m;          // Popoliamo il dizionario Naturale

            // Salviamo ordering_N.txt
            file_ordering << m << " " << old_id << "\n";
        }
        file_ordering.close();

        
        // FASE 3: Assemblaggio dei due Sistemi (Naturale e ND)
        
        std::ofstream file_A_nat("Dati/A_nat_" + std::to_string(N) + ".txt");
        std::ofstream file_A_nd("Dati/A_nd_" + std::to_string(N) + ".txt");

        std::vector<double> rhs_nat(num_nodi, 0.0);
        std::vector<double> rhs_nd(num_nodi, 0.0);

        // Ciclo unico per costruire ENTRAMBE le matrici contemporaneamente
        for (const auto& n : nodi_naturali) {
            int old_id = n.id_originale;

            // Nuovi ID a seconda del sistema
            int id_nat = id_nat_to_nat[old_id];
            int id_nd = id_nat_to_nd[old_id];

            // 3.1 Termine noto base
            double rhs_val_nat = f(n.x, n.y);
            double rhs_val_nd = f(n.x, n.y);

            // 3.2 Diagonale principale
            file_A_nat << id_nat << " " << id_nat << " " << 4.0 * coeff << "\n";
            file_A_nd << id_nd << " " << id_nd << " " << 4.0 * coeff << "\n";

            // 3.3 Vicino SINISTRO
            if (n.i > 1) {
                int vicino = calcola_n(n.i - 1, n.j, N);
                file_A_nat << id_nat << " " << id_nat_to_nat[vicino] << " " << -coeff << "\n";
                file_A_nd << id_nd << " " << id_nat_to_nd[vicino] << " " << -coeff << "\n";
            }
            else {
                rhs_val_nat += coeff * u0(n.x - h, n.y);
                rhs_val_nd += coeff * u0(n.x - h, n.y);
            }

            // 3.4 Vicino DESTRO
            if (n.i < N) {
                int vicino = calcola_n(n.i + 1, n.j, N);
                file_A_nat << id_nat << " " << id_nat_to_nat[vicino] << " " << -coeff << "\n";
                file_A_nd << id_nd << " " << id_nat_to_nd[vicino] << " " << -coeff << "\n";
            }
            else {
                rhs_val_nat += coeff * u0(n.x + h, n.y);
                rhs_val_nd += coeff * u0(n.x + h, n.y);
            }

            // 3.5 Vicino SOTTO
            if (n.j > 1) {
                int vicino = calcola_n(n.i, n.j - 1, N);
                file_A_nat << id_nat << " " << id_nat_to_nat[vicino] << " " << -coeff << "\n";
                file_A_nd << id_nd << " " << id_nat_to_nd[vicino] << " " << -coeff << "\n";
            }
            else {
                rhs_val_nat += coeff * u0(n.x, n.y - h);
                rhs_val_nd += coeff * u0(n.x, n.y - h);
            }

            // 3.6 Vicino SOPRA
            if (n.j < N) {
                int vicino = calcola_n(n.i, n.j + 1, N);
                file_A_nat << id_nat << " " << id_nat_to_nat[vicino] << " " << -coeff << "\n";
                file_A_nd << id_nd << " " << id_nat_to_nd[vicino] << " " << -coeff << "\n";
            }
            else {
                rhs_val_nat += coeff * u0(n.x, n.y + h);
                rhs_val_nd += coeff * u0(n.x, n.y + h);
            }

            // 3.7 Salvataggio Termine Noto
            rhs_nat[id_nat] = rhs_val_nat;
            rhs_nd[id_nd] = rhs_val_nd;
        }

        file_A_nat.close();
        file_A_nd.close();

        
        // FASE 4: Salvataggio Vettori RHS
        
        std::ofstream out_rhs_nat("Dati/rhs_nat_" + std::to_string(N) + ".txt");
        std::ofstream out_rhs_nd("Dati/rhs_nd_" + std::to_string(N) + ".txt");

        for (int k = 0; k < num_nodi; ++k) {
            out_rhs_nat << rhs_nat[k] << "\n";
            out_rhs_nd << rhs_nd[k] << "\n";
        }

        out_rhs_nat.close();
        out_rhs_nd.close();

        std::cout << "File per N=" << N << " generati con successo." << std::endl;
    }

    std::cout << "\n=== TUTTI I CALCOLI COMPLETATI ===" << std::endl;
    return 0;
}