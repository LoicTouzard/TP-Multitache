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
#include <sys/msg.h>
//------------------------------------------------------ Include personnel
#include "Constantes.h"
#include "GMenu.h"
#include "Outils.h"
#include "Voiture.h"

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
static unsigned int G_numeroManuel;

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
	//détachement de la mémoire partagée (superflue juste avant la fin du processus mais 'plus propre')
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
	
	//affectation du premier numero mioneralogique manuel
	G_numeroManuel = numeroMinManuel;
}

static void P_ecritureDuree()
{
	struct sembuf reserverEcritureDuree = { SEMAPHORE_TEMPO, -1, 0 };
	//on reserve pour l'ecriture d'une durée dans la mémoire partagée
	//potentiellement bloquant en attendant la libération des jetons
	while(semop(G_idSem, &reserverEcritureDuree, 1)==-1 && errno== EINTR);
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
	if(entree != AUCUNE && sortie != AUCUNE)
	{
		//création de la voiture et du message
		struct Voiture v = { entree, sortie, G_numeroManuel };
		struct MsgVoiture msgV = { entree, v };
		
		//potentiel operation bloquante en cas de saturation de la file
		while(msgsnd(G_idBAL, &msgV, TAILLE_MSG_VOITURE,0)==-1 && errno== EINTR);
		
		//incrémentation visuelle du nombre de voiture dans la file correspondante
		OperationVoie(PLUS, entree);
		
		Effacer(NUMERO);
		Afficher(NUMERO, v.numero, GRAS);
		
		Effacer(ENTREE);
		Afficher(ENTREE, v.entree, GRAS);
		
		Effacer(SORTIE);
		Afficher(SORTIE, v.sortie, GRAS);
		
		//incrémentation du numéro minéralogique pour la prochaine voiture manuelle
		(G_numeroManuel%=numeroMaxManuel)++;
	}
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
