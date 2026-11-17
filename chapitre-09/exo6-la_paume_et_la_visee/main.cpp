#include <iostream>
#include <string>

int main() {
    int n;
    std::cin >> n;

    int fautes = 0;
    for (int i = 0; i < n; ++i) {
        std::string nom, intention, pose;
        std::cin >> nom >> intention >> pose;

        std::string verdict = "OK";
        if (intention == "TENIR" && pose == "AIM") verdict = "FLOTTE";
        if (intention == "POINTER" && pose == "GRIP") verdict = "PART DE TRAVERS";
        if (verdict != "OK") fautes++;

        std::cout << nom << ' ' << verdict << '\n';
    }

    std::cout << "FAUTES " << fautes << '\n';
    return 0;
}
