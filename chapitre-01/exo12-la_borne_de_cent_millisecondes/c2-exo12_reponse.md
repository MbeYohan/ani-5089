## Code

#include <iostream>
#include <iomanip>
#include <cmath>

namespace
{
    constexpr double kPi = 3.14159265358979323846;

    constexpr double kVitesseInitiale = 180.0 * kPi / 180.0;
    constexpr double kFrequence = 1.5;
    constexpr double kPasDeSimulation = 1e-4;
}

struct Quat
{
    double x;
    double y;
    double z;
    double w;
};

Quat Multiplier(const Quat &a, const Quat &b)
{
    return Quat{
        a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y,
        a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x,
        a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w,
        a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z};
}

Quat Normaliser(const Quat &q)
{
    const double n = std::sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
    return Quat{q.x / n, q.y / n, q.z / n, q.w / n};
}

Quat AutourDeY(double angle)
{
    return Quat{0.0, std::sin(angle / 2.0), 0.0, std::cos(angle / 2.0)};
}

double VitesseReelle(double t)
{
    return kVitesseInitiale * std::cos(2.0 * kPi * kFrequence * t);
}

Quat PoseReelle(double duree)
{
    Quat q{0.0, 0.0, 0.0, 1.0};
    for (double t = 0.0; t < duree; t += kPasDeSimulation)
    {
        const double pas = std::fmin(kPasDeSimulation, duree - t);
        q = Normaliser(Multiplier(AutourDeY(VitesseReelle(t) * pas), q));
    }
    return q;
}

// on suppose la vitesse de l'instant zero constante
Quat PoseExtrapolee(double duree)
{
    return AutourDeY(kVitesseInitiale * duree);
}

double EcartEnDegres(const Quat &a, const Quat &b)
{
    const double produit = std::fabs(a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w);
    const double borne = std::fmin(1.0, produit);
    return 2.0 * std::acos(borne) * 180.0 / kPi;
}

int main()
{
    const double durees[] = {0.010, 0.020, 0.030, 0.050, 0.070, 0.100,
                             0.150, 0.200, 0.300, 0.500, 0.700, 1.000};

    std::cout << "duree_ms erreur_deg\n"
              << std::fixed;
    for (double duree : durees)
    {
        const double erreur = EcartEnDegres(PoseReelle(duree), PoseExtrapolee(duree));
        std::cout << std::setprecision(0) << duree * 1000.0 << ' '
                  << std::setprecision(3) << erreur << '\n';
    }
    return 0;
}

## La courbe de l'erreur

duree_ms erreur_deg
10 0.003
20 0.021
30 0.071
50 0.328
70 0.892
100 2.545
150 8.129
200 17.824
300 48.081
500 109.090
700 120.098
1000 179.982

## Où la borne se justifie

En dessous de cinquante millisecondes, l'erreur reste sous le tiers de degré. C'est en dessous de ce que l'œil distingue, et l'extrapolation est un gain net.

Autour de cent millisecondes, l'erreur passe deux degrés et demi. On est encore utilisable, mais on voit nettement la pente s'installer : la courbe n'est pas linéaire, elle accélère.

Au-delà, tout se dégrade vite. Huit degrés à cent cinquante millisecondes, dix-huit à deux cents, quarante-huit à trois cents. À une seconde, la tête modélisée est revenue à son point de départ pendant que l'extrapolation, elle, a continué tout droit : l'erreur atteint cent quatre-vingts degrés, c'est-à-dire le maximum possible. La prédiction ne se contente plus d'être imprécise, elle regarde dans la direction opposée.

La borne de cent millisecondes tombe donc juste avant le coude de la courbe. Ce n'est pas une valeur ronde choisie par commodité, c'est l'endroit où le modèle à vitesses constantes cesse d'aider plus qu'il ne ment.
