/*************************************************************************
                           GMenu  -  description
                             -------------------
    copyright            : (C) GMenu par Loic Touzard
    e-mail               : loic.touzard@insa-lyon.fr
*************************************************************************/

//---------- Réalisation de la tâche <GMenu> (fichier GMenu.cpp) ---

/////////////////////////////////////////////////////////////////  INCLUDE
//-------------------------------------------------------- Include système
#include <signal.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
//------------------------------------------------------ Include personnel
#include "Constantes.h"
#include "GMenu.h"
#include "Menu.h"
#include "Outils.h"
#include "Voiture.h"

///////////////////////////////////////////////////////////////////  PRIVE
//------------------------------------------------------------- Constantes

//------------------------------------------------------------------ Types

//---------------------------------------------------- Variables statiques

//Pid_t du générateur
static pid_t G_GenerateurPid;
static bool G_GenerateurIsOn;

//ID des IPCs
static int G_idSem;
static int G_idBAL;
static int G_idShm;

//Mémoire partagée
static MemoirePartagee* G_shm;
static unsigned int G_numeroManuel;

//------------------------------------------------------ Fonctions privées

static void P_ecritureDuree()
// Mode d'emploi :
//		Permet de réserver l'accès pour l'écriture dans la mémoire partagée
//		La zone réservée concerne exclusivement la durée des Feux
// Algorithme :
//		Effectue un semop de reservation pour une valeur de 1
{
	struct sembuf reserverEcritureDuree = { SEMAPHORE_TEMPO, -1, 0 };
	//On reserve pour l'ecriture d'une durée dans la mémoire partagée
	//Potentiellement bloquant en attendant la libération des jetons
	while(semop(G_idSem, &reserverEcritureDuree, 1)==-1 && errno== EINTR);
}	//-----	fin de P_ecritureDuree


static void V_ecritureDuree()
// Mode d'emploi :
//		Permet de libérer un jeton d'accès en écriture de la mémoire partagée
//		La zone libérée concerne exclusivement la durée des Feux
// Algorithme :
//		Effectue un semop de libération pour une valeur de 1
{
	struct sembuf libererEcritureDuree = { SEMAPHORE_TEMPO, 1, 0 };
	//On libère le sémaphore sur durée dans la mémoire partagée
	semop(G_idSem, &libererEcritureDuree, 1);
}	//-----	fin de V_ecritureDuree


static void destructionMenu()
// Mode d'emploi :
//		Gère la phase de destruction du processus
// Algorithme :
//		Détache la mémoire partagée
//		Quitte le programme
{
	/************************
	 *	Phase DESTRUCTION	*
	 ************************/

	//Détachement de la mémoire partagée (superflue juste avant la fin du processus mais 'plus propre')
	shmdt(G_shm);
	exit(0);
}	//----- fin de destructionMenu


static void initialisationMenu(int idSem,int idBAL,int idShm,pid_t pidGen)
// Mode d'emploi :
//		Gère la phase d'initialisation du processus
// Algorithme :
//		Initialise les variables statiques contenant les ID des IPCS avec ses paramètres
//		Effectue l'attachement à la mémoire partagée
//		Initialise l'état du générateur
//		Initialise les numéros minéralogiques manuels
{
	G_GenerateurPid=pidGen;
	G_GenerateurIsOn = false;

	//Stockage des ID d'IPCS dans les statiques
	G_idBAL = idBAL;
	G_idSem = idSem;
	G_idShm = idShm;
	
	//Attachement à la mémoire partagée
	G_shm=(MemoirePartagee*)shmat(G_idShm, NULL, 0);
	
	//Affectation du premier numero mioneralogique manuel
	G_numeroManuel = numeroMinManuel;
}	//-----	fin de initialisationMenu


//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques

void GMenu(int idSem,int idBAL,int idShm, pid_t pidGen)
// Algorithme :
//		Lance la phase d'initialisation puis la phase moteur de la tâche
{
	/****************************
	 *	Phase INITIALISATION	*
	 ****************************/

	initialisationMenu(idSem, idBAL, idShm, pidGen);
	
	
	/********************
	 *	Phase MOTEUR 	*
	 ********************/

	Menu();	//Bloquant
}	//-----	fin de GMenu

void Commande(char code)
// Algorithme :
//		Récupère un code caractère
//		Code 'Q', on appelle la phase de destruction de la tâche
//		Code 'G', on change l'état du générateur grâce aux signaux
{
	//commande de terminaison
	if(code=='Q')
	{
		destructionMenu();
	}
	//commande de changement d'etat du générateur
	else if(code=='G')
	{
		//Si le generateur est actif, on le désactive
		if(G_GenerateurIsOn)
		{
			kill(G_GenerateurPid, SIGSTOP);
			Afficher(ETAT_GENERATEUR, "OFF", GRAS);
		}
		//S'il n'est pas actif, on active le generateur
		else
		{
			kill(G_GenerateurPid, SIGCONT);
			Afficher(ETAT_GENERATEUR, "ON ", GRAS);
		}

		//On inverse le mode
		G_GenerateurIsOn = !G_GenerateurIsOn;
	}
}	//-----	fin de Commande


void Commande(TypeVoie entree, TypeVoie sortie)
// Algorithme :
//		Récupère une voie d'entrée et une voie de sortie
//		Construit une voiture manuellement en lui affectant 
//			- Les voies passées en paramètre
//			- Un nouveau numéro numéralogique
//		Envoie cette voiture dans la file de message
//		Met a jour les affichages
{
	if(entree != AUCUNE && sortie != AUCUNE)
	{
		//Création de la voiture et du message
		struct Voiture v = { entree, sortie, G_numeroManuel };
		struct MsgVoiture msgV = { entree, v };
		
		//Potentielle operation bloquante en cas de saturation de la file
		while(msgsnd(G_idBAL, &msgV, TAILLE_MSG_VOITURE,0)==-1 && errno== EINTR);
		
		//Incrémentation visuelle du nombre de voiture dans la file correspondante
		OperationVoie(PLUS, entree);
		
		//Mises à jour de l'IHM sur la dernière voiture générée
		Effacer(NUMERO);
		Afficher(NUMERO, v.numero, GRAS);
		
		Effacer(ENTREE);
		Afficher(ENTREE, v.entree, GRAS);
		
		Effacer(SORTIE);
		Afficher(SORTIE, v.sortie, GRAS);
		
		//Incrémentation du numéro minéralogique pour la prochaine voiture manuelle
		(G_numeroManuel%=numeroMaxManuel)++;
	}
}	//-----	fin de Commande


void Commande(TypeVoie voie, unsigned int duree)
// Algorithme :
//		Récupère une voie et une durée
//		Met à jour dans la mémoire partagée la durée verte de la voie correspondante
{
	//Ecriture de la durée récupérée dans la mémoire partagée
	//Redondance du P / V dans les if mais minimisation des operations dans la reservation
	if(voie == EST || voie == OUEST)
	{
		P_ecritureDuree();
		//Contexte d'exlusion mutuelle pour l'ecriture
		G_shm->tempoEO = duree;
		V_ecritureDuree();
	}
	else if(voie == NORD || voie == SUD)
	{
		P_ecritureDuree();
		//Contexte d'exlusion mutuelle pour l'ecriture
		G_shm->tempoNS = duree;
		V_ecritureDuree();
	}
}	//-----	fin de Commande
