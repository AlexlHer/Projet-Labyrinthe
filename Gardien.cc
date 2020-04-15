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
	_angle = 0;
	_direction = _angle;

	distanceVision = 120;
	_defenseur = false;
}


void Gardien::update(){

	//conversion degré à radiant
	float piAngle = (_direction * M_PI / 180);

	vision();

	if(_alerte){
		int angleCible = aim();

		if(_angle >= angleCible){

			if(_angle - angleCible <= 180){
				_direction-=3;
				checkAngle();
				ciblage(angleCible);
			}
			else{
				_direction+=3;
				checkAngle();
				ciblage(angleCible);
			}
		}
		else{

			if(angleCible - _angle < 180){
				_direction+=3;
				checkAngle();
				ciblage(angleCible);
			}
			else{
				_direction-=3;
				checkAngle();
				ciblage(angleCible);
			}
		}
	}

	if(_aggressif){
		_angle = aim();
		piAngle = (_angle * M_PI / 180);
		_direction = _angle;
		move(-sin(piAngle), cos(piAngle));
		attaque();
	}
	else{
		_angle = _direction;
		move(sin(-piAngle), cos(piAngle));
	}
	return;
}


void Gardien::checkAngle(){
	if( _direction >= 360){
		_direction -= 360;
	}
	else if(_direction < 0){
		_direction += 360;
	}
}


bool Gardien::ciblage(int angleCible){

	if(	((_angle + 365) % 360 >= angleCible && _angle <= angleCible) ||
		((_angle + 355) % 360 <= angleCible && _angle >= angleCible) )
	{
		_alerte = false;
		_aggressif = true;
	}
	return true;
}


/*
 *	Vérifie si un objet fictif de coordonnées x, y est proche d'un autre objet de coordonnées cx, cy
 */

bool Gardien::touche_cible(float x, float y, float cx, float cy){
	return ((x < cx + Environnement::scale) && (x >= cx - Environnement::scale) &&
			(y < cy + Environnement::scale) && (y >= cy - Environnement::scale));
}

/*
 *	Adapte les coordonnées flottantes x, y au graphe _data et vérifie que la case est égale à b
 */

bool Gardien::case_convert(float x, float y, int a){
	return ( a == (_l -> data(	(int)(x / Environnement::scale),
								(int)(y / Environnement::scale))) );
}

bool Gardien::vision(){

	Mover* cible = _l->_guards[0];

	int cibleAngle = aim();
	float cibleAngleRadiant = (cibleAngle * M_PI / 180);	

	//si la cible n'est pas dans le cercle de portée de vision du gardien, le gardien reste passif (les blocs et murs ne sont pour l'instant pas pris en compte)
	if( !((_x - distanceVision <= cible->_x && cible->_x <= _x + distanceVision) &&
		(_y - distanceVision <= cible->_y && cible->_y <= _y + distanceVision)) )
	{
		_alerte = false;
		_aggressif = false;
		return false;
	}

	//on vérifie s'il n'y a pas un obstacle entre la cible et le gardien
	else{
		float addx = sin(-cibleAngleRadiant);
		float addy = cos(cibleAngleRadiant);

		float xfict = _x;
		float yfict = _y; 
	
		while ( !touche_cible(xfict, yfict, cible->_x, cible->_y) )
		{
			//s'il y a un obstacle entre les deux
			if( case_convert(xfict, yfict, 1) ){
				_alerte = false;
				_aggressif = false;
				return false;
			}

			xfict += addx;
			yfict += addy;
		}
	}
	_alerte = true;
	return true;
}


bool Gardien::attaque(){
	int fireRate = rand()%20;
	if(fireRate == 1) fire(180);
	return true;
}


int Gardien::aim(){
	Mover* cible = _l->_guards[0];

	float angle = atan((_x -cible->_x) / (_y - cible->_y));
	if(cible->_y > _y){
		angle += M_PI;

		if(cible->_x > _x){
			angle -= 2 * M_PI;
		}
	}
	return (180 - (angle * 180/M_PI));
}


bool Gardien::move(double dx, double dy)
{
	// On réduit la vitesse.
	dx /= 4;
	dy /= 4;

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
			// _l->set_data(((int)(x / Environnement::scale)), ((int)(y / Environnement::scale)), EMPTY);
			// _l->set_data(((int)((x + dx) / Environnement::scale)), ((int)((y + dy) / Environnement::scale)), 2);
		}
		// On déplace le personnage.
		_x = x - Environnement::scale / 2;
		_y = y - Environnement::scale / 2;

		return true;
	}

	// Sinon, c'est qu'il y a collision et donc on oriente le personnage autre part.
	else
	{
		// Si le Gardien est un défenseur, il va vers le trésor une fois sur deux.
		// if(_defenseur && rand() % 2 == 1)
		if(_defenseur)
		{
			// La case du gardien.
			int cX = (int)(x / Environnement::scale);
			int cY = (int)(y / Environnement::scale);

			// La où le gardien devra aller.
			int versX;
			int versY;

			// On cherche la plus petite case autour du gardien (qui conduira vers le trésor).
			int plusPetit = _l->getDistMax();

			// Pour toutes les cases autour.
			for(int sX = -1; sX <= 1; sX++)
			{
				for (int sY = -1; sY <= 1; sY++)
				{
					// On ne veut pas étudier la case du gardien, donc lorsqu'on est dessus, on saute.
					if (sX == 0 && sY == 0) continue;

					// Si on a trouvé une case menant plus rapidement vers le trésor, on la choisi.
					if (_l->innond(cX + sX, cY + sY) < plusPetit && _l->innond(cX + sX, cY + sY) != -2)
					{
						versX = cX + sX;
						versY = cY + sY;
						plusPetit = _l->innond(cX + sX, cY + sY);
					}
				}
			}
			// std::cout << versX - cX << " " << versY - cY << std::endl;

			// On oriente vers la case menant vers le trésor.
			// TODO : A corriger.
			_direction = (atan((y - versY * Environnement::scale) / (x - versX * Environnement::scale)) * 180 / M_PI) - 90;
		}
		else
		{
			_direction = rand() % 360 + 1;
		}
	}
	return false;
}

void Gardien::fire(int angle_vertical){
	message ("Woooshh...");
	//_hunter_fire -> play ();
	_fb -> init (/* position initiale de la boule */ _x, _y, 10.,
				 /* angles de vis�e */ angle_vertical, 360 - _angle);
	return;
}


bool Gardien::process_fireball(float dx, float dy){
	Mover* cible = _l->_guards[0];

	float a = _fb->get_x();
	float b = _fb->get_y();

	if(touche_cible(a, b, cible->_x, cible->_y)){
		return false;
	}

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
}