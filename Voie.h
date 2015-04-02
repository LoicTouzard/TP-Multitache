/*************************************************************************
                           Voie  -  description
                             -------------------
    copyright            : (C) Voie par Loic Touzard
    e-mail               : loic.touzard@insa-lyon.fr
*************************************************************************/

//---------- Interface de la tâche <Voie> (fichier Voie.h) -------
#if ! defined ( VOIE_H )
#define VOIE_H
//------------------------------------------------------------------------
// Rôle de la tâche <Voie>
//	Voie récupère les voitures générées et lance leur déplacements sur l'IHM
//
//------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////  INCLUDE
//--------------------------------------------------- Interfaces utilisées

#include "Outils.h"

//------------------------------------------------------------- Constantes

//------------------------------------------------------------------ Types

//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques

void Voie(TypeVoie voie, int idSem,int idBAL,int idShm);
// Mode d'emploi :
//		Prend comme premier paramètre la voie ou se positionne la tâche
//		Necessite également l'id des semaphores sur la mémoire partagée
//		L'id de la mémoire partagée afin de lire l'état des feux
//		L'id de la boite aux lettre dans laquelle il pourra récupérer les voiture en attente sur sa voie
//		La tâche Voie se termine correctement à la réception du signal SIGUSR2

#endif // VOIE_H

