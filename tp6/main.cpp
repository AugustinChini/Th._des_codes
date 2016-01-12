//
//  TP6_RSA
//  

#include <stdio.h>
#include <iostream>
#include <gmp.h>


/*
 * exponentiation rapide 
 */
void powm(mpz_t msg, mpz_t base, mpz_t exp, mpz_t mod)
{
    int result =  mpz_cmp_ui(exp, 0);
    
    mpz_t g, k, y;
    
    mpz_init(y);
    mpz_init(g);
    mpz_init(k);
    
    mpz_set_ui(y, 1);
    mpz_set(g, base);

    // exp < 0
    if( result < 0 ) 
    {       
        // base = 1 / base
        mpz_fdiv_q(g, y, base);
        mpz_mul_ui(k, k, -1);
    }
    // exp = 0
    else if( result == 0 )
    {
        mpz_set_ui(msg, 1);
        
        mpz_clear(y);
        mpz_clear(g);
        mpz_clear(k);
        
        return;
    }
    
    mpz_set(k, exp);
    
    // tant que exp > 1
    while( mpz_cmp_ui(k, 1) > 0 )
    {       
        if( mpz_even_p(k) != 0 )
        {           
            mpz_mul(g, g, g);
            mpz_mod(g, g, mod);
            
            mpz_fdiv_q_ui(k, k, 2);
        }
        else
        {
            mpz_mul(y, g, y);
            mpz_mul(g, g, g);
            
            mpz_sub_ui(k, k, 1);
            mpz_fdiv_q_ui(k, k, 2);
        }
    }
    
    mpz_mul(msg, g, y);
    mpz_mod(msg, msg, mod);
    
    mpz_clear(g);
    mpz_clear(k);
    mpz_clear(y);
}


// test sur les nombre prrmier de Rabin Miller si le nombre n'est pas premier la fonction renverra false sinon c'est probable qu'il soit pemier
// alors on execute le test plusieurs fois (iter)
bool RabinMiller_prime_test(mpz_t n, int iter, int primes_size)
{

    mpz_t tmp,a, s, t, mod, seed, p, modTst;
    mpz_init(tmp);
    mpz_init(a);
    mpz_init(mod);
    mpz_init(s);
    mpz_init(t);
    mpz_init(seed);
    mpz_init(p);
    mpz_init(modTst);
	
	// si le nombre est égale 2 deux il n'est pas premier
    if(mpz_cmp_ui(n, 2) < 0)
    {
        return false;
    }

    mpz_mod_ui(mod, n, 2);
		
	if( mpz_cmp_ui(n, 2) != 0 && mpz_cmp_ui(mod, 0) == 0 )
	{
		return false;
	}


    mpz_sub_ui(s, n, 1);
	mpz_mod_ui(mod, s, 2);
	
	while( mpz_cmp_ui(mod, 0) == 0 )
	{
		mpz_fdiv_q_ui(s, s, 2);
		mpz_mod_ui(mod, s, 2);
	}
	
	
	// initialisation des générateurs aléatoire
	unsigned long int ui_seed = time(NULL);
	
	mpz_init_set_ui(seed, ui_seed);
	
	gmp_randstate_t rand;
	gmp_randinit_mt(rand);
	gmp_randseed(rand, seed);
	
	// n - 1
	mpz_sub_ui(p, n, 1);			
	
	// on recommence le test iter fois pour affiner la probabilité d'avoir un nombre premier
	for(int i = 0; i < iter; ++i)
	{		
		//  a=rand()%(p-1)+1
		mpz_urandomb(a, rand, primes_size);
		mpz_mod(a, a, p);
		mpz_add_ui(a, a, 1);
		
		mpz_set(tmp, s);
		
		powm(mod, a, tmp, n);
		
		while( mpz_cmp(tmp, p) != 0 && mpz_cmp_ui(mod, 1) != 0 && mpz_cmp(mod, p) !=0 )
		{
			mpz_mul(mod, mod, mod);
			mpz_mod(mod, mod, n);
		
			mpz_mul_ui(tmp, tmp, 2);
		}
		
		mpz_mod_ui(modTst, tmp, 2);
		
		if( mpz_cmp(mod, p) != 0 && mpz_cmp_ui(modTst, 0) == 0 )
		{
			return false;
		}
	}

    return true;

}

/*
 * test de primalité de Rabin-Miller
 */ 
void nextprime(mpz_t rop, mpz_t op, int primes_size)
{
	do
	{
		mpz_add_ui(rop, op, 1);
		
	} while( ! RabinMiller_prime_test(rop, 10, primes_size) );

}

/* Algorithme d'Euclide etendu
 * 
 * permet de trouver le plus grand commun diviseur entre deux nombres
 * 
 * */
void extendedEuclide(mpz_t rop, mpz_t u, mpz_t v, const mpz_t op1, const mpz_t op2)
{
	mpz_t rop2;
	mpz_init(rop2);
	mpz_t u2;
	mpz_init(u2);
	mpz_t v2;
	mpz_init(v2);
	mpz_t quot;
	mpz_init(quot);
	
	mpz_t rops;
	mpz_init(rops);
	mpz_t us;
	mpz_init(us);
	mpz_t vs;
	mpz_init(vs);
	
	mpz_t qrop;
	mpz_init(qrop);
	mpz_t qu;
	mpz_init(qu);
	mpz_t qv;
	mpz_init(qv);
	
	mpz_set(rop, op1);
	mpz_set(rop2, op2);
	mpz_set_ui(u, 1);
	mpz_set_ui(v, 0);
	mpz_set_ui(u2, 0);
	mpz_set_ui(v2, 1);
	
	while( mpz_cmp_ui(rop2, 0) != 0 )
	{
		mpz_fdiv_q(quot, rop, rop2);
		
		mpz_set(rops, rop);
		mpz_set(us, u);
		mpz_set(vs, v);
		
		mpz_set(rop, rop2);
		mpz_set(u, u2);
		mpz_set(v, v2);
		
		mpz_mul(qrop, quot, rop2);
		mpz_mul(qu, quot, u2);
		mpz_mul(qv, quot, v2);
		
		mpz_sub(rop2, rops, qrop);
		mpz_sub(u2, us, qu);
		mpz_sub(v2, vs, qv); 
	}
	
	mpz_clear(rop2);
	mpz_clear(u2);
	mpz_clear(v2);
	mpz_clear(quot);
	mpz_clear(rops);
	mpz_clear(us);
	mpz_clear(vs);
	mpz_clear(qrop);
	mpz_clear(qu);
	mpz_clear(qv);
}

/*fonction remplacant mpz_invert en utilisant l'algorithme d'euclide etendu*/
int invert(mpz_t rop, const mpz_t op1, const mpz_t op2)
{
	mpz_t r, u, v;
	
	mpz_init(r);
	mpz_init(u);
	mpz_init(v);
	
	extendedEuclide(rop, u, v, op1, op2);
	
	mpz_set(rop, u);
	
	int i = 1;
	while( mpz_cmp_ui(rop, 0) < 0 )
	{
		mpz_mul_ui(r, op2, i);
		mpz_add(rop, u, r);
	}

	mpz_clear(r);
	mpz_clear(u);
	mpz_clear(v);
	
	return 1;
}

/*function which make the rsa encryption
 * This function creates the keys.
 *
 *  1. Generate two large distinct primes p and q randomly
 *  2. Calculate n = pq and x = (p-1)(q-1)
 *  3. Select a random integer e (1<e<x) such that gcd(e,x) = 1
 *  4. Calculate the unique d such that ed = 1(mod x)
 *  5. Public key pair : (e,n), Private key pair : (d,n)
 * and return an array of mpz_t with
 * [0]: n
 * [1]: Private Keys
 * [2]: Public Keys
 * [3]: The encrypted message
 */
mpz_t* rsa_encrypt(mpz_t msg, mpz_t seed, int primes_size, mpz_t tab_res[3])
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
    
    gmp_randstate_t rand;
    gmp_randinit_mt(rand);
    gmp_randseed(rand, seed);
    
    // get rand nbers
    mpz_urandomb(p, rand, primes_size);
    mpz_urandomb(q, rand, primes_size);
    
    // get the next prime nber
    //mpz_nextprime(p, p);
    nextprime(p,p,primes_size);
    //mpz_nextprime(q, q);
    nextprime(q,q,primes_size);
    
   
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
    if(invert (d, e, x) == 0) //if(mpz_invert (d, e, x) == 0)
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
    
    mpz_t c_msg;
    mpz_init(c_msg);
    
    powm(c_msg, msg, e, n);
    
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

    return tab_res;
}

void rsa_decrypt(mpz_t d_msg, mpz_t* tab)
{
    powm(d_msg, tab[3], tab[2], tab[0]);

    /*char dec_str[1000];
    mpz_get_str(dec_str,10,d_msg);

    //std::cout << "le message c'etait bien " << dec_str << " non ?" << std::endl;*/
}

// fonction qui appel plusieurs fois les fonctions de codage / décodage avec des données aléatoires
void rsa_random_test (int iter)
{
	/*
	 * Tableau de mpz_t contenant :
	 * [0]: n
	 * [1]: Private Keys
	 * [2]: Public Keys
	 * [3]: The encrypted message
	 */
    mpz_t tab_res[4];

    /* init the mpz numbers*/
    for(int i = 0; i<4; ++i)
    {
        mpz_init(tab_res[i]);
    }

	// initialisation des variables locales 
    mpz_t msg, d_msg, seed;

    mpz_init(d_msg);
    mpz_init(seed);
    mpz_init(msg);

	// Pour avoir une graines qui n'est pas la même à chaque exécution du code
	// on utilise le temps
    unsigned long int ui_seed = time(NULL);

	// initialisation du génératieur de nombres aléatoires
    mpz_init_set_ui (seed, ui_seed);
    
    gmp_randstate_t rand;
    gmp_randinit_mt(rand);
    gmp_randseed(rand, seed);
    

    for (int i = iter; i != 0; --i)
    {
        // génération d'un nombre aléatoire (c'est le message à envoyer)
        mpz_urandomb(msg, rand, 10);

        // génération d'un nombre aléatoire (c'est la graine pour les générateurs de nombres premiers)
        mpz_urandomb(seed, rand, 5);
		
		// on encrypte le message
        rsa_encrypt(msg, seed, 30, tab_res);
		
		// on decrypte le message
        rsa_decrypt(d_msg, tab_res);

		// conversion en chaine de caractaires pour l'affichage
        char msg_str[1000];
        mpz_get_str(msg_str,10, msg);

        char d_str[1000];
        mpz_get_str (d_str,10, d_msg);

        char e_str[1000];
        mpz_get_str( e_str,10, tab_res[3]);

		// Affichage
        std::cout << "-------------------------------------------------" << std::endl;
        std::cout << "| Message sent      : " << msg_str << std::endl;
        std::cout << "| Message encrypted : " << e_str << std::endl;
        std::cout << "| Message decrypted : " << d_str << std::endl;
        std::cout << "-------------------------------------------------" << std::endl;


    }
}

/* Main subroutine */
int main()
{
	
	// appel de la fonction de test encodage/decodage
    rsa_random_test (5);


    return 0;
}

