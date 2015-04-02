/*************************************************************************
                           Constantes  -  description
                             -------------------
    début                : XXX
    copyright            : (C) XXX par XXX
    e-mail               : XXX
*************************************************************************/

//---------- Constantes utilisables par toutes les tâches (fichier Constante.h) -------
#if ! defined ( CONSTANTES_H )
#define CONSTANTES_H


#define SEMAPHORE_TEMPO 0
#define SEMAPHORE_COULEUR 1

enum CouleurFeu {ROUGE, ORANGE, VERT};

struct MemoirePartagee{
	unsigned int tempoEO;
	unsigned int tempoNS;
	CouleurFeu couleurAxeNS;
	CouleurFeu couleurAxeEO;
};

#endif // CONSTANTES_H
