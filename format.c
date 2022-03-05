/* Radu Cosmin - 313 CB */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lib.h"

//Functie pentru afisarea primei parti a unui tag (nume, atribute etc.)
void afis_inceput(TInfo nod, FILE* out, int niv)
{
	int i;
	//Daca nu are atribute, se afiseaza doar numele acestuia
	if ((nod->style == NULL) && (nod->otherAttributes == NULL))
	{
		fprintf(out, "<%s>\n", nod->type);
	}
	/*
	Altfel, se parcurg cele doua liste cu atribute, daca nu sunt nule si
	se afiseaza
	*/
	else
	{
		TAttr p;
		fprintf(out, "<%s", nod->type);
		if ((nod->otherAttributes != NULL) && (nod->style == NULL))
		{
			for (p = nod->otherAttributes; p != NULL; p = p->next)
			{
				fprintf(out, " %s=\"%s\"", p->name, p->value);
			}
		}
		if ((nod->otherAttributes == NULL) && (nod->style != NULL))
		{
			fprintf(out, " style=\"");
			for (p = nod->style; p != NULL; p = p->next)
			{
				fprintf(out, "%s: %s;", p->name, p->value);
				if (p->next != NULL)
				{
					fprintf(out, " ");
				}
			}
			fprintf(out, "\"");
		}
		if ((nod->otherAttributes != NULL) && (nod->style != NULL))
		{
			fprintf(out, " style=\"");
			for (p = nod->style; p != NULL; p = p->next)
			{
				fprintf(out, "%s: %s;", p->name, p->value);
				if (p->next != NULL)
				{
					fprintf(out, " ");
				}
			}
			fprintf(out, "\"");
			for (p = nod->otherAttributes; p != NULL; p = p->next)
			{
				fprintf(out, " %s=\"%s\"", p->name, p->value);
			}
		}
		//Daca tag-ul este selfClosing, se afiseaza /> la final
		if (nod->isSelfClosing == 1)
		{
			fprintf(out, "/>\n");
		}
		else
		{
			fprintf(out, ">\n");			
		}
	}
	//Daca tag-ul are content, se afiseaza si content-ul
	if (nod->contents != NULL)
	{
		for (i = 0; i < niv + 1; i++)
		{
			fprintf(out, "\t");
		}
		fprintf(out, "%s\n", nod->contents);
	}
}

//Afisarea unui numar de niv tab-uri
void print_tags(int niv, FILE* out)
{
	int i;
	for (i = 0; i < niv; i++)
	{
		fprintf(out, "\t");
	}
}

//Inchiderea unui tag
void print_fin(int ok, FILE* out, int niv, char* type)
{
	if (ok == 0)
	{
		print_tags(niv, out);
		fprintf(out, "</%s>\n", type);
	}
}

//Afisarea propriu-zisa a arborelui, prin parcurgerea lui in adancime
void format(TArb arb, FILE* out)
{
	TArb parc = arb;
	//Variabila pentru nr de taburi de afisat
	int niv = 0;
	while (parc)
	{
		print_tags(niv, out);
		//Deschidere tag
		afis_inceput(parc->info, out, niv);
		if (parc->firstChild == NULL)
		{
			if (parc->nextSibling == NULL)
			{
				//Daca nu mai are copii sau frati, se inchide tag-ul
				print_fin(parc->info->isSelfClosing, out, niv,
					parc->info->type);
				//Se revine apoi la parinti, ei fiind inchisi daca nu au frati
				while (1)
				{
					parc = parc->parent;
					if (!parc)
					{
						break;
					}
					niv--;
					print_fin(parc->info->isSelfClosing, out, niv,
						parc->info->type);
					//Daca au frati, se parcurge si lista acestora
					if (parc->nextSibling != NULL)
					{
						parc = parc->nextSibling;
						break;
					}
				}
				continue;
			}
			/*
			Daca tag-ul curent are frati, se inchide acesta si se parcurge
			lista lor
			*/
			else
			{
				print_fin(parc->info->isSelfClosing, out, niv,
					parc->info->type);
				parc = parc->nextSibling;
				continue;
			}
		}
		//Daca tag-ul mai are copii, se trece la primul dintre acestia
		parc = parc->firstChild;
		niv++;
	}
	if (parc)
	{
		print_fin(parc->info->isSelfClosing, out, niv, parc->info->type);
	}
	/*
	In momentul cand se ajunge la tag-ul final, trebuie inchise si tagurile
	parinte
	*/
	while (1)
	{
		niv--;
		if (!parc)
		{
			break;
		}
		parc = parc->parent;
		print_fin(parc->info->isSelfClosing, out, niv, parc->info->type);
	}
}