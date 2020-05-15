#pragma once
#include <vector>
#include <random>
#include <time.h>
#include <memory>

#include "imgui/imgui.h"

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

class Vec2 {
public:
	int x = NULL;
	int y = NULL;

	Vec2() {

	}
	
	Vec2(int x, int y) {
		this->x = x;
		this->y = y;
	}
};

struct uneCase_t {
	Vec2 coords;
	int num;
};

typedef bool mur;

struct walls_t {
	std::vector<mur> vertical;
	std::vector<mur> horizontal;
	int verticalSize;
	int horizontalSize;
};

class Maze {
private:
	mazeSize_t m_Size;
	walls_t walls;
	std::vector< std::vector<uneCase_t> > idCases;

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

	void resetAndResize(int m, int n) {
		this->m_Size.m = m;
		this->m_Size.n = n;

		walls.verticalSize = (m + 1) * n;
		walls.horizontalSize = m * (n + 1);

		walls.vertical.resize(walls.verticalSize, true);
		walls.horizontal.resize(walls.horizontalSize, true);

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
/*
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

		
		for (i = idCases.begin(); i != idCases.end(); i++) {
			*i = currentNbr;
			currentNbr++;
		}
		

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

*/

void numeroter() {

	idCases.resize(m_Size.m);

	int currentNum = 0;

	for (auto i = idCases.begin(); i != idCases.end(); i++) {
		i->resize(m_Size.n);
	}

	for (int i = 0; i < m_Size.m; i++) {
		for (int j = 0; j < m_Size.n; j++) {
			idCases.at(i).at(j).coords = Vec2(i, j);
			idCases.at(i).at(j).num = currentNum;
			currentNum++;
		}
	}
}

uneCase_t* randomCase() {
	int x, y;
	x = rand() % m_Size.m;
	y = rand() % m_Size.n;
	return &idCases.at(x).at(y);
}

std::vector<uneCase_t*> casesAvecNumDiff(uneCase_t* uneCase) {
	int idTest = uneCase->num;
	std::vector<uneCase_t*> retour;
	/*
	Cases à tester : 
	(x, y+1) ; (x, y-1) pour en haut et en bas
	(x-1, y) ; (x+1, y) pour gauche et droite
	*/

	//Bas
	if (uneCase->coords.y + 1 < m_Size.n) {
		if (idCases.at(uneCase->coords.x).at(uneCase->coords.y + 1).num != idTest) {
			retour.push_back(&idCases.at(uneCase->coords.x).at(uneCase->coords.y + 1));
		}
	}
	//Haut
	if (uneCase->coords.y > 0) {
		if (idCases.at(uneCase->coords.x).at(uneCase->coords.y - 1).num != idTest) {
			retour.push_back(&idCases.at(uneCase->coords.x).at(uneCase->coords.y - 1));
		}
	}

	//Gauche
	if (uneCase->coords.x > 0) {
		if (idCases.at(uneCase->coords.x - 1).at(uneCase->coords.y).num != idTest) {
			retour.push_back(&idCases.at(uneCase->coords.x - 1).at(uneCase->coords.y));
		}
	}
	//Droite
	if (uneCase->coords.x + 1 < m_Size.m) {
		if (idCases.at(uneCase->coords.x + 1).at(uneCase->coords.y).num != idTest) {
			retour.push_back(&idCases.at(uneCase->coords.x + 1).at(uneCase->coords.y));
		}
	}

	return retour;
}

void deleteMur(uneCase_t* case1, uneCase_t* case2) {
	bool sens = true; //vrai si horizontal
	int numMur;

	if ((case1->coords.x == case2->coords.x + 1) || (case1->coords.x == case2->coords.x - 1)) {
		sens = false;
	}

	if (!sens) {
		if (case1->coords.x > case2->coords.x)
			numMur = (case1->coords.x + m_Size.m * case1->coords.y) + case1->coords.y;
		else
			numMur = (case2->coords.x + m_Size.m * case2->coords.y) + case2->coords.y;
		walls.vertical[numMur] = false;
	}
	else {
		if (case1->coords.y > case2->coords.y)
			numMur = (case1->coords.x + m_Size.m * case1->coords.y);
		else
			numMur = (case2->coords.x + m_Size.m * case2->coords.y);
		walls.horizontal.at(numMur) = false;
	}
}

void remplacerNum(int src, int dst) {
	for (auto i = idCases.begin(); i != idCases.end(); i++) {
		for (auto j = i->begin(); j != i->end(); j++) {
			if (j->num == dst)
				j->num = src;
		}
	}
}

bool estFini() {
	bool pasfini = true;
	int i = 0, j = 0;
	while (pasfini && (i < m_Size.m)) {
		while (pasfini && (j < m_Size.n)) {
			if (idCases.at(i).at(j).num != 0)
				pasfini = false;
			j++;
		}
		i++;
	}
	return pasfini;
}

void algo2() {
	uneCase_t* currentCase;
	uneCase_t* currentCase2;
	int nbrCasesDiff;
	std::vector<uneCase_t*> casesDiff;

	this->numeroter();

	srand(time(nullptr)); // on initialise rand avec l'heure mais faudrait faire un système de seed (comme dans minecraft)

	while (!estFini()) {
		currentCase = randomCase();

		casesDiff = casesAvecNumDiff(currentCase);
		nbrCasesDiff = casesDiff.size();

		if (nbrCasesDiff > 0) {
			currentCase2 = casesDiff.at(rand() % nbrCasesDiff);
			deleteMur(currentCase, currentCase2);

			if (currentCase->num < currentCase2->num) {
				remplacerNum(currentCase->num, currentCase2->num);
			}
			else {
				remplacerNum(currentCase2->num, currentCase->num);
			}
		}
	}

	}

	void draw(ImVec4 color, float thick, float size) {
		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		static ImVec4 col = color; // color
		ImU32 col32 = ImColor(col);
		const ImVec2 p = ImGui::GetCursorScreenPos();
		float x = p.x + 4.0f, y = p.y + 4.0f;
		float sz = size; // size
		float thickness = thick;

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

	void drawDebug() {
		// Ajouter un mode pour écrire les valeurs des cases de idCases
	}
};