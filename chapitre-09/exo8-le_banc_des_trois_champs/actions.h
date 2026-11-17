#pragma once

struct EtatBool {
    bool current = false;
    bool changed = false;
    bool active = false;
};

inline void Synchroniser(EtatBool& etat, bool presse, bool lie) {
    bool avant = etat.current;
    etat.active = lie;
    etat.current = lie && presse;
    etat.changed = etat.current != avant;
}
