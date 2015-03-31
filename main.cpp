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
#include "Outils.h"
#include "GMenu.h"
#include "Heure.h"
#include "Constante.h"
#include "Generateur.h"

///////////////////////////////////////////////////////////////////  PRIVE
//------------------------------------------------------------- Constantes
#define DROITS 0660
#define REFERENCE "./Carrefour"
#define NBSEM 2 //Nombre de sémaphores
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

int main(void)
{
	//I_1.	Masquer les signaux (SIGINT, SIGUSR2 et SIGCHLD)
	struct sigaction actionIgnore;
	sigemptyset(&actionIgnore.sa_mask);
	actionIgnore.sa_handler = SIG_IGN;
	actionIgnore.sa_flags = 0;
	sigaction(SIGCHLD, &actionIgnore, NULL);
	sigaction(SIGUSR2, &actionIgnore, NULL);
	sigaction(SIGINT, &actionIgnore, NULL);
	
	InitialiserApplication(XTERM);
	pid_t pidHeure=-1;
	pid_t pidMenu=-1;
	pid_t pidGenerateur=-1;
	/*pid_t pidVoie_N=-1;
	pid_t pidVoie_S=-1;
	pid_t pidVoie_E=-1;
	pid_t pidVoie_O-1;
	pid_t pidFeu=-1;*/
	
	//I_2.	Créer Boite aux lettres ArrivéeVéhicules
	key_t cle=ftok(REFERENCE, 1);
	int idBAL=msgget(cle, IPC_CREAT|DROITS);
	
	//I_3.	Créer un tableau de 2 sémaphores
	int idSem=semget( cle, NBSEM, IPC_CREAT|DROITS);
	
	//Initialisation des sémaphores
	semctl(idSem, 0, SETVAL, 1); //Semaphore 0: Semaphore associé à la mémoire partagée GestionTempo
	semctl(idSem, 1, SETVAL, 4); //Semaphore 1: Semaphore associé à la mémoire partagée CouleurFeu (potentiellement 4 lecteurs)
	
	//I_4.	Créer une mémoire partagée
	int idShm=shmget(cle, sizeof(memoirePartagee), IPC_CREAT|DROITS);
	
	//I_5.	Attacher la tache à la mémoire partagée
	memoirePartagee* shm=(memoirePartagee*) shmat (idShm, NULL, 0);
	shm->tempoEO=10;
	shm->tempoNS=20;
	shm->couleurAxeNS=ROUGE;
	shm->couleurAxeEO=ROUGE;
	
	//I_6.	Détacher la mère de la mémoire partagée
	shmdt(shm);
	
	//I_6.	Réactivation des signaux
	struct sigaction actionDefaut;
	sigemptyset(&actionDefaut.sa_mask);
	actionDefaut.sa_handler = SIG_DFL;
	actionDefaut.sa_flags = 0;
	sigaction(SIGCHLD, &actionDefaut, NULL);
	sigaction(SIGUSR2, &actionDefaut, NULL);
	sigaction(SIGINT, &actionDefaut, NULL);

	//I_7.	Créer la tâche Générateur
	pidGenerateur=CreerEtActiverGenerateur(0, idBAL);
	
	if( (pidMenu=fork() )==0)
	{ //I_8.	Créer la tâche Menu
		GMenu();
	}
	else {
		//I_9.	Créer la tâche Heure
		pidHeure=CreerEtActiverHeure();
		
		//I_10.	Créer une instance de tâche Voie avec le paramètre N
		//I_11.	Créer une instance de tâche Voie avec le paramètre S
		//I_12.	Créer une instance de tâche Voie avec le paramètre E
		//I_13.	Créer une instance de tâche Voie avec le paramètre O

		//I_14.	Créer la tâche Feu

		// M_1.	Attendre la réception du signal SIGCHLD de la part de Menu
		while( waitpid(pidMenu, NULL, 0)==-1 && errno==EINTR);

		//D_1.	Envoyer signal SIGUSR2 à la tâche Heure
		kill(pidHeure, SIGUSR2);
		
		//D_2.	Attendre la fin de la tache Heure
		while( waitpid(pidHeure, NULL, 0)==-1 && errno==EINTR);

		//D_3.	Envoyer signal SIGCONT à la tâche Générateur
		kill(pidGenerateur, SIGCONT);
		
		//D_4.	Envoyer signal SIGUSR2 à la tâche Générateur
		kill(pidGenerateur, SIGUSR2);
		
		//D_5.	Attendre la fin de la tache Générateur
		while( waitpid(pidGenerateur, NULL, 0)==-1 && errno==EINTR);
		
		//D_6.	Envoyer signal SIGUSR2 à la tâche Feu
		//kill(pidFeu, SIGUSR2);

		//D_7.	Attendre la fin de la tache Feu
		//while( waitpid(pidFeu, NULL, 0)==-1 && errno==EINTR);

		//D_8.	Détruire la mémoire partagée
		shmctl(idShm, IPC_RMID, 0);

		//D_9.	Détruire le tableau de sémaphores
		semctl(idSem, 0, IPC_RMID, 0);
		
		//D_10.	Détruire la BAL
		msgctl(idBAL, IPC_RMID, 0);

		//D_11.	Autodestruction
		TerminerApplication(true);
		exit(0);
	}
	return 0;
}

