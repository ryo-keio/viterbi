#include <stdio.h>
#include <math.h>
#include <stdlib.h>

long long combi(int n,int r);
int main(int argc, char *argv[])
{

	double sn = 0.0;
	double var, p = 0;

	for(sn = 0.0; sn <= 6.0; sn += 0.25) {
		//ガウス分布にしたがう乱数を発生させる
		var = pow(10.0, -sn/10.0);

		p = 1.0 - (0.5 * erfc(-1.0/sqrt(2.0*var)));
		double pk, ck, theorial_value;
		int cks[8] = {2,7,18,49,130,333,836,2069};
		int k, e;
		theorial_value = 0;
		for (k = 6; k < 14; k++) {
			pk = 0;
			ck = cks[k-6];

			pk = 1.0/2.0 * erfc(sqrt(k/2.0*var));

			theorial_value += ck * pk;
		}
		if(theorial_value > 0.5){
			theorial_value = 0.5;
		}

		printf("sn : %f,%e\n", sn, theorial_value);
	}
	return 0;
}

long long combi(int n, int r)
{
	int a;
	long pp = 1;

	for (a = 1; a <= r; a++) 
		pp = pp * (n-a+1)/a;
	return pp;
}
