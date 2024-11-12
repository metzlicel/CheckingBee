//
// Created by Metzli Celeste on 12/11/24.
//

#ifndef WIDGETSSS_SPELLCHECK_H
#define WIDGETSSS_SPELLCHECK_H
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
int similarities;

class Trie {
    Trie *children[ALPHABET_SIZE]; //Cada nodo tendra un apuntador para cada letra del abecedario
    bool isEndOfWord;
    string word;

public:
    Trie() {
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
            if (!current)
                return false;
        }
        //Si llegamos a la ultima letra entonces la palabra si existe dentro del arbol
        if (current->isEndOfWord)
            return true;
        return false;
    }

    int showMisspellings() {
        return misspellings;
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

        //a primera fila (que compara la palabra con la cadena vacía) se inicializa correctamente.
        // Esta fila representa la distancia de cada prefijo de la palabra objetivo con una cadena vacía
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

    void loadDictionary(const string& filename) {
        fstream dictionary;
        dictionary.open(filename, ios::in);

        if (dictionary.is_open()) {
            string line;
            while (getline(dictionary, line)) {
                for (char& c : line) {
                    c = tolower(c); // Convertir cada palabra a minúsculas
                }
                // Insertar cada palabra en el Trie
                insert(line);
            }
            dictionary.close();
        }
        else {
            cout << "Error al abrir el archivo" << endl;
        }
    }

    vector<string> suggestCorrection(const string& inputWord) {
        vector<string> list;
        // Verificar si la palabra está en el Trie
        if (!search(inputWord)) {
            misspellings++;

            // Obtener sugerencias basadas en la distancia de Levenshtein
            vector<string> suggestions = getLevenshteinSuggestions(inputWord, MAX_EDIT_DISTANCE);

            if (!suggestions.empty()) {
                list = countSimilarities(suggestions, inputWord);
            }
        }
        return list;
    }

    //Segundo filtro para sugerir las palabras mas cercanas a la palabra mal escrita
    vector<string> countSimilarities(vector<string> suggestions, string w) {
        //Vector con sugerencias filtradas
        vector<string> mostSimilarWords;
        //Numero maximo de letras iguales que tiene una palabra con otra
        int mostSimilarities = 0;
        // Arreglos booleanos para marcar letras ya contadas
        std::vector<bool> checked1(false);
        std::vector<bool> checked2(false);

        for (string &str : suggestions) {
            unordered_map<char, bool> checked1;  // Marca para letras de la sugerencia
            unordered_map<char, bool> checked2;  // Marca para letras de la palabra incorrecta
            int similarities = 0;

            for (char &i : str) {
                for (char &j : w) {
                    if (i == j && !checked1[i] && !checked2[j]) {
                        similarities++;
                        checked1[i] = true;    // Marcar en sugerencia
                        checked2[j] = true;    // Marcar en palabra incorrecta
                        break; // Salir del bucle interno una vez que se encuentra una coincidencia
                    }
                }
            }
            // Calcula el número máximo de letras iguales que tienen las suggestions con la palabra mal escrita
            if (similarities > mostSimilarities) {
                mostSimilarities = similarities;
            }
        }

        //Vuelve a comparar las letras y cuenta similaridades entre la palabra mal escrita y la suggestion
        for (string &str : suggestions) {
            unordered_map<char, bool> checked1;  // Marca para letras de la sugerencia
            unordered_map<char, bool> checked2;  // Marca para letras de la palabra incorrecta
            int similarities = 0;

            for (char &i : str) {
                for (char &j : w) {
                    if (i == j && !checked1[i] && !checked2[j]) {
                        similarities++;
                        checked1[i] = true;    // Marcar en sugerencia
                        checked2[j] = true;    // Marcar en palabra incorrecta<
                        break; // Salir del bucle interno una vez que se encuentra una coincidencia
                    }
                }
            }

            //Pero ahora si la suggestions tiene el maximo numero de similaridades, sera la palabra mas proxima a la palabra mal escrita
            if (similarities == mostSimilarities) {
                //La guardamos en otro vector que seran las nuevas suggestions pero con las palabras mas cercanas a la incorrecta
                mostSimilarWords.push_back(str);
            }
        }
        return mostSimilarWords;
    }



};

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

vector<pair<string, vector<string>>> checkSpelling(string text) {
    Trie trie;
    vector<string> fullText;
    vector<string> suggestions;
    vector<pair<string, vector<string>>> misspelledWords;


    trie.loadDictionary("listado-general.txt");

    fullText = splitText(text);

    //Cada palabra del texto la comparamos y buscamos sugerencias
    for (const string& s : fullText) {
        suggestions = trie.suggestCorrection(s);
        misspelledWords.push_back(make_pair(s, suggestions));
    }
    trie.showMisspellings();
    return misspelledWords;
}
#endif //WIDGETSSS_SPELLCHECK_H
