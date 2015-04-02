/*************************************************************************
                           Voie.cpp  -  description
                             -------------------
    copyright            : (C) Voie par Loic Touzard
    e-mail               : loic.touzard@insa-lyon.fr
*************************************************************************/

//---------- Réalisation de la tâche <Voie> (fichier Voie.cpp) ---

/////////////////////////////////////////////////////////////////  INCLUDE
//-------------------------------------------------------- Include système

#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <errno.h>
#include <set>

//------------------------------------------------------ Include personnel

#include "Voie.h"
#include "Constantes.h"
#include "Voiture.h"


///////////////////////////////////////////////////////////////////  PRIVE
//------------------------------------------------------------- Constantes

//------------------------------------------------------------------ Types

//---------------------------------------------------- Variables statiques

//ID des IPCS
static int G_idSem;
static int G_idBAL;
static int G_idShm;

//Mémoire partagée
static MemoirePartagee* G_shm;

//Conteneur de voitures actuellement déssinées
static std::set<pid_t> G_voitures;

//------------------------------------------------------ Fonctions privées

static void P_lectureCouleur()
// Mode d'emploi :
//		Permet de réserver l'accès pour la lecture dans la mémoire partagée
//		La zone réservée concerne exclusivement la couleur des Feu
// Algorithme :
//		Effectue un semop de reservation pour une valeur de 1
{
	struct sembuf reserverLectureCouleur = { SEMAPHORE_COULEUR, -1, 0 };
	//On reserve pour l'ecriture d'une durée dans la mémoire partagée
	//Potentiellement bloquant en attendant la libération des jetons
	while(semop(G_idSem, &reserverLectureCouleur, 1)==-1 && errno== EINTR);
}	//----- fin de P_lectureCouleur


static void V_lectureCouleur()
// Mode d'emploi :
//		Permet de libérer un jeton d'accès en lecture de la mémoire partagée
//		La zone libérée concerne exclusivement la couleur des Feu
// Algorithme :
//		Effectue un semop de libération pour une valeur de 1
{
	struct sembuf libererLectureCouleur = { SEMAPHORE_COULEUR, 1, 0 };
	//On libère le sémaphore sur durée dans la mémoire partagée
	semop(G_idSem, &libererLectureCouleur, 1);
}	//----- fin de V_lectureCouleur


static void destructionVoie()
// Mode d'emploi :
//		Gère la phase de destruction du processus
// Algorithme :
//		Tue tous ses fils, processus Voitures encore à l'écran
//		Détache la mémoire partagée
//		Quitte le programme
{
	//On masque le signal SIGCHILD pour ne pas être géné par le handlerSIGCHLD lors de la destruction manuelle des fils
	struct sigaction actionSIGCHLD;
	sigemptyset(&actionSIGCHLD.sa_mask);
	actionSIGCHLD.sa_handler = SIG_IGN;
	actionSIGCHLD.sa_flags = 0;
	sigaction(SIGCHLD, &actionSIGCHLD, NULL);
	
	//Destructions de tous les fils
	for (std::set<pid_t>::iterator it=G_voitures.begin(); it!=G_voitures.end(); ++it)
	{
		kill(*it, SIGUSR2);
		while( waitpid(*it, NULL, 0)==-1 && errno==EINTR );
	}
	G_voitures.clear();
	
	//Détachement de la mémoire partagée
	//Superflue juste avant la fin du processus car le detachement serait automatique, mais 'plus propre')
	shmdt(G_shm);
	
	//Terminaison de la tâche
	exit(0);
}	//----- fin de de destructionVoie


static void handlerSIGCHLD(int numSignal)
// Mode d'emploi :
//		Procédure appelée à la réception du signal SIGCHLD
// Algorithme :
//		Récupère le fils mort et le supprime de la liste des voitures dessinées
{
	//réception de la mort d'un fils
	pid_t voitureRecuperee = -1;
	while( (voitureRecuperee = waitpid(-1, NULL, 0))==-1 && errno==EINTR );
	G_voitures.erase(voitureRecuperee);	
}	//----- fin de de handlerSIGCHLD


static void handlerSIGUSR2(int numSignal)
// Mode d'emploi :
//		Procédure appelée à la réception du signal SIGUSR2
// Algorithme :
//		Déclenche la phase de destruction de la tâche
{
	/************************
	 *	Phase DESTRUCTION	*
	 ************************/
	destructionVoie();
}	//----- fin de de handlerSIGUSR2


static void initialisationVoie(int idSem,int idBAL,int idShm)
// Mode d'emploi :
//		Gère la phase d'initialisation du processus
// Algorithme :
//		Initialise les variables statiques contenant les ID des IPCS avec ses paramètres
//		Met en place les handler sur SIGCHLD et SIGUSR2
//		Effectue l'attachement à la mémoire partagée
{	
	//Stockage des ID d'IPCS dans les statiques
	G_idSem = idSem;
	G_idBAL = idBAL;
	G_idShm = idShm;
	
	//Armement des signaux déjà ignorés par la mère
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
	
	//Attachement à la mémoire partagée en lecture seule. L'ecriture n'est pas nécessaire ici.
	G_shm=(MemoirePartagee*)shmat(G_idShm, NULL, SHM_RDONLY);
}	//----- fin de de initialisationVoie


static CouleurFeu lireFeu(TypeVoie voie)
// Mode d'emploi :
//		Renvoie la couleur actuelle du feu demandé en paramètre
// Algorithme :
//		Effectue une lecture des couleurs protégée par un sémaphore dans la mémoire partagée
{
	CouleurFeu feu = ROUGE;
	//Redondance du P / V dans le if mais minimisation des operations dans la reservation
	if(voie == NORD || voie == SUD)
	{
		P_lectureCouleur();
		feu = G_shm->couleurAxeNS;
		V_lectureCouleur();
	}
	else if(voie == EST || voie == OUEST)
	{
		P_lectureCouleur();
		feu = G_shm->couleurAxeEO;
		V_lectureCouleur();
	}
	return feu;
}	//----- fin de de lireFeu


static void moteurVoie(const TypeVoie voie)
// Mode d'emploi :
//		Gère la phase moteur du processus
//		Cette procédure est bloquante
// Algorithme :
//		Récupère une à une les voitures correspondantes à sa voie depuis la file de message
//		Dessine et lance cette voiture lorsque le feu est vert
{
	//boucle infinie sur la phase moteur
	for( ; ; )
	{
		struct MsgVoiture msgV; 
		//Lire dans la bal un message qui correspond a la voie
		while(msgrcv(G_idBAL, &msgV, TAILLE_MSG_VOITURE, voie, 0) == -1 && errno == EINTR);
		
		//Dessiner au feu la voiture bloquant tant qu'il n'y a pas de place
		DessinerVoitureFeu(msgV.uneVoiture.numero, msgV.uneVoiture.entree, msgV.uneVoiture.sortie);
		
		//On décrémente le nombre de voitures non dessinées
		OperationVoie(MOINS, voie);

		CouleurFeu feu = lireFeu(voie);
		while(feu != VERT)
		{
			//Lectures répétées de la couleur tant que le feu n'est pas vert
			//Avec une temporisation d'une secondes pour ne pas monopoliser la lecture de la mémoire partagée
			sleep(1);
			feu = lireFeu(voie);
		}
		
		//Lancement de la voiture et création de tâche fille
		pid_t pidVoiture = -1;
		pidVoiture = DeplacerVoiture(msgV.uneVoiture.numero, msgV.uneVoiture.entree, msgV.uneVoiture.sortie);
		G_voitures.insert(pidVoiture);
	}
}	//----- fin de de moteurVoie



//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques

void Voie(TypeVoie voie, int idSem,int idBAL,int idShm)
// Algorithme :
//		Lance la phase d'initialisation puis la phase moteur de la tâche
{
	/****************************
	 *	Phase INITIALISATION	*
	 ****************************/
	initialisationVoie(idSem, idBAL, idShm);
	
	/********************
	 *	Phase MOTEUR 	*
	 ********************/
	moteurVoie(voie);	//bloquant
}	//----- fin de de Voie

