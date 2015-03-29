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
	//I_1.	Masquer les signaux (SIUGUSR2, SIGCHLD)
	
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
	
	//I_2.	Créer Boite aux lettres ArrivéeVéhicules
	key_t cleBAL=ftok(REFERENCE, 1);
	int idBAL=msgget(cleBAL, IPC_CREAT|DROITS);
	
	//Il manque un parametre taille pour les mémoires partagées
	
	//I_3.	Créer Mémoire partagée NombreVéhicules
	
	/*key_t cleNbVoitures=ftok(REFERENCE, 1);
	int idShNbVoitures=shmget(cleNbVoitures, IPC_CREAT|DROITS);*/
	
	//I_4.	Créer Mémoire partagée positionVoiture
	
	/*key_t clePositionVoitures=ftok(REFERENCE, 1);
	int idShPositionVoitures=shmget(clePositionVoitures, IPC_CREAT|DROITS);*/
	
	//I_5.	Créer un tableau de 2 sémaphores (élémentaires ou globaux ?)
	//I_6.	Attacher chaque sémaphore aux Mémoires partagées
	//I_7.	Attacher la tâche courante(Mère) aux IPCs
	
	//I_8.	Créer la tâche Générateur
	pidG=CreerEtActiverGenerateur(0, idBAL);
	
	if( (pidC=fork() )==0)
	{ //I_9.	Créer la tâche Menu
		GMenu();
	}
	else {
		//I_10.	Créer la tâche Heure
		pidH=CreerEtActiverHeure();
		
		//I_11.	Créer une instance de tâche Voie avec le paramètre
		//I_12.	Créer une instance de tâche Voie avec le paramètre
		//I_13.	Créer une instance de tâche Voie avec le paramètre
		//I_14.	Créer une instance de tâche Voie avec le paramètre

		//I_15.	Créer la tâche Feu

		// M_1.	Attendre la réception du signal SIGCHLD de la part de Menu
		while( waitpid(pidC, NULL, 0)==-1 && errno==EINTR);

		//D_1.	Envoyer signal SIGUSR2 à la tâche Heure
		kill(pidH, SIGUSR2);
		
		//D_2.	Attendre la fin de la tache Heure
		while( waitpid(pidH, NULL, 0)==-1 && errno==EINTR);

		//D_3.	Envoyer signal SIGCONT à la tâche Générateur
		kill(pidG, SIGCONT);
		
		//D_4.	Envoyer signal SIGUSR2 à la tâche Générateur
		kill(pidG, SIGUSR2);
		
		//D_5.	Attendre la fin de la tache Générateur
		while( waitpid(pidG, NULL, 0)==-1 && errno==EINTR);
		
		//D_6.	Envoyer signal SIGUSR2 à la tâche Feu
		//D_7.	Attendre la fin de la tache Feu
		
		//D_8.	Détruire le tableau de sémaphores
				
		//D_9.	Détacher les IPCs de la tâche courante(Mère)
				
		//D_10.	Détruire les IPCs
		msgctl(idBAL, IPC_RMID, 0);
		
		
		TerminerApplication(true);
		
		////D_11.	Autodestruction
		exit(0);
	}
	return 0;
}

