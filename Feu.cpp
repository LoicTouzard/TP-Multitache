/*************************************************************************
                           XXX  -  description
                             -------------------
    début                : XXX
    copyright            : (C) XXX par XXX
    e-mail               : XXX
*************************************************************************/

//---------- Réalisation du module <XXX> (fichier XXX.cpp) -----

/////////////////////////////////////////////////////////////////  INCLUDE
//-------------------------------------------------------- Include système
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <errno.h>
//------------------------------------------------------ Include personnel

///////////////////////////////////////////////////////////////////  PRIVE
//------------------------------------------------------------- Constantes
#define DROITS 0660
#define REFERENCE "./Carrefour"
//------------------------------------------------------------------ Types

//---------------------------------------------------- Variables statiques

//------------------------------------------------------ Fonctions privées
//static type nom ( liste de paramètres )
// Mode d'emploi :
//
// Contrat :
//
// Algorithme :
//
//{
//} //----- fin de nom

//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
//type Nom ( liste de paramètres )
// Algorithme :
//
//{
//} //----- fin de Nom
static void masquerSignaux(int noSignal)
{
	//QUE FAIRE ?
}
int main(void)
{
	//I_1.	Masquer les signaux
	
	//I_2.	Créer handler finTache

	//I_3.	Armer SIGUSR2 sur finTache
	
	//I_4.	S’attacher à la mémoire partagée GestionTempo
	
	//M_1.	Afficher les temps sur l’écran (section Fonctionnement)
	
	//M_2.	Ecrire dans la mémoire partagée GestionTempo (MAJ Enum)
	
	// M_3.	Dormir TEMPO_VERT1 secondes
		//a.	Afficher le temps restant à l’écran
	// M_4.	Ecrire dans la mémoire partagée GestionTempo (MAJ Enum)
	// M_5.	Dormir TEMPO_ORANGE secondes
		//a.	Afficher le temps restant à l’écran
	// M_6.	Ecrire dans la mémoire partagée GestionTempo (MAJ Enum)
	// M_7.	Dormir TEMPO_ROUGE secondes
		//a.	Afficher le temps restant à l’écran
	// M_8.	Ecrire dans la mémoire partagée GestionTempo (MAJ Enum)
	// M_9.	Dormir TEMPO_VERT2secondes
		//a.	Afficher le temps restant à l’écran
	// M_10.	Ecrire dans la mémoire partagée GestionTempo (MAJ Enum)
	// M_11.	Dormir TEMPO_ORANGE secondes
		//a.	Afficher le temps restant à l’écran
	// M_12.	Ecrire dans la mémoire partagée GestionTempo (MAJ Enum)
	// M_13.	Dormir TEMPO_ROUGE secondes
		//a.	Afficher le temps restant à l’écran

		
	// D_1.	Se détacher de la mémoire partagée GestionTempo
	// D_2.	Autodestruction
	exit(0);
}

