#include "Gardien.h"
#include "iostream"
#include <time.h>
#include <math.h>
#include <unistd.h>
#include "Sound.h"

Gardien::Gardien(Labyrinthe *l, const char *modele) : Mover(120, 80, l, modele)
{
	// initialise les sons.
	_hunter_fire = new Sound("sons/hunter_fire.wav");
	_hunter_hit = new Sound("sons/hunter_hit.wav");
	if (_wall_hit == 0)
		_wall_hit = new Sound("sons/hit_wall.wav");

	//initialise l'aléatoire
	srand(time(NULL));

	//met l'angle de départ du gardien à 0
	_angle = 270;
	_rotation = 270;

	
}

void Gardien::update()
{
	//conversion degré à radiant
	float piAngle = (_rotation * M_PI / 180);

	int vision = 80;

	analyse(vision);

	if (_aggressif)
	{
		attaque();
		//move(sin(-piAngle), cos(piAngle));
	}
	else
	{
		_angle = _rotation;
		//move(sin(-piAngle), cos(piAngle));
	}

	return;
}

bool Gardien::analyse(int vision)
{
	Mover *cible = _l->_guards[0];
	int cibleX = (int)(cible->_x / Environnement::scale);
	int cibleY = (int)(cible->_y / Environnement::scale);

	int myX = (int)(_x / Environnement::scale);
	int myY = (int)(_y / Environnement::scale);

	if (cibleX > _x)
	{
	}

	if ((_x - vision <= cible->_x && cible->_x <= _x + vision) &&
		(_y - vision <= cible->_y && cible->_y <= _y + vision))
	{

		_aggressif = true;
		return true;
	}
	_aggressif = false;

	return true;
}

bool Gardien::attaque()
{
	aim();
	int i = rand() % 20;
	if (i == 1)
		fire(180);
	return true;
}

bool Gardien::aim()
{
	Mover *cible = _l->_guards[0];
	//if(_x > cible->_x)
	float angle = atan((_y - cible->_y) / (_x - cible->_x));
	if (cible->_x < _x)
	{
		angle += M_PI;
	}
	_angle = (angle * 180 / M_PI) - 90;

	return true;
}

bool Gardien::move(double dx, double dy)
{
	// On réduit la vitesse.
	dx /= 5;
	dy /= 5;

	// On prend le centre du personnage.
	float x = _x + Environnement::scale / 2;
	float y = _y + Environnement::scale / 2;

	//récupère la case sur laquelle le gardien va se déplacer
	int c = _l->data((int)((x + dx) / Environnement::scale),
					 (int)((y + dy) / Environnement::scale));

	// On regarde si la case est vide.
	if (c == EMPTY || c == 2)
	{
		// On ajoute le déplacement.
		x += dx;
		y += dy;

		// Si ce déplacement fait un changement de case.
		if (((int)(x / Environnement::scale)) != ((int)((x + dx) / Environnement::scale)) ||
			((int)(y / Environnement::scale)) != ((int)((y + dy) / Environnement::scale)))
		{
			// On change la case du perso.
			_l->set_data(((int)(x / Environnement::scale)), ((int)(y / Environnement::scale)), EMPTY);
			_l->set_data(((int)((x + dx) / Environnement::scale)), ((int)((y + dy) / Environnement::scale)), 2);
		}
		// On déplace le personnage.
		_x = x - Environnement::scale / 2;
		_y = y - Environnement::scale / 2;

		return true;
	}

	// Sinon, c'est qu'il y a collision et donc on oriente le personnage autre part.
	else
	{
		_angle = rand() % 360 + 1;
	}
	return false;
}

void Gardien::fire(int angle_vertical)
{
	//affiche l'état du labyrinthe
	//_l->display_tab();

	message("Woooshh...");
	_hunter_fire -> play ();
	_fb->init(/* position initiale de la boule */ _x, _y, 10.,
			  /* angles de vis�e */ angle_vertical, _angle);
	return;
}

bool Gardien::process_fireball(float dx, float dy)
{
	Mover *cible = _l->_guards[0];

	float a = _fb->get_x();
	float b = _fb->get_y();
	if ((a < cible->_x + Environnement::scale) && (a >= cible->_x - Environnement::scale) &&
		(b < cible->_y + Environnement::scale) && (b >= cible->_y - Environnement::scale))
	{
		return false;
	}

	// calculer la distance entre le chasseur et le lieu de l'explosion.
	float x = (_x - _fb->get_x()) / Environnement::scale;
	float y = (_y - _fb->get_y()) / Environnement::scale;
	float dist2 = x * x + y * y;
	// on bouge que dans le vide!
	if (EMPTY == _l->data((int)((_fb->get_x() + dx) / Environnement::scale),
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
	_wall_hit -> play (1. - dist2/dmax2);
	message("Booom...");
	return false;
}