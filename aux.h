/* Radu Cosmin - 313 CB */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//Functie pentru determinarea selectorului dintr-una din ultimele comenzi
char* det_selector(char* comanda);

//Functie pentru a determina daca un tag are o anumita clasa
int det_class(char* selector, TArb nod);

//Functie pentru a determina daca un tag este de tipul dat de selector
int det_tag_type(char* selector, TArb nod);

//Functie pentru a determina daca un tag are un anumit tip si o anumita clasa
int det_tag_class(char* selector, TArb nod);

/*
Functie pentru a determina daca un nod cu un anumit tip, are un parinte cu un
anumit tip
*/
int det_parinte(char* selector, TArb nod);

/*
Functie pentru a determina daca un nod cu un anumit tip, are un stramos cu un
anumit tip
*/
int det_stramos(char* selector, TArb nod);

//Functie pentru cautarea unui nod pe baza id-ului
TArb cauta_id(TArb arb, char* id);