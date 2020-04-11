#include "Gardien.h"
#include "iostream"
#include <time.h>
#include <math.h>
#include <unistd.h>

Gardien::Gardien(Labyrinthe *l, const char *modele) : Mover(120, 80, l, modele){

	//initialise l'aléatoire
	srand(time(NULL));
	
	//met l'angle de départ du gardien à 0
	_angle = 270;
}

void Gardien::update()
{
	//conversion degré à radiant
	float piAngle = _angle * M_PI / 180;

	//déplacement dans la direction vers laquelle est tourné le gardien
	move(-sin(piAngle), cos(piAngle));

	// int i = rand()%100;
	// if(i ==1) fire(180);
	
	return;
}

bool Gardien::move(double dx, double dy)
{
	//utilisable pour un rendu plus joli mais fausse les dimensions
	// int ex = (dx > 0 ? Environnement::scale : 0);
	// int ey = (dy > 0 ? Environnement::scale : 0);


	//récupère la case sur laquelle le gardien va se déplacer
	int c = _l -> data ((int)((_x + dx) / Environnement::scale),
						(int)((_y + dy) / Environnement::scale));

							 
	// int x = (int)(_x / Environnement::scale);
	// int new_x = ((int)((_x + dx) / Environnement::scale));
	// int y = (int)(_y / Environnement::scale);
	// int new_y = ((int)((_y + dy) / Environnement::scale));
	


	if (EMPTY ==  c ||
		2 == c)
	{
		_x += dx/5;
		_y += dy/5;

		if(	((int)(_x / Environnement::scale)) != ((int)((_x + dx) / Environnement::scale)) || 
			((int)(_y / Environnement::scale)) != ((int)((_y + dy) / Environnement::scale)))
		{
			std::cout << "changement" << std::endl;
			std::cout << "x : " << (int)(_x / Environnement::scale) << std::endl;
			std::cout << "y : " << (int)(_y / Environnement::scale) << std::endl;
			_l->set_data(((int)(_x / Environnement::scale)), ((int)(_y / Environnement::scale)), 0);
			_l->set_data(((int)((_x + dx) / Environnement::scale)), ((int)((_y + dy) / Environnement::scale)), 2);
		}

		return true;
	}
	else{
		_angle = rand() % 360 + 1;

	}
	return false;

}

void Gardien::fire(int angle_vertical)
{
	//affiche l'état du labyrinthe
	//_l->display_tab();

	message ("Woooshh...");
	//_hunter_fire -> play ();
	_fb -> init (/* position initiale de la boule */ _x, _y, 10.,
				 /* angles de vis�e */ angle_vertical, _angle);
	return;
}

bool Gardien::process_fireball(float dx, float dy)
{
	// calculer la distance entre le chasseur et le lieu de l'explosion.
	float	x = (_x - _fb -> get_x ()) / Environnement::scale;
	float	y = (_y - _fb -> get_y ()) / Environnement::scale;
	float	dist2 = x*x + y*y;
	// on bouge que dans le vide!
	if (EMPTY == _l -> data ((int)((_fb -> get_x () + dx) / Environnement::scale),
							 (int)((_fb -> get_y () + dy) / Environnement::scale)))
	{
		message ("Woooshh ..... %d", (int) dist2);
		// il y a la place.
		return true;
	}
	// collision...
	// calculer la distance maximum en ligne droite.
	float	dmax2 = (_l -> width ())*(_l -> width ()) + (_l -> height ())*(_l -> height ());
	// faire exploser la boule de feu avec un bruit fonction de la distance.
	//_wall_hit -> play (1. - dist2/dmax2);
	message ("Booom...");
	return false;
	return false;
}