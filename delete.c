/* Radu Cosmin - 313 CB */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lib.h"
#include "aux.h"

//Functie pentru distrugerea unei liste de atribute
void distruge_atribute(TAttr* atribute)
{
	TAttr aux;
	while (*atribute)
	{
		aux = (*atribute);
		*atribute = aux->next;
		//Se elibereaza treptat numele, valoarea si celula fiecarui atribut
		free(aux->value);
		free(aux->name);
		free(aux);
	}
	*atribute = NULL;
}

//Functie pentru stergerea informatiilor unui tag
void distruge_info_nod(TInfo* info)
{
	TInfo sters = *info;
	//Se elimina, daca exista, tipul, id-ul, content-ul si atributele unui tag
	if (sters->type != NULL)
	{
		free(sters->type);
	}
	if (sters->id != NULL)
	{
		free(sters->id);
	}
	if (sters->contents != NULL)
	{
		free(sters->contents);
	}
	if (sters->style != NULL)
	{
		distruge_atribute(&(sters->style));
	}
	if (sters->otherAttributes != NULL)
	{
		distruge_atribute(&(sters->otherAttributes));
	}
}

//Functie pentru stergerea unui nod al arborelui
void distruge_nod(TArb *nod)
{
	/*
	Se elimina intai informatiile unui nod, apoi campul info,
	iar in final nodul
	*/
	distruge_info_nod(&((*nod)->info));
	free((*nod)->info);
	free(*nod);
}

/*
Functie pentru stergerea nerecursiva a intregului arbore prin parcurgerea
lui in adancime
*/
void distruge_arbore(TArb* arb)
{
	TArb parc = *arb, aux, aux2, ant;
	while (parc)
	{
		if (parc->firstChild == NULL)
		{
			if (parc->nextSibling == NULL)
			{
				/*
				Daca este nod frunza, acesta este eliberat, fiind retinuta
				adresa la parintele lui
				*/
				ant = parc->parent;
				aux = parc;
				distruge_nod(&aux);
				//Apoi se urca la parinte, eliberandu-se nodurile din subarbori
				while (1)
				{
					parc = ant;
					ant = parc->parent;
					if (parc->parent == NULL)
					{
						aux = parc;
						distruge_nod(&aux);
						parc = NULL;
						break;
					}
					if (parc->nextSibling != NULL)
					{
						aux = parc;
						aux2 = parc->nextSibling;
						distruge_nod(&aux);
						parc = aux2;
						break;
					}
					aux = parc;
					distruge_nod(&aux);
				}
				continue;
			}
			//Daca nu e nod frunza, dar nu are copii, se elibereaza fratii sai
			else
			{
				aux = parc;
				aux2 = parc->nextSibling;
				distruge_nod(&aux);
				parc = aux2;
				continue;
			}
		}
		parc = parc->firstChild;
	}
}

/*
Functie pentru renumerotarea nodurilor, daca se sterge primul copil al unui nod
*/
void renumerotare_inc(TArb arb)
{
	//Se retine adresa parintelui pentru preluarea id-ului in vederea reformarii
	TArb parc = arb->firstChild, in;
	int contor = 1;
	char *id = calloc(MAX_TYPE, sizeof(char));
	if (!id)
	{
		return;
	}
	while (parc)
	{
		//Refacerea id-ului pentru primul copil
		sprintf(id, "%s.1", parc->parent->info->id);
		free(parc->info->id);
		parc->info->id = strdup(id);
		strcpy(id, "");
		//Daca mai are si alti copii, se reface id-ul pe baza contor-ului
		if (parc->nextSibling != NULL)
		{
			for (in = parc->nextSibling; in != NULL; in = in->nextSibling)
			{
				contor++;
				sprintf(id, "%s.%d", parc->parent->info->id, contor);
				free(in->info->id);
				in->info->id = strdup(id);
				strcpy(id, "");
			}
		}
		parc = parc->firstChild;
	}
	free(id);
}

/*
Functie pentru renumerotarea nodurilor, daca se sterge un copil al unui nod,
altul decat primul, pe baza numarului count
*/
void renumerotare_parc(TArb arb, int count)
{
	TArb parc = arb->firstChild, in;
	int contor = 1;
	char *id = calloc(MAX_TYPE, sizeof(char));
	if (!id)
	{
		return;
	}
	while (parc)
	{
		/*
		Daca ne aflam pe urmatorul nivel ce succede parintele nodului de la
		care se incepe renumeroatarea, acesta va avea id-ul parintelui + .count
		*/
		if (parc->parent == arb)
		{
			sprintf(id, "%s.%d", parc->parent->info->id, count);
			free(parc->info->id);
			parc->info->id = strdup(id);
			strcpy(id, "");
			/*
			Daca mai are frati, sunt si acestia renumerotati pornind de la
			count
			*/
			if (parc->nextSibling != NULL)
			{
				for (in = parc->nextSibling; in != NULL; in = in->nextSibling)
				{
					count++;
					sprintf(id, "%s.%d", parc->parent->info->id, count);
					free(in->info->id);
					in->info->id = strdup(id);
					strcpy(id, "");
				}
			}
		}
		//Altfel, daca este prim copil al unui nod va primi id-ul anterior + .1
		sprintf(id, "%s.1", parc->parent->info->id);
		free(parc->info->id);
		parc->info->id = strdup(id);
		strcpy(id, "");
		//Daca are frati, vor fi si acestia renumerotati utilizandu-se contor
		if (parc->nextSibling != NULL)
		{
			contor = 1;
			for (in = parc->nextSibling; in != NULL; in = in->nextSibling)
			{
				contor++;
				sprintf(id, "%s.%d", parc->parent->info->id, contor);
				free(in->info->id);
				in->info->id = strdup(id);
				strcpy(id, "");
			}
		}
		//Se realizeaza inaintarea
		parc = parc->firstChild;
	}
	free(id);
}

//Functie pentru stergerea recursiva a unui subabore
void distruge_recursiv(TArb* arb)
{
	TArb aux;
	/*
	Daca nodul nu mai are nici copii, nici frati si nici parinte, acesta este
	eliberat
	*/
	if ((*arb)->firstChild == NULL)
	{
		if ((*arb)->nextSibling == NULL)
		{
			if ((*arb)->parent == NULL)
			{
				aux = *arb;
				distruge_nod(&aux);
				return;
			}
		}
	}
	/*
	Daca nodul nu mai are nici copii si nici frati, acesta este, de asemenea,
	eliberat
	*/
	if ((*arb)->firstChild == NULL)
	{
		if ((*arb)->nextSibling == NULL)
		{
			aux = *arb;
			distruge_nod(&aux);
			return;
		}
	}
	/*
	Daca nodul curent are copii si frati, se apeleaza functia intai pentru
	frati, apoi pentru copii
	*/
	if ((*arb)->firstChild != NULL)
	{
		if ((*arb)->nextSibling != NULL)
		{
			distruge_recursiv(&((*arb)->nextSibling));
		}
		distruge_recursiv(&((*arb)->firstChild));
		aux = *arb;
		distruge_nod(&aux);
		return;
	}
	//Daca nodul curent are frati, se apeleaza functia pentru frati
	if ((*arb)->nextSibling != NULL)
	{
		distruge_recursiv(&((*arb)->nextSibling));
	}
}

//Functie pentru eliminarea unui anumit nod din arbore
void delete_nod(TArb* arb)
{
	TArb aux, ant;
	if ((*arb)->nextSibling != NULL)
	{
		/*
		Daca este prim copil si are frati, campul firstChild al parintelui
		va pointa la nextSibling al nodului de eliminat
		*/
		if ((*arb)->parent->firstChild == *arb)
		{
			(*arb)->parent->firstChild = (*arb)->nextSibling;
			(*arb)->nextSibling = NULL;
			ant = (*arb)->parent;
			(*arb)->parent = NULL;
			//Renumerotarea nodurilor
			renumerotare_inc(ant);
			//Eliberarea memoriei ocupate de subarborele nodului
			distruge_recursiv(arb);
		}
		else
		{
			int contor = 1;
			TArb l, ant;
			/*
			Daca nu este prim copil, se determina locul sau in lista de frati
			si se refac legaturile
			*/
			for (l = (*arb)->parent->firstChild; l->nextSibling != *arb;
				l = l->nextSibling)
			{
				contor++;
				ant = l;
			}
			ant->nextSibling = (*arb)->nextSibling;
			aux = (*arb)->parent;
			(*arb)->nextSibling = NULL;
			renumerotare_parc(aux, contor);
			distruge_recursiv(arb);
		}
	}
	else
	{
		/*
		Daca este prim copil si nu are frati, parintele sau va avea campul
		firstChild setat pe NULL
		*/
		if ((*arb)->parent->firstChild == *arb)
		{
			(*arb)->parent->firstChild = NULL;
			(*arb)->parent = NULL;
			distruge_recursiv(arb);
		}
		/*
		Daca este ultimul copil, se seteaza campul nextSibling al ultimului
		element din lista de frati la NULL
		*/
		else
		{
			TArb l, ant;
			for (l = (*arb)->parent->firstChild; l->nextSibling != NULL;
				l = l->nextSibling)
			{
				ant = l;
			}
			ant->nextSibling = NULL;
			(*arb)->parent = NULL;
			distruge_recursiv(arb);
		}
	}
}

/*
Functie pentru determinarea nodurilor ce respecta conditia data de pointerul
la functie comp
*/
void det_delete(TArb arb, char* selector, TFC comp, FILE* out)
{
	//Coada pentru parcurgerea in latime
	AQ coada = initq(sizeof(TNodArb));
	TArb parc = arb, aux;
	int contor = 0;
	intrq((void*)coada, (void*)arb);
	//Cat timp exista elemente in coada
	while (extrq((void*)coada, (void*)&aux) == 1)
	{
		char *sel = strdup(selector);
		//Daca se indeplineste conditia, se elimina nodul si subarborele sau
		if (comp(sel, aux) == 1)
		{
			delete_nod(&aux);
			contor++;
			free(sel);
			continue;
		}
		//Se introduc in coada copii nodului extras
		for (parc = aux->firstChild; parc != NULL; parc = parc->nextSibling)
		{
			intrq((void*)coada, (void*)parc);
		}
		free(sel);
	}
	/*
	Daca nu s-a gasit vreun nod care sa indeplineasca respectiva conditie,
	se va afisa mesajul de eroare
	*/
	if (contor == 0)
	{
		fprintf(out,
			"Delete recursively failed: no node found for selector %s!\n",
				selector);
	}
	distr_q(coada);
}

/*
Functia care determina ce pointer la functie trebuie folosit in stergerea
recursiva sau daca trebuie apelata functia de cautare a id-ului
*/
void delete_rec(TArb arb, char* com, FILE* out)
{
	char* selector = det_selector(com);
	if (strstr(selector, "#") != NULL)
	{
		char *copie, *p;
		p = strtok(selector, "#");
		copie = strdup(p);
		TArb nod = cauta_id(arb, copie);
		if (!nod)
		{
			fprintf(out,
				"Delete recursively failed: no node found for selector %s!\n",
					selector);
		}
		else
		{
			delete_nod(&nod);
		}
		free(copie);
	}
	else if ((selector[0] != '.') && (strstr(selector, ".") != NULL))
	{
		det_delete(arb, selector, det_tag_class, out);
	}
	else if (strstr(selector, ".") != NULL)
	{
		det_delete(arb, selector, det_class, out);
	}
	else if (strstr(selector, ">") != NULL)
	{
		det_delete(arb, selector, det_parinte, out);
	}
	else if (strstr(selector, " ") != NULL)
	{
		det_delete(arb, selector, det_stramos, out);
	}
	else
	{
		det_delete(arb, selector, det_tag_type, out);
	}
	free(selector);
}