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

//Semaphore 0 : Pour la gestion des temporisations
struct sembuf reserverTempo={SEMAPHORE_TEMPO,-1,0};
struct sembuf deposerTempo={SEMAPHORE_TEMPO,1,0};
	
//SEMAPHORE 1 : Pour la couleur des feux
struct sembuf reserverCouleur={SEMAPHORE_COULEUR,-1,0};
struct sembuf deposerCouleur={SEMAPHORE_COULEUR,1,0};

//---------------------------------------------------- Variables statiques

//------------------------------------------------------ Fonctions privées

//////////////////////////////////////////////////////////////////  PRIVE
//---------------------------------------------------- Fonctions privées
static void actionFinTache(int noSignal){
	if(noSignal==SIGUSR2){
		// D_1	Autodestruction
		//Détachement de la shm implicite
		exit(0);
	}
}
//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques

void Decomptetempo(unsigned int temps, unsigned int &tempoNS,unsigned int &tempoEO){
	unsigned int i;
	for(i=0; i<temps; i++){
		Effacer(TEMPS_AXE_NS);
		Afficher(TEMPS_AXE_NS, tempoNS);
		Effacer(TEMPS_AXE_EO);
		Afficher(TEMPS_AXE_EO, tempoEO);
		tempoNS--;
		tempoEO--;
		sleep(1);
	}
}

void EcritureShmAxeNS(int idSem, MemoirePartagee* shm, CouleurFeu couleurDuFeu){
	//Prendre un jeton
	semop(idSem, &reserverCouleur, 1);
	//Ecrire dans la mémoire partagée (MAJ des couleurs)
	shm->couleurAxeNS=couleurDuFeu;
	//Déposer le jeton
	semop(idSem, &deposerCouleur, 1);
}

void EcritureShmAxeEO(int idSem, MemoirePartagee* shm, CouleurFeu couleurDuFeu){
	//Prendre un jeton
	semop(idSem, &reserverCouleur, 1);
	//Ecrire dans la mémoire partagée (MAJ des couleurs)
	shm->couleurAxeEO=couleurDuFeu;
	//Déposer le jeton
	semop(idSem, &deposerCouleur, 1);
}

void MAJTempoShm(int idSem, MemoirePartagee* shm, unsigned int &dureeNS,unsigned int &dureeEO){
	//Prendre un jeton
	semop(idSem, &reserverTempo, 1);
	//Lire les valeurs de temporisation des feux dans la mémoire partagée (potentielles MAJ)
	dureeNS=shm->tempoNS;
	dureeEO=shm->tempoEO;
	//Déposer le jeton
	semop(idSem, &deposerTempo, 1);
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
	
	//I_2.	Créer handler finTache
	struct sigaction finTache;
	sigemptyset(&finTache.sa_mask);
	finTache.sa_handler = actionFinTache;
	finTache.sa_flags = 0;

	//I_3.	Armer SIGUSR2 sur finTache
	sigaction(SIGUSR2, &finTache, NULL);

	//I_4.	Attacher la tache à la mémoire partagée
	MemoirePartagee* shm=(MemoirePartagee*) shmat (shmId, NULL, 0);

	unsigned int dureeNS;
	unsigned int dureeEO;
	
	//I_5.On récupère les durees de chaque Axe dans la mémoire partagée
	MAJTempoShm(idSem, shm, dureeNS, dureeEO);

	unsigned int tempoNS=dureeNS;
	unsigned int tempoEO=dureeNS+TEMPO_ORANGE+TEMPO_ROUGE;
	
	//I_6.	Mettre à blanc les zones où l'on s'apprete à Ecrire
	Effacer(TEMPS_AXE_NS);
	Effacer(TEMPS_AXE_EO);
	
	for(;;){

		//Afficher les temps pour chaque feu dans la zone Fonctionnement
		Afficher(DUREE_AXE_NS, dureeNS);
		Afficher(DUREE_AXE_EO, dureeEO);
		
		//Passer la couleur de l'axe dans l'enum
		EcritureShmAxeNS(idSem, shm, VERT);

		//Mettre à jour les couleurs sur l'écran
		Effacer(COULEUR_AXE_NS);
		Afficher(COULEUR_AXE_NS,"Vert", GRAS, INVERSE);

		//Patienter pendant dureeNS secondes
		Decomptetempo(dureeNS,tempoNS, tempoEO);
		
		//Passer la couleur de l'axe dans l'enum
		EcritureShmAxeNS(idSem, shm, ORANGE);
		
		//Mettre à jour les couleurs sur l'écran
		Effacer(COULEUR_AXE_NS);
		Afficher(COULEUR_AXE_NS, "Orange", GRAS, INVERSE);
		
		tempoNS=TEMPO_ORANGE;
		
		//Patienter pendant TEMPO_ORANGE secondes
		Decomptetempo(TEMPO_ORANGE,tempoNS, tempoEO);

		//Passer la couleur de l'axe dans l'enum
		EcritureShmAxeNS(idSem, shm, ROUGE);

		tempoNS=TEMPO_ROUGE;
		
		//Mettre à jour les couleurs sur l'écran
		Effacer(COULEUR_AXE_NS);
		Afficher(COULEUR_AXE_NS, "Rouge", GRAS, INVERSE);
		
		//Patienter pendant TEMPO_ROUGE secondes
		Decomptetempo(TEMPO_ROUGE,tempoNS, tempoEO);

		//On récupère à nouveau les durees de chaque Axe dans la mémoire partagée
		MAJTempoShm(idSem, shm, dureeNS, dureeEO);

		Afficher(DUREE_AXE_NS, dureeNS);
		Afficher(DUREE_AXE_EO, dureeEO);

		//Passer la couleur de l'axe dans l'enum
		EcritureShmAxeEO(idSem, shm, VERT);

		//Mettre à jour les couleurs sur l'écran
		Effacer(COULEUR_AXE_EO);
		Afficher(COULEUR_AXE_EO, "Vert", GRAS, INVERSE);
		
		tempoNS=dureeEO+TEMPO_ORANGE+TEMPO_ROUGE;
		tempoEO=dureeEO;

		//Patienter pendant dureeEO secondes
		Decomptetempo(dureeEO,tempoNS, tempoEO);
		
		//Passer la couleur de l'axe dans l'enum
		EcritureShmAxeEO(idSem, shm, ORANGE);
		
		//Mettre à jour les couleurs sur l'écran
		Effacer(COULEUR_AXE_EO);
		Afficher(COULEUR_AXE_EO, "Orange", GRAS, INVERSE);
		
		tempoEO=TEMPO_ORANGE;

		//Patienter pendant TEMPO_ORANGE secondes
		Decomptetempo(TEMPO_ORANGE,tempoNS, tempoEO);

		//Passer la couleur de l'axe dans l'enum
		EcritureShmAxeEO(idSem, shm, ROUGE);

		tempoEO=TEMPO_ROUGE;
		
		//Mettre à jour les couleurs sur l'écran
		Effacer(COULEUR_AXE_EO);
		Afficher(COULEUR_AXE_EO, "Rouge", GRAS, INVERSE);
		
		//Patienter pendant TEMPO_ROUGE secondes
		Decomptetempo(TEMPO_ROUGE,tempoNS, tempoEO);
		
		//On récupère à nouveau les durees de chaque Axe dans la mémoire partagée
		MAJTempoShm(idSem, shm, dureeNS, dureeEO);

		tempoEO=dureeNS+TEMPO_ORANGE+TEMPO_ROUGE;
		tempoNS=dureeNS;
		
	}
}

