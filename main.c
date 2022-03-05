/* Radu Cosmin - 313 CB */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lib.h"

//Functie pentru determinarea unei comenzi folosind strstr
int consola(char* comanda)
{
	char command1[] = "format";
	char command2[] = "add";
	char command3[] = "deleteRecursively";
	char command4[] = "overrideStyle";
	char command5[] = "appendStyle";
	if (strstr(comanda, command1) != NULL) return PARSE_CONTENTS;
	if (strstr(comanda, command2) != NULL) return PARSE_OPENING_BRACKET;
	if (strstr(comanda, command3) != NULL) return PARSE_TAG_TYPE;
	if (strstr(comanda, command4) != NULL) return PARSE_CLOSING_TAG;
	if (strstr(comanda, command5) != NULL) return PARSE_REST_OF_TAG;
	return 0;
}

/*
Functie ce apeleaza, in functie de tipul de atribute, alte functii ce
creaza liste de atribute sau liste de atribute pentru style
*/
void creaza_atrib(TArb nod, char* attr, char* attr_value)
{
	if (strcmp(attr, "style") == 0)
	{
		prelucreaza_style(nod, attr_value);
	}
	else
	{
		prelucreaza_atribut(nod, attr, attr_value);
	}
	//Resetarea sirurilor pentru alte tag-uri
	strcpy(attr, "");
	strcpy(attr_value, "");
}

/*
Functie care, in functie de continutul content-ului, apeleaza sau nu functia
pentru prelucrarea acestuia
*/
int creaza_content(TArb nod, char* content)
{
	if (strcmp(content, "\r\n") == 0)
	{
		strcpy(content, "");
		return 0;
	}
	if (strcmp(content, "") != 0)
	prelucreaza_content(nod, content);
	strcpy(content, "");
	return 1;
}

//Functie pentru constructia arborelui
void construieste_arbore(TArb *arbore_html, FILE* html)
{
	TArb nod = NULL, nod_curent;
	char c, type[MAX_TYPE] = "", attr[MAX_TYPE] = "", *attr_value, *content;
	content = calloc(MAX_CONTENT, sizeof(char));
	attr_value = calloc(MAX_CONTENT, sizeof(char));
	if (!content)
	{
		exit(EXIT_FAILURE);
	}
	if (!attr_value)
	{
		free(content);
		exit(EXIT_FAILURE);
	}
	//Variabile pentru starea curenta si anterioara
	int stare = PARSE_CONTENTS, stare_ant;
	while ((c = (char)(fgetc(html))) != EOF)
	{
		stare_ant = stare;
		stare = interpret(stare, c);
		/*
		Daca se ajunge la un tag nou, se creaza un nou nod si se adauga
		prima litera a numelui sau la variabila
		*/
		if ((stare == PARSE_TAG_TYPE) && (stare_ant == PARSE_OPENING_BRACKET))
		{
			strncat(type, &c, 1);
			nod = creaza(arbore_html, &nod_curent);
		}
		//Se adauga litere pana se ajunge la finalul lumelui
		if ((stare == PARSE_TAG_TYPE) && (stare_ant == PARSE_TAG_TYPE))
		{
			strncat(type, &c, 1);
		}
		//La final, se leaga noul nod la arbore si se reinitializeaza type
		if ((stare != PARSE_TAG_TYPE) && (stare_ant == PARSE_TAG_TYPE))
		{
			leaga(*arbore_html, type, nod, &nod_curent);
			strcpy(type, "");
		}
		/*
		Daca se detecteaza starile pentru atribut si valoarea sa, se adauga
		litere in variabilele corespunzatoare pana la trecerea la o noua stare
		*/
		if ((stare == PARSE_ATTRIBUTE_NAME) && (stare_ant == PARSE_REST_OF_TAG))
		{
			strncat(attr, &c, 1);
		}
		if ((stare == PARSE_ATTRIBUTE_NAME) && (stare_ant ==
			PARSE_ATTRIBUTE_NAME))
		{
			strncat(attr, &c, 1);
		}
		if ((stare == PARSE_ATTRIBUTE_VALUE) && (stare_ant ==
			PARSE_ATTRIBUTE_VALUE))
		{
			strncat(attr_value, &c, 1);
		}
		/*
		La finalul starilor corespunzatoare unui atribut, acesta este creat
		pe baza sirurilor attr si attr_value si adaugat nodului
		*/
		if ((stare != PARSE_ATTRIBUTE_VALUE) && (stare_ant ==
			PARSE_ATTRIBUTE_VALUE))
		{
			creaza_atrib(nod, attr, attr_value);
		}
		/*
		Incepe crearea sirului pentru content la detectarea starii, cu
		ignorarea unor caractere
		*/
		if ((stare_ant != PARSE_CONTENTS) && (stare == PARSE_CONTENTS) &&
			(c != '>') && (c != '\r') && (c != '\n'))
		{
			strncat(content, &c, 1);
		}
		if ((stare_ant == PARSE_CONTENTS) && (stare == PARSE_CONTENTS))
		{
			strncat(content, &c, 1);
		}
		//La finalul starii pentru content, acesta este atribuit nodului
		if ((stare_ant == PARSE_CONTENTS) && (stare != PARSE_CONTENTS))
		{
			if (creaza_content(nod, content) == 0)
			{
				continue;
			}
		}
		/*
		Se marcheaza nodul ca fiind selfColsing, daca se detecteaza starea
		respectiva
		*/
		if (stare == PARSE_SELF_CLOSING)
		{
			nod->info->isSelfClosing = 1;
		}
		//La inchiderea unui tag, se modifica variabila ce retine nodul curent
		if ((stare == PARSE_CONTENTS) && (stare_ant == PARSE_CLOSING_TAG))
		{
			nod_curent = nod_curent->parent;
		}
	}	
	free(content);
	free(attr_value);
}

int main(int argc, char const *argv[])
{
	FILE* html = fopen(argv[argc - PARSE_TAG_TYPE], "rt");
	FILE* in = fopen(argv[argc - PARSE_OPENING_BRACKET], "rt");
	FILE* out = fopen(argv[argc - 1], "wt");
	TArb arbore_html = NULL;
	int n, i;
	//Testarea deschiderii fisierelor
	if (!html)
	{
		exit(EXIT_FAILURE);
	}
	if (!in)
	{
		exit(EXIT_FAILURE);
	}
	if (!out)
	{
		exit(EXIT_FAILURE);
	}
	//Construirea arborelui
	construieste_arbore(&arbore_html, html);
	//Setarea parintelui primului nod la NULL
	arbore_html->parent = NULL;
	//Citirea numarului de comenzi
	fscanf(in, "%d\n", &n);
	char com[MAX_CONTENT];
	for (i = 0; i < n; i++)
	{
		//In functie de comanda detectata, se apeleaza o functie
		fgets(com, MAX_CONTENT, in);
		com[strlen(com) - 1] = '\0';
		switch (consola(com))
		{
			case 0:
				exit(0);
			case PARSE_CONTENTS:
				format(arbore_html, out);
				break;
			case PARSE_OPENING_BRACKET:
				add(com, &arbore_html, out);
				break;
			case PARSE_TAG_TYPE:
				delete_rec(arbore_html, com, out);
				break;
			case PARSE_CLOSING_TAG:
				over(arbore_html, com, out);
				break;
			case PARSE_REST_OF_TAG:
				append(arbore_html, com, out);
				break;
		}
	}
	//Eliberarea memoriei ocupate de arbore
	distruge_arbore(&arbore_html);
	fclose(html);
	fclose(in);
	fclose(out);
	return 0;
}