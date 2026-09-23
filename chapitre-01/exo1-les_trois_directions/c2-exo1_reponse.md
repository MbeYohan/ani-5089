## Code

#include <iostream>
#include <iomanip>

struct Vec3
{
    double x;
    double y;
    double z;
};

// +x vers la droite, +y vers le haut, l'avant c'est -z
Vec3 Avant() { return Vec3{0.0, 0.0, -1.0}; }
Vec3 Haut() { return Vec3{0.0, 1.0, 0.0}; }
Vec3 Droite() { return Vec3{1.0, 0.0, 0.0}; }

double ProduitScalaire(const Vec3 &a, const Vec3 &b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

int main()
{
    Vec3 p{};
    if (!(std::cin >> p.x >> p.y >> p.z))
    {
        return 1;
    }

    std::cout << std::fixed << std::setprecision(4);
    std::cout << ProduitScalaire(p, Avant()) << '\n';
    std::cout << ProduitScalaire(p, Haut()) << '\n';
    std::cout << ProduitScalaire(p, Droite()) << '\n';
    return 0;
}

## Exemple d'exécution

Entrée : 1 2 3

Sortie :-3.0000 2.0000 1.0000
