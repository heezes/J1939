#include "rsa_ed.h"

/*
For calculating ((base^toPower)*mod(modVal))
Implemented by (base mod(modVal))^toPower
> param@ base	: Data which has to be encrypted
> param@ toPower: This is typically the key
> param@ modVal : n = pq (for defining the range of data that can be encrypted/decrypted) 
> retval@		: encrypted/decrypted data
*/
static int powerAndMod(int base, int toPower, int modVal)
{
	unsigned long long result = 1; //not used int to avoid overflows
	for(int ijk = 0; ijk<toPower; ijk++)
	{
		result *= base;
		result = result%modVal;
	}
	return (int)result;
}

/*Checks wether the input value is a Prime Number*/
static int isPrime(int val)
{
	int retVal = 0;
	for(int lmn = 2; lmn<=val; lmn++)
	{
		if(val%lmn == 0 && val != lmn)
			break;
		else if(val%lmn == 0 && val == lmn)
		{
			retVal = 1;
		}
	}
	return retVal;
}	
/*
Generate Public and Private Key based on n and phi
ed = 1+phi (based on ab (= congruent) 1modr then by congruence of modularity ab = 1+kr)
1+phi is calculated and it should not be prime and its factor are calculated and thus e and d are assigned.
*/
static void calculatePublicPrivateKey(RSA_params *rsa_param)
{
	int ed = rsa_param->phi + 1;
	for(int ijk = 1; ijk<100; ijk++)
	{
		ed =  (rsa_param->phi*ijk) + 1;
		if(isPrime(ed))
			continue;
		else
			break;
	}
	for(int i=3; i <= ed; ++i)
    {
        if (ed%i == 0)
        {
        	rsa_param->e = i;
        	rsa_param->d = ed/i;
            break;
        }
    }
}
/*
> param@ rsa_param: This Structure contains information regarding RSA Algorithm
> param@ a:	Unique Prime Number
> param@ b: Unique Prime Number
*/
void CryptoInit(RSA_params *rsa_param, int prime_number_1, int prime_number_2)
{
	rsa_param->n = prime_number_1*prime_number_2;
	rsa_param->phi = (prime_number_1-1)*(prime_number_2-1);
	calculatePublicPrivateKey(rsa_param);
}

/*
> param@ RSA_params : This Structure contains information regarding RSA Algorithm
> param@ data		: Data to be encrypted
> retval@  		  	: Encrypted Data
*/
int encrypt(RSA_params *rsa_param, int data)
{
	return (powerAndMod(data,rsa_param->e,rsa_param->n));
}

/*
> param@ RSA_params : This Structure contains information regarding RSA Algorithm
> param@ data		: Data to be decrypted
> retval@  		  	: Decrypted Data
*/
int decrypt(RSA_params *rsa_param, int data)
{
	return (powerAndMod(data,rsa_param->d,rsa_param->n));
}


/*
> param@ RSA_params : This Structure contains information regarding RSA Algorithm
> param@ buf		: Array Containing the raw data
> param@ cryptoBuf	: Array containing the encrypted data.
*/
void encryptString(RSA_params *rsa_param, uint8_t* buf, int* cryptoBuf, int len)
{
	int ijk = 0;
	while(ijk < len)
	{
		cryptoBuf[ijk] = powerAndMod(*(buf++),rsa_param->e,rsa_param->n);
		ijk++;
	}
}

/*
> param@ RSA_params : This Structure contains information regarding RSA Algorithm
> param@ buf		: Array Containing the raw data
> param@ cryptoBuf	: Array containing the encrypted data.
*/
void decryptString(RSA_params *rsa_param, uint8_t* buf, int* cryptoBuf, int len)
{
	int ijk  = 0;
	while(ijk < len)
	{
		*(buf++) = powerAndMod(*(cryptoBuf++),rsa_param->d,rsa_param->n);
		ijk++;
	}
}
