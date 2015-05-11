/************************************************/
/* Progetto Programmazione Procedurale e Logica */
/* Sessione autunnale 2011/2012                 */
/* Michele Sorcinelli, Matricola n° 248412      */
/************************************************/

/**********************************************************************/
/* Programma per la trasformazione di formule di logica dei predicati */
/*  in forma normale prenessa in formule in forma normale di Skolem   */
/**********************************************************************/


/*****************************/
/* Inclusione delle librerie */
/*****************************/

#include <stdio.h>   /* Libreria Standard di Input/Output del C       */
#include <stdlib.h>  /* Libreria Standard del C                       */
#include <ctype.h>   /* Libreria di analisi e gestione dei caratteri  */

/********************************/
/* Definizione dei tipi di dati */
/********************************/

/* definizione del tipo di dato "tipo di quantificatore" */
typedef enum { esist,
               univ } tipo_quant_t;

/* definizione del tipo di dato "booleano" */
typedef enum { falso,
               vero } bool_t;

/* definizione del tipo di dato "tipo di termine */
typedef enum { var,
               cos,
               fun } tipo_term_t;

/* definizione di un elemento della lista dei quantificatori */
typedef struct elem_lista_quant
{
  tipo_quant_t tipo;               /* tipo di quantificatore                */
  char variab_ass;                 /* variabile associata al quantificatore */
  struct elem_lista_quant *succ_p; /* puntatore all'elemento successivo     */
  struct elem_lista_quant *prec_p; /* puntatore all'elemento precedente     */
} elem_lista_quant_t;              

/* definizione di un elemento della lista degli argomenti di una funzione */
typedef struct elem_lista_arg_funz
{
  char simbolo;                        /* simbolo dell'argomento            */
  struct elem_lista_arg_funz *succ_p;  /* puntatore all'elemento successivo */
  struct elem_lista_arg_funz *prec_p;  /* puntatore all'elemento precedente */
} elem_lista_arg_funz_t;               

/* definizione di un elemento della lista dei termini */
typedef struct elem_lista_term
{
  tipo_term_t tipo;               /* tipo del termine                           */
  char simbolo;                   /* simbolo del termine                        */
  struct elem_lista_term *succ_p; /* puntatore all'elemento successivo          */
  struct elem_lista_term *prec_p; /* puntatore all'elemento precedente          */
  elem_lista_arg_funz_t *testa_p; /* puntatore alla testa della lista argomenti */
} elem_lista_term_t;              

/********************************/
/* Dichiarazione delle funzioni */
/********************************/

void stampa_messaggio_iniziale(void);

bool_t inserisci_quant(tipo_quant_t,
                       char,
                       elem_lista_quant_t **);

void inserisci_term(tipo_term_t,
                    char, 
                    elem_lista_term_t **,
                    elem_lista_arg_funz_t *);

void inserisci_arg_funz(char,
                        elem_lista_arg_funz_t **);

bool_t acquisisci_formula(elem_lista_quant_t **,
			  elem_lista_term_t **,
                          char *,
                          int *);

void elimina_lista_quant(elem_lista_quant_t *);

void elimina_lista_term(elem_lista_term_t *);

void stampa_formula(elem_lista_quant_t *,
                    elem_lista_term_t *,
                    char,
                    int);

void applica_skolem(elem_lista_quant_t **,
                    elem_lista_term_t *,
                    char);

void rimuovi_quant(elem_lista_quant_t **,
                   elem_lista_quant_t *);

void sost_variab_con_cost(elem_lista_term_t *,
                          char);

void sost_variab_con_funz(elem_lista_term_t *,
                          char,
                          char,
                          elem_lista_arg_funz_t *);

/******************************/
/* Definizione delle funzioni */
/******************************/

/* definizione della funzione main */
int main(void)
{
  /* dichiarazione delle variabili locali alla funzione */
  elem_lista_quant_t *testa_quant_p; /* lavoro: puntatore alla testa della lista quantificatori */
  elem_lista_term_t  *testa_term_p;  /* lavoro: puntatore alla testa della lista argomenti      */
  bool_t esito_lett = falso;         /* lavoro: esito della lettura della formula               */
  int  num_neg;                      /* input: numero di negazioni nella formula                */
  char pred;                         /* input: simbolo di predicato della formula               */

  /* stampa a video del messaggio iniziale del programma */
  stampa_messaggio_iniziale();

  /* acquisizione della formula ripetuta finché non è sintatticamente corretta */
  while (esito_lett == falso)
    esito_lett = acquisisci_formula(&testa_quant_p,
                                    &testa_term_p,
                                    &pred,
                                    &num_neg);

  /* applicazione dell'algoritmo di Skolem */
  applica_skolem(&testa_quant_p,
                 testa_term_p,
                 pred);

  /* stampa a video della formula in forma normale di Skolem */
  stampa_formula(testa_quant_p,
                 testa_term_p,
                 pred,
                 num_neg);
 
  return(0);
}

/* definizione della funzione di stampa a video del messaggio iniziale del programma */
void stampa_messaggio_iniziale(void)
{
  /* viene stampato a video il messaggio iniziale del programma */
  printf("\nInserire la formula di logica dei predicati in forma normale prenessa.\n\n"
         "==============================\n"
         "# = esiste\n"
         "* = per ogni\n"
         "^ = operatore di negazione\n"
         "==============================\n\n"
         "Le costanti devono appartenere a {a, b, c, d, e, f, g, h, i, j, k, l, m}\n" 
         "Le variabili devono appartenere a {n, o, p, q, r, s, t, u, v, w, x, y, z}\n"
         "La formula deve contenere un (ed un solo) simbolo di predicato.\n"
         "Il simbolo di predicato dev'essere una lettera maiuscola.\n\n"
         "Esempi di sintassi correttamente acquisibili dal programma:\n\n"
         "1) #x*y^Pxyabc\n"
         "2) #x *y ^P x y a b c\n"
         "3) #x*y ^P x,y,a,b,c\n"
         "4) #x *y ^P x, y, a, b, c\n"
         "5) #x *y ^P (x,y,a,b,c)\n"
         "6) #x *y ^P(x,y,a,b,c) \n"
         "7) #x*y ^P (x, y, a, b, c)\n\n"
         "... ");
}

/* definizione della funzione di inserimento di un quantificatore in lista */
bool_t inserisci_quant(tipo_quant_t tipo,            /* input: tipo del quantificatore                 */
                       char variab_ass,              /* input: variabile associata al quantificatore   */
                       elem_lista_quant_t **testa_p) /* lavoro: punt. all'ind. della testa della lista */
{  
  /* dichiarazione delle variabili locali alla funzione */
  elem_lista_quant_t *nuovo_p, /* lavoro: puntatore al nuovo elemento da inserire in lista */
                     *corr_p;  /* lavoro: puntatore di supporto all'inserimento            */
  bool_t esito_inserimento;    /* lavoro: valore di esito dell'inserimento                 */
  
  /* si scorre la lista finché non viene trovata la variabile associata 
     al quantificatore da inserire e non viene oltrepassata la fine     */
  for(corr_p = *testa_p;
      (corr_p != NULL && corr_p->variab_ass != variab_ass);
      corr_p = corr_p->succ_p);

  /* nel caso in cui non è stata trovata la variabile all'interno della lista */
  if (corr_p == NULL)
  {
    /* si segnala l'esito positivo dell'inserimento */
    esito_inserimento = vero;

    /* viene allocato spazio in memoria per il nuovo elemento */
    nuovo_p = (elem_lista_quant_t *)malloc(sizeof(elem_lista_quant_t));

    /* vengono assegnati i valori al nuovo elemento */
    nuovo_p->tipo = tipo;
    nuovo_p->variab_ass = variab_ass;

    /* se la lista è vuota il nuovo elemento viene inserito in testa alla lista */
    if (*testa_p == NULL)
    {
      nuovo_p->succ_p = NULL;
      nuovo_p->prec_p = NULL;
      *testa_p = nuovo_p;
    }
 
    /* altrimenti il nuovo elemento viene inserito in fondo alla lista */
    else 
    {
      for (corr_p = *testa_p; 
           (corr_p->succ_p != NULL); 
           corr_p = corr_p->succ_p);
    
      corr_p->succ_p = nuovo_p;
      nuovo_p->succ_p = NULL;
      nuovo_p->prec_p = corr_p;
    }
  }

  else
    /* viene segnalato l'esito negativo dell'inserimento */
    esito_inserimento = falso;

  /* viene restituito l'esito dell'inserimento */
  return(esito_inserimento);

}
 
/* definizione della funzione di inserimento di un termine in lista */ 
void inserisci_term(tipo_term_t tipo,                   /* input: tipo del termine                        */
                    char simbolo,                       /* input: simbolo del termine                     */
                    elem_lista_term_t **testa_p,        /* lavoro: punt. all'ind. della testa della lista */
                    elem_lista_arg_funz_t *testa_arg_p) /* lavoro: punt. alla testa della lista argomenti */
{

  /* dichiarazione delle variabili locali alla funzione */ 
  elem_lista_term_t *nuovo_p, /* lavoro: puntatore al nuovo elemento da inserire in lista */
                    *corr_p;  /* lavoro: puntatore di supporto all'inserimento            */
  
  /* viene allocato spazio in memoria per il nuovo elemento */
  nuovo_p = (elem_lista_term_t *)malloc(sizeof(elem_lista_term_t));

  /* vengono assegnati i valori al nuovo elemento */
  nuovo_p->tipo = tipo;
  nuovo_p->simbolo = simbolo;
  nuovo_p->testa_p = testa_arg_p;

  /* se la lista è vuota il nuovo elemento viene inserito in testa alla lista */
  if (*testa_p == NULL)
  {
    nuovo_p->succ_p = NULL;
    nuovo_p->prec_p = NULL;
    *testa_p = nuovo_p;
  }
 
  /* altrimenti il nuovo elemento viene inserito in fondo alla lista */
  else 
  {
    for (corr_p = *testa_p; 
         (corr_p->succ_p != NULL); 
         corr_p = corr_p->succ_p);
    
    corr_p->succ_p = nuovo_p;
    nuovo_p->succ_p = NULL;
    nuovo_p->prec_p = corr_p;
  }

}

/* definizione della funzione di inserimento di un argomento funzione in lista */
void inserisci_arg_funz(char simbolo,                    /* input: simbolo dell'argomento                  */
                        elem_lista_arg_funz_t **testa_p) /* lavoro: punt. all'ind. della testa della lista */
{
  /* dichiarazione delle variabili locali alla funzione */ 
  elem_lista_arg_funz_t *nuovo_p, /* lavoro: puntatore al nuovo elemento da inserire in lista */ 
                        *corr_p;  /* lavoro: puntatore di supporto all'inserimento            */
  
  /* viene allocato spazio in memoria per il nuovo elemento */
  nuovo_p = (elem_lista_arg_funz_t *)malloc(sizeof(elem_lista_arg_funz_t));

  /* viene assegnato il simbolo argomento al nuovo elemento */
  nuovo_p->simbolo = simbolo;

  /* se la lista è vuota il nuovo elemento viene inserito in testa alla lista */
  if (*testa_p == NULL)
  {
    nuovo_p->succ_p = NULL;
    nuovo_p->prec_p = NULL;
    *testa_p = nuovo_p;
  }


  /* altrimenti il nuovo elemento viene inserito in fondo alla lista */
  else 
  {
    for (corr_p = *testa_p; 
         (corr_p->succ_p != NULL); 
         corr_p = corr_p->succ_p);
    
    corr_p->succ_p = nuovo_p;
    nuovo_p->succ_p = NULL;
    nuovo_p->prec_p = corr_p;
  }
}

/* definizione della funzione di acquisizione della formula */
bool_t acquisisci_formula(elem_lista_quant_t **testa_quant_p, /* lavoro: punt. all'ind. della testa della lista quan. */ 
                          elem_lista_term_t **testa_term_p,   /* lavoro: punt. all'ind. della testa della lista term. */
                          char *pred,                         /* input: punt. al simbolo di predicato della formula   */
                          int *num_neg)                       /* input: punt. al numero di negazioni nella formula    */
{
  /* dichiarazione delle variabili locali alla funzione */ 
  bool_t esito_lett = vero,          /* lavoro: esito della lettura della formula */
         aper_par = falso,           /* lavoro: apertura della parentesi          */
         chius_par = falso;          /* lavoro: chiusura della parentesi          */
  char carattere = ' ';              /* input: carattere acquisito                */
  tipo_quant_t tipo_quant_acquisito; /* input: tipo del quantificatore acquisito  */
  tipo_term_t  tipo_term_acquisito;  /* input: tipo del termine acquisito         */

  /* vengono inizializzate le variabili */
  *testa_quant_p = NULL;
  *testa_term_p = NULL;
  *pred = '~';
  *num_neg = 0;

  /* finché non ci sono errori di lettura e il carattere acquisito è diverso da invio */
  while (esito_lett == vero &&
         carattere != '\n')
  {
    /* si prosegue fino alla prima occorrenza di un carattere diverso dallo spazio e dal tab */
    while ((carattere == ' ') ||
           (carattere == '\t'))
      carattere = getchar();

    /* se il carattere acquisito è relativo a un quantificatore, 
       e ci troviamo nella prima parte della formula             */
    if ((carattere == '#' || carattere == '*') && 
        (*pred == '~') && 
        (*num_neg == 0))
    {
      /* viene valutato il tipo del quantificatore */
      tipo_quant_acquisito = ((carattere == '#')?
                              esist:
                              univ);

      /* si passa al successivo carattere della formula diverso dallo spazio e dal tab */
      do
        carattere = getchar();
      while ((carattere == ' ') ||
             (carattere == '\t'));

      /* se il carattere non è una lettera dell'alfabeto minuscola */
      if (islower(carattere) == 0)
      {
        /* viene avvisato l'utente e si registra l'errore */
        printf("Dopo un simbolo di quantificatore è necessario inserire una variabile.");
        esito_lett = falso;
      }

      /* altrimenti, se il carattere non è alfabeticamente compreso tra 'n' e 'z' */
      else if ((carattere < 'n') || 
               (carattere > 'z'))
      {
        /* si avvisa l'utente e viene registrato l'errore */
        printf("Le variabili devono appartenere a {n, o, p, q, r, s, t, u, v, w, x, y, z}");
        esito_lett = falso;
      }

      else 
        /* viene tentato l'inserimento del quantificatore in lista, e se la variabile 
           associata è già stata usata si segnala l'errore e si avvisa l'utente       */
        if (inserisci_quant(tipo_quant_acquisito, carattere, testa_quant_p) == falso)
        {
          esito_lett = falso;
          printf("Non è possibile utilizzare lo stesso simbolo di variabile in più quantificatori.");
        }
    } 

    /* altrimenti, se il carattere è un operatore di negazione 
       e il simbolo di predicato non è già stato acquisito     */
    else if ((carattere == '^') && 
             (*pred == '~'))
      /* viene incrementato il numero di negazioni */
      (*num_neg)++;

    /* altrimenti, se il carattere è maiuscolo e il simbolo predicato non è già stato acquisito */
    else if ((isupper(carattere) != 0) && 
             (*pred == '~'))
      /* viene registrato il carattere come simbolo di predicato */
      *pred = carattere;

    /* altrimenti, se il carattere è minuscolo, il predicato     
       è già stato acquisito e la parentesi non è stata chiusa   */ 
    else if ((islower(carattere) != 0) &&
             (*pred != '~') && 
             (chius_par == falso))
    {
      /* viene valutato il tipo del termine */
      tipo_term_acquisito = ((carattere < 'n')?
                             cos:
                             var);

      /* viene inserito il termine in lista */
      inserisci_term(tipo_term_acquisito,
                     carattere,
                     testa_term_p,
                     NULL);
    } 

    /* se il carattere è '(', il predicato è stato acquisito, la
       lista termini non è vuota, e la parentesi non è già stata
       aperta */                
    else if ((carattere == '(') && 
             (*pred != '~') &&
             (*testa_term_p == NULL) &&
             (aper_par != vero))
      /* viene registrata l'apertura della parentesi */
      aper_par = vero;

    /* altrimenti, se il carattere è una virgola,la lista termini non è vuota, 
       e la parentesi non è già stata chiusa, il carattere viene ignorato     */
    else if ((carattere == ',') && 
            (*testa_term_p != NULL) && 
            (chius_par == falso));

    /* se il carattere è ')' ed è stata aperta una parentesi ma non è stata già chiusa */
    else if ((carattere == ')') &&  
             (aper_par == vero) &&
             (chius_par != vero))
      /* viene registrata la chiusura della parentesi */
      chius_par = vero;
      
    else if (carattere != '\n')
      /* viene registrato l'errore di lettura */
      esito_lett = falso;


    /* se la lettura non è andata a buon fine */
    if (esito_lett == falso)
    {
      /* viene pulito il buffer del file stdin */
      while (carattere != '\n')
        carattere = getchar();

      /* se la lista quantificatori non è vuota viene eliminata */
      if (*testa_quant_p != NULL)
        elimina_lista_quant(*testa_quant_p);

      /* se la lista termini non è vuota viene eliminata */
      if (*testa_term_p != NULL)
        elimina_lista_term(*testa_term_p);
    }
    
    else if (carattere != '\n')
        /* viene acquisito il carattere successivo */
        carattere = getchar();

  }

  /* se è stata aperta la parentesi ma non è stata chiusa,
     oppure non è stato acquisito il simbolo di predicato,
     oppure non è stato acquisito neanche un termine      */
  if ((aper_par != chius_par) || 
      (*pred == '~') ||
      (*testa_term_p == NULL))
    /* viene registrato l'errore di lettura */
    esito_lett = falso;


  /* se la lettura non è andata a buon fine viene avvisato l'utente */
  if (esito_lett == falso)
    printf("\nLa formula inserita non è valida... riprovare.\n... ");

  /* viene restiutito l'esito della lettura alla funzione main */
  return(esito_lett);
}

/* definizione della funzione per eliminare la lista quantificatori */
void elimina_lista_quant(elem_lista_quant_t *testa_p) /* lavoro: puntatore alla testa della lista */
{
  
  /* fintanto che la testa non ha il puntatore all'elemento successivo nullo */
  while (testa_p->succ_p != NULL)
  {
    /* l'elemento successivo della testa diventa la testa */
    testa_p = testa_p->succ_p;
    
    /* segna la zona di memoria riservata all'elemento che precede la testa come riallocabile */ 
    free(testa_p->prec_p);
  } 
   
  /* segna la zona di memoria riservata alla testa come riallocabile */
  free(testa_p);
}

/* definizione della funzione per eliminare la lista quantificatori */
void elimina_lista_term(elem_lista_term_t *testa_p) /* lavoro: puntatore alla testa della lista */
{
  
  /* fintanto che la testa non ha il puntatore all'elemento successivo nullo */
  while (testa_p->succ_p != NULL)
  {
    /* l'elemento successivo della testa diventa la testa */
    testa_p = testa_p->succ_p;
    
    /* segna la zona di memoria riservata all'elemento che precede la testa come riallocabile */ 
    free(testa_p->prec_p);
  } 
  
  /* segna la zona di memoria riservata alla testa come riallocabile */
  free(testa_p);
}

/* definizione della funzione per la stampa della formula */
void stampa_formula(elem_lista_quant_t *testa_quant_p, /* lavoro: punt. alla testa della lista quant.  */
                    elem_lista_term_t  *testa_term_p,  /* lavoro: punt. alla testa della lista termini */
                    char pred,                         /* input: simbolo di predicato della formula    */
                    int num_neg)                       /* input: numero di negazioni nella formula     */
{
  /* dichiarazione delle variabili locali alla funzione */
  
  elem_lista_quant_t *quant_corr_p;  /* lavoro: puntatore al quantificatore da stampare */
  elem_lista_term_t *term_corr_p;    /* lavoro: puntatore al termine da stampare        */
  elem_lista_arg_funz_t *arg_corr_p; /* lavoro: puntatore all'argomento da stampare     */
  int i;                             /* lavoro: indice di supporto                      */

  printf("\nLa formula ottenuta dall'applicazione dell'algoritmo di Skolem è:\n");

  /* il ciclo percorre la lista quantificatori fino all'ultimo elemento */
  for (quant_corr_p = testa_quant_p;
       (quant_corr_p != NULL);
       quant_corr_p = quant_corr_p->succ_p)
  {
    /* stampa del quantificatore */
    printf("*%c", 
           quant_corr_p->variab_ass); 
    
    /* viene stampato uno spazio dopo l'ultimo quantificatore */
    if (quant_corr_p->succ_p == NULL)
      printf(" ");
  }

  /* se il numero di negazioni è dispari viene stampato l'operatore di negazione */
  if ((num_neg % 2) == 1)
    printf("^");

  /* stampa del predicato */
  printf("%c\(", 
         pred);

  /* il ciclo percorre la lista termini fino all'ultimo elemento */
  for (term_corr_p = testa_term_p, i = 0;
       (term_corr_p != NULL);
       term_corr_p = term_corr_p->succ_p)
  {
    /* se il termine è una funzione */
    if (term_corr_p->tipo == fun)
    {
      /* stampa del simbolo di funzione e di '(' */
      printf("%c\(", 
      term_corr_p->simbolo);

      for (arg_corr_p = term_corr_p->testa_p;
           (arg_corr_p != NULL);
           arg_corr_p = arg_corr_p->succ_p)
      {
        /* stampa degli argomenti */
        printf("%c", 
               arg_corr_p->simbolo);

        /* se l'argomento non è l'ultimo viene stampato ", " */
        if (arg_corr_p->succ_p != NULL)
          printf(", ");
      }
      /* stampa di ')' */
      printf(")");
    }
   
    /* altrimenti, se il termine è una costante, ma il simbolo non è tra 'a' e 'm' */
    else if ((term_corr_p->tipo == cos) &&
             (term_corr_p->simbolo < 'a' || term_corr_p->simbolo > 'm'))
    {
      /* viene incrementato i e stampata la costante come c con indice i */
      i++;
      printf("c%d",
             i);
    }
    
    else
      /* viene stampato il termine */
      printf("%c", 
             term_corr_p->simbolo);

    /* se ci sono altri termini da stampare viene stampato ", " */
    if (term_corr_p->succ_p != NULL)
      printf(", ");
  }
  
  /* vengono stampati ')' e i caratteri di new line */
  printf(")\n\n");
}

/* definizione della funzione che implementa l'algoritmo di Skolem */
void applica_skolem(elem_lista_quant_t **testa_quant_p, /* lavoro: punt. all'ind. della testa della lista quant. */
                    elem_lista_term_t *testa_term_p,    /* lavoro: puntatore alla testa della lista dei termini  */
                    char pred)                          /* input: simbolo di predicato della formula             */
{
  /* dichiarazione delle variabili locali alla funzione */
  elem_lista_quant_t *quant_corr_p = *testa_quant_p, /* lavoro: puntatore al quantificatore da elaborare */
                     *quant_supp_p;                  /* lavoro: puntatore di supporto                    */
  elem_lista_arg_funz_t *testa_arg_funz_p;           /* lavoro: puntatore alla testa argomenti funzioni  */
  char variab_corr;                                  /* lavoro: variabile da elaborare                   */

  /* se la testa della lista contiene un quantificatore di tipo esistenziale */
  if ((quant_corr_p != NULL) && 
      (quant_corr_p->tipo == esist))
  {
    /* viene registrata la variabile associata al quantificatore */
    variab_corr = quant_corr_p->variab_ass;

    /* viene rimosso il quantificatore */
    rimuovi_quant(testa_quant_p,
                  quant_corr_p);

    /* viene sostiuita la variabile associata al quantificatore rimosso con una costante */
    sost_variab_con_cost(testa_term_p,
                         variab_corr); 

    /* l'elaborazione passa al quantificatore successivo (se esiste) */
    quant_corr_p = *testa_quant_p;
  }
    
  /* fintanto che abbiamo un quantificatore da elaborare */
  while (quant_corr_p != NULL)
  {
    /* se il quantificatore è di tipo esistenziale */
    if (quant_corr_p->tipo == esist)
    {
      /* viene inizializzata la testa alla lista degli argomenti funzioni */
      testa_arg_funz_p = NULL;
     
      /* vengono inseriti nella lista argomenti le variabili associate ai quantificatori
         di tipo universale che precedono il quantificatore che si sta elaborando        */
      for (quant_supp_p = *testa_quant_p;
           (quant_supp_p != NULL && quant_supp_p->tipo == univ);
           quant_supp_p = quant_supp_p->succ_p)
        inserisci_arg_funz(quant_supp_p->variab_ass,
                           &testa_arg_funz_p);

      /* NOTA: alla fine del ciclo for il puntatore di supporto punta al quantificatore da rimuovere */

      /* viene registrata la variabile associata al quantificatore */
      variab_corr = quant_corr_p->variab_ass;

      /* viene rimosso il quantificatore */
      rimuovi_quant(testa_quant_p,
                    quant_supp_p);

      /* se la lista argomenti non è vuota */
      if (testa_arg_funz_p != NULL)
        /* viene sostiuita la variabile associata al quantificatore rimosso con una funzione */
        sost_variab_con_funz(testa_term_p,
                             variab_corr,
                             pred,
                             testa_arg_funz_p);

      else 
        /* viene sostituita la variabile associata al quantificatore rimosso con una costante */
        sost_variab_con_cost(testa_term_p,
                             variab_corr); 
    }

    /* l'elaborazione passa al quantificatore successivo (se esiste) */
    quant_corr_p = quant_corr_p->succ_p;
  }
  
}

/* definizione della funzione per la rimozione di un quantificatore */
void rimuovi_quant(elem_lista_quant_t **testa_p, /* lavoro: punt. all'ind. della testa della lista */
                   elem_lista_quant_t *quant_p)  /* lavoro: punt. al quantificatore da rimuovere   */
{

  /* se il quantificatore si trova in testa alla lista */
  if (quant_p == *testa_p)
  { 
    /* se esiste un quantificatore successivo a quello da rimuovere */
    if (quant_p->succ_p != NULL)
    {
      /* il quantificatore successivo a quello da rimuovere diventa la testa */
      quant_p->succ_p->prec_p = NULL;
      *testa_p = quant_p->succ_p;
    }

    else 
      /* viene reinizializzata la testa a NULL */
      *testa_p = NULL;
  }

  /* altrimenti, se il quantificatore da rimuovere si trova in fondo alla lista */
  else if (quant_p->succ_p == NULL) 
  {
    /* viene assegnato NULL al puntatore succ_p del quantificatore precedente a quello da rimuovere */
    quant_p->prec_p->succ_p = NULL;
  }

  else 
  {
    /* viene collegato il quantificatore che precede quello da rimuovere
       con il quantificatore che segue quello da rimuovere               */
    quant_p->prec_p->succ_p = quant_p->succ_p;
    quant_p->succ_p->prec_p = quant_p->prec_p;
  }

  /* viene segnata la zona di memoria riservata al quantificatore come riallocabile */
  free(quant_p);
}

/* definizione della funzione per la sostituzione variabile/costante */
void sost_variab_con_cost(elem_lista_term_t *testa_p, /* lavoro: puntatore alla testa della lista termini */
                          char variab)                /* input: simbolo della variabile da sostituire     */
{
  /* dichiarazione delle variabili locali alla funzione */
  char cost = 'a';                     /* output: simbolo di costante che verrà usato nella sostituzione */
  elem_lista_term_t *corr_p = testa_p; /* lavoro: puntatore di supporto alla sostituzione                */

  /* il ciclo cambia il valore del simbolo di costante finché non
     ne trova uno che non sia già presente nella lista dei termini */
  while (corr_p != NULL)
  {
    if (corr_p->simbolo != cost)
      corr_p = corr_p->succ_p;

    else 
    {
      corr_p = testa_p;
      cost++;
    }
  }
 
  /* viene sostituita ogni occorrenza della variabile con la costante */
  for (corr_p = testa_p;
       (corr_p != NULL);
       corr_p = corr_p->succ_p)

    if (corr_p->simbolo == variab)
    {
       corr_p->simbolo = cost;
       corr_p->tipo = cos;
    } 
}

/* definizione della funzione per la sostituzione variabile/funzione */
void sost_variab_con_funz(elem_lista_term_t *testa_term_p,         /* lavoro: punt. alla testa della lista termini   */
                          char variab,                             /* input: simbolo della variabile da sostituire   */
                          char pred,                               /* input: simbolo di predicato della formula      */
                          elem_lista_arg_funz_t *testa_arg_funz_p) /* lavoro: punt. alla testa della lista argomenti */
{
  /* dichiarazione delle variabili locali alla funzione */
  char simbolo_di_funz = 'F';               /* output: simbolo della funzione da utilizzare nella sostituzione */
  elem_lista_term_t *corr_p = testa_term_p; /* lavoro: puntatore di supporto                                   */
  
  /* il ciclo cambia il valore del simbolo di funzione finché non
     ne trova uno che non sia già presente nella lista dei termini
     e che sia diverso dal simbolo di predicato della formula      */
  while (corr_p != NULL)
  {
    if (corr_p->simbolo != simbolo_di_funz &&
        simbolo_di_funz != pred)
      corr_p = corr_p->succ_p;

    else 
    {
      corr_p = testa_term_p;
      simbolo_di_funz++;
    }
  }

  /* viene sostituita ogni occorrenza della variabile con la funzione */
  for (corr_p = testa_term_p;
       (corr_p != NULL);
       corr_p = corr_p->succ_p)

    if (corr_p->simbolo == variab)
    {
       corr_p->simbolo = simbolo_di_funz;
       corr_p->tipo = fun;
       corr_p->testa_p = testa_arg_funz_p;
    } 
}
