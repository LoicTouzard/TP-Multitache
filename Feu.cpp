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

static void actionFinTache(int noSignal){
	if(noSignal==SIGUSR2){
		exit(0);
	}
}

int main(void)
{
	//I_1.	Démasquer les signaux
	struct sigaction actionDefaut;
	sigemptyset(&actionDefaut.sa_mask);
	actionDefaut.sa_handler = SIG_DFL;
	actionDefaut.sa_flags = 0;
	sigaction(SIGCHLD, &actionDefaut, NULL);
	sigaction(SIGUSR2, &actionDefaut, NULL);
	sigaction(SIGINT, &actionDefaut, NULL);
	
	//I_2.	Créer handler finTache
	struct sigaction finTache;
	sigemptyset(&actionIgnore.sa_mask);
	finTache.sa_handler = actionFinTache();
	finTache.sa_flags = 0;

	//I_3.	Armer SIGUSR2 sur finTache
	sigaction(SIGUSR2, &finTache, NULL);

	//I_4.	S’attacher à la mémoire partagée GestionTempo
	shmat(shmId, const void *zone, SHM_RDONLY);
	
	//M_1.	Afficher les temps sur l’écran (section Fonctionnement)
	
	
	//M_2.	Ecrire dans la mémoire partagée GestionTempo (MAJ Enum)

	int i=0;
	while(i<shm.tempoNS){
		sleep(1);
		i++;
	}


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
	shmdt(const void *zone);
	
	// D_2.	Autodestruction
	exit(0);
}

