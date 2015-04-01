/*************************************************************************
                           XXX  -  description
                             -------------------
    début                : XXX
    copyright            : (C) XXX par XXX
    e-mail               : XXX
*************************************************************************/

//---------- Réalisation de la tâche <Voie> (fichier Voie.cpp) ---

/////////////////////////////////////////////////////////////////  INCLUDE
//-------------------------------------------------------- Include système

#include <signal.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <errno.h>
#include <set>
//------------------------------------------------------ Include personnel
#include "Voie.h"
#include "Constantes.h"


///////////////////////////////////////////////////////////////////  PRIVE
//------------------------------------------------------------- Constantes

//------------------------------------------------------------------ Types

//---------------------------------------------------- Variables statiques
static int G_idSem;
static int G_idBAL;
static int G_idShm;

static set<pid_t> G_voitures;
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

static void P_lectureCouleur()
{
	struct sembuf reserverLectureCouleur = { SEMAPHORE_COULEUR, -1, 0 };
	//on reserve pour l'ecriture d'une durée dans la mémoire partagée
	//potentiellement bloquant en attendant la libération des jetons
	while(semop(G_idSem, &reserverLectureCouleur, 1)==-1 && errno== EINTR);
}

static void V_lectureCouleur()
{
	struct sembuf libererLectureCouleur = { SEMAPHORE_COULEUR, 1, 0 };
	//on libère le sémaphore sur durée dans la mémoire partagée
	semop(G_idSem, &libererLectureCouleur, 1);
}

static void handlerSIGCHLD(int numSignal)
{
	//réception de la mort d'un fils
}

static void handlerSIGUSR2(int numSignal)
{
	//réception demande de fin de la tâche
	exit(0);
}

static void initialisationVoie()
{
	//armement des signaux ignorés par la mère
	//SIGCHILD
	struct sigaction actionSIGCHLD;
	sigemptyset(&actionSIGCHLD.sa_mask);
	actionSIGCHLD.sa_handler = handlerSIGCHLD;
	actionSIGCHLD.sa_flags = 0;
	sigaction(SIGCHLD, &actionSIGCHLD, NULL);
	
	//SIGUSR2
	struct sigaction actionSIGUSR2;
	sigemptyset(&actionSIGUSR2.sa_mask);
	actionSIGUSR2.sa_handler = handlerSIGUSR2;
	actionSIGUSR2.sa_flags = 0;
	sigaction(SIGUSR2, &actionSIGUSR2, NULL);
	
}


//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
//type Nom ( liste de paramètres )
// Algorithme :
//
//{
//} //----- fin de Nom

void Voie(TypeVoie voie, int idSem,int idBAL,int idShm)
{
	/*
	 *	Phase INITIALISATION 
	 */
	G_idSem = idSem;
	G_idBAL = idBAL;
	G_idShm = idShm;
	const TypeVoie C_voie = voie;
	initialisationVoie();
	
	
	/*
	 *	Phase MOTEUR 
	 */
}

