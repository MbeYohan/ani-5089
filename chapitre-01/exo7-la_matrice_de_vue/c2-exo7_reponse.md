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

struct Mat4
{
    double m[4][4];
};

Mat4 Identite()
{
    Mat4 r{};
    for (int i = 0; i < 4; ++i)
    {
        r.m[i][i] = 1.0;
    }
    return r;
}

Mat4 MatriceDePose(const Pose &pose)
{
    const Quat &q = pose.orientation;
    Mat4 r = Identite();
    r.m[0][0] = 1.0 - 2.0 * (q.y * q.y + q.z * q.z);
    r.m[0][1] = 2.0 * (q.x * q.y - q.z * q.w);
    r.m[0][2] = 2.0 * (q.x * q.z + q.y * q.w);
    r.m[1][0] = 2.0 * (q.x * q.y + q.z * q.w);
    r.m[1][1] = 1.0 - 2.0 * (q.x * q.x + q.z * q.z);
    r.m[1][2] = 2.0 * (q.y * q.z - q.x * q.w);
    r.m[2][0] = 2.0 * (q.x * q.z - q.y * q.w);
    r.m[2][1] = 2.0 * (q.y * q.z + q.x * q.w);
    r.m[2][2] = 1.0 - 2.0 * (q.x * q.x + q.y * q.y);
    r.m[0][3] = pose.position.x;
    r.m[1][3] = pose.position.y;
    r.m[2][3] = pose.position.z;
    return r;
}

Mat4 InversionGenerale(const Mat4 &a)
{
    double t[4][8];
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            t[i][j] = a.m[i][j];
            t[i][j + 4] = (i == j) ? 1.0 : 0.0;
        }
    }

    for (int colonne = 0; colonne < 4; ++colonne)
    {
        int pivot = colonne;
        for (int ligne = colonne + 1; ligne < 4; ++ligne)
        {
            if (std::fabs(t[ligne][colonne]) > std::fabs(t[pivot][colonne]))
            {
                pivot = ligne;
            }
        }
        if (std::fabs(t[pivot][colonne]) < 1e-12)
        {
            return Identite();
        }
        for (int j = 0; j < 8; ++j)
        {
            const double tmp = t[colonne][j];
            t[colonne][j] = t[pivot][j];
            t[pivot][j] = tmp;
        }
        const double inverse = 1.0 / t[colonne][colonne];
        for (int j = 0; j < 8; ++j)
        {
            t[colonne][j] *= inverse;
        }
        for (int ligne = 0; ligne < 4; ++ligne)
        {
            if (ligne == colonne)
            {
                continue;
            }
            const double facteur = t[ligne][colonne];
            for (int j = 0; j < 8; ++j)
            {
                t[ligne][j] -= facteur * t[colonne][j];
            }
        }
    }

    Mat4 r{};
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            r.m[i][j] = t[i][j + 4];
        }
    }
    return r;
}

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

Mat4 VueAnalytique(const Pose &pose)
{
    const Quat conjugue{-pose.orientation.x, -pose.orientation.y,
                        -pose.orientation.z, pose.orientation.w};
    const Vec3 opposee{-pose.position.x, -pose.position.y, -pose.position.z};
    Pose inverse{Tourner(conjugue, opposee), conjugue};
    return MatriceDePose(inverse);
}

void Afficher(const char *nom, const Mat4 &a)
{
    std::cout << nom << '\n'
              << std::fixed << std::setprecision(6);
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            std::cout << a.m[i][j] << (j == 3 ? '\n' : ' ');
        }
    }
}

int main()
{
    Pose pose{};
    if (!(std::cin >> pose.position.x >> pose.position.y >> pose.position.z >> pose.orientation.x >> pose.orientation.y >> pose.orientation.z >> pose.orientation.w))
    {
        return 1;
    }

    const Mat4 parInversion = InversionGenerale(MatriceDePose(pose));
    const Mat4 parConstruction = VueAnalytique(pose);
    Afficher("inversion generale", parInversion);
    Afficher("construction directe", parConstruction);

    double ecartMax = 0.0;
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            ecartMax = std::fmax(ecartMax,
                                 std::fabs(parInversion.m[i][j] - parConstruction.m[i][j]));
        }
    }
    std::cout << "ecart maximal sur les seize coefficients "
              << std::setprecision(12) << ecartMax << '\n';

    Mat4 degeneree = MatriceDePose(pose);
    degeneree.m[1][0] = 0.0;
    degeneree.m[1][1] = 0.0;
    degeneree.m[1][2] = 0.0;
    Afficher("inversion generale sur matrice degeneree", InversionGenerale(degeneree));
    std::cout << "aucun message d'erreur n'a ete emis\n";
    return 0;
}


## Exemple d'exécution

Entrée : 0.2 1.6 -0.5 0 0.5000000000 0 0.8660254038

Sortie :
inversion generale
0.500000 0.000000 -0.866025 -0.533013
0.000000 1.000000 0.000000 -1.600000
0.866025 0.000000 0.500000 0.076795
0.000000 0.000000 0.000000 1.000000

construction directe
0.500000 0.000000 -0.866025 -0.533013
0.000000 1.000000 0.000000 -1.600000
0.866025 0.000000 0.500000 0.076795
0.000000 0.000000 0.000000 1.000000

ecart maximal sur les seize coefficients 0.000000000023

inversion generale sur matrice degeneree
1.000000 0.000000 0.000000 0.000000
0.000000 1.000000 0.000000 0.000000
0.000000 0.000000 1.000000 0.000000
0.000000 0.000000 0.000000 1.000000
