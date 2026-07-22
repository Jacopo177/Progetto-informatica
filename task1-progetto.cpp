// task1-progetto.cpp : Questo file contiene la funzione 'main', in cui inizia e termina l'esecuzione del programma.
//

#include <iostream>
#include <fstream>

using namespace std;

// Funzione matematica per calcolare l'indice progressivo 'n' a partire dalle coordinate della griglia
int calcola_n(int i, int j, int N) {
    // Mappa la griglia 2D in un indice 1D che parte da 0
    return (j - 1) * N + (i - 1);
}

int main() {
    int N;

    // Richiesta in input del valore N
    cout << "Inserisci il valore di N: ";
    cin >> N;

    if (N <= 0) {
        cerr << "Errore: N deve essere un numero positivo." << endl;
        return 1;
    }

    // Calcolo del passo h della griglia
    double h = 1.0 / (N + 1);

    // Creazione e apertura dei file di testo in scrittura
    ofstream file_coords("coords.txt");
    ofstream file_conn("connectivity.txt");

    if (!file_coords.is_open() || !file_conn.is_open()) {
        cerr << "Errore: impossibile creare i file di output." << endl;
        return 1;
    }

    // --- 1. GENERAZIONE DEL FILE coords.txt ---
    // I nodi interni (blu) vanno da 1 a N sia per le x che per le y
    for (int j = 1; j <= N; ++j) {
        for (int i = 1; i <= N; ++i) {
            int n = calcola_n(i, j, N); // Indice progressivo del nodo
            double x = i * h;           // Calcolo coordinata x_i
            double y = j * h;           // Calcolo coordinata y_j

            // Scrittura nel formato richiesto: n i j x y
            file_coords << n << " " << i << " " << j << " " << x << " " << y << "\n";
        }
    }

    // --- 2. GENERAZIONE DEL FILE connectivity.txt ---
    int e = 0; // Indice progressivo degli archi (parte da 0 come richiesto)

    for (int j = 1; j <= N; ++j) {
        for (int i = 1; i <= N; ++i) {
            int n1 = calcola_n(i, j, N); // Nodo di partenza

            // Per evitare archi duplicati in un grafo non orientato,
            // colleghiamo il nodo corrente SOLO con quello alla sua DESTRA e in ALTO.

            // Connessione con il nodo a destra (stessa riga j, colonna successiva i+1)
            if (i < N) {
                int n2 = calcola_n(i + 1, j, N);
                // Scrittura nel formato richiesto: e n1 n2
                file_conn << e << " " << n1 << " " << n2 << "\n";
                e++;
            }

            // Connessione con il nodo in alto (colonna i, riga successiva j+1)
            if (j < N) {
                int n2 = calcola_n(i, j + 1, N);
                // Scrittura nel formato richiesto: e n1 n2
                file_conn << e << " " << n1 << " " << n2 << "\n";
                e++;
            }
        }
    }

    // Chiusura dei file (fondamentale per salvare i dati sul disco)
    file_coords.close();
    file_conn.close();

    cout << "Task 1 completato! File 'coords.txt' e 'connectivity.txt' generati." << endl;
    return 0;
}

// Per eseguire il programma: CTRL+F5 oppure Debug > Avvia senza eseguire debug
// Per eseguire il debug del programma: F5 oppure Debug > Avvia debug

// Suggerimenti per iniziare: 
//   1. Usare la finestra Esplora soluzioni per aggiungere/gestire i file
//   2. Usare la finestra Team Explorer per connettersi al controllo del codice sorgente
//   3. Usare la finestra di output per visualizzare l'output di compilazione e altri messaggi
//   4. Usare la finestra Elenco errori per visualizzare gli errori
//   5. Passare a Progetto > Aggiungi nuovo elemento per creare nuovi file di codice oppure a Progetto > Aggiungi elemento esistente per aggiungere file di codice esistenti al progetto
//   6. Per aprire di nuovo questo progetto in futuro, passare a File > Apri > Progetto e selezionare il file con estensione sln
