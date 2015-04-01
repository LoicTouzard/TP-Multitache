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

enum couleurFeu {ROUGE, ORANGE, VERT};



struct memoirePartagee{
	unsigned int tempoEO;
	unsigned int tempoNS;
	couleurFeu couleurAxeNS;
	couleurFeu couleurAxeEO;
};

#endif // CONSTANTE_H
