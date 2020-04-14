#ifndef GARDIEN_H
#define GARDIEN_H

#include "Mover.h"
#include "Sound.h"

class Labyrinthe;

class Gardien : public Mover
{
private:
	bool _aggressif = false;
	float _rotation;

public:

	Gardien(Labyrinthe *l, const char *modele);
	// : Mover(120, 80, l, modele) {}

	Sound *_hunter_fire; // bruit de l'arme du chasseur.
	Sound *_hunter_hit;	// cri du chasseur touch�.
	Sound *_wall_hit;	// on a tap� un mur.

	// mon gardien pense tr�s mal!
	void update(void);
	// et ne bouge pas!
	bool move(double dx, double dy);
	// ne sait pas tirer sur un ennemi.
	void fire(int angle_vertical);
	// quand a faire bouger la boule de feu...
	bool process_fireball(float dx, float dy);

	bool aim();

	bool attaque();

	bool analyse(int vision);
};

#endif
