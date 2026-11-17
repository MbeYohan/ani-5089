#include <algorithm>
#include <cmath>
#include <cstdio>
#include <vector>

struct Vec3 { double x = 0, y = 0, z = 0; };
Vec3 operator+(Vec3 a, Vec3 b) { return {a.x + b.x, a.y + b.y, a.z + b.z}; }
Vec3 operator-(Vec3 a, Vec3 b) { return {a.x - b.x, a.y - b.y, a.z - b.z}; }
Vec3 operator-(Vec3 a) { return {-a.x, -a.y, -a.z}; }
Vec3 operator*(double s, Vec3 v) { return {s * v.x, s * v.y, s * v.z}; }
Vec3 Cross(Vec3 a, Vec3 b) { return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x}; }

struct Quat { double w = 1, x = 0, y = 0, z = 0; };
Quat operator*(Quat a, Quat b) {
    return {a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z, a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y,
            a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x, a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w};
}
Quat Conj(Quat q) { return {q.w, -q.x, -q.y, -q.z}; }
Quat AutourDeX(double angle) { return {std::cos(angle / 2), std::sin(angle / 2), 0, 0}; }
Vec3 Tourner(Quat q, Vec3 v) {
    Vec3 u{q.x, q.y, q.z};
    Vec3 t = 2.0 * Cross(u, v);
    return v + q.w * t + Cross(u, t);
}

struct Pose { Vec3 position; Quat orientation; };
Pose operator*(Pose a, Pose b) { return {a.position + Tourner(a.orientation, b.position), a.orientation * b.orientation}; }
Pose Inverse(Pose p) {
    Quat c = Conj(p.orientation);
    return {-Tourner(c, p.position), c};
}

struct EtatBool { bool current = false; bool changed = false; bool active = false; };
void Synchroniser(EtatBool& etat, bool presse, bool lie) {
    bool avant = etat.current;
    etat.active = lie;
    etat.current = lie && presse;
    etat.changed = etat.current != avant;
}

Pose PosePaume(int image) {
    int k = std::min(std::max(image, 3), 7) - 3;
    Vec3 depart{0.0, 1.02, -0.03};
    return {depart + Vec3{0.0, 0.05 * k, -0.04 * k}, AutourDeX(0.1 * k)};
}

double Distance(Vec3 a, Vec3 b) { return std::sqrt((a.y - b.y) * (a.y - b.y) + (a.z - b.z) * (a.z - b.z)); }

void Dessiner(const char* fichier, Pose cube, Pose paume, const std::vector<Vec3>& trace, bool tenu, bool actif) {
    const int L = 480, H = 640;
    const double ECHELLE = 400.0, F_MIN = -0.40, Y_MAX = 1.55;
    std::vector<unsigned char> pixels(L * H * 3, 255);

    for (int py = 0; py < H; ++py) {
        for (int px = 0; px < L; ++px) {
            Vec3 p{0.0, Y_MAX - (py + 0.5) / ECHELLE, -(F_MIN + (px + 0.5) / ECHELLE)};
            int rgb[3] = {255, 255, 255};
            auto peindre = [&](int r, int g, int b) { rgb[0] = r; rgb[1] = g; rgb[2] = b; };

            if (p.y < 0) peindre(170, 170, 170);
            for (Vec3 t : trace) {
                if (Distance(p, t) < 0.008) peindre(110, 110, 110);
            }
            Vec3 local = Tourner(Conj(cube.orientation), p - cube.position);
            if (std::fabs(local.y) <= 0.05 && std::fabs(local.z) <= 0.05) peindre(240, 140, 0);
            if (Distance(p, paume.position) < 0.025) peindre(200, 0, 0);
            if (px >= 10 && px < 40 && py >= 10 && py < 40) {
                if (tenu) peindre(0, 170, 0); else peindre(200, 0, 0);
            }
            if (px >= 10 && px < 40 && py >= 50 && py < 80) {
                if (actif) peindre(0, 170, 0); else peindre(20, 20, 20);
            }

            for (int c = 0; c < 3; ++c) pixels[(py * L + px) * 3 + c] = (unsigned char)rgb[c];
        }
    }

    std::FILE* f = std::fopen(fichier, "wb");
    if (!f) return;
    std::fprintf(f, "P6\n%d %d\n255\n", L, H);
    std::fwrite(pixels.data(), 1, pixels.size(), f);
    std::fclose(f);
}

int main(int argc, char** argv) {
    const double G = 9.81, DT = 0.05;
    const int IMAGE_CAPTURE = 14;

    EtatBool appui;
    Pose cube{{0.0, 1.0, 0.0}, {}};
    Pose attache;
    double vitesse = 0.0;
    bool tenu = false;
    bool dejaSaisi = false;
    std::vector<Vec3> trace;

    for (int image = 1; image <= 20; ++image) {
        bool presse = image >= 3;
        bool lie = !(image >= 8 && image <= 17);
        Synchroniser(appui, presse, lie);
        Pose paume = PosePaume(image);

        std::printf("image : %d, courant : %d, change : %d, actif : %d\n", image, appui.current, appui.changed, appui.active);

        if (appui.active && appui.current && appui.changed) { attache = Inverse(paume) * cube; tenu = true; }
        if (appui.active && !appui.current && appui.changed) tenu = false;

        if (tenu) {
            cube = paume * attache;
            vitesse = 0.0;
            dejaSaisi = true;
        } else if (dejaSaisi) {
            vitesse -= G * DT;
            cube.position.y += vitesse * DT;
            if (cube.position.y < 0.05) { cube.position.y = 0.05; vitesse = 0.0; }
        }

        std::fprintf(stderr, "image %d : tenu %d, cube y %.3f, paume y %.3f\n", image, tenu, cube.position.y, paume.position.y);

        if (argc > 1 && image == IMAGE_CAPTURE) Dessiner(argv[1], cube, paume, trace, tenu, appui.active);
        trace.push_back(cube.position);
    }
    return 0;
}
