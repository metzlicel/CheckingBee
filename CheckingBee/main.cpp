//spell checking through TRIE, running succesfully
#include<iostream>
#include<fstream>
#include<string>
#include<cstdlib>
using namespace std;

const int ALPHABET_SIZE = 26;
const int MAX_EDIT_DISTANCE = 2;

// Nodo del Trie
struct TrieNode {
    string word;
    TrieNode* children[ALPHABET_SIZE]; // Hijo para cada letra
    bool isEndOfWord; // Marca si es el final de una palabra

    TrieNode() {
        isEndOfWord = false;
        for (int i = 0; i < ALPHABET_SIZE; i++)
            children[i] = nullptr;
    }
};

// Clase Trie
class Trie {
private:
    TrieNode* root;

public:
    int misspellings;
    Trie() {
        root = new TrieNode();
    }

    // Insertar palabra en el Trie
    // En la clase Trie
    void insert(const string& word) {  // No necesitas pasar root como argumento
        TrieNode* current = root;
        for (char c : word) {
            int index = c - 'a';
            if (current->children[index] == nullptr) {
                current->children[index] = new TrieNode();
            }
            current = current->children[index];
        }
        current->word = word;  // Marca el final de la palabra
    }


    // Función para buscar una palabra exacta en el Trie
    bool search(const string& word) {
        TrieNode* node = root;
        for (char c : word) {
            int index = c - 'a';
            if (!node->children[index])
                return false;
            node = node->children[index];
        }
        if (!node && node->isEndOfWord)
        {
            countMisspellings();
        }
        return node && node->isEndOfWord;
    }

    void countMisspellings() {
        misspellings++;
    }

    // Función principal para buscar palabras en el Trie que tengan una distancia de Levenshtein menor o igual a maxDistance
    vector<string> getLevenshteinSuggestions(const string& targetWord, int maxDistance = MAX_EDIT_DISTANCE) {
        vector<string> suggestions;
        vector<int> currentRow(targetWord.size() + 1);

        //a primera fila (que compara la palabra con la cadena vacía) se inicializa correctamente. Esta fila representa la distancia de cada prefijo de la palabra objetivo con una cadena vacía
        for (int i = 0; i <= targetWord.size(); i++) {
            currentRow[i] = i;
        }

        // Inicializar la primera fila de la tabla de Levenshtein (comparando con la cadena vacía)
        for (int i = 0; i <= targetWord.size(); i++) {
            currentRow[i] = i;
        }

        // Recorrer los hijos del nodo raíz
        for (int i = 0; i < 26; i++) {
            if (root->children[i] != nullptr) {
                searchLevenshtein(root->children[i], 'a' + i, targetWord, currentRow, suggestions, maxDistance);
            }
        }

        return suggestions;
    }

    void searchLevenshtein(TrieNode* node, char letter, const string& word, vector<int>& previousRow, vector<string>& suggestions, int MAX_DISTANCE = 2) {
        int numColumns = word.size() + 1;
        vector<int> currentRow(numColumns);
        currentRow[0] = previousRow[0] + 1;

        for (int j = 1; j < numColumns; j++) {
            // Comparación de los caracteres y cálculo de las operaciones posibles
            int insertCost = currentRow[j - 1] + 1;
            int deleteCost = previousRow[j] + 1;
            int replaceCost = previousRow[j - 1];
            if (word[j - 1] != letter) {
                replaceCost++;
            }
            currentRow[j] = min({insertCost, deleteCost, replaceCost});
        }
        // Si este nodo representa el final de una palabra, y la distancia es menor o igual al máximo permitido, añadir a sugerencias
        if (node->word != "" && currentRow.back() <= MAX_EDIT_DISTANCE ) {
            suggestions.push_back(node->word);
        }
        // Si cualquier valor en la fila actual es menor o igual al máximo permitido, seguimos buscando en los hijos del nodo
        if (*min_element(currentRow.begin(), currentRow.end()) <= MAX_EDIT_DISTANCE) {
            for (int i = 0; i < 26; i++) {
                if (node->children[i] != nullptr) {
                    searchLevenshtein(node->children[i], 'a' + i, word, currentRow, suggestions, 2);
                }
            }
        }
    }


};

void loadDictionary(Trie& trie, const string& filename) {
    fstream dictionary;
    dictionary.open(filename, ios::in);

    if (dictionary.is_open()) {
        string line;
        while (getline(dictionary, line)) {
            // Insertar cada palabra en el Trie
            trie.insert(line);
        }
        dictionary.close();
    }
    else {
        cout << "Error al abrir el archivo" << endl;
    }
}




//string suggestCorrection(Trie& trie, const string& inputWord) {
//    vector<string> suggestions = trie.getSuggestions(inputWord);
//
//    if (suggestions.empty()) {
//        return "No se encontraron sugerencias.";
//    } else {
//        // Retornar la primera sugerencia, o mejorar esta lógica para mostrar varias opciones
//        return "¿Quiso decir: " + suggestions[0] + "?";
//    }
//}


int main(){

        Trie trie;

        // Cargar el diccionario desde un archivo
        loadDictionary(trie, "palabras.txt");

        // Solicitar palabra al usuario
        string inputWord;
        cout << "Ingrese una palabra para verificar: ";
        cin >> inputWord;

        // Verificar si la palabra está en el Trie
        if (trie.search(inputWord)) {
            cout << "La palabra '" << inputWord << "' está escrita correctamente." << endl;
        } else {
            cout << "La palabra '" << inputWord << "' podría estar mal escrita. Sugerencias:" << endl;

            // Obtener sugerencias basadas en la distancia de Levenshtein
            vector<string> suggestions = trie.getLevenshteinSuggestions(inputWord, MAX_EDIT_DISTANCE);

            if (suggestions.empty()) {
                cout << "No se encontraron sugerencias." << endl;
            } else {
                for (const string& suggestion : suggestions) {
                    cout << " - " << suggestion << endl;
                }
            }
        }



    return 0;
}