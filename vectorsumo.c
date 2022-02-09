/* Somma ed ordinamento di Array */
#include <stdio.h>
int main ()
{
	int temp;
	int n,i,j;
  
	/* Legge gli Array1 e Array2 */
	printf("Inserisci la dimensione degli array: ");
	scanf("%d", &n);

	int vect1[n], vect2[n];	/* Due Array  */
	int result[n];		/* Array risultato */
  
	printf ("Inserisci gli elementi dell'array1:\n");
	for (i=0; i<n; i++){
		scanf("%d", &vect1[i]);
    	}

	printf("Inserisci gli elementi dell'array2:\n");
	for(i=0; i<n; i++){
		scanf("%d", &vect2[i]);
	}

/* Esegue la somma degli array */
	for(i=0; i<n; i++){
		result[i] = vect1[i] + vect2[i];
	}

/* Stampa la somma dei due array */
	printf("Array sommato:\n");
	for(i=0; i<n; i++){
		printf("%d ", result[i]);
	}

// Array Sorting - Ordine Ascendente
	for(i=0; i<n; i++){
		for(j=i+1; j<n; j++){
			if(result[i] > result[j]){
				temp = result[i];
				result[i] = result[j];
				result[j] = temp;
			}
		}
	}

/* Ristampa l'array ordinato */
	printf ("\nArray ordinato:\n");
	for(i=0; i<n; i++){
		printf("%d ", result[i]);
	}
	printf("\n");
	return 0;
}
