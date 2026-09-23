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

Vec3 Appliquer(const Pose &pose, const Vec3 &point)
{
    const Vec3 t = Tourner(pose.orientation, point);
    return Vec3{t.x + pose.position.x, t.y + pose.position.y, t.z + pose.position.z};
}

Pose Inverser(const Pose &pose)
{
    const Quat conjugue{
        -pose.orientation.x,
        -pose.orientation.y,
        -pose.orientation.z,
        pose.orientation.w};
    const Vec3 opposee{-pose.position.x, -pose.position.y, -pose.position.z};
    return Pose{Tourner(conjugue, opposee), conjugue};
}

int main()
{
    Pose pose{};
    Vec3 point{};
    if (!(std::cin >> pose.position.x >> pose.position.y >> pose.position.z >> pose.orientation.x >> pose.orientation.y >> pose.orientation.z >> pose.orientation.w >> point.x >> point.y >> point.z))
    {
        return 1;
    }

    const Vec3 transforme = Appliquer(pose, point);
    const Vec3 retour = Appliquer(Inverser(pose), transforme);
    const double ecart = std::sqrt(
        (retour.x - point.x) * (retour.x - point.x) +
        (retour.y - point.y) * (retour.y - point.y) +
        (retour.z - point.z) * (retour.z - point.z));

    std::cout << std::fixed << std::setprecision(4);
    std::cout << transforme.x << ' ' << transforme.y << ' ' << transforme.z << '\n';
    std::cout << retour.x << ' ' << retour.y << ' ' << retour.z << '\n';
    std::cout << std::setprecision(10) << ecart << '\n';
    return 0;
}


## Exemple d'exécution

Entrée :0.3 1.2 -0.7 0.1903827938 0.2538437251 0 0.9483236552 1 2 3

Sortie :
2.8088 2.0684 1.9366
1.0000 2.0000 3.0000
0.0000000000