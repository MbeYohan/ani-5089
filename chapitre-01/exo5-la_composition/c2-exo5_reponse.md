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

Vec3 Appliquer(const Pose &pose, const Vec3 &point)
{
    const Vec3 t = Tourner(pose.orientation, point);
    return Vec3{t.x + pose.position.x, t.y + pose.position.y, t.z + pose.position.z};
}

// le parent d'abord, l'enfant ensuite
Pose Composer(const Pose &parent, const Pose &enfant)
{
    const Vec3 decale = Tourner(parent.orientation, enfant.position);
    return Pose{
        Vec3{
            parent.position.x + decale.x,
            parent.position.y + decale.y,
            parent.position.z + decale.z},
        Multiplier(parent.orientation, enfant.orientation)};
}

int main()
{
    Pose parent{};
    Pose enfant{};
    Vec3 point{};
    if (!(std::cin >> parent.position.x >> parent.position.y >> parent.position.z >> parent.orientation.x >> parent.orientation.y >> parent.orientation.z >> parent.orientation.w >> enfant.position.x >> enfant.position.y >> enfant.position.z >> enfant.orientation.x >> enfant.orientation.y >> enfant.orientation.z >> enfant.orientation.w >> point.x >> point.y >> point.z))
    {
        return 1;
    }

    const Vec3 parComposition = Appliquer(Composer(parent, enfant), point);
    const Vec3 parEtapes = Appliquer(parent, Appliquer(enfant, point));
    const double ecart = std::sqrt(
        (parComposition.x - parEtapes.x) * (parComposition.x - parEtapes.x) +
        (parComposition.y - parEtapes.y) * (parComposition.y - parEtapes.y) +
        (parComposition.z - parEtapes.z) * (parComposition.z - parEtapes.z));

    std::cout << std::fixed << std::setprecision(4);
    std::cout << parComposition.x << ' ' << parComposition.y << ' ' << parComposition.z << '\n';
    std::cout << parEtapes.x << ' ' << parEtapes.y << ' ' << parEtapes.z << '\n';
    std::cout << std::setprecision(10) << ecart << '\n';
    return 0;
}


## Exemple d'exécution

Entrée : 0.1 0.2 0.3 0 0.4226182617 0 0.9063077870 0.5 0 0 0.1736481777 0 0 0.9848077530 1 1 1

Sortie :
2.0460 0.7977 -0.0252
2.0460 0.7977 -0.0252
0.0000000000