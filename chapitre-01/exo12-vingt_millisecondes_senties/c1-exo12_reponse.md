## Le programme

Une fenêtre noire, un disque blanc qui suit la souris, et un retard réglable au clavier. Le principe est un historique horodaté des positions : à chaque image, au lieu d'afficher la position courante, on remonte l'historique jusqu'à celle qui date exactement du retard demandé. C'est plus juste qu'une interpolation, parce que le retard reste constant quelle que soit la vitesse du mouvement.

Flèches haut et bas pour changer le retard par paliers de dix millisecondes, R pour remettre à zéro, Échap pour quitter. Le retard courant s'affiche dans la barre de titre, ce qui est pratique pour moi et gênant pour l'expérience : il faut cacher la barre de titre au testeur, sinon il lit la valeur au lieu de la ressentir.


```cpp
#include <windows.h>
#include <deque>
#include <string>

namespace {

struct Echantillon {
    DWORD instant;
    POINT position;
};

std::deque<Echantillon> g_historique;
int g_retardMs = 0;
POINT g_affichee = {0, 0};

void Enregistrer(HWND fenetre) {
    POINT souris;
    GetCursorPos(&souris);
    ScreenToClient(fenetre, &souris);
    g_historique.push_back(Echantillon{GetTickCount(), souris});

    const DWORD maintenant = GetTickCount();
    const DWORD cible = maintenant - static_cast<DWORD>(g_retardMs);

    for (const Echantillon& e : g_historique) {
        if (e.instant <= cible) {
            g_affichee = e.position;
        }
    }
    while (g_historique.size() > 1 && g_historique.front().instant < maintenant - 1000) {
        g_historique.pop_front();
    }
}

void MettreAJourLeTitre(HWND fenetre) {
    const std::string titre = "Retard : " + std::to_string(g_retardMs) + " ms";
    SetWindowTextA(fenetre, titre.c_str());
}

LRESULT CALLBACK Procedure(HWND fenetre, UINT message, WPARAM wp, LPARAM lp) {
    switch (message) {
    case WM_CREATE:
        SetTimer(fenetre, 1, 5, nullptr);
        MettreAJourLeTitre(fenetre);
        return 0;

    case WM_TIMER:
        Enregistrer(fenetre);
        InvalidateRect(fenetre, nullptr, TRUE);
        return 0;

    case WM_KEYDOWN:
        if (wp == VK_UP && g_retardMs < 200) {
            g_retardMs += 10;
        } else if (wp == VK_DOWN && g_retardMs > 0) {
            g_retardMs -= 10;
        } else if (wp == 'R') {
            g_retardMs = 0;
        } else if (wp == VK_ESCAPE) {
            PostQuitMessage(0);
        }
        MettreAJourLeTitre(fenetre);
        return 0;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC dc = BeginPaint(fenetre, &ps);
        RECT zone;
        GetClientRect(fenetre, &zone);
        FillRect(dc, &zone, static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)));

        HBRUSH pinceau = CreateSolidBrush(RGB(255, 255, 255));
        HGDIOBJ ancien = SelectObject(dc, pinceau);
        const int r = 22;
        Ellipse(dc, g_affichee.x - r, g_affichee.y - r, g_affichee.x + r, g_affichee.y + r);
        SelectObject(dc, ancien);
        DeleteObject(pinceau);

        EndPaint(fenetre, &ps);
        return 0;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(fenetre, message, wp, lp);
}

}  

int WINAPI WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int montrer) {
    WNDCLASSA classe = {};
    classe.lpfnWndProc = Procedure;
    classe.hInstance = instance;
    classe.lpszClassName = "RetardReglable";
    classe.hCursor = LoadCursor(nullptr, IDC_ARROW);
    RegisterClassA(&classe);

    HWND fenetre = CreateWindowA("RetardReglable", "Retard : 0 ms",
                                 WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                                 1000, 700, nullptr, nullptr, instance, nullptr);
    ShowWindow(fenetre, montrer);

    MSG message;
    while (GetMessage(&message, nullptr, 0, 0) > 0) {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }
    return 0;
}
```


## Les cinq seuils



| Personne | Seuil | Mots exacts | Faux positifs |
|----------|-------|-------------|---------------|
| 1 | | | |
| 2 | | | |
| 3 | | | |
| 4 | | | |
| 5 | | | |

Seuil le plus bas : 
Le plus haut : 
Moyenne :

## La comparaison avec les vingt millisecondes


## Pourquoi le seuil est bien plus bas dans un casque

La différence n'est pas que les yeux deviennent plus sensibles. Elle tient à ce qui sert de référence.

Sur un écran, la personne compare la position du disque à un souvenir : elle se rappelle où elle a déplacé la souris et juge après coup si l'image a suivi. C'est une comparaison lente, tolérante, et qui repose sur la mémoire.

Dans un casque, la référence n'est pas un souvenir, c'est une mesure. L'oreille interne a mesuré le mouvement de la tête, en continu et sans délai, et elle attend l'image correspondante. Quand l'image arrive en retard, ce n'est pas une impression de lenteur, c'est une contradiction entre deux sens, et le corps traite cette contradiction comme il traite une intoxication.

Le résultat n'est pas le même non plus. Un retard perçu sur écran agace. Un retard perçu en casque donne la nausée, et elle ne disparaît pas dès qu'on arrête.

Dernier point, qui se lira directement dans les cinq seuils : leur dispersion. Elle interdit de fixer un budget sur son propre ressenti. On le fixe sur la personne la plus sensible, pas sur la moyenne, et c'est une des raisons pour lesquelles la cible est aussi basse.
