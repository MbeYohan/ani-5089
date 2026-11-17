#include <iostream>
#include <string>
#include <vector>

int main() {
    int nbActions;
    std::cin >> nbActions;

    std::vector<std::string> noms(nbActions);
    std::vector<std::string> types(nbActions);
    int entrees = 0;
    int sorties = 0;
    for (int i = 0; i < nbActions; ++i) {
        std::cin >> noms[i] >> types[i];
        if (types[i] == "HAPTIC") sorties++;
        if (types[i] == "BOOL" || types[i] == "FLOAT" || types[i] == "VEC2" || types[i] == "POSE") entrees++;
    }

    int nbOperations;
    std::cin >> nbOperations;
    for (int i = 0; i < nbOperations; ++i) {
        std::string nom, operation;
        std::cin >> nom >> operation;

        std::string type;
        for (int j = 0; j < nbActions; ++j) {
            if (noms[j] == nom) type = types[j];
        }
        if (type.empty()) continue;

        bool sortie = (type == "HAPTIC");
        if (operation == "LIT" && sortie) std::cout << nom << " LIT UNE SORTIE\n";
        if (operation == "ECRIT" && !sortie) std::cout << nom << " ECRIT UNE ENTREE\n";
    }

    std::cout << "ENTREES " << entrees << '\n';
    std::cout << "SORTIES " << sorties << '\n';
    return 0;
}
