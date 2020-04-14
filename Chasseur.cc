#include "Chasseur.h"
#include <iostream>
#include <unistd.h>

/*
 *	Tente un deplacement.
 */

bool Chasseur::move_aux (double dx, double dy)
{

	int c = _l -> data ((int)((_x + dx) / Environnement::scale),
				(int)((_y + dy) / Environnement::scale));	
					 
	int x = (int)(_x / Environnement::scale);
	int new_x = (int)((_x + dx) / Environnement::scale);
	int y = (int)(_y / Environnement::scale);
	int new_y = (int)((_y + dy) / Environnement::scale);


	if (EMPTY ==  c ||
		3 == c)
	{
		_x += dx;
		_y += dy;
		
			if(x != new_x || y != new_y){
			_l->set_data(x, y, 0);
			_l->set_data(new_x, new_y, 3);
		}
		return true;
	}
	return false;
}

/*
 *	Constructeur.
 */

Chasseur::Chasseur (Labyrinthe* l) : Mover (100, 80, l, 0)
{
	// initialise les sons.
	_hunter_fire = new Sound ("sons/hunter_fire.wav");
	_hunter_hit = new Sound ("sons/hunter_hit.wav");
	if (_wall_hit == 0)
		_wall_hit = new Sound ("sons/hit_wall.wav");
}

/*
 *	Fait bouger la boule de feu (ceci est une exemple, � vous de traiter les collisions sp�cifiques...)
 */

bool Chasseur::process_fireball (float dx, float dy)
{
	Mover *cible = _l->_guards[1];
	float a = _fb->get_x();
	float b = _fb->get_y();

	// calculer la distance entre le chasseur et le lieu de l'explosion.
	float x = (_x - _fb->get_x()) / Environnement::scale;
	float y = (_y - _fb->get_y()) / Environnement::scale;
	float dist2 = x * x + y * y;
	// on bouge que dans le vide!

	if ((a < cible->_x + Environnement::scale) && (a >= cible->_x - Environnement::scale) &&
		(b < cible->_y + Environnement::scale) && (b >= cible->_y - Environnement::scale))
	{
		return false;
	}
	else if (EMPTY == _l->data((int)((_fb->get_x() + dx) / Environnement::scale),
							   (int)((_fb->get_y() + dy) / Environnement::scale)))
	{
		message("Woooshh ..... %d", (int)dist2);
		// il y a la place.
		return true;
	}

	// collision...
	// calculer la distance maximum en ligne droite.
	float dmax2 = (_l->width()) * (_l->width()) + (_l->height()) * (_l->height());
	// faire exploser la boule de feu avec un bruit fonction de la distance.
	_wall_hit->play(1. - dist2 / dmax2);
	message("Booom...");
	return false;
}

/*
 *	Tire sur un ennemi.
 */

void Chasseur::fire (int angle_vertical)
{
	//affiche l'état du labyrinthe
	_l->display_tab();

	message ("Woooshh...");
	_hunter_fire -> play ();
	_fb -> init (/* position initiale de la boule */ _x, _y, 10.,
				 /* angles de vis�e */ angle_vertical, _angle);
}
