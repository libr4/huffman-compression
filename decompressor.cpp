#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <bitset>
#include <string>
#include <queue>
using namespace std;

struct Node {
    int frequency;
    unsigned char byte;
    bool isLeaf;
    Node* left = nullptr;
    Node* right = nullptr;
};

map<unsigned char, string> createHuffTable(Node* node, vector<string> path, map<unsigned char, string>& huffTable, string direction = "") {
    if (direction != "") {
        path.push_back(direction);
    }

    if(node->isLeaf) {
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

bool comp(Node* i, Node* j) {
    return i->frequency < j->frequency;
}

struct CompareNodes {
    bool operator()(const Node* a, const Node* b) const {
        return a->frequency > b->frequency;
    }
};

string get_fName(string fileName) {
    size_t dotIndex = fileName.find_last_of(".");
    if (dotIndex != string::npos && dotIndex != fileName.length() - 1) {
        return fileName.substr(0, dotIndex);
    }
    return "";  // No extension found
}

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
    cout << "Nome do arquivo comprimido (exemplo.huff):\n";
    string INPUT_FILE = "";
    cin >> INPUT_FILE;
    cin.ignore();

    std::ifstream fileIn(INPUT_FILE, std::ios::binary);
    if (!fileIn) {
        std::cerr << "Erro ao abrir " + INPUT_FILE << std::endl;
        return 1;
    }

    string fName = get_fName(INPUT_FILE);
    std::ifstream metadataF(fName + ".metadata", std::ios::binary);
    
    string key;
    string value;
    unsigned int counter = 0;
    vector<Node*> frequencyTable;
    int i = 0;
    string extension = "";
    string padding_bits;
    getline(metadataF, extension);
    getline(metadataF, padding_bits);

    while (getline(metadataF, key) && getline(metadataF, value)) {
        bitset<8> byte{key};
        unsigned long uByte = byte.to_ulong();
        unsigned char c = static_cast<unsigned char>(uByte);
        int v = stoi(value);
        Node* n = new Node;
        n->byte = c;
        n->frequency = v;
        n->isLeaf = true;
        frequencyTable.push_back(n);
    }
    
    cout << "recriando a arvore de huffman..." << endl;
    Node* root = buildHuffmanTree(frequencyTable);
    unsigned char byte;
    string acc = "";

    cout << "carregando o arquivo na memoria..." << endl;
    while (fileIn.read((char*)&byte, sizeof(unsigned char))) {
        bitset<8> bbyte{byte};
        string sByte = bbyte.to_string();
        acc += sByte;
    }

    string output_file = fName + "Descomp" + extension;
    std::ofstream fileOut(output_file, std::ios::binary);
    if (!fileOut) {
        std::cerr << "Erro ao criar arquivo" << std::endl;
        return 1;
    }

    cout << "descomprimindo..." << endl;
    Node* aux = root;
    int ipadding = stoi(padding_bits);
    for(int i = 0; i < (acc.size() - ipadding); i++) {
        if(acc[i] == '0') {
            aux = aux->left;
        }
        else {
            aux = aux->right;
        }
        if (aux->isLeaf) {
            fileOut.write((char*)&aux->byte, sizeof(unsigned char));
            aux = root;
        }
    }
    cout << "arquivo descomprimido em: " + (fName + "Descomp" + extension) << endl;
}