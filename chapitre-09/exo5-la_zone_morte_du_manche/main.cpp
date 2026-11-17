#include <iostream>

int main() {
    long long seuil, maximum;
    std::cin >> seuil >> maximum;

    int n;
    std::cin >> n;

    int morts = 0;
    for (int i = 0; i < n; ++i) {
        long long brut;
        std::cin >> brut;

        long long absolu = brut < 0 ? -brut : brut;
        long long vitesse = 0;

        if (absolu < seuil) {
            morts++;
        } else {
            long long reste = absolu - seuil;
            long long course = 1000 - seuil;
            if (course != 0) vitesse = maximum * reste * reste / (course * course);
            if (brut < 0) vitesse = -vitesse;
        }
        std::cout << vitesse << '\n';
    }

    std::cout << "MORTS " << morts << '\n';
    return 0;
}
