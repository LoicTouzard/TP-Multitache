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

int main(void)
{
	InitialiserApplication(XTERM);
	pid_t pidH=-1;
	pid_t pidC=-1;
	pid_t pidG=-1;
	
	//Création de la boite aux lettres
	key_t cleIPC=ftok(REFERENCE, 1);
	int msgId=msgget(cleIPC, IPC_CREAT|DROITS);
	
	
	//Création du générateur
	pidG=CreerEtActiverGenerateur(0, msgId);
	
	if( (pidC=fork() )==0)
	{
		GMenu();
	}
	else {
		//Création de l'heure
		pidH=CreerEtActiverHeure();

		//lorsque le menu est terminé
		while( waitpid(pidC, NULL, 0)==-1 && errno==EINTR);
		
		
		//Fin de l'heure
		kill(pidH, SIGUSR2);

		//On attend le retour du signal de fin de l'heure
		while( waitpid(pidH, NULL, 0)==-1 && errno==EINTR);
		std::cout<<"ligne0"<<std::endl;

		
		//Fin du générateur
		kill(pidG, SIGUSR2);
		//On attend le retour du signal de fin de générateur
		while( waitpid(pidG, NULL, 0)==-1 && errno==EINTR); //ICI c'est pas bon
		
		//Destruction de la boite aux lettres
		msgctl(msgId, IPC_RMID, 0);
		TerminerApplication(true);
		exit(0);
	}
	return 0;
}

