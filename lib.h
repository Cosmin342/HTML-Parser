/* Radu Cosmin - 313 CB */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define IC(a) (((AQ)(a))->ic)
#define SC(a) (((AQ)(a))->sc)

#define MAX_TYPE		30
#define MAX_VAR			130
#define MAX_CONTENT		300

typedef struct TNodAttr {
	char *name;
	char *value;
	struct TNodAttr *next;
} TNodAttr, *TAttr;

typedef struct TNodInfo {
	char *type;
	char *id;
	TAttr style;
	TAttr otherAttributes;
	int isSelfClosing;
	char *contents;
} TNodInfo, *TInfo;

typedef struct TNodArb {
	TInfo info;
	struct TNodArb *nextSibling;
	struct TNodArb *firstChild;
	struct TNodArb *parent;
} TNodArb, *TArb;

//Structura unei celule a listei generice pt stiva sau coada
typedef struct cel{
	struct cel *urm;
	void* info;
} TCel, *ACel;

//Structura coada
typedef struct coada{
	size_t dime;
	ACel ic, sc;
} TCoada, *AQ;

typedef enum
{
    PARSE_ERROR             = 0,
    PARSE_CONTENTS          = 1,
    PARSE_OPENING_BRACKET   = 2,
    PARSE_TAG_TYPE          = 3,
    PARSE_CLOSING_TAG       = 4,
    PARSE_REST_OF_TAG       = 5,
    PARSE_ATTRIBUTE_NAME    = 6,
    PARSE_ATTRIBUTE_EQ      = 7,
    PARSE_ATTRIBUTE_VALUE   = 8,
    PARSE_SELF_CLOSING      = 9,
} TParseState;

/*
Pointer la o functie ce intoarce un intreg, utilizat la ultimele
functii
*/
typedef int (*TFC)(char*, TArb);

//Functie pentru initializarea cozii
void* initq(size_t d);

//Introducere element in coada
int intrq(void* q, void* ae);

//Extragere element in coada
int extrq(void* q, void** ae);

//Distrugere coada
void distr_q(void* q);

//Functia pentru copiere si alocare, deoarece nu exista in c99
char* strdup(char* sir);

//Functia care creaza un nou nod
TArb creaza(TArb* arbore_html, TArb* nod_curent);

//Functia pentru interpretarea codului html
TParseState interpret(TParseState currentState, char c);

//Functie pentru alocarea unui nod si a campului info al acestuia
TArb aloca_nod_arb();

//Functie folosita la creearea listei de atribute pentru style
void prelucreaza_style(TArb arb, char* attr);

//Functie folosita la creearea listei de alte atribute
void prelucreaza_atribut(TArb arb, char* attr, char* attr_value);

//Functie pentru legarea content-ului la campul corespunzator al unui nod
void prelucreaza_content(TArb arb, char* content);

//Functie pentru legarea unui nod la nod-ul curent
void leaga(TArb arbore_html, char* type, TArb nod, TArb *nod_curent);

//Functia de adaugare a unui nod in arbore
void distruge_arbore(TArb* arb);

//Afisarea propriu-zisa a arborelui, prin parcurgerea lui in adancime
void format(TArb arb, FILE* out);

//Functia de adaugare a unui nod in arbore
void add(char* com, TArb *arb, FILE* out);

/*
Functia care determina ce pointer la functie trebuie folosit in suprascrierea
listei de la style sau daca trebuie apelata functia de cautare a id-ului
*/
void over(TArb arb, char* com, FILE* out);

/*
Functia care determina ce pointer la functie trebuie folosit in updatarea
listei de la style sau daca trebuie apelata functia de cautare a id-ului
*/
void append(TArb arb, char* com, FILE* out);

/*
Functia care determina ce pointer la functie trebuie folosit in stergerea
recursiva sau daca trebuie apelata functia de cautare a id-ului
*/
void delete_rec(TArb arb, char* com, FILE* out);