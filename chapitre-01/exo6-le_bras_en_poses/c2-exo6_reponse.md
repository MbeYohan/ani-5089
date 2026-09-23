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

struct Pose
{
    Vec3 position;
    Quat orientation;
};

Vec3 Tourner(const Quat &q, const Vec3 &v)
{
    const double tx = 2.0 * (q.y * v.z - q.z * v.y);
    const double ty = 2.0 * (q.z * v.x - q.x * v.z);
    const double tz = 2.0 * (q.x * v.y - q.y * v.x);
    return Vec3{
        v.x + q.w * tx + (q.y * tz - q.z * ty),
        v.y + q.w * ty + (q.z * tx - q.x * tz),
        v.z + q.w * tz + (q.x * ty - q.y * tx)};
}

Quat Multiplier(const Quat &a, const Quat &b)
{
    return Quat{
        a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y,
        a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x,
        a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w,
        a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z};
}

Pose Composer(const Pose &parent, const Pose &enfant)
{
    const Vec3 decale = Tourner(parent.orientation, enfant.position);
    return Pose{
        Vec3{parent.position.x + decale.x,
             parent.position.y + decale.y,
             parent.position.z + decale.z},
        Multiplier(parent.orientation, enfant.orientation)};
}

Quat AutourDeY(double degres)
{
    const double demi = degres * 3.14159265358979323846 / 180.0 / 2.0;
    return Quat{0.0, std::sin(demi), 0.0, std::cos(demi)};
}

Quat Identite() { return Quat{0.0, 0.0, 0.0, 1.0}; }

void Afficher(const char *nom, const Vec3 &p)
{
    std::cout << nom << ' ' << std::fixed << std::setprecision(4)
              << p.x << ' ' << p.y << ' ' << p.z << '\n';
}

int main()
{
    const double bras = 0.30;
    const double avantBras = 0.25;

    Pose epaule{Vec3{0.0, 0.0, 0.0}, Identite()};
    const Pose coudeLocal{Vec3{0.0, 0.0, -bras}, Identite()};
    const Pose mainLocale{Vec3{0.0, 0.0, -avantBras}, Identite()};

    double angle = 90.0;
    std::cin >> angle;

    Pose coude = Composer(epaule, coudeLocal);
    Pose main = Composer(coude, mainLocale);
    Afficher("coude", coude.position);
    Afficher("main", main.position);

    epaule.orientation = AutourDeY(angle);
    coude = Composer(epaule, coudeLocal);
    main = Composer(coude, mainLocale);
    Afficher("coude", coude.position);
    Afficher("main", main.position);
    return 0;
}


## Exemple d'exécution

Entrée : 90

Sortie :
coude 0.0000 0.0000 -0.3000
main 0.0000 0.0000 -0.5500
coude -0.3000 0.0000 -0.0000
main -0.5500 0.0000 -0.0000