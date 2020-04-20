#include "Chasseur.h"
#include "Gardien.h"
#include <iostream>
#include <unistd.h>

/*
 *	Tente un deplacement.
 */

bool Chasseur::move_aux(double dx, double dy)
{
	if(_life<=0){
		return false;
	}

	float x = _x + Environnement::scale / 2;
	float y = _y + Environnement::scale / 2;

	check_treasor(dx, dy);

	if (EMPTY == _l->data(	(int)((_x + dx) / Environnement::scale),
					 		(int)((_y + dy) / Environnement::scale)))
	{
		for(int i = 1; i < _l->_nguards; i++){
			Mover *cible = _l->_guards[i];
			if(check_coordonnees(_x+dx, _y+dy, cible->_x, cible->_y, Environnement::scale)){
				return false;
			}
		}
		_x += dx;
		_y += dy;
		return true;
	}
	return false;
}


void Chasseur::check_treasor(double dx, double dy){
	int treasorX = _l->_treasor._x;
	int treasorY = _l->_treasor._y;

	if(	(	(_x + dx)/Environnement::scale >= treasorX && (_x + dx)/Environnement::scale <  treasorX + 1) &&
		(	(_y + dy)/Environnement::scale >= treasorY && (_y + dy)/Environnement::scale <  treasorY + 1)){
			std::cout << "VICTOIRE" << std::endl;
		}
	return;
}

/*
 *	Constructeur.
 */

Chasseur::Chasseur(Labyrinthe *l) : Mover(100, 80, l, 0)
{
	// initialise les sons.
	_hunter_fire = new Sound("sons/hunter_fire.wav");
	_hunter_hit = new Sound("sons/hunter_hit.wav");
	if (_wall_hit == 0)
		_wall_hit = new Sound("sons/hit_wall.wav");
}

/*
 *	Fait bouger la boule de feu (ceci est une exemple, � vous de traiter les collisions sp�cifiques...)
 */

bool Chasseur::process_fireball(float dx, float dy)
{
	float a = _fb->get_x();
	float b = _fb->get_y();

	// calculer la distance entre le chasseur et le lieu de l'explosion.
	float x = (_x - _fb->get_x()) / Environnement::scale;
	float y = (_y - _fb->get_y()) / Environnement::scale;
	float dist2 = x * x + y * y;
	// on bouge que dans le vide!

	for(int i = 1; i < _l->_nguards; i++){
		Mover *cible = _l->_guards[i];

		if ((a >= cible->_x && a < cible->_x + Environnement::scale) &&
			(b >= cible->_y && b < cible->_y + Environnement::scale))
		{
			float dmax2 = (_l->width()) * (_l->width()) + (_l->height()) * (_l->height());
			// faire exploser la boule de feu avec un bruit fonction de la distance.
			_wall_hit->play(1. - dist2 / dmax2);
			Gardien *g = dynamic_cast<Gardien*>(cible);

			if(g->getLife() > 0){
				g->setLife(g->getLife() - 1);
			}
			if(g->getLife() == 0){
				cible->rester_au_sol();
				for(int j = 1; j < _l->_nguards; j++){
					Gardien *gg = dynamic_cast<Gardien*>(_l->_guards[j]);
					std::cout << "life : " << gg->getLife() << std::endl;
					if(gg->getLife() > 0){
						return true;
					}
					else{
						message("Victoire, les gardiens ne sont plus en état de se battre");
					}
				}
				std::cout << std::endl;
			}
			else{
				cible->tomber();
			}
			return false;
		}
	}
	if(EMPTY == _l->data(	(int)((_fb->get_x() + dx) / Environnement::scale),
							(int)((_fb->get_y() + dy) / Environnement::scale)))
	{
		// il y a la place.
		return true;
	}

	// collision...
	// calculer la distance maximum en ligne droite.
	float dmax2 = (_l->width()) * (_l->width()) + (_l->height()) * (_l->height());
	// faire exploser la boule de feu avec un bruit fonction de la distance.
	_wall_hit->play(1. - dist2 / dmax2);
	return false;
}


/*
 *	Tire sur un ennemi.
 */

void Chasseur::fire(int angle_vertical)
{
	//affiche l'état du labyrinthe
	//_l->display_tab();
	
	//_hunter_fire->play();

	_fb->init(/* position initiale de la boule */ _x, _y, 10.,
			  /* angles de vis�e */ angle_vertical, _angle);
}


bool Chasseur::check_coordonnees(float x, float y, float cx, float cy, float ecart){
	// std::cout << "x : " << x << std::endl;
	// std::cout << "y : " << y << std::endl;
	// std::cout << "cx- : " << cx - ecart << std::endl;
	// std::cout << "cy- : " << cy - ecart << std::endl;
	// std::cout << "cx+ : " << cx + ecart<< std::endl;
	// std::cout << "cy+ : " << cy + ecart << std::endl << std::endl;
	return (	(x >= cx && x < cx + ecart) &&
				(y >= cy && y < cy + ecart) );
	
}

