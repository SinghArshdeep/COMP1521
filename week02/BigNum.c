// BigNum.c ... LARGE positive integer values

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "BigNum.h"

// Initialise a BigNum to N bytes, all zero
void initBigNum (BigNum *bn, int Nbytes)
{
    bn -> nbytes = Nbytes;
    bn -> bytes = calloc(Nbytes, sizeof(Byte));
    for(int i = 0; i <= Nbytes; i++)
    {
        bn -> bytes[i] = '0';
    }
    assert(bn -> bytes);
	return;
}

// Add two BigNums and store result in a third BigNum
void addBigNums (BigNum bnA, BigNum bnB, BigNum *res)
{
    int carry = 0, min = 0, max = 0, total = 0;
    BigNum big ;

    if(bnA.nbytes < bnB.nbytes)
    {
        min = bnA.nbytes;
        max = bnB.nbytes;
        big = bnB;
    }
    else
    {
        min = bnB.nbytes;
        max = bnA.nbytes;
        big = bnA;
    }
    
    if(max > res -> nbytes)
    {
        res -> nbytes = max ;
        res -> bytes = realloc(res -> bytes , res -> nbytes * sizeof(Byte));
    }
    
    for(int i = 0 ; i < min ; i++)
    {
        if(carry == 0)
        {
            if(((bnA.bytes[i] -'0') + (bnB.bytes[i]-'0')) <= 9)
            {
                res ->bytes[i] = ((bnA.bytes[i]-'0') + (bnB.bytes[i]-'0'))+'0';
            }
            else
            {
                total = (bnA.bytes[i]-'0') + (bnB.bytes[i]-'0');
                res ->bytes[i] = (total % 10)+'0';
                carry = 1;
            }
            
        }
        else
        {
            if(((bnA.bytes[i]-'0') + (bnB.bytes[i]-'0') + carry) <= 9)
            {
                res ->bytes[i] = ((bnA.bytes[i] - '0') + (bnB.bytes[i] - '0') + carry)+'0';
                carry = 0;
            }
            else
            {
                total = (bnA.bytes[i] - '0') + (bnB.bytes[i] - '0') + carry;
                res ->bytes[i] = (total % 10)+'0';
                carry = 1;
            }
        }
        
    }
    for(int i = min ; i < max ; i++)
    {
        if ((big.bytes[i] - '0') + carry <= 9)
        {
            res->bytes[i] = ((big.bytes[i] - '0') + carry) + '0';
            carry = 0;
        }
        else
        {
            total = ((big.bytes[i] - '0') + carry);
            res ->bytes[i] = (total % 10) + '0';
            carry = 1;
        }
    }
    
    if(carry == 1)
    {
        res -> nbytes++;
        res ->bytes = realloc(res->bytes , res -> nbytes*sizeof(Byte));
        res ->bytes[res->nbytes - 1] = 1 + '0';
    }
    
    return;
}


// Set the value of a BigNum from a string of digits
// Returns 1 if it *was* a string of digits, 0 otherwise
int scanBigNum (char *s, BigNum *bn)
{
    int a = strlen(s), found = 0;
    
    if(strlen(s) > 20)
    {
        bn -> bytes = realloc(bn -> bytes,a * sizeof(Byte));
        bn -> nbytes = a;
    }
    
    for(int i = a - 1, j = 0; i >= 0; i--)
    {
        if(isdigit(s[i]))
        {
            found = 1;
            bn -> bytes[j] = s[i];
            j++;
        }
    }
    
    if(found)
        return 1;
    
    return 0;
}


// Display a BigNum in decimal format
void showBigNum (BigNum bn)
{
    int post = 0;
    for(int i = bn.nbytes - 1; i >= 0; i--)
    {
        if (bn.bytes[i] != '0') {
            post = 1;
        }
        
        if(post)
            printf("%c", bn.bytes[i]);
    }
    
    if(post == 0)
    {
        printf("0");
    }
    return;
}

