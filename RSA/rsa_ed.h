#ifndef __CRYPTO_H__
#define __CRYPTO_H__

#include "stdint.h"

/*Structures*/
typedef struct 
{
	int n;
	int e;
	int d;
	int phi;
}RSA_params;

/*Function Forward Decleration*/
void CryptoInit(RSA_params *rsa_param, int prime_number_1, int prime_number_2);
int encrypt(RSA_params *rsa_param, int data);
int decrypt(RSA_params *rsa_param, int data);
void encryptString(RSA_params *rsa_param, uint8_t* buf, int* cryptoBuf, int len);
void decryptString(RSA_params *rsa_param, uint8_t* buf, int* cryptoBuf, int len);
#endif