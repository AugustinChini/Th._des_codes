//
//  TP6_RSA
//  

#include <stdio.h>
#include <iostream>
#include <gmp.h>

#define BITSTRENGTH  14              /* size of modulus (n) in bits */
#define PRIMESIZE (BITSTRENGTH / 2)  /* size of the primes p and q  */


/*function which make the rsa encryption
 * This function creates the keys.
 *
 *  1. Generate two large distinct primes p and q randomly
 *  2. Calculate n = pq and x = (p-1)(q-1)
 *  3. Select a random integer e (1<e<x) such that gcd(e,x) = 1
 *  4. Calculate the unique d such that ed = 1(mod x)
 *  5. Public key pair : (e,n), Private key pair : (d,n)
 * and return an array of mpz_t with
 * [0]: Public Keys
 * [1]: Private Keys
 * [2]: The encrypted message
 */
mpz_t* rsa_encrypt(const char* s_msg, const char* s_seed, int primes_size, mpz_t tab_res[3])
{
    /* Declare global variables */
    mpz_t d,e,n;
    mpz_t M,c;


    /* Initialize the GMP integers */
    mpz_init(d);
    mpz_init(e);
    mpz_init(n);
    
    mpz_init(M);
    mpz_init(c);
    
    /*
     *  Step 1 : Get two large primes.
     */
    mpz_t p,q;
    mpz_init(p);
    mpz_init(q);

    mpz_t seed;
    mpz_init(seed);
    mpz_init_set_str(seed, s_seed, 0);
    
    gmp_randstate_t rand;
    gmp_randinit_mt(rand);
    gmp_randseed(rand, seed);
    
    // get rand numbers
    mpz_urandomb(p, rand, primes_size);
    mpz_urandomb(q, rand, primes_size);
    
    // get the next prime number
    mpz_nextprime(p, p);
    mpz_nextprime(q, q);
    /*mpz_init_set_str(p, "47", 0);
    mpz_init_set_str(q, "71", 0);*/

    char p_str[1000];
    char q_str[1000];
    mpz_get_str(p_str,10,p);
    mpz_get_str(q_str,10,q);
    
    std::cout << "Random Prime 'p' = " << p_str <<  std::endl;
    std::cout << "Random Prime 'q' = " << q_str <<  std::endl;
    
    /*
     *  Step 2 : Calculate n (=pq) ie the 1024 bit modulus
     *  and x (=(p-1)(q-1)).
     */
    char n_str[1000];
    mpz_t x;
    mpz_init(x);

    /* Calculate n... */
    mpz_mul(n,p,q);
    mpz_get_str(n_str,10,n);
    std::cout << "\t n = " << n_str << std::endl;

    /* fill the returned table*/
    mpz_set(tab_res[0], n);
    
    
    /* Calculate x... */
    mpz_t p_minus_1,q_minus_1;
    mpz_init(p_minus_1);
    mpz_init(q_minus_1);

    mpz_sub_ui(p_minus_1,p,(unsigned long int)1);
    mpz_sub_ui(q_minus_1,q,(unsigned long int)1);

    mpz_mul(x,p_minus_1,q_minus_1);
    char phi_str[1000];
    mpz_get_str(phi_str,10,x);
    std::cout << "\t phi(n) = " << phi_str << std::endl;

    /*
     *  Step 3 : Get small odd integer e such that gcd(e,x) = 1.
     */
    mpz_t gcd;
    mpz_init(gcd);
    
    mpz_set_ui(e, (unsigned long int)1);
   
    do
    {
        mpz_add_ui(e, e, (unsigned long int)2);
        mpz_gcd(gcd, x, e);
    } while( mpz_cmp_ui(gcd, (unsigned long int)1) != 0 );
     
    char e_str[1000];
    mpz_get_str(e_str,10,e);
    std::cout << "\t e = " << e_str << std::endl;

    /* fill the returned table*/
    mpz_set(tab_res[1], e);

    /*
     *  Step 4 : Calculate unique d such that ed = 1(mod x)
     */
    //mpz_init_set_str(d, "1019", 0);
    if(mpz_invert (d, e, x) == 0)
    {
        std::cerr << "T_T invert error" << std::endl;
        exit(-1);
    }
    char d_str[1000];
    mpz_get_str(d_str,10,d);
    std::cout << "\t d = " << d_str << std::endl << std::endl;

    /* fill the returned table*/
    mpz_set(tab_res[2], d);

    /*
     *  Step 5 : Print the public and private key pairs...
     */
    std::cout << "Public Keys  (e,n): ( " << e_str <<" , " << n_str << " )" << std::endl;
    std::cout << "Private Keys (d,n): ( " << d_str <<" , " << n_str << " )" << std::endl;
    

    /*
     *  Encrypt
     */

    mpz_t msg;
    mpz_init(msg);
    
    mpz_init_set_str(msg, s_msg, 0);
    
    mpz_t c_msg;
    mpz_init(c_msg);
    
    mpz_powm(c_msg, msg, e, n);
    
    char c_str[1000];
    mpz_get_str(c_str,10,c_msg);
    std::cout << "\t encrypted = " << c_str << std::endl << std::endl;

    /* Fill the returned table */
    mpz_set(tab_res[3], c_msg);
    
    
    /* Clean up the GMP integers */
    mpz_clear(p_minus_1);
    mpz_clear(q_minus_1);
    mpz_clear(x);
    mpz_clear(p);
    mpz_clear(q);
    
    mpz_clear(d);
    mpz_clear(e);
    mpz_clear(n);
    
    mpz_clear(M);
    mpz_clear(c);


    mpz_set(tab_res[3], c_msg);

    return tab_res;
}

void rsa_decrypt(mpz_t* tab)
{
    mpz_t d_msg;

    mpz_init(d_msg);

    mpz_powm(d_msg, tab[3], tab[2], tab[0]);

    char dec_str[1000];
    mpz_get_str(dec_str,10,d_msg);

    std::cout << "le message c'etait bien " << dec_str << " non ?" << std::endl;

    //return d_msg;
}

/* Main subroutine */
int main()
{
    mpz_t tab_res[4];

    /* init the mpz number*/
    for(int i = 0; i<4; ++i)
    {
        mpz_init(tab_res[i]);
    }

    rsa_encrypt("123456789", "2369464435", 30, tab_res);

    rsa_decrypt(tab_res);

    return 0;
}

