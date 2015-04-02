/*************************************************************************
                           Constante  -  description
                             -------------------
    début                : XXX
    copyright            : (C) XXX par XXX
    e-mail               : XXX
*************************************************************************/

//---------- Constantes utilisables par toutes les tâches (fichier Constante.h) -------
#if ! defined ( CONSTANTE_H )
#define CONSTANTE_H


#define SEMAPHORE_TEMPO 0
#define SEMAPHORE_COULEUR 1

enum CouleurFeu {ROUGE, ORANGE, VERT};

struct MemoirePartagee{
	unsigned int tempoEO;
	unsigned int tempoNS;
	CouleurFeu couleurAxeNS;
	CouleurFeu couleurAxeEO;
};

#endif // CONSTANTE_H
