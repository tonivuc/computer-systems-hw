/******************************************************************************

                            Online C Compiler.
                Code, Compile, Run and Debug C program online.
Write your code in this editor and press "Run" button to compile and execute it.

*******************************************************************************/

#include <stdio.h>

int main()
{
	int k,m,n,sum, kStore, mStore, nStore;

	sum = 0;

	printf("Please enter an integer greater than 1 (k): ");
	scanf("%d",&k);
	if (k < 2) {
		printf("You failed.");
		return -1;
	}
	printf("Please enter an integer that is not 0 (m): ");
	scanf("%d",&m);
	printf("Thank you, another, different integer that is not 0 please. (n): ");
	scanf("%d",&n);

	kStore = k;
	mStore = m;
	nStore = n;

	if (m < n) {
		for (m = m+1; m < n; m++) {
			if (m % k == 0) { //&& ()
				sum += m;
			}
		}
	}
	else if (n < m) {
		for (n = n+1; n < m; n++) {
			if (n % k == 0) { //&& ()
				sum += n;
			}
		}
	}
	else {
		printf("The integers must be different");
	}

	n = nStore;
	k = kStore;
	m = mStore;

	printf("The sum of all integers devisible by %d between %d and %d is %d",k,m,n,sum);

	return 0;
}
