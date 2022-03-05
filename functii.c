/* Radu Cosmin - 313 CB */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lib.h"
#include "aux.h"
#include "delete.h"

//Functia pentru copiere si alocare, deoarece nu exista in c99
char* strdup(char* sir)
{
	size_t l = strlen(sir);
	l++;
	char* alocat = calloc(l, sizeof(char));
	if (!alocat)
	{
		return NULL;
	}
	strcpy(alocat, sir);
	return alocat;
}

//Functie pentru alocarea unui nod si a campului info al acestuia
TArb aloca_nod_arb()
{
	TArb nod = (TArb)calloc(1, sizeof(TNodArb));
	if (!nod)
	{
		return NULL;
	}
	nod->info = (TInfo)calloc(1, sizeof(TNodInfo));
	if (!(nod->info))
	{
		free(nod);
		return NULL;
	}
	return nod;
}

//Functia care creaza un nou nod
TArb creaza(TArb* arbore_html, TArb* nod_curent)
{
	TArb nod = aloca_nod_arb();
	if (!nod)
	{
		exit(EXIT_FAILURE);
	}
	//Daca arborele ese gol, noul nod va fi primul nod al acestuia
	if (*arbore_html == NULL)
	{
		nod->parent = *arbore_html;
		*arbore_html = nod;
		*nod_curent = nod;
	}
	return nod;
}

//Functie pentru legarea unui nod la nod-ul curent
void leaga(TArb arbore_html, char* type, TArb nod, TArb *nod_curent)
{
	TArb parc = NULL, ant = NULL;
	nod->info->type = strdup(type);
	/*
	Daca nod-ul nou este de tip "head", atunci va fi legat direct la arbore
	ca prim fiu si va primi id-ul 1
	*/
	if (strcmp(type, "head") == 0)
	{
		nod->info->id = strdup("1");
		nod->parent = arbore_html;
		arbore_html->firstChild = nod;
		*nod_curent = nod;
	}
	/*
	Daca nod-ul nou este de tip "body", atunci va fi legat direct la arbore
	ca al doilea fiu si va primi id-ul 2
	*/
	else if (strcmp(type, "body") == 0)
	{
		nod->info->id = strdup("2");
		nod->parent = arbore_html;
		arbore_html->firstChild->nextSibling = nod;
		*nod_curent = nod;
	}
	/*
	Altfel, se verifica daca nod-ul curent are fii, iar daca nu are, noul
	nod va fi legat la campul firstChild al sau si va primi id-ul anterior
	+ .1
	*/
	else if ((*nod_curent)->firstChild == NULL)
	{
		char id[20];
		if ((*nod_curent)->info->id != NULL)
		{
			strcpy(id, (*nod_curent)->info->id);
			strcat(id, ".1");
			nod->info->id = strdup(id);
		}
		nod->parent = *nod_curent;
		(*nod_curent)->firstChild = nod;
		*nod_curent = nod;
	}
	/*
	Daca nod-ul curent are deja fii, noul nod se va insera la sfarsitul
	listei de frati
	*/
	else
	{
		char id[MAX_TYPE];
		int contor = 1;
		for (parc = (*nod_curent)->firstChild; parc != NULL;
			parc = parc->nextSibling)
		{
			//Se incrementeaza contor pentru a crea si id-ul
			contor++;
			ant = parc;
		}
		ant->nextSibling = nod;
		sprintf(id, "%s.%d", (*nod_curent)->info->id, contor);
		nod->info->id = strdup(id);
		nod->parent = *nod_curent;
		*nod_curent = nod;
	}
}

//Functie care elimina un spatiu de la inceput si de la sfarsit, daca exista
char* trim(char* string)
{
	size_t i;
	if (string[0] == ' ')
	{
		for (i = 0; i < strlen(string) - 1; i++)
		{
			string[i] = string[i + 1];
		}
		string[strlen(string) - 1] = '\0';
	}
	if (string[strlen(string) - 1] == ' ')
	{
		string[strlen(string) - 1] = '\0';
	}
	return string;
}

//Functie ce elimina toate spatiile dintr-un sir
void sterge_spatii(char* sir)
{
	char* d = sir;
	do
	{
		while (*d == ' ')
		{
			++d;
		}
	}
	while ((*sir++ = *d++));
}

//Functie folosita la creearea listei de atribute pentru style
void prelucreaza_style(TArb arb, char* attr)
{
	TAttr lista = NULL;
	char *p;
	//Se face strtok dupa : si ;
	p = strtok(attr, ";:");
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
		p = strtok(NULL, ";:");
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
		p = strtok(NULL, ";:");
	}
}

//Functie folosita la creearea listei de alte atribute
void prelucreaza_atribut(TArb arb, char* attr, char* attr_value)
{
	TAttr lista = NULL, ant;
	TAttr attribute = (TAttr)calloc(1, sizeof(TNodAttr));
	if (!attribute)
	{
		return;
	}
	attribute->name = strdup(attr);
	attribute->value = strdup(attr_value);
	/*
	Daca lista nodului actual este vida, atunci se leaga noul atribut la
	aceasta
	*/
	if (!arb->info->otherAttributes)
	{
		arb->info->otherAttributes = attribute;
	}
	//Altfel, se cauta finalul listei si se leaga la campul next al acestuia
	else
	{
		for (lista = arb->info->otherAttributes; lista != NULL;
			lista = lista->next)
		{
			ant = lista;
		}
		ant->next = attribute;
	}
}

//Functie pentru legarea content-ului la campul corespunzator al unui nod
void prelucreaza_content(TArb arb, char* content)
{
	char *p = strtok(content, "\r\n");
	if (p == NULL)
	{
		return;
	}
	arb->info->contents = strdup(p);
	//Daca are mai multe spatii la inceput, se elimina toate spatiile din acesta
	if ((arb->info->contents[0] == ' ') && (arb->info->contents[1] == ' '))
	{
		sterge_spatii(arb->info->contents);
	}
}

//Functie pentru determinarea id-ului din comanda add
char* det_id(char* comanda)
{
	char *p, *id, *com;
	com = calloc(MAX_VAR, sizeof(char));
	if (!com)
	{
		return NULL;
	}
	//Se va considera id-ul ca fiind al treilea token
	strcpy(com, comanda);
	p = strtok(com, " =");
	p = strtok(NULL, " =");
	p = strtok(NULL, " =");
	id = strdup(p);
	free(com);
	return id;
}

//Functie pentru extragerea tag-ului din comanda add
char* det_tag(char* comanda)
{
	char *p, *tag, *com;
	com = calloc(MAX_VAR, sizeof(char));
	if (!com)
	{
		return NULL;
	}
	strcpy(com, comanda);
	p = strtok(com, "\"=");
	p = strtok(NULL, "\"==");
	p = strtok(NULL, "\"==");
	tag = strdup(p);
	free(com);
	return tag;
}

//Functie ce creaza un nod pe baza unui sir de caractere
TArb creaza_nod(char* tag)
{
	char c, type[MAX_TYPE] = "", content[MAX_CONTENT] = "";
	int stare = PARSE_CONTENTS, stare_ant;
	TArb nod = aloca_nod_arb();
	if (!nod)
	{
		return NULL;
	}
	size_t i;
	//Functioneaza pe acelasi principiu ca functia de creare a arborelui
	for (i = 0; i < strlen(tag); i++)
	{
		stare_ant = stare;
		c = tag[i];
		//Se utilizeaza interpret pentru a afla starile
		stare = interpret(stare, c);
		//Se determina tipul tag-ului
		if ((stare == PARSE_TAG_TYPE) && (stare_ant == PARSE_OPENING_BRACKET))
		{
			strncat(type, &c, 1);
		}		
		if ((stare == PARSE_TAG_TYPE) && (stare_ant == PARSE_TAG_TYPE))
		{
			strncat(type, &c, 1);
		}
		if ((stare != PARSE_TAG_TYPE) && (stare_ant == PARSE_TAG_TYPE))
		{
			nod->info->type = strdup(type);
		}
		//Apoi conten-ul, cu ignorarea unor caractere
		if ((stare_ant != PARSE_CONTENTS) && (stare == PARSE_CONTENTS) &&
			(c != '>') && (c != '\r') && (c != '\n'))
		{
			strncat(content, &c, 1);
		}
		if ((stare_ant == PARSE_CONTENTS) && (stare == PARSE_CONTENTS))
		{
			strncat(content, &c, 1);
		}
		if ((stare_ant == PARSE_CONTENTS) && (stare != PARSE_CONTENTS))
		{
			//Daca totusi nu sunt ignorate caracterele, se resteaza variabila
			if (strcmp(content, "\r\n") == 0)
			{
				strcpy(content, "");
				continue;
			}
			if (strcmp(content, "") != 0)
			{
				prelucreaza_content(nod, content);
			}
			strcpy(content, "");
		}
		//Se verifica daca tag-ul e selfClosing
		if (stare == PARSE_SELF_CLOSING)
		{
			nod->info->isSelfClosing = 1;
		}
	}
	return nod;
}

//Functia de adaugare a unui nod in arbore
void add(char* com, TArb* arb, FILE* out)
{
	char* id = det_id(com), *tag = det_tag(com);
	char var[MAX_TYPE] = "";
	TArb nod, parc, ant, nou_nod;
	//Se creaza noul nod
	nou_nod = creaza_nod(tag);
	//Se cauta in arbore daca exista un nod cu id-ul id
	nod = cauta_id(*arb, id);
	/*
	Daca nu exista nod cu id-ul respectiv, se elibereaza memoria ocupata de
	nodul creat anterior si se afiseaza un mesaj de eroare
	*/
	if (!nod)
	{
		distruge_nod(&nou_nod);
		fprintf(out, "Add tag failed: node with id %s not found!\n", id);
		free(id);
		free(tag);
		return;
	}
	/*
	Daca nod-ul cu id-ul id are copii, noul nod se leaga la finalul listei
	de frati a primului copil al nodului
	*/
	if (nod->firstChild != NULL)
	{
		int nr = 1;
		for (parc = nod->firstChild; parc != NULL; parc = parc->nextSibling)
		{
			ant = parc;
			nr++;
		}
		//Tot aici i se atribuie si id
		sprintf(var, "%s.%d", id, nr);
		nou_nod->info->id = strdup(var);
		ant->nextSibling = nou_nod;
		//In plus, campul parent al nodului creat va pointa la nodul gasit
		nou_nod->parent = nod;
	}
	/*
	Daca nod-ul cu id-ul id nu are copii, noul nod se leaga la campul
	firstChild al nodului
	*/
	else
	{
		sprintf(var, "%s.%d", id, 1);
		nou_nod->info->id = strdup(var);
		nod->firstChild = nou_nod;
		nou_nod->parent = nod;
	}
	free(id);
	free(tag);
}