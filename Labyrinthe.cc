#include "Labyrinthe.h"
#include "Chasseur.h"
#include "Gardien.h"
#include <iostream>
#include <fstream>
#include <regex>
#include <map>

Sound *Chasseur::_hunter_fire; // bruit de l'arme du chasseur.
Sound *Chasseur::_hunter_hit;  // cri du chasseur touché.
Sound *Chasseur::_wall_hit;	   // on a tapé un mur.

Environnement *Environnement::init(char *filename)
{
	return new Labyrinthe(filename);
}

Labyrinthe::Labyrinthe(char *filename)
{
	// std::cout << filename << std::endl;
	std::ifstream fichier;
	fichier.open(filename);

	// Si fichier introuvable.
	if(!fichier.is_open())
	{
		std::cerr << "Ouverture de fichier impossible, sortie." << std::endl;
		return;
	}

	std::string ligne;
	std::smatch matchh;

	// std::regex laby("[\+][\+\-\| \na-zA-Z]+");

	std::regex commentaire("#.*");
	std::regex texture("[a-z][ 	]+[A-z1-9]+.jpg");
	std::regex fichier_texture("[A-z1-9]+.jpg");
	std::regex debutLaby("[ ]*[+]+[-a-z ]*");

	std::map<char, std::string> images;
	std::vector<std::vector<char>> laby;
	std::vector<char> tmp;
	char tmp2;


	// Récupération des images.
	// Tant qu'on est pas sur le laby.
	while (getline(fichier, ligne) && !std::regex_search(ligne, debutLaby))
	{
		// Si c'est une texture.
		if (std::regex_search(ligne, matchh, texture))
		{
			// std::cout << "texture" << matchh.str();
			// On prend son id.
			tmp2 = matchh.str()[0];

			// On prend juste le nom du fichier de la texture.
			std::regex_search(ligne, matchh, fichier_texture);

			// On met dans le dico.
			images[tmp2] = matchh.str();
		}
		// std::cout << std::endl;
	}

	// std::cout << "a" << images['a'] << " b" << images['b'] << std::endl;


	// Récupération du laby.
	// On récupère toutes les lignes.
	widthLaby = 0;
	do
	{
		tmp.clear();
		for(int i = 0; i < ligne.length(); i++)
		{
			tmp.push_back(ligne[i]);
		}
		laby.push_back(tmp);

		if(ligne.length() > widthLaby) widthLaby = ligne.length();

	} while (getline(fichier, ligne));

	heightLaby = laby.size();
	// std::cout << "w " << widthLaby << " " << laby[0].size() << " h" << heightLaby << " " << laby.size() << std::endl;

	// Création du tableau _data.
	tab = new char[widthLaby * heightLaby];
	_data = new char *[heightLaby];

	// On met des cases vide.
	for(int i = 0; i < widthLaby * heightLaby; i++)
	{
		tab[i] = EMPTY;
	}

	for (int i = 0; i < heightLaby; i++)
	{
		_data[i] = tab + (i * widthLaby);
	}


	// // Print laby
	// for(int i = 0; i < laby.size(); i++)
	// {
	// 	for(int j = 0; j < laby[i].size(); j++)
	// 	{
	// 		std::cout << laby[i][j];
	// 	}
	// 	std::cout << std::endl;
	// }

	std::regex affiche("[a-wy-z]");
	std::regex non_mur("[CGx ]");

	int debut;
	std::vector<Wall> murs;

	std::string cts;
	std::string cts_droite;
	std::string cts_bas;

	std::vector<Wall> affiches;
	Wall a;

	std::vector<Box> caisses;

	std::vector<Mover*> joueurs;

	std::map<int, int> debutsVerticales;

	// Analyse laby.
	for (int i = 0; i < laby.size(); i++)
	{
		debut = -1;

		for (int j = 0; j < laby[i].size(); j++)
		{
			// Pour regex.
			cts = std::string(1, laby[i][j]);
			cts_droite = (j + 1 < laby[i].size() ? std::string(1, laby[i][j + 1]) : " ");
			cts_bas = (i + 1 < laby.size() ? std::string(1, laby[i + 1][j]) : " ");

			// Début d'un mur H.
			// debut == -1 car il peut y avoir des '+' sur un mur déjà commencé (+---+---+).
			// Après, on doit avoir un mur (non non_mur).
			if (laby[i][j] == '+' && debut == -1 && !std::regex_match(cts_droite, non_mur))
			{
				debut = j;
				// std::cout << "debut H" << debut << std::endl;
			}

			// Fin du mur H (si après, on a un non_mur.)
			else if (laby[i][j] == '+' && debut != -1 && std::regex_match(cts_droite, non_mur))
			{
				murs.push_back({i, debut, i, j, 0});
				for (int k = debut; k <= j; k++)
				{
					_data[i][k] = 1;
				}
				// std::cout << "Mur H" << debut << " " << j << " " << i << std::endl;
				debut = -1;
			}

			// Début d'un mur V.
			// Si y'a un + au-dessus et que debut n'est pas déjà utilisé.
			if (laby[i][j] == '+' 
			&& (debutsVerticales.find(j) == debutsVerticales.end() || debutsVerticales.find(j)->second == -1) 
			&& !std::regex_match(cts_bas, non_mur))
			{
				debutsVerticales[j] = i;
				// std::cout << "debut V" << debutsVerticales[j] << std::endl;
			}

			// Fin d'un mur V.
			// Si y'a un + au-dessus et que debut est utilisé.
			else if (laby[i][j] == '+' 
			&& debutsVerticales.find(j) != debutsVerticales.end() && debutsVerticales.find(j)->second != -1
			&& std::regex_match(cts_bas, non_mur))
			{
				murs.push_back({debutsVerticales.find(j)->second, j, i, j, 0});
				for (int k = debutsVerticales.find(j)->second; k <= i; k++)
				{
					_data[k][j] = 1;
				}
				// std::cout << "Mur V" << debutsVerticales.find(j)->second << " " << i << " " << j << std::endl;
				debutsVerticales[j] = -1;
			}

			// Caisses.
			if (laby[i][j] == 'x')
			{
				caisses.push_back({i, j, 0});
				_data[i][j] = 1;
			}

			// Chasseur.
			else if (laby[i][j] == 'C')
			{
				// On crée le chasseur.
				Chasseur *c = new Chasseur(this);
				c->_x = (float)i * 10;
				c->_y = (float)j * 10;

				// std::cout << "Chasseur" << c->_x << " " << c->_y << std::endl;

				// On le met au début de la liste.
				joueurs.emplace(joueurs.begin(), c);
			}

			// Gardiens.
			else if (laby[i][j] == 'G')
			{
				// On crée le chasseur.
				Gardien *g = new Gardien(this, "Lezard");
				g->_x = (float)i * 10;
				g->_y = (float)j * 10;
				// std::cout << "Gardiens" << g->_x << " " << g->_y << std::endl;

				// On le met au début de la liste.
				joueurs.push_back(g);
				_data[i][j] = 2;
			}

			// Trésor.
			else if (laby[i][j] == 'T')
			{
				_treasor._x = i;
				_treasor._y = j;
				_data[i][j] = 1;
				// std::cout << "Trésor" << _treasor._x << " " << _treasor._y << std::endl;
			}

			// Affiches.
			else if (std::regex_match(cts, affiche))
			{
				// Mur +--a--+.
				if (debut != -1)
					a = Wall({i, j, i, j + 2, 0});

				// Mur +||a||+.
				else
					a = Wall({i, j, i + 2, j, 0});

				char *tmp3 = new char[128];
				sprintf(tmp3, "%s/%s", texture_dir, images.find(laby[i][j])->second.c_str());
				a._ntex = wall_texture(tmp3);

				affiches.push_back(a);
				_data[i][j] = 1;
			}
		}
	}

	// _nwall = murs.size();
	// std::copy(murs.begin(), murs.end(), _walls);

	// _npicts = affiches.size();
	// std::copy(affiches.begin(), affiches.end(), _picts);

	// _nboxes = caisses.size();
	// std::copy(caisses.begin(), caisses.end(), _boxes);

	// _nguards = joueurs.size();
	// std::copy(joueurs.begin(), joueurs.end(), _guards);

	// _nwall = murs.size();
	// _walls = murs.data();

	// _npicts = affiches.size();
	// _picts = affiches.data();

	// _nboxes = caisses.size();
	// _boxes = caisses.data();

	// _nguards = joueurs.size();
	// _guards = joueurs.data();

	_nwall = murs.size();
	_walls = new Wall[murs.size()];
	for (int i = 0; i < _nwall; i++)
	{
		_walls[i] = murs[i];
	}

	_npicts = affiches.size();
	_picts = new Wall[affiches.size()];
	for (int i = 0; i < _npicts; i++)
	{
		_picts[i] = affiches[i];
	}

	_nboxes = caisses.size();
	_boxes = new Box[caisses.size()];
	for (int i = 0; i < _nboxes; i++)
	{
		_boxes[i] = caisses[i];
	}

	_nguards = joueurs.size();
	_guards = new Mover *[joueurs.size()];
	for (int i = 0; i < _nguards; i++)
	{
		_guards[i] = joueurs[i];
	}
}

bool Labyrinthe::set_data(int i, int j, int b){

	if((int)_data[i][j] == 1){
		return false;
		std::cout << "error" << std::endl;
	}
	
	else{
		_data[i][j] = b;
	}
	return true;
}


//provisoire
void Labyrinthe::display_tab(){
	int a;
	for(int i = 0; i < width(); i++){
		for(int j = 0; j < height(); j++){
			a = data(i, j);
			std::cout << a;
		}
		std::cout << std::endl;
	}
	std::cout << "val : "<< (int)data(10, 13) <<std::endl;
}

