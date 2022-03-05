/* Radu Cosmin - 313 CB */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lib.h"

//Initializare coada cu elemente ce au dimensiunea d
void* initq(size_t d)
{
	AQ q;
	//Alocare coada + testare
	q = (AQ)calloc(1, sizeof(TCoada));
	if(!q)
	{
		return NULL;
	}
	//Campul dime al cozii va fi egal cu d
	q->dime = d;
	return (void*)q;
}

//Functie pentru introducerea unui element in coada
int intrq(void* q, void* ae)
{
	//Alocare + testare celula pentru lista generica a cozii
	ACel aux = (ACel)calloc(1, sizeof(TCel));
	if (!aux)
	{
		return 0;
	}
	//Setare info la un element deja alocat cu dimensiunea aux->dime
	aux->info = ae;
	//Daca inceputul si sfarsitul sunt nule, ele vor pointa la noua celula
	if ((IC(q) == NULL) && (SC(q) == NULL))
	{
		IC(q) = aux;
		SC(q) = aux;
	}
	//Altfel, noua celula este adaugata la sfarsit
	else
	{
		SC(q)->urm = aux;
		SC(q) = aux; 
	}
	return 1;
}

//Functie pentru scoaterea unui element din coada
int extrq(void* q, void** ae)
{
	//Testarea coada vida
	if ((IC(q) == NULL) && (SC(q) == NULL))
	{
		return 0;
	}
	//Se extrage primul element introdus
	ACel aux = IC(q);
	*ae = aux->info;
	IC(q) = aux->urm;
	//Eliberare memorie ocupata de celula ce contine elementul extras
	free(aux);
	if (IC(q) == NULL)
	{
		SC(q) = NULL;
	}
	return 1;
}

//Distrugere coada
void distr_q(void* q)
{
	free(q);
}