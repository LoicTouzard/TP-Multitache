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
#include <sys/sem.h>
#include <sys/shm.h>
#include <errno.h>
//------------------------------------------------------ Include personnel
#include "Constantes.h"
#include "Outils.h"
#include "Feu.h"

///////////////////////////////////////////////////////////////////  PRIVE
//------------------------------------------------------------- Constantes
//------------------------------------------------------------- Constantes
#define TEMPO_ORANGE 3
#define TEMPO_ROUGE 2
//------------------------------------------------------------------ Types

//---------------------------------------------------- Variables statiques

//------------------------------------------------------ Fonctions privées

//////////////////////////////////////////////////////////////////  PRIVE
//---------------------------------------------------- Fonctions privées
static void actionFinTache(int noSignal){
	if(noSignal==SIGUSR2){
		// D_1.	Se détacher de la mémoire partagée
		//shmdt(shm);
		
		// D_2	Autodestruction
		exit(0);
	}
}
//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques


void Decomptetempo(unsigned int temps, unsigned int &tempoNS,unsigned int &tempoEO){
	int i;
	for(i=0; i<temps; i++){
		Afficher(TEMPS_AXE_NS, tempoNS);
		Afficher(TEMPS_AXE_EO, tempoEO);
		tempoNS--;
		tempoEO--;
		sleep(1);
	}
}

void Feu(int idSem, int shmId)
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
	sigemptyset(&finTache.sa_mask);
	finTache.sa_handler = actionFinTache;
	finTache.sa_flags = 0;

	//I_3.	Armer SIGUSR2 sur finTache
	sigaction(SIGUSR2, &finTache, NULL);

	//I_4.	S’attacher à la mémoire partagée GestionTempo
	shmat(shmId, NULL, SHM_RDONLY);
	
	//Semaphore 0 : Pour la gestion des temporisations
	struct sembuf reserverTempo={SEMAPHORE_TEMPO,-1,0};
	struct sembuf deposerTempo={SEMAPHORE_TEMPO,1,0};
	
	//SEMAPHORE 1 : Pour la couleur des feux
	struct sembuf reserverCouleur={SEMAPHORE_COULEUR,-1,0};
	struct sembuf deposerCouleur={SEMAPHORE_COULEUR,1,0};
	
	//I_5.	Attacher la tache à la mémoire partagée
	memoirePartagee* shm=(memoirePartagee*) shmat (shmId, NULL, 0);
	
	
	//Prendre un jeton
	semop(idSem, &reserverTempo, 1);
	//Lire les valeurs de temporisation des feux dans la mémoire partagée (potentielles MAJ)
	unsigned int dureeNS=shm->tempoNS;
	unsigned int dureeEO=shm->tempoEO;
	//Déposer le jeton
	semop(idSem, &deposerTempo, 1);
	unsigned int tempoNS=dureeNS;
	unsigned int tempoEO=dureeNS+TEMPO_ORANGE+TEMPO_ROUGE;
	
	while(true){

		//Afficher les temps pour chaque feu dans la zone Fonctionnement
		Afficher(DUREE_AXE_NS, dureeNS);
		Afficher(DUREE_AXE_EO, dureeEO);

		
		//Prendre un jeton
		semop(idSem, &reserverCouleur, 1);
		//Ecrire dans la mémoire partagée (MAJ des couleurs)
		shm->couleurAxeNS=VERT;
		//Déposer le jeton
		semop(idSem, &deposerCouleur, 1);
		
	
		
		//Mettre à jour les couleurs
		Afficher(COULEUR_AXE_NS, "Vert");


		//Patienter pendant dureeNS secondes
		Decomptetempo(dureeNS,tempoNS, tempoEO);
		
		//Prendre un jeton
		semop(idSem, &reserverCouleur, 1);
		//Ecrire dans la mémoire partagée (MAJ des couleurs)
		shm->couleurAxeNS=ORANGE;
		//Déposer le jeton
		semop(idSem, &deposerCouleur, 1);
		
		//Mettre à jour les couleurs
		Afficher(COULEUR_AXE_NS, "Orange");
		
		
		
		
		//OK
		tempoNS=TEMPO_ORANGE;
		
		
		
		
		
		//Patienter pendant TEMPO_ORANGE secondes
		Decomptetempo(TEMPO_ORANGE,tempoNS, tempoEO);

		//Prendre un jeton
		semop(idSem, &reserverCouleur, 1);
		//Ecrire dans la mémoire partagée (MAJ des couleurs)
		shm->couleurAxeNS=ROUGE;
		//Déposer le jeton
		semop(idSem, &deposerCouleur, 1);
		

		tempoNS=TEMPO_ROUGE;

		Afficher(COULEUR_AXE_NS, "Rouge");

		//Patienter pendant TEMPO_ROUGE secondes
		Decomptetempo(TEMPO_ROUGE,tempoNS, tempoEO);

		//Prendre un jeton
		semop(idSem, &reserverTempo, 1);
		//Lire les valeurs de temporisation des feux dans la mémoire partagée (potentielles MAJ)
		dureeNS=shm->tempoNS;
		dureeEO=shm->tempoEO;
		//Déposer le jeton
		semop(idSem, &deposerTempo, 1);

		
		Afficher(DUREE_AXE_NS, dureeNS);
		Afficher(DUREE_AXE_EO, dureeEO);
		

		//Prendre un jeton
		semop(idSem, &reserverCouleur, 1);
		//Ecrire dans la mémoire partagée
		shm->couleurAxeEO=VERT;
		//Déposer le jeton
		semop(idSem, &deposerCouleur, 1);
		
		//Mettre à jour les couleurs
		Afficher(COULEUR_AXE_EO, "Vert");
		
		tempoNS=dureeEO+TEMPO_ORANGE+TEMPO_ROUGE;
		tempoEO=dureeEO;

		//Patienter pendant dureeEO secondes
		Decomptetempo(dureeEO,tempoNS, tempoEO);
		
		
		//Prendre un jeton
		semop(idSem, &reserverCouleur, 1);
		//Ecrire dans la mémoire partagée (MAJ des couleurs)
		shm->couleurAxeEO=ORANGE;
		//Déposer le jeton
		semop(idSem, &deposerCouleur, 1);

		Afficher(COULEUR_AXE_EO, "Orange");
		
		tempoEO=TEMPO_ORANGE;

	
	
		//Patienter pendant TEMPO_ORANGE secondes
		Decomptetempo(TEMPO_ORANGE,tempoNS, tempoEO);
		
		//Prendre un jeton
		semop(idSem, &reserverCouleur, 1);
		//Ecrire dans la mémoire partagée (MAJ des couleurs)
		shm->couleurAxeEO=ROUGE;
		//Déposer le jeton
		semop(idSem, &deposerCouleur, 1);



		tempoEO=TEMPO_ROUGE;
		

		
		Afficher(COULEUR_AXE_EO, "Rouge");
		
		//Patienter pendant TEMPO_ROUGE secondes
		Decomptetempo(TEMPO_ROUGE,tempoNS, tempoEO);
		
		//Prendre un jeton
		semop(idSem, &reserverTempo, 1);
		//Lire les valeurs de temporisation des feux dans la mémoire partagée (potentielles MAJ)
		dureeNS=shm->tempoNS;
		dureeEO=shm->tempoEO;
		//Déposer le jeton
		semop(idSem, &deposerTempo, 1);
		
		
		tempoEO=dureeNS+TEMPO_ORANGE+TEMPO_ROUGE;
		tempoNS=dureeNS;

		
	}
}

