#ifndef PERSONNAGE_H
#define PERSONNAGE_H

#include <iostream>
#include <vector>
#include <math.h>
#include <unistd.h>

#include "Mover.h"

class Labyrinthe;
class Personnage;

class Personnage : public Mover{
    private:
    public:
        int _life;
        std::vector<Personnage*> _allPerso;

        Personnage(Labyrinthe *l, const char *modele, int life) : Mover(120, 80, l, modele){
            _life = life;
        }
        
        int getLife(){
            return _life;
        }

        void setLife(int i){
            _life = i;
        }
};

//initialisation de la variable statique
//std::vector<Personnage*> Personnage::_allPerso; 

#endif
