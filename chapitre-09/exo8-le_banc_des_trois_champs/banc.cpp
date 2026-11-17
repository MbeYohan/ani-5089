#include <iomanip>
#include <iostream>
#include <string>

#include "actions.h"

void AfficherGroupe(const EtatBool& e) {
    std::cout << std::setw(7) << e.current << ' ' << std::setw(6) << e.changed << ' ' << std::setw(5) << e.active;
}

int main() {
    const int IMAGES = 6;
    const bool toucheA[IMAGES] = {false, true, true, true, false, false};
    const bool toucheB[IMAGES] = {false, false, false, true, true, false};

    EtatBool saisir;
    EtatBool viser;
    EtatBool orpheline;

    std::cout << "image | " << std::left << std::setw(20) << "saisir" << " | " << std::setw(20) << "viser"
              << " | orpheline" << std::right << '\n';
    std::cout << "      | courant change actif | courant change actif | courant change actif\n";

    for (int image = 0; image < IMAGES; ++image) {
        Synchroniser(saisir, toucheA[image], true);
        Synchroniser(viser, toucheB[image], true);
        Synchroniser(orpheline, false, false);

        std::cout << std::setw(5) << image + 1 << " | ";
        AfficherGroupe(saisir);
        std::cout << " | ";
        AfficherGroupe(viser);
        std::cout << " | ";
        AfficherGroupe(orpheline);
        std::cout << '\n';
    }
    return 0;
}
