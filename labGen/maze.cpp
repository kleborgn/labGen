#pragma once
#include <vector>
#include <random>
#include <time.h>

#include "imgui.h"

// Grille

/*
Le principe pour stocker le labyrinthe c'est de faire 2 matrices booléenne pour stocker les "murs" du lab.
Notre lab fait m colonnes sur n lignes.
On stocke donc les murs verticaux dans une matrice M de taille (m+1) * n et les horizontaux dans une de m * (n+1)
*/

struct mazeSize_t {
	int m;
	int n;
};

struct walls_t {
	std::vector<bool> vertical;
	std::vector<bool> horizontal;
	int verticalSize;
	int horizontalSize;
};

class Maze {
private:
	mazeSize_t m_Size;
	const int MAX_SIZE = 100;
	walls_t walls;
	std::vector<int> idCases;

public:
	Maze(int m, int n) {
		this->m_Size.m = m;
		this->m_Size.n = n;

		//On init les murs

		walls.verticalSize = (m + 1) * n;
		walls.horizontalSize = m * (n + 1);

		walls.vertical.resize(walls.verticalSize, true);
		walls.horizontal.resize(walls.horizontalSize, true);
	}

	void reset() {
		for (int i = 0; i < walls.horizontalSize; i++) {
			walls.horizontal[i] = true;
		}
		for (int i = 0; i < walls.verticalSize; i++) {
			walls.vertical[i] = true;
		}
	}

	/*
	Maintenant pour générer le lab on va utiliser une procédure ascendante. On va fusionner aléatoirement les chemins.
	Def Labyrinthe : "Chaque cellule est reliée à toutes les autres, et ce, de manière unique"
		1) On numérote toutes les cases
		2) On parcourt tous les murs et on en enlève 1 au hasard :
			a) Il ne faut pas que les deux cellules ait le même numéro
			b) L'id de la 1ere cellule est affecté à la 2eme
		3) On doit supprimer m * n - 1 murs pour obtenir un chemin unique
	*/


	//Verifie si un mur peut etre supprimé

	bool murValide(int index, bool sens) { // sens vrai si horizontal
		if (sens) {
			if ((index < m_Size.m) || (index >= m_Size.m * m_Size.n)) { // mur extremite
				return false;
			}
			else {
				if (idCases.at(index) == idCases.at(index - m_Size.m)) {
					return false;
				}
				if (idCases.at(index) < idCases.at(index - m_Size.m)) {
					for (int i = 0; i < m_Size.m * m_Size.n; i++) {
						if (idCases.at(i) == idCases.at(index - m_Size.m))
							idCases.at(i) = idCases.at(index);
					}
				}
				else {
					for (int i = 0; i < m_Size.m * m_Size.n; i++) {
						if (idCases.at(i) == idCases.at(index))
							idCases.at(i) = idCases.at(index - m_Size.m);
					}
				}
			}
		}
		else {
			if (((index - (index / m_Size.m)) % m_Size.m == 0) || (((index - (index / m_Size.m)) -1) % m_Size.m == 0)) { //mur extremite
				return false;
			}
			else {
				if (idCases.at(index - (index / m_Size.m)) == idCases.at(index - (index / m_Size.m) - 1))
				{
					return false;
				}
				if (idCases.at(index - (index / m_Size.m)) < idCases.at(index - (index / m_Size.m) - 1))
				{
					for (int i = 0; i < m_Size.m * m_Size.n; i++) {
						if (idCases.at(i) == idCases.at(index - (index / m_Size.m) - 1))
							idCases.at(i) = idCases.at(index - (index / m_Size.m));
					}
				}
				else {
					for (int i = 0; i < m_Size.m * m_Size.n; i++) {
						if (idCases.at(i) == idCases.at(index - (index / m_Size.m)))
							idCases.at(i) = idCases.at(index - (index / m_Size.m) - 1);
					}
				}
			}
		}
		return true;
	}

	void generate() {
		//vars
		std::vector<int>::iterator i;//on utilise un iterator pour faire moderne ^^
		int nbrMursSuppr = 0;
		bool randSensMur;
		int randMurIndexV;
		int randMurIndexH;
		bool currentWall;

		int currentNbr = 0;

		int case1, case2;

		std::srand(time(nullptr));

		// On initialise les id des cases

		//idCases.resize(m_Size.n * m_Size.n);

		// On leur attribue un nombre unique

		/*
		for (i = idCases.begin(); i != idCases.end(); i++) {
			*i = currentNbr;
			currentNbr++;
		}
		*/

		for (int i = 0; i < m_Size.m * m_Size.n; i++) {
			idCases.push_back(i);
		}
		// On supprime les murs

		while (nbrMursSuppr < m_Size.m * m_Size.n) {
			randSensMur = std::rand() % 2; //horizontal si vrai
			randMurIndexV = std::rand() % walls.verticalSize;
			randMurIndexH = std::rand() % walls.horizontalSize;

			// On sélectionne le mur
			if (randSensMur) {
				currentWall = walls.horizontal[randMurIndexH];
				if (murValide(randMurIndexH, randSensMur)) {
					walls.horizontal[randMurIndexH] = false;
					nbrMursSuppr++;
				}
			}
			else {
				currentWall = walls.vertical[randMurIndexV];
				if (murValide(randMurIndexV, randSensMur)) {
					walls.horizontal[randMurIndexV] = false;
					nbrMursSuppr++;
				}
			}

			// On vérifie que les 2 cases n'ont pas le meme id
		}
	}

	void draw() {
		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		static ImVec4 col = ImVec4(1.0f, 1.0f, 0.4f, 1.0f); // color
		const ImU32 col32 = ImColor(col);
		const ImVec2 p = ImGui::GetCursorScreenPos();
		float x = p.x + 4.0f, y = p.y + 4.0f, spacing = 8.0f;
		static float sz = 15.0f; // size
		float thickness = 1.0f;

		//draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + sz, y + sz), col32);

		for (int i = 0; i != walls.horizontalSize; i++) {
			if (walls.horizontal.at(i)) {
				draw_list->AddLine(ImVec2(x, y), ImVec2(x + sz, y), col32, thickness);
			}
			x += sz;

			if (i % m_Size.m == m_Size.m -1) {
				y += sz;
				x = p.x + 4.0f;
			}
		}

		x = p.x + 4.0f, y = p.y + 4.0f;
		
		for (int i = 0; i != walls.verticalSize; i++) {
			if (walls.vertical.at(i)) {
				draw_list->AddLine(ImVec2(x, y), ImVec2(x, y + sz), col32, thickness);
			}
			x += sz;

			if ((i != m_Size.m-1) && ((i - (i / m_Size.m)) % m_Size.m == m_Size.m -1)) {
				y += sz;
				x = p.x + 4.0f;
			}
		}
	}
};