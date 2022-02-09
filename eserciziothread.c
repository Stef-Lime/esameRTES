/* Il sistema modella un turno del gioco delle bandierine. ci sono due giocatore che al "Via!" devonp tentare di prendere la bandierina tenuta dal giudice. Il primo che riesce a  prendere la bandierina  deve tornare alla base, mentre l'altro deve cercare di raggiungerlo.
Il sistema è modellato da tre thread: uno per il giudice, ed uno per ogni giocatore.
struct bandierine_t {...} bandierine;
void init_bandierine(struct bandierine_t *b);
void via(struct bandierine_t *b);
void attendi_il_via(struct bandierine_t *b, int n);
int bandierina_presa(struct bandierine_t *b, int n);
int sono_salvo(struct bandierine_t *b, int n);
int ti_ho_preso(struct bandierine_t *b, int n);
int risultato_gioco(struct bandierine_t *b);

attendi_giocatori: attende che entrambi i giocatori siano pronti in attesa di dare il via
via: dà il via al gioco sbloccando i giocatori in attesa
attendi_il_via: sempre bloccante
bandierina_presa: prende la bandierina (se non è stata già presa) e ritorna 1 se il giocatore ha preso la bandierina, 0 altrimenti
sono salvo: ritorna 1 se il giocatore non è stato ancora preso, 0 altrimenti
ti_ho_preso: ritorna 1 se il giocatore non si è ancora salvato, 0 altirmenti.
risultato_gioco: ritorna il gicoatore non appena il risultato della partita è certo */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>
#include <string.h>

//Ho corretto solo qualche errore di distrazione, il flusso logico è medesimo a quello scritto su carta

void pausetta(void)
{
	struct timespec t;
	t.tv_sec = 0;
	t.tv_nsec = (rand()%10+1)*1000000;
	nanosleep(&t,NULL);
}

struct bandierine_t{
//Il primo semaforo serve a bloccare i giocatori in attesa del via del giudice
//Il secondo semaforo serve a bloccare il giudice in attesa di entrambi i giocatori
//Il terzo semaforo serve per riferire al giudice il risultato appena esso è certo, per questo motivo non si attende il giocatore perdente
//La prima variabile indica se la bandiera è presa (0 se libera mentre 1 se occupata)
//La seconda variabile indica il giocatore attualmente in possesso della bandiera
//La terza variabile indica se il giocatore che ha preso la bandiera è rientrato in base (0 se sta ancora correndo mentre 1 se è arrivato alla base)
sem_t semg,semarb,semris,mutex;
int presa,possesso,base;
}bandierine;

void init_bandierine(struct bandierine_t *b)
{
	sem_init(&b->semg,0,0);
	sem_init(&b->mutex,0,1);
	sem_init(&b->semris,0,0);
	sem_init(&b->semarb,0,0);
	//A parte il mutex tutti gli altri semafori sono inizializzati a 0 perché devono essere bloccanti
	b->presa=0;
	b->possesso=0;
	b->base=0;
}
void attendi_il_via(struct bandierine_t *b, int n)
{
	//Il giocatore sblocca parzialmente il giudice (perché servono 2 post) e poi aspetta il via
	printf("--Sono il giocatore numero %d ed attendo il via \n",n);
	sem_post(&b->semg);
	sem_wait(&b->semarb);
}

int bandierina_presa(struct bandierine_t *b, int n)
{
	/*Se la variabile "presa" è 0 significa che nessuno ha ancora preso la bandiera, per questo motivo se la condizione è soddisfatta si 
	cambia tale parametro ad 1 e si imposta il valore della variabile "possesso" con il numero del giocatore*/
	int ris=0;
	sem_wait(&b->mutex);
	if(b->presa==0)
	{
		b->presa=1;
		b->possesso=n;
		ris=1;
	}
	sem_post(&b->mutex);
	return(ris);

}
int ti_ho_preso(struct bandierine_t *b, int n)
{
	/*Se il giocatore con la bandiera non è ancora arrivato alla base allora lo si cattura, per questo motivo vince giocatore che invoca tale 
	funzione e si può comunicare tale risultato al giudice*/
	int ris=0;
	sem_wait(&b->mutex);
	/*Sul foglio ho messo la condizione "b->possesso!=n" ma effettivamente non ce n'è bisogno perché se un giocatore usa questa funzione
	vuol dire che non è riuscito a prendere la bandiera, per questo motivo tale controllo è superfluo */
	if(b->base==0)
	{
		b->possesso=n;
		ris=1;
		//per distrazione sul foglio ho scritto: sem_post(&semris)
		sem_post(&b->semris);
	}
	sem_post(&b->mutex);
	return(ris);
}
int sono_salvo(struct bandierine_t *b, int n)
{
	/*Se il valore di "possesso" corrisponde al numero del giocatore che ha invocato tale funzione vuol dire che è riuscito ad arrivare alla 
	base senza farsi catturare, per questo motivo si può comunicare tale risultato al giudice*/
	int ris=0;
	sem_wait(&b->mutex);
	if(b->possesso==n)
	{
		ris=1;
		b->base=1;
		sem_post(&b->semris);	
	}
	sem_post(&b->mutex);
	return(ris);
}


void *giocatore(void *arg)
{
	int *numerogiocatore = (int *)arg;
	attendi_il_via(&bandierine,*numerogiocatore);
	pausetta();
	if(bandierina_presa(&bandierine,*numerogiocatore)==1)
	{
		printf("--Sono il giocatore %d ed ho PRESO la bandierina \n",*numerogiocatore);
		pausetta();
		if(sono_salvo(&bandierine,*numerogiocatore)==1)
		{
			printf("--Sono il giocatore %d e sono SALVO \n",*numerogiocatore);
		}
		else
		{
			printf("--Sono il giocatore %d ed ho PERSO la bandierina \n",*numerogiocatore);
		}
	}
	else
	{
		printf("--Sono il giocatore %d e NON HO PRESO la bandierina \n",*numerogiocatore);
		pausetta();
		if(ti_ho_preso(&bandierine,*numerogiocatore)==1)
		{
			printf("--Sono il giocatore %d ed ho RECUPERATO la bandierina \n",*numerogiocatore);
		}
		else
		{
			printf("--Sono il giocatore %d e NON HO RECUPERATO la bandierina \n",*numerogiocatore);
		}
	}


}
void attendi_giocatori(struct bandierine_t *b)
{
	//Per distrazione sul compito ho messo anche il parametro n ma in questo caso non ce n'è bisogno
	printf("-Sono il giudice ed aspetto entrambi i giocatori \n");
	sem_wait(&b->semg);
	sem_wait(&b->semg);

}

void via(struct bandierine_t *b)
{
	//Per distrazione sul compito ho messo anche il parametro n ma in questo caso non ce n'è bisogno
	printf("-Via! \n");
	sem_post(&b->semarb);
	sem_post(&b->semarb);
}
int risultato_gioco(struct bandierine_t *b)
{
	//Sul compito ho usato una variabile intermedia per riportare il vincitore, il risultato  è uguale ma così è più snello
	sem_wait(&b->semris);
	return(b->possesso);
	
}
void *giudice(void *id){
	int *pi = (int *)id;
	attendi_giocatori(&bandierine);
	pausetta();
	via(&bandierine);
	printf("-Il vincitore corrisponde al giocatore %d \n",risultato_gioco(&bandierine));

}

int main (int argc, char **argv)
{
	/* inizializzo i numeri casuali, usati nella funzione pausetta */
	srand(555);
	pthread_t *thread;
	int *taskids;
	int i;
	int *p;
	int NUM_THREADS=3;
	srand(time(NULL));
	//Inizializzo i parametri della struttura
	init_bandierine(&bandierine);
	thread=(pthread_t *) malloc(NUM_THREADS * sizeof(pthread_t));
	if (thread == NULL)
	{
		perror("Problemi con l'allocazione dell'array thread\n");
		exit(3);
	}
	taskids = (int *) malloc(NUM_THREADS * sizeof(int));
	if (taskids == NULL)
	{
		perror("Problemi con l'allocazione dell'array taskids\n");
		exit(4);
	}
	//Creo il thread del giudice
	taskids[0] = 0;
	if (pthread_create(&thread[0], NULL, giudice, (void *) (&taskids[0])) != 0)
        {
		printf("SONO IL MAIN E CI SONO STATI PROBLEMI DELLA CREAZIONE DEL thread %d-esimo\n", taskids[0]);
		exit(7);
       	}
	//Creo i threads dei giocatori
	for (i=1;i<NUM_THREADS;i++) 
	{	
		taskids[i] = i;
		if (pthread_create(&thread[i], NULL, giocatore, (void *) (&taskids[i])) != 0)
        	{
                	printf("SONO IL MAIN E CI SONO STATI PROBLEMI DELLA CREAZIONE DEL thread %d-esimo\n", taskids[i]);
			exit(5);
       		}
	}
	//Attendo la terminazione dei threads 
   	for (i=0; i < NUM_THREADS; i++)
	{
   		pthread_join(thread[i], (void**) & p);
	} 
	exit(0);
	
}








