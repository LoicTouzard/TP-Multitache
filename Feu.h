/*************************************************************************
                           GMenu  -  description
                             -------------------
    début                : XXX
    copyright            : (C) XXX par XXX
    e-mail               : XXX
*************************************************************************/

//---------- Interface de la tâche <Feu> (fichier Feu.h) -------
#if ! defined ( FEU_H )
#define FEU_H

//------------------------------------------------------------------------
// Rôle de la tâche <GMenu>
//
//
//------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////  INCLUDE
//--------------------------------------------------- Interfaces utilisées

//------------------------------------------------------------- Constantes

//------------------------------------------------------------------ Types

//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
void Decomptetempo(unsigned int temps, unsigned int &tempoNS, unsigned int &tempoEO);
void Feu(int idSem, int shmId);
void EcritureShmAxeNS(int idSem, memoirePartagee* shm, couleurFeu couleurDuFeu);
void EcritureShmAxeEO(int idSem, memoirePartagee* shm, couleurFeu couleurDuFeu);
void MAJTempoShm(int idSem, memoirePartagee* shm, unsigned int &dureeNS,unsigned int &dureeEO);

#endif // FEU_H




