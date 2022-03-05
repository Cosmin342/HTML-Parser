/* Radu Cosmin - 313 CB */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lib.h"

//Functie pentru determinarea a primelor niv parti dintr-un id
char* det_id_part(char* id, int niv)
{
	char id_part[MAX_TYPE], *p, ans[MAX_TYPE] = "", *ret;
	int i = 1;
	strcpy(id_part, id);
	//Se realizeaza strtok pentru determinarea bucatilor id-ului
	p = strtok(id_part, ".");
	while (p)
	{
		strcat(ans, p);
		strcat(ans, ".");
		/*
		In momentul in care au fost concatenate niv parti, se opreste strtok-ul
		*/
		if (i == niv)
		{
			break;
		}
		i++;
		p = strtok(NULL, ".");
	}
	//Se elimina punctul aditional
	ans[strlen(ans) - 1] = '\0';
	ret = strdup(ans);
	return ret;
}

//Functie pentru cautarea unui nod pe baza id-ului
TArb cauta_id(TArb arb, char* id)
{
	TArb parc = arb->firstChild;
	char *id_part;
	int niv = 1, gasit = 0;
	//Se determina prima cifra din id
	id_part = det_id_part(id, niv);
	while(parc)
	{
		//Daca s-a gasit nodul cu id-ul dorit, se opreste cautarea
		if (strcmp(parc->info->id, id) == 0)
		{
			gasit = 1;
			break;
		}
		/*
		Daca se gaseste nodul cu id-ul partial curent, se calculeaza noul
		id partial si se coboara cu un nivel, daca exista acel nivel
		*/
		if (strcmp(parc->info->id, id_part) == 0)
		{
			parc = parc->firstChild;
			free(id_part);
			niv++;
			id_part = det_id_part(id, niv);
			continue;
		}
		parc = parc->nextSibling;
	}
	//Daca nu s-a gasit nodul, se intoarce NULL
	if (gasit == 0)
	{
		free(id_part);
		return NULL;
	}
	free(id_part);
	return parc;
}

//Functie pentru determinarea selectorului dintr-una din ultimele comenzi
char* det_selector(char* comanda)
{
	char *p, *tag, *com;
	com = calloc(MAX_VAR, sizeof(char));
	if (!com)
	{
		return NULL;
	}
	strcpy(com, comanda);
	//Se realizeaza strtok dupa caracterele " si =
	p = strtok(com, "\"=");
	p = strtok(NULL, "\"=");
	tag = strdup(p);
	free(com);
	return tag;
}

//Functie pentru a determina daca un tag are o anumita clasa
int det_class(char* selector, TArb nod)
{
	char *nou, *p, *copie = strdup(selector);
	//Se elimina punctul
	p = strtok(copie, ".");
	nou = strdup(p);
	TAttr l;
	//Daca are atribute, se parcurge lista lor pana se detecteaza campul class
	if (nod->info->otherAttributes != NULL)
	{
		for (l = nod->info->otherAttributes; l != NULL; l = l->next)
		{
			//Daca valoarea lui class este cea din nou, atunci se intoarce 1
			if ((strcmp(l->name, "class") == 0) && (strcmp(l->value, nou) == 0))
			{
				free(copie);
				free(nou);
				return 1;
			}
		}
	}
	free(copie);
	free(nou);
	return 0;
}

//Functie pentru a determina daca un tag este de tipul dat de selector
int det_tag_type(char* selector, TArb nod)
{
	char *copie = strdup(selector);
	//Se compara campul type din info cu copia selectorului
	if (strcmp(nod->info->type, copie) == 0)
	{
		free(copie);
		return 1;
	}
	free(copie);
	return 0;
}

//Functie pentru a determina daca un tag are un anumit tip si o anumita clasa
int det_tag_class(char* selector, TArb nod)
{
	char *tag, *class, *p, *copie = strdup(selector);
	//Se elimina punctul si se despart tipul si clasa
	p = strtok(copie, ".");
	tag = strdup(p);
	p = strtok(NULL, ".");
	class = strdup(p);
	TAttr l;
	//Se verifica tipul
	if (strcmp(nod->info->type, tag) == 0)
	{
		//Daca are atribute, se parcurg acestea pana la gasirea class
		if (nod->info->otherAttributes != NULL)
		{
			for (l = nod->info->otherAttributes; l != NULL; l = l->next)
			{
				//Apoi se verifica daca are valoarea ceruta
				if ((strcmp(l->name, "class") == 0) &&
					(strcmp(l->value, class) == 0))
				{
					free(copie);
					free(tag);
					free(class);
					return 1;
				}
			}
		}
	}
	free(tag);
	free(class);
	free(copie);
	return 0;
}

/*
Functie pentru a determina daca un nod cu un anumit tip, are un parinte cu un
anumit tip
*/
int det_parinte(char* selector, TArb nod)
{
	char *tag, *parinte, *p, *copie = strdup(selector);
	//Se despart tipurile din selector separate de caracterul >
	p = strtok(copie, ">");
	parinte = strdup(p);
	p = strtok(NULL, ">");
	tag = strdup(p);
	/*
	Se verifica daca tag-ul curent are tipul tag si daca parintele are
	tipul parinte
	*/
	if ((strcmp(nod->info->type, tag) == 0) &&
		(strcmp(nod->parent->info->type, parinte) == 0))
	{
		free(tag);
		free(parinte);
		free(copie);
		return 1;
	}
	//Altfel se returneaza 0
	free(tag);
	free(parinte);
	free(copie);
	return 0;
}

/*
Functie pentru a determina daca un nod cu un anumit tip, are un stramos cu un
anumit tip
*/
int det_stramos(char* selector, TArb nod)
{
	char *tag, *stramos, *p, *copie = strdup(selector);
	TArb l;
	//Se despart tipurile din selector separate printr-un spatiu
	p = strtok(copie, " ");
	stramos = strdup(p);
	p = strtok(NULL, " ");
	tag = strdup(p);
	//Se verifica daca tag-ul curent are tipul tag
	if ((strcmp(nod->info->type, tag) == 0))
	{
		/*
		Daca il are, se verifica parintii pana cand este gasit unul cu tipul
		stramos
		*/
		for (l = nod; l->parent != NULL; l = l->parent)
		{
			if ((strcmp(l->parent->info->type, stramos) == 0))
			{
				free(tag);
				free(stramos);
				free(copie);
				return 1;
			}
		}
	}
	free(tag);
	free(stramos);
	free(copie);
	return 0;
}