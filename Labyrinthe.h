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
		int width() { return heightLaby; }	 // retourne la largeur du labyrinthe.
		int height() { return widthLaby; }   // retourne la longueur du labyrinthe.
		char data(int i, int j){ return _data[i][j]; } // retourne la case (i, j).

		bool set_data(int i, int j, int b);

		//provisoire
		void display_tab();

};

#endif
