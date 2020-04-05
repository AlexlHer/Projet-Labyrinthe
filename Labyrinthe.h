#ifndef LABYRINTHE_H
#define LABYRINTHE_H

#include "Environnement.h"

class Labyrinthe : public Environnement
{
	private:
		char *tab;
		char **_data;
		int widthLaby;
		int heightLaby;

	public:
		Labyrinthe(char *);
		int width() { return widthLaby; }	// retourne la largeur du labyrinthe.
		int height() { return heightLaby; } // retourne la longueur du labyrinthe.
		char data(int i, int j){ return _data[j][i]; } // retourne la case (i, j).
};

#endif
