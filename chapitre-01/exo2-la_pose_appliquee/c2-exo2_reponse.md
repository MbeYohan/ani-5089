## code

#include <iostream>
#include <iomanip>

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

// Rotation d'un vecteur par un quaternion unitaire.
Vec3 Tourner(const Quat& q, const Vec3& v) {
    const double tx = 2.0 * (q.y * v.z - q.z * v.y);
    const double ty = 2.0 * (q.z * v.x - q.x * v.z);
    const double tz = 2.0 * (q.x * v.y - q.y * v.x);
    return Vec3{
        v.x + q.w * tx + (q.y * tz - q.z * ty),
        v.y + q.w * ty + (q.z * tx - q.x * tz),
        v.z + q.w * tz + (q.x * ty - q.y * tx)
    };
}

// on tourne d'abord, puis on deplace
Vec3 Appliquer(const Pose& pose, const Vec3& point) {
    const Vec3 tourne = Tourner(pose.orientation, point);
    return Vec3{
        tourne.x + pose.position.x,
        tourne.y + pose.position.y,
        tourne.z + pose.position.z
    };
}

int main() {
    Pose pose{};
    Vec3 point{};
    if (!(std::cin >> pose.position.x >> pose.position.y >> pose.position.z
                  >> pose.orientation.x >> pose.orientation.y
                  >> pose.orientation.z >> pose.orientation.w
                  >> point.x >> point.y >> point.z)) {
        return 1;
    }

    const Vec3 r = Appliquer(pose, point);
    std::cout << std::fixed << std::setprecision(4);
    std::cout << r.x << ' ' << r.y << ' ' << r.z << '\n';
    return 0;
}

## Exemple d'exécution

Entrée : 0 1 0 0 0.7071067812 0 0.7071067812 1 0 0

Sortie : -0.0000 1.0000 -1.0000
