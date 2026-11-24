#pragma once

enum class Espace { LOCAL, STAGE };

struct Vue {
    int chaine;         
    int indexImage;    
    double lacetDeg;   
    double champDeg;    
};

struct CoucheProjection {
    Espace espace;
    Vue vues[2];        
};
