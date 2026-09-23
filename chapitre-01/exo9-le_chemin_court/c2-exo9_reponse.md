## Code

#include <iostream>
#include <iomanip>
#include <cmath>

struct Vec3
{
    double x;
    double y;
    double z;
};

struct Quat
{
    double x;
    double y;
    double z;
    double w;
};

Quat Conjugue(const Quat &q) { return Quat{-q.x, -q.y, -q.z, q.w}; }

Quat Multiplier(const Quat &a, const Quat &b)
{
    return Quat{
        a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y,
        a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x,
        a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w,
        a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z};
}

Vec3 VitesseAngulaire(const Quat &debut, const Quat &fin, double dt, bool cheminCourt)
{
    Quat delta = Multiplier(fin, Conjugue(debut));
    if (cheminCourt && delta.w < 0.0)
    {
        delta = Quat{-delta.x, -delta.y, -delta.z, -delta.w};
    }

    const double sinusDemi = std::sqrt(delta.x * delta.x +
                                       delta.y * delta.y +
                                       delta.z * delta.z);
    if (sinusDemi < 1e-12)
    {
        return Vec3{0.0, 0.0, 0.0};
    }

    const double cosinusDemi = std::fmin(1.0, std::fmax(-1.0, delta.w));
    const double angle = 2.0 * std::atan2(sinusDemi, cosinusDemi);
    const double facteur = angle / (sinusDemi * dt);
    return Vec3{delta.x * facteur, delta.y * facteur, delta.z * facteur};
}

double Norme(const Vec3 &v)
{
    return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

void Afficher(const char *nom, const Vec3 &v)
{
    const double degres = Norme(v) * 180.0 / 3.14159265358979323846;
    std::cout << nom << ' ' << std::fixed << std::setprecision(4)
              << v.x << ' ' << v.y << ' ' << v.z
              << "  soit " << degres << " deg/s\n";
}

int main()
{
    Quat debut{};
    Quat fin{};
    double dt = 0.0;
    if (!(std::cin >> debut.x >> debut.y >> debut.z >> debut.w >> fin.x >> fin.y >> fin.z >> fin.w >> dt))
    {
        return 1;
    }

    Afficher("avec forcage  ", VitesseAngulaire(debut, fin, dt, true));
    Afficher("sans forcage  ", VitesseAngulaire(debut, fin, dt, false));
    return 0;
}


## Exemple d'exécution

Entrée : 0 0 0 1 0 -0.017452406437 0 -0.999847695156 0.01

Sortie :
avec forcage   -0.0000 3.4907 -0.0000  soit 200.0000 deg/s
sans forcage   0.0000 -624.8279 0.0000  soit 35800.0000 deg/s