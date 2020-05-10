#include "Chasseur.h"
#include "Gardien.h"
#include <iostream>
#include <unistd.h>
#include "Personnage.h"

#define TP 2

/*
 *	Constructeur.
 */

Chasseur::Chasseur(Labyrinthe *l) : Personnage(l, 0, 5)
{
	// initialise les sons.
	_hunter_fire = new Sound("sons/hunter_fire.wav");
	_hunter_hit = new Sound("sons/hunter_hit.wav");
	if (_wall_hit == 0)
		_wall_hit = new Sound("sons/hit_wall.wav");
}


/*
 *	Tente un deplacement.
 */

bool Chasseur::move_aux(double dx, double dy)
{
	if(_life<=0){
		_life = 0;
		return false;
	}

	//float x = _x + Environnement::scale / 2;
	//float y = _y + Environnement::scale / 2;

	int posX = (int)((_x + dx) / Environnement::scale);
	int posY = (int)((_y + dy) / Environnement::scale);

	//vérifie si le Chasseur est proche du trésor
	check_treasor(dx, dy);

	if (EMPTY == _l->data(posX, posY) || (_l->data(posX, posY) >= 65 && _l->data(posX, posY) <= 90))
	{
		//empêche le Chasseur de se déplacer sur la position des Gardiens
		for(int i = 1; i < _l->_nguards; i++){
			Personnage *cible = _allPerso[i];
			if(cible->getLife() > 0){
				if(check_collision_ennemi(_x+dx, _y+dy, cible->_x, cible->_y, Environnement::scale)){
					return false;
				}
			}
		}
		_x += dx;
		_y += dy;
		return true;
	}


	else if(_l->data(posX, posY) >= 97 && _l->data(posX, posY) <= 122)
	{
		std::cout << "val : " << _l->data(posX, posY) -32 << std::endl;
		teleportation(_l->data(posX, posY) - 32);	
	}

	return false;
}


/*
 *	Téléporte le chasseur
 */

void Chasseur::teleportation(int newPos){
	//int a;
	for(int i = 0; i < _l->width(); i++){
		for(int j = 0; j < _l->height(); j++){
			if(_l->data(i, j) == newPos){
				_x = i * Environnement::scale;
				_y = j * Environnement::scale;
			}
		}
	}
}


/*
 *	Vérifie si le Chasseur ne se trouve pas à côté du trésor
 */

void Chasseur::check_treasor(double dx, double dy){
	int treasorX = _l->_treasor._x;
	int treasorY = _l->_treasor._y;

	if(	(	(_x + dx)/Environnement::scale >= treasorX && (_x + dx)/Environnement::scale <  treasorX + 1) &&
		(	(_y + dy)/Environnement::scale >= treasorY && (_y + dy)/Environnement::scale <  treasorY + 1)){
			message("VICTOIRE, le trésor est à vous");
		}
	return;
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

	//
	for (int i = 1; i < (int)_allPerso.size(); i++)
	{
		Personnage *cible = _allPerso[i];

		if ((a >= cible->_x && a < cible->_x + Environnement::scale) &&
			(b >= cible->_y && b < cible->_y + Environnement::scale))
		{
			//float dmax2 = (_l->width()) * (_l->width()) + (_l->height()) * (_l->height());
			// faire exploser la boule de feu avec un bruit fonction de la distance.
			//_wall_hit->play(1. - dist2 / dmax2);

			//Gardien *g = dynamic_cast<Gardien*>(cible);
			if(cible->getLife() > 0){
				cible->setLife(cible->getLife() - 1);
			}

			//l'ennemi est KO, il ne se relèvera pas
			if(cible->getLife() == 0){
				cible->rester_au_sol();

				//utilisé pour vérifier s'il y a encore des gardiens en vie
				bool fin = true;
				for (int j = 1; j < (int)_allPerso.size(); j++)
				{
					//Gardien *gg = dynamic_cast<Gardien*>(_l->_guards[j]);
					std::cout << "life : " << _allPerso[j]->_life << std::endl;
					if(_allPerso[j]->_life > 0){
						fin = false;
						break;
					}
				}
				if(fin){
					message("Victoire, les gardiens ne sont plus en état de se battre");
				}
				std::cout << std::endl;
			}

			//l'ennemi est touché, mais il peut encore se relever
			else{
				cible->tomber();
			}
			return false;
		}
	}
	int fireBallX = (int)((_fb->get_x() + dx) / Environnement::scale);
	int fireBallY = (int)((_fb->get_y() + dy) / Environnement::scale);

	if(EMPTY == _l->data(fireBallX, fireBallY)){
		// il y a la place.
		return true;
	}

	else if(_l->data(fireBallX, fireBallY) >= 97 && _l->data(fireBallX, fireBallY) <= 122){

		//getLabyrinthe()->set_data(fireBallX, fireBallY, 0);
		for (int i = 0; i < _l->_nwall; i++){
			if(	fireBallX >= _l->_walls[i]._x1 && fireBallX <= _l->_walls[i]._x2 &&
				fireBallY >= _l->_walls[i]._y1 && fireBallY <= _l->_walls[i]._y2){
					std::cout << "salut" << std::endl;

					for(int j = i; j < _l->_nwall - 1; j++){
						_l->_walls[j] = _l->_walls[j+1];
					}
					_l->_nwall --;
					break;
			}

			std::cout << "x1 : " << _l->_walls[i]._x1 << std::endl;
			std::cout << "y1 : " << _l->_walls[i]._y1 << std::endl;
			std::cout << "x2 : " << _l->_walls[i]._x2 << std::endl;
			std::cout << "y2 : " << _l->_walls[i]._y2 << std::endl;
			std::cout << "fireball x : " << (int)((_fb->get_x() + dx) / Environnement::scale) << std::endl;
			std::cout << "fireball y : " << (int)((_fb->get_y() + dy) / Environnement::scale) << std::endl;
			std::cout << std::endl;
		}
		
		//_l->_npicts --;
		_l->reconfigure();
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
	//affiche la vie de tous les persos
	// std::vector<Personnage*> a;
	// for(int i = 0; i < _allPerso.size(); i++){
	// 	int a = _allPerso[i]->_life;
	// 	std::cout << "sa vie : " << a << std::endl;
	// }
	// std::cout << std::endl;

	//_hunter_fire->play();

	if(_life > 0){
		_myLaby->display_tab();

		_fb->init(/* position initiale de la boule */ _x, _y, 10.,
			  /* angles de vis�e */ angle_vertical, _angle);
	}
	
}


/*
 *	Vérifie si l'objet fictif de coordonnées X, y se trouve aux alentours de cx, cy
 */

bool Chasseur::check_collision_ennemi(float x, float y, float cx, float cy, float ecart){
	return (	(x >= cx && x < cx + ecart) &&
				(y >= cy && y < cy + ecart) );
	
}

