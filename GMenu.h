/*************************************************************************
                           GMenu  -  description
                             -------------------
    copyright            : (C) GMenu par Loic Touzard
    e-mail               : loic.touzard@insa-lyon.fr
*************************************************************************/

//---------- Interface de la tâche <GMenu> (fichier GMenu.h) -------
#if ! defined ( GMENU_H )
#define GMENU_H

//------------------------------------------------------------------------
// Rôle de la tâche <GMenu>
//		GMenu récupère et traite les informations donnée par clavier 
//		à travers des methodes Commande
//
//------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////  INCLUDE
//--------------------------------------------------- Interfaces utilisées
#include "Outils.h"
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
// Mode d'emploi :
//		PLusieurs id sont necessaires:
//			L'id des semaphores sur la mémoire partagée
//			L'id de la boite aux lettre dans laquelle il pourra envoyer les 
//			voitures manuellement sur une voie
//			L'id de la mémoire partagée afin d'ecrire le durée des feux
//		Necessite egalement le pid-t du générateur pour l'activer / désactiver
// Contrat :
//		Les IPCs dont les ID sont passé doivent être créés et configurés.
//		Le générateur doit être créé, et désactivé


void Commande(char code);
// Mode d'emploi :
//		Receptionne un code caractère:
//			Code 'Q', terminaison de la tache, appel à sa phase de destruction
//			Code 'G', activation / désactivation du générateur


void Commande(TypeVoie entree, TypeVoie sortie);
// Mode d'emploi :
//		Permet la création manuelle d'une voiture
//		Prend en paramètre la voie d'entree et de sortie de la voiture a créer.


void Commande(TypeVoie voie, unsigned int duree);
// Mode d'emploi :
//		Permet de modifier la durée du temps vert des feux
//		L'axe NS sera changé pour les voies N et S
//		L'axe EO sera changé pour les voies E et O


#endif // GMENU_H




