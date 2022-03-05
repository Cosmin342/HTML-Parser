/* Radu Cosmin - 313 CB */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lib.h"
#include "style.h"
#include "aux.h"
#include "delete.h"

//Functie ce determina atributele de style dintr-una din ultimele doua comenzi
char* det_style(char* comanda)
{
	char *p, *tag, *com;
	com = calloc(MAX_VAR, sizeof(char));
	if (!com)
	{
		return NULL;
	}
	strcpy(com, comanda);
	//Atributele sunt reprezentate de al 4-lea token
	p = strtok(com, "\"=");
	p = strtok(NULL, "\"=");
	p = strtok(NULL, "\"=");
	p = strtok(NULL, "\"=");
	tag = strdup(p);
	free(com);
	return tag;
}

//Functie ce construieste lista de atribute style pe baza sirului attr
void prelucreaza_style_primit(TArb arb, char* attr)
{
	TAttr lista = NULL;
	char *p;
	//Se face strtok dupa :, spatiu si ;
	p = strtok(attr, ";: ");
	while (p)
	{
		TAttr attribute = (TAttr)calloc(1, sizeof(TNodAttr));
		if (!attribute)
		{
			return;
		}
		/*
		Nu exista atribut fara valoare la style, asa ca se iau doi tokeni odata
		*/
		p = trim(p);
		attribute->name = strdup(p);
		p = strtok(NULL, ";: ");
		p = trim(p);
		attribute->value = strdup(p);
		//Daca mai exista spatii in value, acestea sunt eliminate
		if (strstr(attribute->value, " "))
		{
			sterge_spatii(attribute->value);
		}
		//Daca lista este goala, se leaga un atribut la aceasta
		if (!arb->info->style)
		{
			arb->info->style = attribute;
			lista = arb->info->style;
		}
		/*
		Altfel, se leaga la lista care pointeaza la ultima celula a listei
		de atribute
		*/
		else
		{
			lista->next = attribute;
			lista = lista->next;
		}
		p = strtok(NULL, ";: ");
	}
}

//Functie pentru updatarea listei de atribute style a unui nod
void update_style(TArb arb, char* attr)
{
	/*
	Daca nodul curent nu are atribute, se creaza o noua lista cu functia
	anterioara
	*/
	if (arb->info->style == NULL)
	{
		prelucreaza_style_primit(arb, attr);
		return;
	}
	/*
	Altfel, se fac aceiasi pasi ca in functia prelucreaza_style_primit, cu
	exceptia faptului ca se verifica daca exista deja un atribut cu numele
	curent, urmand a se actualiza valoarea acestuia, daca exista
	*/
	TAttr lista = NULL, ant;
	char *p, *name, *value;
	p = strtok(attr, ";:");
	while (p)
	{
		name = strdup(p);
		name = trim(name);		
		if (strstr(name, " "))
		{
			sterge_spatii(name);
		}
		p = strtok(NULL, ";:");
		if (p == NULL)
		{
			free(name);
			break;
		}
		value = strdup(p);
		value = trim(value);
		for (lista = arb->info->style; lista != NULL; lista = lista->next)
		{
			ant = lista;
			if (strcmp(lista->name, name) == 0)
			{
				free(lista->value);
				lista->value = strdup(value);
				if (strstr(lista->value, " "))
				{
					sterge_spatii(lista->value);
				}
				break;
			}
		}
		if (lista == NULL)
		{
			TAttr attribute = (TAttr)calloc(1, sizeof(TNodAttr));
			if (!attribute)
			{
				return;
			}
			attribute->name = strdup(name);
			if (strstr(attribute->name, " "))
			{
				sterge_spatii(attribute->name);
			}
			attribute->value = strdup(value);
			if (strstr(attribute->value, " "))
			{
				sterge_spatii(attribute->value);
			}
			ant->next = attribute;
		}
		p = strtok(NULL, ";:");
		free(name);
		free(value);
	}
}

/*
Functie pentru determinarea nodurilor ce respecta conditia data de pointerul
la functie comp
*/
void det_update(TArb arb, char* selector, char* style, TFC comp, FILE* out)
{
	//Coada pentru parcurgerea in latime
	AQ coada = initq(sizeof(TNodArb));
	TArb parc = arb, aux;
	int contor = 0;
	intrq((void*)coada, (void*)arb);
	while (extrq((void*)coada, (void*)&aux) == 1)
	{
		char *sel = strdup(selector);
		//Daca se indeplineste conditia, se updateaza lista style
		if (comp(sel, aux) == 1)
		{
			char *copie = strdup(style);
			update_style(aux, copie);
			free(copie);
			contor++;
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
			"Append to style failed: no node found for selector %s!\n",
				selector);
	}
	distr_q(coada);
}

/*
Functia care determina ce pointer la functie trebuie folosit in updatarea
listei de la style sau daca trebuie apelata functia de cautare a id-ului
*/
void append(TArb arb, char* com, FILE* out)
{
	char* selector = det_selector(com), *style = det_style(com);
	if (strstr(selector, "#") != NULL)
	{
		char *copie, *p;
		p = strtok(selector, "#");
		copie = strdup(p);
		TArb nod = cauta_id(arb, copie);
		if (!nod)
		{
			fprintf(out,
				"Append to style failed: no node found for selector %s!\n",
					selector);
		}
		else
		{
			update_style(nod, style);
		}
		free(copie);
	}
	else if ((selector[0] != '.') && (strstr(selector, ".") != NULL))
	{
		det_update(arb, selector, style, det_tag_class, out);
	}
	else if (strstr(selector, ".") != NULL)
	{
		det_update(arb, selector, style, det_class, out);
	}
	else if (strstr(selector, ">") != NULL)
	{
		det_update(arb, selector, style, det_parinte, out);
	}
	else if (strstr(selector, " ") != NULL)
	{
		det_update(arb, selector, style, det_stramos, out);
	}
	else
	{
		det_update(arb, selector, style, det_tag_type, out);
	}
	free(selector);
	free(style);
}

//Functie pentru suprascrierea listei de atribute style a unui nod
void over_style(TArb arb, char* attr)
{
	/*
	Daca nodul curent nu are atribute, se creaza o noua lista cu functia
	prelucreaza_style_primit
	*/
	if (arb->info->style == NULL)
	{
		prelucreaza_style_primit(arb, attr);
		return;
	}
	//Altfel, se distruge lista curenta si se creaza una noua
	distruge_atribute(&(arb->info->style));
	prelucreaza_style_primit(arb, attr);
}

/*
Functie pentru determinarea nodurilor ce respecta conditia data de pointerul
la functie comp
*/
void det_over(TArb arb, char* selector, char* style, TFC comp, FILE* out)
{
	//Coada pentru parcurgerea in latime
	AQ coada = initq(sizeof(TNodArb));
	TArb parc = arb, aux;
	int contor = 0;
	intrq((void*)coada, (void*)arb);
	while (extrq((void*)coada, (void*)&aux) == 1)
	{
		char *sel = strdup(selector);
		//Daca se indeplineste conditia, se suprascrie lista style
		if (comp(sel, aux) == 1)
		{
			char *copie = strdup(style);
			over_style(aux, copie);
			free(copie);
			contor++;
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
			"Override style failed: no node found for selector %s!\n",
				selector);
	}
	distr_q(coada);
}

/*
Functia care determina ce pointer la functie trebuie folosit in suprascrierea
listei de la style sau daca trebuie apelata functia de cautare a id-ului
*/
void over(TArb arb, char* com, FILE* out)
{
	char* selector = det_selector(com), *style = det_style(com);
	if (strstr(selector, "#") != NULL)
	{
		char *copie, *p;
		p = strtok(selector, "#");
		copie = strdup(p);
		TArb nod = cauta_id(arb, copie);
		if (!nod)
		{
			fprintf(out,
				"Override style failed: no node found for selector %s!\n",
					selector);
		}
		else
		{
			over_style(nod, style);
		}
		free(copie);
	}
	else if ((selector[0] != '.') && (strstr(selector, ".") != NULL))
	{
		det_over(arb, selector, style, det_tag_class, out);
	}
	else if (strstr(selector, ".") != NULL)
	{
		det_over(arb, selector, style, det_class, out);
	}
	else if (strstr(selector, ">") != NULL)
	{
		det_over(arb, selector, style, det_parinte, out);
	}
	else if (strstr(selector, " ") != NULL)
	{
		det_over(arb, selector, style, det_stramos, out);
	}
	else
	{
		det_over(arb, selector, style, det_tag_type, out);
	}
	free(selector);
	free(style);
}