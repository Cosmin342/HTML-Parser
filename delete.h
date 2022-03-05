/* Radu Cosmin - 313 CB */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//Functie pentru stergerea recursiva a unui subabore
void distruge_recursiv(TArb* arb);

//Functie pentru stergerea informatiilor unui tag
void distruge_info_nod(TInfo* info);

//Functie pentru distrugerea unei liste de atribute
void distruge_atribute(TAttr* atribute);

//Functie pentru stergerea unui nod al arborelui
void distruge_nod(TArb *nod);