#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <string>

// Struttura per memorizzare le informazioni di base di ogni nodo
struct Nodo {
    int id_originale;
    int i, j;
    double x, y;
};

// Funzione f(x,y) - termine noto dell'equazione (Sorgente di calore Gaussiana)
double f(double x, double y) {
    return std::exp(-10.0 * (x * x + y * y));
}

// Funzione u0(x,y) - condizioni al bordo (temperatura nulla sui lati)
double u0(double x, double y) {
    return 0.0;
}

int main() {
    std::cout << "TASK 3: Costruzione Matrice A e Vettore RHS" << std::endl;

    // 1. Scelta dell'ordinamento
    int scelta_ordine;
    std::cout << "Quale ordinamento vuoi usare?\n";
    std::cout << "0 = Ordinamento Naturale (da coords.txt)\n";
    std::cout << "1 = Nested Dissection (da ordering.txt)\n";
    std::cout << "Scelta: ";
    std::cin >> scelta_ordine;

    // 2. Lettura dei nodi da coords.txt
    std::ifstream file_coords("coords.txt");
    if (!file_coords) {
        std::cerr << "file coords.txt non trovato" << std::endl;
        return 1;
    }

    std::vector<Nodo> nodi;
    Nodo temp;
    while (file_coords >> temp.id_originale >> temp.i >> temp.j >> temp.x >> temp.y) {
        nodi.push_back(temp);
    }
    file_coords.close();

    int num_nodi = nodi.size();
    int N = std::sqrt(num_nodi); // Risaliamo a N sapendo che i nodi sono N^2
    double h = 1.0 / (N + 1);

    //Valore di kappa specificato dal progetto
    double kappa = 0.01;
    double coeff = kappa / (h * h); // Il fattore k/h^2

    // 3. Creazione del dizionario di traduzione (old ID -> new ID)
    std::vector<int> old_to_new(num_nodi);
    if (scelta_ordine == 1) {
        // Lettura da ordering.txt
        std::ifstream file_ordering("ordering.txt");
        if (!file_ordering) {
            std::cerr << "ordering.txt non trovato" << std::endl;
            return 1;
        }
        int new_id, old_id;
        while (file_ordering >> new_id >> old_id) {
            old_to_new[old_id] = new_id;
        }
        file_ordering.close();
    }
    else {
        for (int k = 0; k < num_nodi; ++k) {
            old_to_new[k] = k;
        }
    }

    // 4. Preparazione file di output e vettore RHS
    std::ofstream file_A("A.txt");
    std::vector<double> rhs_vector(num_nodi, 0.0);

    // 5. Costruzione della matrice e del termine noto
    for (const auto& n : nodi) {
        int old_id = n.id_originale;
        int new_id = old_to_new[old_id];

        // Calcolo base del right hand side con la nuova funzione
        double rhs_val = f(n.x, n.y);

        // Diagonale principale: 4 * (k/h^2)
        file_A << new_id << " " << new_id << " " << 4.0 * coeff << "\n";

        // Controllo dei vicini

        // Vicino sinistro (i-1)
        if (n.i > 1) {
            int vicino_vecchio_id = (n.j - 1) * N + (n.i - 2);
            file_A << new_id << " " << old_to_new[vicino_vecchio_id] << " " << -coeff << "\n";
        }
        else {
            rhs_val += coeff * u0(n.x - h, n.y);
        }

        // Vicino destro (i+1)
        if (n.i < N) {
            int vicino_vecchio_id = (n.j - 1) * N + n.i;
            file_A << new_id << " " << old_to_new[vicino_vecchio_id] << " " << -coeff << "\n";
        }
        else {
            rhs_val += coeff * u0(n.x + h, n.y);
        }

        // Vicino sotto (j-1)
        if (n.j > 1) {
            int vicino_vecchio_id = (n.j - 2) * N + (n.i - 1);
            file_A << new_id << " " << old_to_new[vicino_vecchio_id] << " " << -coeff << "\n";
        }
        else {
            rhs_val += coeff * u0(n.x, n.y - h);
        }

        // Vicino sopra (j+1)
        if (n.j < N) {
            int vicino_vecchio_id = n.j * N + (n.i - 1);
            file_A << new_id << " " << old_to_new[vicino_vecchio_id] << " " << -coeff << "\n";
        }
        else {
            rhs_val += coeff * u0(n.x, n.y + h);
        }

        // Salviamo il valore RHS 
        rhs_vector[new_id] = rhs_val;
    }

    file_A.close();

    // 6. Scrittura del vettore RHS
    std::ofstream file_rhs("rhs.txt");
    for (double val : rhs_vector) {
        file_rhs << val << "\n";
    }
    file_rhs.close();

    std::cout << "File A.txt e rhs.txt generati" << std::endl;
    return 0;
}