/*************************************************************************
                           GMenu  -  description
                             -------------------
    début                : XXX
    copyright            : (C) XXX par XXX
    e-mail               : XXX
*************************************************************************/

//---------- Interface de la tâche <GMenu> (fichier GMenu.h) -------
#if ! defined ( GMENU_H )
#define GMENU_H

//------------------------------------------------------------------------
// Rôle de la tâche <GMenu>
//
//
//------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////  INCLUDE
//--------------------------------------------------- Interfaces utilisées
#include "Outils.h"
#include "Menu.h"
//------------------------------------------------------------- Constantes

//------------------------------------------------------------------ Types

//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
// type Nom ( liste de paramètres );
// Mode d'emploi :
//
// Contrat :
//

void GMenu(int idSem,int idBAL,int idShm,pid_t pidGenerateur);
void Commande(char code);
void Commande(TypeVoie entree, TypeVoie sortie);
void Commande(TypeVoie voie, unsigned int duree);


#endif // GMENU_H




