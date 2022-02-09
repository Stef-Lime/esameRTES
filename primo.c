#include <stdio.h>
#include <math.h>
/*Controlla se un numero e' primo*/

int main()
{   
	int num, div_cor, e_primo;
    
	printf("Dare il numero da controllare e poi andare a capo\n");
	scanf("%d",&num);
    
	div_cor = 2;
	e_primo = 1; /*vero*/
    
	if(num<0)
	{
		printf("Il numero doveva essere positivo\n");
	}
	else
	{
		if(num>1)
		{
			while(div_cor < num && e_primo)
			{
				if(num % div_cor != 0)
				{
					div_cor = div_cor +1;
				}
				else
				{
					e_primo = 0; /*falso*/
				}
			}
		}
        }
	if(e_primo)
	{
            printf("Il numero %d e\' primo.\n",num);
	}
        else
	{
            printf("Il numero %d non e\' primo.\n",num);
	}
	return 0;
}
