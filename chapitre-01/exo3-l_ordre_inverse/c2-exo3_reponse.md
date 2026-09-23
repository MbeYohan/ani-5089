## Code

#include <iostream>
#include <iomanip>

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

// rotation puis translation.
Vec3 AppliquerRotationPuisTranslation(const Pose &pose, const Vec3 &point)
{
    const Vec3 t = Tourner(pose.orientation, point);
    return Vec3{t.x + pose.position.x, t.y + pose.position.y, t.z + pose.position.z};
}

// translation puis rotation.
Vec3 AppliquerTranslationPuisRotation(const Pose &pose, const Vec3 &point)
{
    const Vec3 deplace{
        point.x + pose.position.x,
        point.y + pose.position.y,
        point.z + pose.position.z};
    return Tourner(pose.orientation, deplace);
}

int main()
{
    Pose pose{};
    Vec3 point{};
    if (!(std::cin >> pose.position.x >> pose.position.y >> pose.position.z >> pose.orientation.x >> pose.orientation.y >> pose.orientation.z >> pose.orientation.w >> point.x >> point.y >> point.z))
    {
        return 1;
    }

    const Vec3 a = AppliquerRotationPuisTranslation(pose, point);
    const Vec3 b = AppliquerTranslationPuisRotation(pose, point);

    std::cout << std::fixed << std::setprecision(4);
    std::cout << a.x << ' ' << a.y << ' ' << a.z << '\n';
    std::cout << b.x << ' ' << b.y << ' ' << b.z << '\n';
    return 0;
}


## Exemple d'exécution

Entrée : 1 0 0 0 0.7071067812 0 0.7071067812 1 0 0

Sortie :
1.0000 0.0000 -1.0000
-0.0000 0.0000 -2.0000

Avec une translation de un mètre vers la droite et une rotation d'un quart de tour autour de l'axe vertical, les deux ordres donnent des points différents : l'objet part en orbite autour de l'origine du monde au lieu de tourner sur lui-même.

## coincidence
Le cas qui coïncide, obtenu avec la même rotation mais une translation d'un mètre vers le haut, donc portée par l'axe de rotation :

Entrée : 0 1 0 0 0.7071067812 0 0.7071067812 1 0 0
Sortie : 
-0.0000 1.0000 -1.0000
-0.0000 1.0000 -1.0000

Les deux lignes sont identiques parce que tourner d'un quart de tour autour de l'axe vertical laisse le vecteur vertical inchangé.
