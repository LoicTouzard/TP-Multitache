/*************************************************************************
                           XXX  -  description
                             -------------------
    début                : XXX
    copyright            : (C) XXX par XXX
    e-mail               : XXX
*************************************************************************/

//---------- Réalisation de la tâche <XXX> (fichier XXX.cpp) ---

/////////////////////////////////////////////////////////////////  INCLUDE
//-------------------------------------------------------- Include système
#include <iostream>
#include <signal.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/sem.h>
//------------------------------------------------------ Include personnel
#include "GMenu.h"
#include "Outils.h"
#include "Constantes.h"


///////////////////////////////////////////////////////////////////  PRIVE
//------------------------------------------------------------- Constantes

//------------------------------------------------------------------ Types

//---------------------------------------------------- Variables statiques

static pid_t G_GenerateurPid;
static bool G_GenerateurIsOn;

static int G_idSem;
static int G_idBAL;
static int G_idShm;

static memoirePartagee* G_shm;

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

//destruction du processus
static void destructionMenu()
{
	//détachement de la mémoire partagée
	shmdt(G_shm);
	exit(0);
}

//initialisation du processus
static void initialisationMenu(int idSem,int idBAL,int idShm,pid_t pidGen)
{
	G_GenerateurPid=pidGen;
	G_GenerateurIsOn = false;
	G_idBAL = idBAL;
	G_idSem = idSem;
	G_idShm = idShm;
	
	//récupération de la mémoire partagée
	G_shm=(memoirePartagee*)shmat(G_idShm, NULL, 0);
}

static void P_ecritureDuree()
{
	struct sembuf reserverEcritureDuree = { SEMAPHORE_TEMPO, -1, 0 };
	//on reserve pour l'ecriture d'une durée dans la mémoire partagée
	semop(G_idSem, &reserverEcritureDuree, 1);
}

static void V_ecritureDuree()
{
	struct sembuf libererEcritureDuree = { SEMAPHORE_TEMPO, 1, 0 };
	//on libère le sémaphore sur durée dans la mémoire partagée
	semop(G_idSem, &libererEcritureDuree, 1);
}

//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
//type Nom ( liste de paramètres )
// Algorithme :
//
//{
//} //----- fin de Nom

void GMenu(int idSem,int idBAL,int idShm,pid_t pidGen)
{
	initialisationMenu(idSem, idBAL, idShm, pidGen);

	
	//bloquant
	Menu();
}

void Commande(char code)
{
	//commande de terminaison
	if(code=='Q')
	{
		destructionMenu();
	}
	//commande de changement d'etat du générateur
	else if(code=='G')
	{
		//si le generateur est actif, on le desactive
		if(G_GenerateurIsOn)
		{
			kill(G_GenerateurPid, SIGSTOP);
			Afficher(ETAT_GENERATEUR, "OFF", GRAS);
		}
		//s'il n'est pas actif, on active le generateur
		else
		{
			kill(G_GenerateurPid, SIGCONT);
			Afficher(ETAT_GENERATEUR, "ON ", GRAS);
		}
		//on change le mode
		G_GenerateurIsOn = !G_GenerateurIsOn;
	}
}


void Commande(TypeVoie entree, TypeVoie sortie)
{
	
}


//commande ayant pour but de changer la temporisation d'un feu
void Commande(TypeVoie voie, unsigned int duree)
{
	//ecriture de la durée récupérée dans la mémoire partagée
	if(voie == EST || voie == OUEST)
	{
		P_ecritureDuree();
		//contexte d'exlusion mutuelle pour l'ecriture
		G_shm->tempoEO = duree;
		V_ecritureDuree();
	}
	else if(voie == NORD || voie == SUD)
	{
		P_ecritureDuree();
		//contexte d'exlusion mutuelle pour l'ecriture
		G_shm->tempoNS = duree;
		V_ecritureDuree();
	}
}





