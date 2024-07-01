#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <bitset>
#include <queue>

using namespace std;


/************************/
//nó da arvore de huffman
struct Node {
    int frequency;
    unsigned char byte;
    bool isLeaf;
    Node* left = nullptr;
    Node* right = nullptr;
};

//percorre a arvore de huffman e cria um map que mapeia codigo -> byte para facilitar a compressao
map<unsigned char, string> createHuffTable(Node* node, vector<string> path, map<unsigned char, string>& huffTable, string direction = "") {
    if (direction != "") {
        path.push_back(direction);
    }

    if(node->isLeaf) {
        // cout << "recursive print: " << node->byte << " " << node->frequency << endl;
        string acc = "";
        for (auto v : path) {
            acc += v;
        }
        huffTable.emplace(node->byte, acc);
        return huffTable;
    }
        
    createHuffTable(node->left, path, huffTable, "0");
    createHuffTable(node->right, path, huffTable, "1");
    return huffTable;
}

//pega a extensao do arquivo
string get_extension(string fileName) {
    size_t dotIndex = fileName.find_last_of(".");
    if (dotIndex != string::npos && dotIndex != fileName.length() - 1) {
        return fileName.substr(dotIndex);
    }
    return "";  // No extension found
}

//pega o nome do arquivo
string get_fName(string fileName) {
    size_t dotIndex = fileName.find_last_of(".");
    if (dotIndex != string::npos && dotIndex != fileName.length() - 1) {
        return fileName.substr(0, dotIndex);
    }
    return "";  // No extension found
}

struct CompareNodes {
    bool operator()(const Node* a, const Node* b) const {
        return a->frequency > b->frequency;
    }
};

//retorna a raiz da arvore de huffman
Node* buildHuffmanTree(const std::vector<Node*>& frequencyTable) {

    std::priority_queue<Node*, std::vector<Node*>, CompareNodes> orderedFrequency;
    for (Node* node : frequencyTable) {
        orderedFrequency.push(node);
    }
    
    while (orderedFrequency.size() >= 2) {
        Node* leastFrequent = orderedFrequency.top();
        orderedFrequency.pop();

        Node* secondLeast = orderedFrequency.top();
        orderedFrequency.pop();

        Node* n = new Node;
        n->frequency = leastFrequent->frequency + secondLeast->frequency;
        n->left = leastFrequent;
        n->right = secondLeast;
        n->isLeaf = false;

        orderedFrequency.push(n);
    }

    // Return the root of the Huffman tree
    return orderedFrequency.top();
}

int main() {
    string INPUT_FILE = "";
    cout << "Nome do arquivo com extensao (exemplo.ext):\n";
    cin >> INPUT_FILE;
    cin.ignore();
    std::ifstream fileIn(INPUT_FILE, std::ios::binary);
    if (!fileIn) {
        std::cerr << "Error ao abrir: " + INPUT_FILE << std::endl;
        return 1;
    }


    map <unsigned char, int> countFrequency;
    unsigned char byte;

    cout << "contando frequencias..." << endl;
    while (fileIn.read((char*)&byte, sizeof(unsigned char))) {
        countFrequency[byte]++;
    }

    if (!fileIn.eof()) {
        std::cerr << "Erro durante a leitura do arquivo" << std::endl;
        return 1;
    }

    vector <Node*> frequencyTable;
    for(auto pair:countFrequency) {
        Node* n = new Node;
        n->byte = pair.first;
        n->frequency = pair.second;
        n->isLeaf = true;
        frequencyTable.push_back(n);
    }

    cout << "criando a arvore de huffman..." << endl;
    Node* root = buildHuffmanTree(frequencyTable);

    cout << "criando uma tabela de codigos..." << endl;
    vector<string> path;
    map<unsigned char, string> huffTable;
    createHuffTable(root, path, huffTable);

    unsigned char byte2;
    string acc = "";
    fileIn.clear();
    fileIn.seekg(0);
    cout << "comprimindo..." << endl;
    while (fileIn.read((char*)&byte2, sizeof(unsigned char))) {
        acc += huffTable[byte2];
    }

    if (!fileIn.eof()) {
        std::cerr << "Erro durante a leitura do arquivo" << std::endl;
        return 1;
    }


    unsigned long int counter = 0;
    string fName = get_fName(INPUT_FILE);
    std::ofstream fileOut(fName + ".huff", std::ios::binary);
    if (!fileOut) {
        std::cerr << "Nao foi possivel abrir " + INPUT_FILE << std::endl;
        return 1;
    }

    cout << "escrevendo arquivo comprimido..." << endl;
    size_t accSize = acc.size();
    for (size_t i = counter; i + 8 <= accSize; i += 8) {
        std::string bits;
        for (size_t j = i; j < i + 8; ++j) {
            bits += acc[j];
        }

        bitset<8> toWrite{bits};
        unsigned long uBits = toWrite.to_ulong(); //converte em inteiro
        unsigned char cBits = static_cast<unsigned char>(uBits); //converte em char

        fileOut.write((char*)&cBits, sizeof(unsigned char));
    }

    counter = accSize - (accSize % 8);

    unsigned int diff = acc.size() - counter;
    string bits = acc.substr(counter, diff);


    unsigned int diffToEight = 8 - diff;
    if (diff > 0) {
            string completeZero = "";
            for(int i = 0; i < diffToEight; i++) {
                completeZero += "0";
            }
            bits += completeZero;
            // cout << "final bits size: " << bits << endl;
            bitset<8> toWrite{bits};
            unsigned long uBits = toWrite.to_ulong();
            unsigned char cBits = static_cast<unsigned char>( uBits );
            fileOut.write((char*)&cBits, sizeof(unsigned char));
    }

    cout << "escrevendo metadados..." << endl;
    std::ofstream metadataF(fName +  ".metadata", ios::binary);
     if (!metadataF.is_open()) {
        std::cerr << "Failed to open file." << std::endl;
        return 1;
    }

    string extension = get_extension(INPUT_FILE);
    metadataF << extension << '\n';
    metadataF << diffToEight << '\n';


    for(auto pair: frequencyTable) {
        bitset<8> byte{pair->byte};
        metadataF << byte.to_string() << '\n';
        metadataF << pair->frequency << '\n';
    }
    cout << "pronto!" << endl;

    metadataF.close();
    fileIn.close();
    fileOut.close();

    return 0;
}