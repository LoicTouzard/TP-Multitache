/*************************************************************************
                           Constante  -  description
                             -------------------
    début                : XXX
    copyright            : (C) XXX par XXX
    e-mail               : XXX
*************************************************************************/

//---------- Interface de la tâche <Constante> (fichier Constante.h) -------
#if ! defined ( CONSTANTE_H )
#define CONSTANTE_H

//------------------------------------------------------------------------
// Rôle de la tâche <Constante>
//
//
//------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////  INCLUDE
//--------------------------------------------------- Interfaces utilisées

//------------------------------------------------------------- Constantes

//------------------------------------------------------------------ Types

//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
// type Nom ( liste de paramètres );
// Mode d'emploi :
//
// Contrat :
//
enum couleurFeu {ROUGE, ORANGE, VERT};

struct memoirePartagee{
	int tempoEO;
	int tempoNS;
	couleurFeu couleurAxeNS;
	couleurFeu couleurAxeEO;
};

#endif // CONSTANTE_H
