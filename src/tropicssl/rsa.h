/* RSAREF.H - header file for RSAREF cryptographic toolkit
 */

#ifndef _RSAREF_H_
#define _RSAREF_H_ 1

#ifdef __cplusplus
extern "C" {
#endif

#ifndef PROTOTYPES
#define PROTOTYPES 1
#endif

typedef unsigned char *POINTER;/* POINTER defines a generic pointer type */
typedef unsigned short int UINT2;/* UINT2 defines a two byte word */
typedef unsigned long int UINT4;/* UINT4 defines a four byte word */

#ifndef NULL_PTR
#define NULL_PTR ((POINTER)0)
#endif

#ifndef UNUSED_ARG
#define UNUSED_ARG(x) x = *(&x);
#endif

#if PROTOTYPES
#define PROTO_LIST(list) list
#else
#define PROTO_LIST(list) ()
#endif

#define NN_DIGIT		UINT4
#define NN_HALF_DIGIT	UINT2

/* RSA key lengths.
 */
#define MIN_RSA_MODULUS_BITS 508
#define MAX_RSA_MODULUS_BITS 512
#define MAX_RSA_MODULUS_LEN ((MAX_RSA_MODULUS_BITS + 7) / 8)
#define MAX_RSA_PRIME_BITS ((MAX_RSA_MODULUS_BITS + 1) / 2)
#define MAX_RSA_PRIME_LEN ((MAX_RSA_PRIME_BITS + 7) / 8)

/* Maximum length of Diffie-Hellman parameters.
 */
#define DH_PRIME_LEN(bits) (((bits) + 7) / 8)

/* Random structure.
 */
typedef struct {
  unsigned int bytesNeeded;
  unsigned char state[16];
  unsigned int outputAvailable;
  unsigned char output[16];
} R_RANDOM_STRUCT;

/* RSA public and private key.
 */
typedef struct {
  unsigned int  bits;                          /* length in bits of modulus */
  unsigned char modulus[MAX_RSA_MODULUS_LEN];  /* modulus */
  unsigned char exponent[MAX_RSA_MODULUS_LEN]; /* public exponent */
} R_RSA_PUBLIC_KEY;

typedef struct {
  unsigned int  bits;                           /* length in bits of modulus */
  unsigned char modulus[MAX_RSA_MODULUS_LEN];       /* modulus */
  unsigned char publicExponent[MAX_RSA_MODULUS_LEN];/* public exponent */
  unsigned char exponent[MAX_RSA_MODULUS_LEN];      /* private exponent */
  unsigned char prime[2][MAX_RSA_PRIME_LEN];        /* prime factors */
  unsigned char primeExponent[2][MAX_RSA_PRIME_LEN];/* exponents for CRT */
  unsigned char coefficient[MAX_RSA_PRIME_LEN];      /* CRT coefficient */
} R_RSA_PRIVATE_KEY;

/* RSA prototype key.
 */
typedef struct {
  unsigned int bits;     /* length in bits of modulus */
  int useFermat4;        /* public exponent (1 = F4, 0 = 3) */
} R_RSA_PROTO_KEY;

/* Diffie-Hellman parameters.
 */
typedef struct {
  unsigned char *prime;          /* prime */
  unsigned int primeLen;         /* length of prime */
  unsigned char *generator;      /* generator */
  unsigned int generatorLen;     /* length of generator */
} R_DH_PARAMS;

/* LOCAL_MD5 context. */
typedef struct {
  UINT4 state[4];                                   /* state (ABCD) */
  UINT4 count[2];        /* number of bits, modulo 2^64 (lsb first) */
  unsigned char buffer[64];                         /* input buffer */
} LOCAL_MD5_CTX;

void LOCAL_MD5Init PROTO_LIST ((LOCAL_MD5_CTX *));
void LOCAL_MD5Update PROTO_LIST ((LOCAL_MD5_CTX *, unsigned char *, unsigned int));
void LOCAL_MD5Final PROTO_LIST ((unsigned char [16], LOCAL_MD5_CTX *));

/* Random structures.
 */
int R_RandomInit PROTO_LIST ((R_RANDOM_STRUCT *));
int R_RandomUpdate PROTO_LIST ((R_RANDOM_STRUCT *, unsigned char *, unsigned int));
int R_GetRandomBytesNeeded PROTO_LIST ((unsigned int *, R_RANDOM_STRUCT *));
void R_RandomFinal PROTO_LIST ((R_RANDOM_STRUCT *));

/* Printable ASCII encoding and decoding.
 */
int R_EncodePEMBlock PROTO_LIST ((unsigned char *, unsigned int *, unsigned char *, unsigned int));
int R_DecodePEMBlock PROTO_LIST ((unsigned char *, unsigned int *, unsigned char *, unsigned int));
  
/* Key-pair generation.
 */
int R_GeneratePEMKeys PROTO_LIST
  ((R_RSA_PUBLIC_KEY *, R_RSA_PRIVATE_KEY *, R_RSA_PROTO_KEY *, R_RANDOM_STRUCT *));
/*
 *RSA function
 */
int RSAPublicEncrypt PROTO_LIST 
  ((unsigned char *, unsigned int *, unsigned char *, unsigned int,R_RSA_PUBLIC_KEY *, R_RANDOM_STRUCT *));
int RSAPrivateEncrypt PROTO_LIST
  ((unsigned char *, unsigned int *, unsigned char *, unsigned int,R_RSA_PRIVATE_KEY *));
int RSAPublicDecrypt PROTO_LIST 
  ((unsigned char *, unsigned int *, unsigned char *, unsigned int,R_RSA_PUBLIC_KEY *));
int RSAPrivateDecrypt PROTO_LIST
  ((unsigned char *, unsigned int *, unsigned char *, unsigned int,R_RSA_PRIVATE_KEY *));

/* Diffie-Hellman key agreement.
 */
int R_GenerateDHParams PROTO_LIST ((R_DH_PARAMS *, unsigned int, unsigned int, R_RANDOM_STRUCT *));
int R_SetupDHAgreement PROTO_LIST
  ((unsigned char *, unsigned char *, unsigned int, R_DH_PARAMS *, R_RANDOM_STRUCT *));
int R_ComputeDHAgreedKey PROTO_LIST
  ((unsigned char *, unsigned char *, unsigned char *, unsigned int, R_DH_PARAMS *));

/*
 *Random function
 */
int R_GenerateBytes PROTO_LIST ((unsigned char *, unsigned int, R_RANDOM_STRUCT *));

/*
 * Prime function
 */
int GeneratePrime PROTO_LIST
  ((NN_DIGIT *, NN_DIGIT *, NN_DIGIT *, NN_DIGIT *, unsigned int, R_RANDOM_STRUCT *));

/*
 * Digit function
 */
void NN_DigitMult PROTO_LIST ((NN_DIGIT[2], NN_DIGIT, NN_DIGIT));
void NN_DigitDiv PROTO_LIST ((NN_DIGIT *, NN_DIGIT[2], NN_DIGIT));

/* Routines supplied by the implementor.
 */
void R_memset PROTO_LIST ((POINTER, int, unsigned int));
void R_memcpy PROTO_LIST ((POINTER, POINTER, unsigned int));
int R_memcmp PROTO_LIST ((POINTER, POINTER, unsigned int));


/* Error codes.
 */
#define RE_CONTENT_ENCODING 0x0400
#define RE_DATA 0x0401
#define RE_DIGEST_ALGORITHM 0x0402
#define RE_ENCODING 0x0403
#define RE_KEY 0x0404
#define RE_KEY_ENCODING 0x0405
#define RE_LEN 0x0406
#define RE_MODULUS_LEN 0x0407
#define RE_NEED_RANDOM 0x0408
#define RE_PRIVATE_KEY 0x0409
#define RE_PUBLIC_KEY 0x040a
#define RE_SIGNATURE 0x040b
#define RE_SIGNATURE_ENCODING 0x040c
#define RE_ENCRYPTION_ALGORITHM 0x040d

#ifdef __cplusplus
}
#endif

#endif
