/*
    MOVHEX — Prova finale Algoritmi e Strutture Dati 2024–2025
*/
//LIBRERIE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

//DEFINIZIONI

#define MAXR 5
#define MINV (-10)
#define MAXV 10
#define MIN_COSTO 1
#define MAX_COSTO 100
#define LATI 6
#define INTRANSITABILE 0


//STRUTTURE DATI

typedef struct rotta_aerea_s {
    int to_x;
    int to_y;
    int costo;
} rotta_aerea_t;

typedef struct cella_s {
    int costo;
    unsigned char n_rotte;
    rotta_aerea_t rotte[MAXR];
} cella_t;

typedef struct mappa_s {
    int map_x;
    int map_y;
    cella_t *celle;
} mappa_t;

typedef struct nodo_bfs_s {
    int x;
    int y;
} nodo_bfs_t;

typedef struct nodo_heap_s {
    int x;
    int y;
    int sommacosti;
    int priorita;
} nodo_heap_t;

typedef struct min_heap_s {
    nodo_heap_t *nodi;
    int dim;
    int quantita;
} min_heap_t;

//PROTOTIPI FUNZIONI

static inline cella_t * trovacella(mappa_t *, int, int);
bool coordinate_valide(int, int);
static inline int floor_per_divisione(int, int);
static inline int rispettalimiti(int);
static void coordinate_cubiche(int, int, int*, int*, int*);
int distanza_esagoni(int, int, int, int);
bool uscita_possibile(int);
min_heap_t * crea_heap(int);
void libera_heap (min_heap_t *);
void heap_push (min_heap_t *, int, int, int, int);
nodo_heap_t heap_pop(min_heap_t *);
void init (int, int);
void change_cost (int, int, int, int);
int travel_cost (int, int, int, int);
void toggle_air_route (int, int, int, int);
void gestisci_richieste (char *);

//VARIABILI GLOBALI

char comando[20] = "";
mappa_t *map = NULL;

//se riga è pari --> colonna spostata verso sx
static const int indici_vicini_riga_pari[6][2] =
        {
        { 1,  0},  // dx
        { 0, -1},  // dx alto
        {-1, -1},  // sx alto
        {-1,  0},  // sx
        {-1,  1},  // sx basso
        { 0,  1}   // dx basso
        };

// se riga è dispari --> colonna spostata verso dx
static const int indici_vicini_riga_dispari[6][2] =
        {
        { 1,  0},  // dx
        { 1, -1},  // dx alto
        { 0, -1},  // sx alto
        {-1,  0},  // sx
        { 0,  1},  // sx basso
        { 1,  1}   // dx basso
        };

//FUNZIONI UTILI

//trova cella nella mappa
static inline cella_t * trovacella(mappa_t *mappa, int x, int y)
{
    cella_t *c = NULL;
    c = &mappa->celle[y * mappa->map_x + x];
    return c;
}

//le coordinate sono valide?
bool coordinate_valide(int x, int y)
{
    bool ris = false;
    if (map != NULL)
    {
        if((x >= 0 && x < map->map_x) && (y >= 0 && y < map->map_y))
        {
            ris = true;
        }
    }
    return ris;
}

//divisione safe in change_cost
static inline int floor_per_divisione(int a,int b)
{
    int ris;
    if (a >= 0)
    {
        ris = a / b;
    }
    else
    {
        ris = - ((-a + b - 1) / b); //se numero negativo voglio floor e non troncamentp verso 0
    }
    return ris;
}

//mi assicuro che costo sia >= 0 e < 100
static inline int rispettalimiti(int val)
{
    int ris;
    if (val < MIN_COSTO)
    {
        ris = INTRANSITABILE;
    }
    else if (val > MAX_COSTO)
    {
        ris = MAX_COSTO;
    }
    else
    {
        ris = (int)val;
    }
    return ris;
}


//supporto per distanza cubica
static void coordinate_cubiche(int col, int rig, int *cx, int *cy, int *cz)
{
    int x = col - ((rig - (rig & 1)) / 2);
    int z = rig;
    int y = -x - z;

    *cx = x;
    *cy = y;
    *cz = z;
}


//calcolo distanza esagoni
int distanza_esagoni(int xp, int yp, int xd, int yd)
{
    int x1, y1, z1;
    int x2, y2, z2;
    int x_def, y_def, z_def;
    int ris;

    coordinate_cubiche(xp, yp, &x1, &y1, &z1);
    coordinate_cubiche(xd, yd, &x2, &y2, &z2);

    x_def = abs(x1 - x2);
    y_def = abs(y1 - y2);
    z_def = abs(z1 - z2);
    ris = (x_def + y_def + z_def) / 2;
    return ris;
}


//posso uscire dalla cella?
bool uscita_possibile(int costo)
{
    bool ris;
    if (costo > INTRANSITABILE)
        ris = true;
    else
        ris = false;
    return ris;
}

//FUNZIONI PER HEAP

//crea heap
min_heap_t * crea_heap(int quant)
{
    min_heap_t *h = (min_heap_t*) malloc (sizeof(min_heap_t));
    h->nodi = (nodo_heap_t *) malloc(quant * sizeof(nodo_heap_t));
    h->dim = 0;
    h->quantita = quant;
    return h;
}

//elimina heap
void libera_heap(min_heap_t * h)
{
    if(h != NULL)
    {
        free(h->nodi);
        free(h);
    }
}

//inserisci elemento in heap
void heap_push(min_heap_t * h, int x, int y, int somma, int prior)
{
    int stop = 0;
    if (h->dim >= h->quantita) {
        int nuovospazio = h->quantita * 2;
        nodo_heap_t *temp = realloc(h->nodi, (size_t) nuovospazio * sizeof(nodo_heap_t));
        if (temp != NULL) {
            h->nodi = temp;
            h->quantita = nuovospazio;
        } else {
            return;
        }
    }

    int i = h->dim++;
    while (i > 0 && stop == 0)
    {
        int val = (i - 1) / 2;
        if (h->nodi[val].priorita > prior)
        {
            h->nodi[i] = h->nodi[val];
            i = val;
        }
        else
        {
            stop = 1;
        }
    }

    h->nodi[i] = (nodo_heap_t) {x, y, somma, prior};
}

//rimuovi elemento dall'heap e ritorna
nodo_heap_t heap_pop(min_heap_t *h)
{
    int dim = h->dim;
    int stop = 0;
    nodo_heap_t ris = h->nodi[0];
    nodo_heap_t ultimo = h->nodi[dim - 1];
    int i = 0;
    h->dim--;
    while (i * 2 + 1 < h->dim && stop == 0)
    {
        int figlio = i * 2 + 1;
        if (figlio + 1 < h->dim && h->nodi[figlio + 1].priorita < h->nodi[figlio].priorita)
        {
            figlio++;
        }
        if (ultimo.priorita <= h->nodi[figlio].priorita)
        {
            stop = 1;
        }
        h->nodi[i] = h->nodi[figlio];
        i = figlio;
    }
    h->nodi[i] = ultimo;
    return ris;
}



//FUNZIONI PRINCIPALI

//INIT
void init (int x, int y)
{
    //se non nulla
    if(map != NULL)
    {
        free(map->celle);
        free(map);
        map = NULL;
    }

    //sempre
    map = (mappa_t *) malloc (sizeof (mappa_t));
    if(map != NULL)
    {
        map->map_x = x ;
        map->map_y = y ;

        size_t tot = (size_t) map->map_x * (size_t) map->map_y;
        map->celle = (cella_t *) malloc(tot * sizeof(cella_t));

        if (map->celle != NULL)
        {
            for (int j = 0; j < map->map_y; j++)
            {
                for (int i = 0; i < map->map_x; i++)
                {
                    cella_t *c = &map->celle[j * map->map_x + i];
                    c->costo = 1;
                    c->n_rotte = 0;
                }
            }
            printf ("OK\n");
        }
        else printf("KO\n");
    }
    else printf("KO\n");
}

//CHANGE_COST
void change_cost(int cx, int cy, int v, int raggio)
{
    int fattore;
    int modifica;
    int totcelle = map->map_x * map->map_y;

    char * visti = (char*) calloc(totcelle, sizeof(char));
    nodo_bfs_t * coda = (nodo_bfs_t *) malloc (sizeof(nodo_bfs_t) * totcelle);
    nodo_bfs_t curr;

    if(coda != NULL && visti != NULL)
    {
        int inizio = 0;
        int fine = 0;

        coda[fine++] = (nodo_bfs_t) {cx, cy};
        visti[cy * map->map_x + cx] = 1;

        while (inizio < fine)
        {
            curr = coda[inizio++];
            int dist = distanza_esagoni(curr.x, curr.y, cx, cy);

            if (dist < raggio)
            {
                cella_t *c = trovacella(map, curr.x, curr.y);

                fattore = v * (raggio - dist);
                modifica = floor_per_divisione(fattore, raggio);

                if (modifica != 0)
                {
                    //modifica costo cella
                    c->costo = rispettalimiti(c->costo + modifica);

                    //modifica costo rotte da cella
                    for (int i = 0; i < c->n_rotte; i++)
                    {
                        c->rotte[i].costo = rispettalimiti(c->rotte[i].costo + modifica);
                    }
                }
            }

            // Espandi vicini
            if (dist + 1 < raggio)
            {
                const int (*direzioni) [2];
                if(curr.y % 2 == 0)
                {
                    direzioni = indici_vicini_riga_pari;
                }
                else
                {
                    direzioni = indici_vicini_riga_dispari;
                }

                for (int i = 0; i < LATI; i++)
                {
                    int nx = curr.x + direzioni[i][0];
                    int ny = curr.y + direzioni[i][1];

                    if (coordinate_valide(nx, ny))
                    {
                        int idx = ny * map->map_x + nx;
                        if (!visti[idx])
                        {
                            visti[idx] = 1;
                            coda[fine++] = (nodo_bfs_t) {nx, ny};
                        }
                    }
                }
            }
        }
    }

    if(visti != NULL)
    {
        free(visti);
    }
    if (coda != NULL)
    {
        free(coda);
    }
    printf("OK\n");
}


//FUNZIONE TRAVEL COST

int travel_cost (int xp, int yp, int xd, int yd)
{
    int risultato = -1; // Valore di default se non raggiungibile
    int *dist = NULL;
    int nx, ny;
    min_heap_t *heap = NULL;
    int arrivati = 0;

    if(map == NULL)
    {
        risultato = -1;
    }
    else if (xp == xd && yp == yd)
    {
        risultato = 0;
    }
    else
    {
        int dim = map->map_x * map->map_y;
        dist = (int*) malloc(dim * sizeof(int));

        if(dist != NULL)
        {
            for (int i = 0; i < dim; i++)
            {
                dist[i] = -1;
            }

            heap = crea_heap(dim);
            if(heap != NULL)
            {
                dist[yp * map->map_x + xp] = 0;
                heap_push(heap, xp, yp, 0, 0);

                // Quando trovo un percorso migliore, faccio push di un nuovo nodo su min-heap
                while (heap->dim > 0 && arrivati == 0)
                {
                    nodo_heap_t curr = heap_pop(heap);
                    int cx = curr.x;
                    int cy = curr.y;
                    int costocurr = curr.sommacosti; // Costo per arrivare qui
                    int idx = cy * map->map_x + cx;

                    if (dist[idx] != -1 && costocurr > dist[idx])
                    {
                        continue;
                    }

                    if (cx == xd && cy == yd) // Se siamo arrivati a destinazione
                    {
                        risultato = costocurr;
                        arrivati = 1;
                    }

                    cella_t *cella = trovacella(map, cx, cy);

                    // espansione vicini di terra
                    if (uscita_possibile(cella->costo))
                    {
                        const int (* direzioni) [2];
                        if(cy % 2 == 0)
                        {
                            direzioni =  indici_vicini_riga_pari;
                        }
                        else
                        {
                            direzioni = indici_vicini_riga_dispari;
                        }

                        for (int i = 0; i < LATI; i++)
                        {
                            nx = cx + direzioni [i][0];
                            ny = cy + direzioni [i][1];

                            if (coordinate_valide(nx, ny))
                            {
                                int nuovocosto = costocurr + cella->costo; // pago l’uscita dalla cella corrente
                                int n_idx = ny * map->map_x + nx;

                                if (dist[n_idx] == -1 || nuovocosto < dist[n_idx])
                                {
                                    dist[n_idx] = nuovocosto;
                                    heap_push(heap, nx, ny, nuovocosto, nuovocosto);
                                }
                            }
                        }
                    }

                    // espansione rotte aeree
                    for (int i = 0; i < cella->n_rotte; i++)
                    {
                        rotta_aerea_t *rotta = &cella->rotte[i];
                        if (uscita_possibile(cella->rotte[i].costo) && rotta->costo > 0)
                        {
                             nx = rotta->to_x;
                             ny = rotta->to_y;

                             if(coordinate_valide(nx, ny))
                             {
                                 int nuovocosto = costocurr + rotta->costo;
                                 int n_idx = ny * map->map_x + nx;

                                 if (dist[n_idx] == -1 || nuovocosto < dist[n_idx])
                                 {
                                      dist[n_idx] = nuovocosto;
                                      heap_push(heap, nx, ny, nuovocosto, nuovocosto);
                                 }
                             }
                        }
                    }
                }
            }
        }
    }
    if(dist != NULL)
    {
        free(dist);
    }
    if (heap!=NULL)
    {
        libera_heap(heap);
    }
    return (int)risultato;
}

//FUNZIONE TOGGLE AIR ROUTE
void toggle_air_route(int x1, int y1, int x2, int y2)
{
    cella_t *c = trovacella(map, x1, y1);

    // Controlla se esiste già
    int trovato = -1;
    for (int i = 0; i < c->n_rotte && trovato == -1; i++)
    {
        if (c->rotte[i].to_x == x2 && c->rotte[i].to_y == y2)
        {
            trovato = i;
        }
    }

    //se presente tolgo
    if (trovato >= 0)
    {
        c->rotte[trovato] = c->rotte[c->n_rotte - 1];
        c->n_rotte--;
        printf("OK\n");
    }

    //se assente metto
    else
    {
        if (c->n_rotte >= MAXR)
        {
            printf("KO\n");
        }
        else
        {
            int somma = c->costo;
            for (int i = 0; i < c->n_rotte; i++)
            {
                somma = somma + c->rotte[i].costo;
            }

            int nuovocosto = (int) (somma / (c->n_rotte + 1));
            nuovocosto = rispettalimiti(nuovocosto);

            c->rotte[c->n_rotte].to_x = x2;
            c->rotte[c->n_rotte].to_y = y2;
            c->rotte[c->n_rotte].costo = nuovocosto;
            c->n_rotte++;

            printf("OK\n");
        }
    }
}

//GESTISCI RICHIESTE INPUT

void gestisci_richieste (char * input)
{
    if(sscanf(input, "%s", comando) == 1)
    {
        //INIT
        if(strcmp(comando, "init") == 0)
        {
            int x, y, n ;
            n = sscanf (input, "%*s %d %d", &x, &y);
            if(n==2)
            {
                if(x > 0 && y > 0)
                {
                    init(x, y); //chiamo funzione init
                }
                else printf("KO\n");
            }
            else printf("KO\n");
        }

        //CHANGE_COST
        else if(strcmp(comando, "change_cost") == 0)
        {
            int x, y, v, raggio, n;
            n = sscanf (input, "%*s %d %d %d %d", &x, &y, &v, &raggio);
            if(n == 4)
            {
                if (map != NULL && raggio > 0 && coordinate_valide(x, y) && v >= MINV && v <= MAXV)
                {
                    change_cost(x, y, v, raggio);
                }
                else
                {
                    printf("KO\n");
                }
            }
            else
            {
                printf("KO\n");
            }
        }

        //TRAVEL_COST
        else if(strcmp(comando, "travel_cost") == 0)
        {
            int xp, yp, xd, yd, n;
            int costo = -1;
            n = sscanf (input, "%*s %d %d %d %d", &xp, &yp, &xd, &yd);
            if(n == 4 && map != NULL)
            {
                if(coordinate_valide(xp,yp) && coordinate_valide(xd,yd))
                {
                    costo = travel_cost(xp, yp, xd, yd);
                }
            }
            printf("%d\n", costo);
        }

        //TOGGLE_AIR_ROUTE
        else if(strcmp(comando, "toggle_air_route") == 0)
        {
            int xp, yp, xd, yd, n;
            n = sscanf (input, "%*s %d %d %d %d", &xp, &yp, &xd, &yd);
            if(n == 4)
            {
                if (map != NULL && coordinate_valide(xp, yp) && coordinate_valide(xd, yd))
                {
                    toggle_air_route(xp, yp, xd, yd);
                }
                else
                {
                    printf("KO\n");
                }
            }
            else
            {
                printf("KO\n");
            }
        }

        //COMANDO SCONOSCIUTO
        else
        {
            printf("KO\n");
        }
    }
}

//MAIN

int main (int argc, char *argv[])
{
    char input[100];

    while(fgets(input, sizeof(input), stdin) != NULL)
    {
        input [strcspn(input, "\r\n")] = 0;
        if(strlen(input) > 0)
        {
            gestisci_richieste(input);
        }
        else
        {
            printf("KO\n");
        }
    }
    return 0;
}
