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
#include "Outils.h"
#include "GMenu.h"
#include "Heure.h"
#include "Generateur.h"

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
	// I 0_1: Masquer les signaux SIGUSR2, SIGINT et SIGCHLD
	
	/*struct sigaction action;
	action.sa_handler = masquerSignaux;
	sigemptyset(&action.sa_mask);
	action.sa_flags=0;
	sigaction(SIGCHLD, &action, NULL);
	sigaction(SIGUSR2, &action, NULL);
	sigaction(SIGINT, &action, NULL);*/

	InitialiserApplication(XTERM);
	pid_t pidH=-1;
	pid_t pidC=-1;
	pid_t pidG=-1;
	
	// I 1_1: Création de la boite aux lettres
	key_t cleBAL=ftok(REFERENCE, 1);
	int idBAL=msgget(cleBAL, IPC_CREAT|DROITS);
	
	//Il manque un parametre taille pour les mémoires partagées
	
	// I 1_2: Création de la mémoire partagée nbVoitures
	
	/*key_t cleNbVoitures=ftok(REFERENCE, 1);
	int idShNbVoitures=shmget(cleNbVoitures, IPC_CREAT|DROITS);*/
	
	// I 1_3: Création de la mémoire partagée positionVoitures
	
	/*key_t clePositionVoitures=ftok(REFERENCE, 1);
	int idShPositionVoitures=shmget(clePositionVoitures, IPC_CREAT|DROITS);*/
	
	// I 1_4: Création de la mémoire partagée GestionTempo
	
	/*key_t cleGestionTempo=ftok(REFERENCE, 1);
	int idShGestionTempo=shmget(cleGestionTempo, IPC_CREAT|DROITS);*/
	
	//Faire un tableau de sémaphore
	
	// I 1_5: Création sémaphore associé à la mémoire partagée nbVoitures
	// I 1_6: Création sémaphore associé à la mémoire partagée positionVoiture
	// I 1_7: Création sémaphore associé à la mémoire partagée GestionTempo
	
	// I 2_1: Création du Générateur
	pidG=CreerEtActiverGenerateur(0, idBAL);
	
	if( (pidC=fork() )==0)
	{ // I 2_2: Création de la tache Menu
		GMenu();
	}
	else {
		// I 2_3: Création de la tache Heure
		pidH=CreerEtActiverHeure();
		
		// I 2_4: Création de la tache Voie (x4)
		
		// I 2_5: Création de la tache Feu

		// M 1: Attendre reception du signal SIGCHLD depuis la tache Menu
		while( waitpid(pidC, NULL, 0)==-1 && errno==EINTR);

		// D 1_1: Envoyer signal de fin SIGUSR2 à la tache Heure
		kill(pidH, SIGUSR2);
		
		// D 1_2: On attend le retour du signal de fin de la tache Heure
		while( waitpid(pidH, NULL, 0)==-1 && errno==EINTR);

		// D 1_3: Envoyer signal SIGCONT à la tache Générateur
		kill(pidG, SIGCONT);
		
		// D 1_4: Envoyer signal de fin SIGUSR2 à la tache Générateur
		kill(pidG, SIGUSR2);
		
		// D 1_5: On attend le retour du signal de fin de la tache Générateur
		while( waitpid(pidG, NULL, 0)==-1 && errno==EINTR);
		
		// D 2_1: Détruire le tableau de sémpahores
				
		// D 2_2: Destruction de la mémoire partagée GestionTempo
				
		// D 2_3: Destruction de la mémoire partagée positionVoitures
		
		// D 2_4: Destruction de la mémoire partagée nbVoitures
		
		// D 2_5: Destruction de la boite aux lettres
		
		msgctl(idBAL, IPC_RMID, 0);
		TerminerApplication(true);
		
		// D 3: Autodestruction
		exit(0);
	}
	return 0;
}

