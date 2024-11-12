#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include <algorithm>
#include <vector>

using namespace std;

const int ALPHABET_SIZE = 26;
const int MAX_EDIT_DISTANCE = 2;
int misspellings = 0;

class Trie {
    Trie *children[ALPHABET_SIZE]; //Cada nodo tendra un apuntador para cada letra del abecedario
    bool isEndOfWord;
    string word;
    int similarities;

public:
    Trie() {
        similarities = 0;
        word = "";
        isEndOfWord = false;
        //inicializamos los apuntadores como null
        for (int i = 0; i < ALPHABET_SIZE; i++)
            children[i] = nullptr;
    }

    void insert(string s) {
        Trie* current = this;
        //Iteramos cada letra de la palabra
        for (char c : s) {
            if (c < 'a' || c > 'z') continue;
            int index = c - 'a';
            //Si no esta en el nodo la agregamos
            if(!(current->children[index]))
                current->children[index] = new Trie;
            //Avanzamos de nodo
            current = current->children[index];
        }
        //Marcamos el final de la palabra y la guardamos
        current->word = s;
        current->isEndOfWord = true;
    }

    bool search(string s) {
        Trie* current = this;
        //Buscamos letra por letra
        for(char c : s) {
            int index = c - 'a';
            current = current->children[index];
            current->
            if (!current)
                return false;
        }
        //Si llegamos a la ultima letra entonces la palabra si existe dentro del arbol
        if (current->isEndOfWord)
            return true;
        return false;
    }

    static void showMisspellings() {
        cout << "Cantidad de palabras incorrectas: " << misspellings << endl;
    }

//    bool startsWith(string word) {
//        Trie* current = this;
//        string prefix = word.substr(0,3);
//        for (char c: prefix) {
//            int index = c - 'a';
//            current = current->children[index];
//            if (!current)
//                return false;
//        }
//        return true;
//    }

    // Función principal para buscar palabras en el Trie que tengan una distancia de Levenshtein menor o igual a maxDistance
    vector<string> getLevenshteinSuggestions(const string& targetWord, int maxDistance = MAX_EDIT_DISTANCE) {
        vector<string> suggestions;
        vector<int> currentRow(targetWord.size() + 1);

        //a primera fila (que compara la palabra con la cadena vacía) se inicializa correctamente. Esta fila representa la distancia de cada prefijo de la palabra objetivo con una cadena vacía
        for (int i = 0; i <= targetWord.size(); i++) {
            currentRow[i] = i;
        }

        // Recorrer los hijos del nodo raíz
        Trie* current = this;
        for (int i = 0; i < ALPHABET_SIZE; i++) {
            if (current->children[i] != nullptr) {
                searchLevenshtein(current->children[i], 'a' + i, targetWord, currentRow, suggestions, maxDistance);
            }
        }

        return suggestions;
    }

    void searchLevenshtein(Trie* node, char letter, const string& s, vector<int>& previousRow, vector<string>& suggestions, int max) {
        int numColumns = s.size() + 1;
        vector<int> currentRow(numColumns);
        currentRow[0] = previousRow[0] + 1;

        for (int j = 1; j < numColumns; j++) {
            // Comparación de los caracteres y cálculo de las operaciones posibles
            int insertCost = currentRow[j - 1] + 1;
            int deleteCost = previousRow[j] + 1;
            int replaceCost = previousRow[j - 1];
            if (s[j - 1] != letter) {
                replaceCost++;
            }
            currentRow[j] = min({insertCost, deleteCost, replaceCost});
        }

        // Si este nodo representa el final de una palabra, y la distancia es menor o igual al máximo permitido, añadir a sugerencias
        if (node->isEndOfWord && currentRow.back() <= max) {
            suggestions.push_back(node->word);
        }

        // Si cualquier valor en la fila actual es menor o igual al máximo permitido, seguimos buscando en los hijos del nodo
        if (*min_element(currentRow.begin(), currentRow.end()) <= MAX_EDIT_DISTANCE) {
            for (int i = 0; i < ALPHABET_SIZE; i++) {
                if (node->children[i] != nullptr) {
                    searchLevenshtein(node->children[i], 'a' + i, s, currentRow, suggestions, 2);
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
            for (char& c : line) {
                c = tolower(c); // Convertir cada palabra a minúsculas
            }
            // Insertar cada palabra en el Trie
            trie.insert(line);
        }
        dictionary.close();
    }
    else {
        cout << "Error al abrir el archivo" << endl;
    }
}

void suggestCorrection(Trie& trie, const string& inputWord) {
    // Verificar si la palabra está en el Trie
    if (!trie.search(inputWord)) {
        vector<vector<string>> list;
        misspellings++;
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
}

vector<string> splitText(const string& text) {
    stringstream str_strm(text);
    string tmp;
    vector<string> wordsInText;
    char delim = ' '; // Define el delimitador donde se dividira la palabra
    while (getline(str_strm, tmp, delim)) { //Donde exista un espacio, se cuenta como una palabra
        wordsInText.push_back(tmp); //Y se guarda en un vector
    }

    //Cada palabra descartamos cualquier signo que no sea una letra y la pasamos a minusculas
    for (string& word : wordsInText) {
        for (int i = 0; i < word.length(); i++) {
            if (!isalpha(word[i]))
                word.erase(i, 1);
            word[i] = tolower(word[i]);
        }
    }
    return wordsInText;
}

int main(){
    Trie trie;
    vector<string> fullText;

    // Cargar el diccionario desde un archivo
    loadDictionary(trie, "listado-general.txt");

    //Solicitar texto al usuario
    string text;
    cout << "Ingrese el texto a verificar: ";
    getline(cin, text);
    fullText = splitText(text);

    //Cada palabra del texto la comparamos y buscamos sugerencias
    for (const string& s : fullText) {
        suggestCorrection(trie, s);
    }
    Trie::showMisspellings();

    return 0;
}