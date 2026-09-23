## Code

#include <iostream>
#include <iomanip>
#include <cmath>

struct Vec3 {
    double x;
    double y;
    double z;
};

struct Quat {
    double x;
    double y;
    double z;
    double w;
};

struct Pose {
    Vec3 position;
    Quat orientation;
};

Quat Multiplier(const Quat& a, const Quat& b) {
    return Quat{
        a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y,
        a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x,
        a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w,
        a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z
    };
}

Quat Normaliser(const Quat& q) {
    const double n = std::sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
    if (n < 1e-12) {
        return Quat{0.0, 0.0, 0.0, 1.0};
    }
    return Quat{q.x / n, q.y / n, q.z / n, q.w / n};
}

Pose Extrapoler(const Pose& pose, const Vec3& vitesseLineaire,
                const Vec3& vitesseAngulaire, double dt) {
    Pose r{};
    r.position.x = pose.position.x + vitesseLineaire.x * dt;
    r.position.y = pose.position.y + vitesseLineaire.y * dt;
    r.position.z = pose.position.z + vitesseLineaire.z * dt;

    const double norme = std::sqrt(vitesseAngulaire.x * vitesseAngulaire.x +
                                   vitesseAngulaire.y * vitesseAngulaire.y +
                                   vitesseAngulaire.z * vitesseAngulaire.z);

    if (norme < 1e-9) {
        r.orientation = pose.orientation;
        return r;
    }

    const double angle = norme * dt;
    const double demi = angle / 2.0;
    const double s = std::sin(demi) / norme;
    const Quat increment{vitesseAngulaire.x * s,
                         vitesseAngulaire.y * s,
                         vitesseAngulaire.z * s,
                         std::cos(demi)};
    r.orientation = Normaliser(Multiplier(increment, pose.orientation));
    return r;
}

int main() {
    Pose pose{};
    Vec3 vitesseLineaire{};
    Vec3 vitesseAngulaire{};
    double dt = 0.0;
    if (!(std::cin >> pose.position.x >> pose.position.y >> pose.position.z
                  >> pose.orientation.x >> pose.orientation.y
                  >> pose.orientation.z >> pose.orientation.w
                  >> vitesseLineaire.x >> vitesseLineaire.y >> vitesseLineaire.z
                  >> vitesseAngulaire.x >> vitesseAngulaire.y >> vitesseAngulaire.z
                  >> dt)) {
        return 1;
    }

    const Pose r = Extrapoler(pose, vitesseLineaire, vitesseAngulaire, dt);
    std::cout << std::fixed << std::setprecision(4);
    std::cout << r.position.x << ' ' << r.position.y << ' ' << r.position.z << '\n';
    std::cout << r.orientation.x << ' ' << r.orientation.y << ' '
              << r.orientation.z << ' ' << r.orientation.w << '\n';
    return 0;
}


## Exemple d'exécution

Entrée : 0 1.6 0 0 0 0 1 0 0 0 0 3.1415926536 0 0.05

Sortie :
0.0000 1.6000 0.0000
0.0000 0.0785 0.0000 0.9969


Second cas, avec une vitesse angulaire nulle et une vitesse linéaire d'un demi-mètre par seconde pendant cent millisecondes :

Entrée : 0 1.6 0  0 0 0 1  0.5 0 0  0 0 0  0.1

Sortie : 
0.0500 1.6000 0.0000
0.0000 0.0000 0.0000 1.0000

La position avance de cinq centimètres, l'orientation reste l'identité, et aucune valeur non numérique n'apparaît. C'est le cas que la garde en tête de fonction protège.
