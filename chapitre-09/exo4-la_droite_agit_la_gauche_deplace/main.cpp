#include <iostream>
#include <sstream>
#include <string>

int mainDe(const std::string& usage) {
    if (usage == "MENU" || usage == "MOVE") return 2;
    if (usage == "SELECT_LEFT" || usage == "GRAB_LEFT" || usage == "AIM_POSE_LEFT" ||
        usage == "GRIP_POSE_LEFT" || usage == "HAPTIC_LEFT") return 2;
    if (usage == "SELECT" || usage == "GRAB" || usage == "AIM_POSE" || usage == "GRIP_POSE" ||
        usage == "HAPTIC" || usage == "MOVE_RIGHT") return 1;
    return 0;
}

int main() {
    int n;
    std::cin >> n;

    int deuxMains = 0;
    for (int i = 0; i < n; ++i) {
        std::string ligne;
        std::getline(std::cin >> std::ws, ligne);
        std::istringstream mots(ligne);

        std::string nom, usage;
        mots >> nom;

        int droite = 0;
        int gauche = 0;
        while (mots >> usage) {
            int m = mainDe(usage);
            if (m == 1) droite++;
            if (m == 2) gauche++;
        }

        bool deux = droite > 0 && gauche > 0;
        if (deux) deuxMains++;
        std::cout << nom << ' ' << droite << ' ' << gauche << ' ' << (deux ? "DEUX MAINS" : "UNE MAIN") << '\n';
    }

    std::cout << "DEUX MAINS " << deuxMains << '\n';
    return 0;
}
