#include <iostream>
#include <string>
#include <vector>

int main() {
    int nbActions;
    std::cin >> nbActions;

    std::vector<std::string> noms(nbActions);
    std::vector<std::string> usages(nbActions);
    for (int i = 0; i < nbActions; ++i) {
        std::cin >> noms[i] >> usages[i];
    }

    int nbFournis;
    std::cin >> nbFournis;
    std::vector<std::string> fournis(nbFournis);
    for (int i = 0; i < nbFournis; ++i) {
        std::cin >> fournis[i];
    }

    int mortes = 0;
    for (int i = 0; i < nbActions; ++i) {
        bool active = false;
        for (const std::string& usage : fournis) {
            if (usage == usages[i]) active = true;
        }
        if (!active) mortes++;
        std::cout << noms[i] << ' ' << (active ? "ACTIVE" : "MORTE") << '\n';
    }

    std::cout << "MORTES " << mortes << '\n';
    return 0;
}
