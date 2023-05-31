/****************************************************************************
**
** Copyright (C) 2009 Kaufman V.S. All rights reserved.
**
****************************************************************************/

#ifndef AES_CRYPTO_H
#define AES_CRYPTO_H

#include <QtGui>

// The number of columns comprising a state in AES. This is a constant in AES. Value=4  
#define Nb 4  

class Aes
{
// Members.
private:
// The number of rounds in AES Cipher. It is simply initiated to zero. The actual value is recieved in the program.  
	int Nr;  
// The number of 32 bit words in the key. It is simply initiated to zero. The actual value is recieved in the program.  
	int Nk;  
// in - it is the array that holds the plain text to be encrypted.  
// out - it is the array that holds the output CipherText after encryption.  
// state - the array that holds the intermediate results during encryption.
public:
	unsigned char in[16], out[16], state[4][4];  
// The array that stores the round keys.  
	unsigned char RoundKey[240];  
// The Key input to the AES Program  
	unsigned char m_key[32];  

// The round constant word array, Rcon[i], contains the values given by   
// x to th e power (i-1) being powers of x (x is denoted as {02}) in the field GF(28)  
// Note that i starts at 1, not 0).  
	int Rcon[255];  

// Methods.
private:
	int getSBoxValue(unsigned char num);
	int getSBoxInvert(unsigned char num);
	void AddRoundKey(int round);
	void SubBytes();
	void InvSubBytes();
	void ShiftRows();
	void InvShiftRows();
	void MixColumns();
	void InvMixColumns();
public:
	Aes(int keyLength);
	void KeyExpansion();
	void Cipher();
	void InvCipher();
	QByteArray crypt(QByteArray key,QByteArray array);
	QByteArray decrypt(QByteArray key,QByteArray array);
};

#endif
