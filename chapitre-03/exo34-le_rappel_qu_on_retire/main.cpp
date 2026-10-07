#include <algorithm>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

using Rappel = std::pair<long long, std::string>;

static void Retirer(std::vector<Rappel>& registre, long long id) {
    registre.erase(std::remove_if(registre.begin(), registre.end(),
                                  [id](const Rappel& r) { return r.first == id; }),
                   registre.end());
}

int main() {
    int n = 0;
    std::cin >> n;
    std::vector<Rappel> registre;
    for (int i = 0; i < n; ++i) {
        std::string commande;
        std::cin >> commande;
        if (commande == "poser") {
            long long id = 0;
            std::string type;
            std::cin >> id >> type;
            Retirer(registre, id);
            registre.emplace_back(id, type);
        } else if (commande == "retirer") {
            long long id = 0;
            std::cin >> id;
            Retirer(registre, id);
        } else if (commande == "envoyer") {
            std::string type;
            std::cin >> type;
            bool premier = true;
            for (const Rappel& r : registre) {
                if (r.second != type) {
                    continue;
                }
                if (!premier) {
                    std::cout << ' ';
                }
                std::cout << r.first;
                premier = false;
            }
            if (premier) {
                std::cout << "AUCUN";
            }
            std::cout << '\n';
        }
    }
    return 0;
}
