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
static int G_idSem;
static int G_idBAL;
static int G_idShm;
static memoirePartagee* G_shm;


static std::set<pid_t> G_voitures;
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

static void destruction()
{
	for (std::set<pid_t>::iterator it=G_voitures.begin(); it!=G_voitures.end(); ++it)
	{
		kill(*it, SIGUSR2);
		while( waitpid(*it, NULL, 0)==-1 && errno==EINTR);
	}
	
	G_voitures.clear();
}

static void handlerSIGCHLD(int numSignal)
{
	//réception de la mort d'un fils
	pid_t voitureRecuperee = -1;
	while( (voitureRecuperee = waitpid(-1, NULL, 0))==-1 && errno==EINTR );
	G_voitures.erase(voitureRecuperee);	
}

static void handlerSIGUSR2(int numSignal)
{
	//on masque le signal SIGCHILD
	//SIGCHILD
	struct sigaction actionSIGCHLD;
	sigemptyset(&actionSIGCHLD.sa_mask);
	actionSIGCHLD.sa_handler = SIG_IGN;
	actionSIGCHLD.sa_flags = 0;
	sigaction(SIGCHLD, &actionSIGCHLD, NULL);
	
	destruction();
	//réception demande de fin de la tâche
	exit(0);
}

static void initialisationVoie(int idSem,int idBAL,int idShm)
{	
	G_idSem = idSem;
	G_idBAL = idBAL;
	G_idShm = idShm;
	
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
	
	//attachement à la mémoire partagée en lecture seule. L'ecriture n'est pas nécessaire ici.
	G_shm=(memoirePartagee*)shmat(G_idShm, NULL, SHM_RDONLY);

}

static couleurFeu lireFeu(TypeVoie voie)
{
	couleurFeu feu = ROUGE;
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
}
static void moteur(const TypeVoie voie)
{
	//boucle infinie sur la phase moteur
	for(;;)
	{
		struct MsgVoiture msgV; 
		//lire dans la bal un message qui correspond a la voie
		while(msgrcv(G_idBAL, &msgV, TAILLE_MSG_VOITURE, voie, 0) == -1 && errno == EINTR);
		
		//dessiner au feu la voiture bloquant tant qu'il n'y a pas de place
		DessinerVoitureFeu(msgV.uneVoiture.numero, msgV.uneVoiture.entree, msgV.uneVoiture.sortie);
		
		//on décrémente
		OperationVoie(MOINS, voie);

		couleurFeu feu = lireFeu(voie);
		while(feu != VERT)
		{
			sleep(1);
			feu = lireFeu(voie);
		}
		
		
		//lancer la voiture
		pid_t pidVoiture = -1;
		pidVoiture = DeplacerVoiture(msgV.uneVoiture.numero, msgV.uneVoiture.entree, msgV.uneVoiture.sortie);
		G_voitures.insert(pidVoiture);
	}
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
	initialisationVoie(idSem, idBAL, idShm);
	
	/*
	 *	Phase MOTEUR 
	 */
	 
	//bloquant
	moteur(voie);
}

