#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <bitset>
#include <cstdlib>
#include <ctime>

const int N = 2;
const int K = 1;
const int R = 4;

#define DEBUG

using namespace std;

////////////////////////////////////////////////////////////
//      template<int bits> bitset<bits> randBitset()      //
//                                                        //
//               Generate random bitset                   //
////////////////////////////////////////////////////////////

template<int bits> bitset<bits> randBitset()
{
	bitset<bits> r(rand());
	for (int i = 0; i < bits / 16 - 1; i++)
	{
		r <<= 16;
		r |= bitset<bits>(rand());
	}
	return r;
}

////////////////////////////////////////////////////////////
// vector< bitset<N> > GSM_code(vector< bitset<K> > mess) //
//                                                        //
//     Convolutional coding of a message (GSM norm)       //
////////////////////////////////////////////////////////////

void encodage(const bitset<R + 1>& reg, const bitset<R + 1> G0, const bitset<R + 1> G1, bitset<N>& cod_out)
{
	int g0, g1;

	g0 = (reg & G0).count() % 2;
	g1 = (reg & G1).count() % 2;

	cod_out.set(0, g0);
	cod_out.set(1, g1);
}

vector< bitset<N> > GSM_code(vector< bitset<K> > mess)
{
	int i = 0;
	vector< bitset<N> > mess_out;

	bitset<N> cod_out;
	bitset<R + 1> G0(25);
	bitset<R + 1> G1(27);
	bitset<R + 1> reg;
	reg.reset();

#ifdef DEBUG
	std::cout << "-------------------- Debug Informations (Coding) --------------------" << endl << endl;
	std::cout << "Initial register ( u(i-4)  u(i-3)  u(i-2)  u(i-1)  u(i)  ): " << reg << endl;
	std::cout << "Polynom G0       ( g0(i-4) g0(i-3) g0(i-2) g0(i-1) g0(i) ): " << G0 << endl;
	std::cout << "Polynom G1       ( g1(i-4) g1(i-3) g1(i-2) g1(i-1) g1(i) ): " << G1 << endl << endl;
#endif

	for (vector<bitset<K> >::iterator it = mess.begin(); it != mess.end(); ++it)
	{
		reg = reg << 1;
		reg.set(0, (*it).count());

		cod_out.reset();

		encodage(reg, G0, G1, cod_out);

		mess_out.push_back(cod_out);

#ifdef DEBUG
		std::cout << "Block number: " << ++i << " - In frame: " << *it << endl;
		std::cout << "\t Current status of registers: " << reg << endl;
		std::cout << "\t Out : " << cod_out << endl;
#endif
	}
#ifdef DEBUG
	std::cout << "------------------------------------------------------------------" << endl << endl;
#endif

	return mess_out;
}

/////////////////////////////////////////////////////////////////////////
// vector< bitset<N> >  GSM_transmission(vector< bitset<N> > mess_cod) //
//                                                                     //
//         Simulation of a transmission channel => adding errors       //
/////////////////////////////////////////////////////////////////////////

vector< bitset<N> >  GSM_transmission(vector< bitset<N> > mess_cod)
{
	vector< bitset<N> > mess_tra = mess_cod;

	//TODO: Code here
	
    for(int i = 0; i < 4; ++i)
    {
        int idxError = rand() % mess_cod.size();	//on choisi un index au hasard dans le message
        mess_tra[idxError] = randBitset<N>();		//et on ajoute un bitset<N> généré aleatoirement a l'index choisi
        cout << "Error index: " << idxError << endl;	//on affiche à l'utilisateur quel index a été choisi pour inserer l'erreur
    }   

	return mess_tra;
}

// class contenant toute les informations utiles concernant l'etat courrant a un noeud
// d'un des chemins qui peut mener au decodage
class code_stat
{
	public :
	
	        // registre d'état courrant
		bitset<R + 1> registre;
		
		// code decode (qui peut etre le code final)
		vector< bitset<K> > code;
		
		// erreur cumule au cour du chemin
		unsigned int err;

                // operateur de comparaison de registre
		inline bool operator==(bitset<R + 1> &rcs)
		{
			return registre == rcs;
		}
};


/*
 * Cette fonction sert a eliminer les doubles, c'est a dire a un instant t
 * certaine class code_stat ont le meme registre, elle sont donc sur le meme
 * noeud il faut donc en supprimer l'une des deux: celle qui a l'erreure 
 * cumule la moins grande --> mais cette fonction met beaucoupe de temps a l'execution et nous nous interogons sur sont utilite
*/
void suppr_double(vector<code_stat> &vect)
{

	vector<code_stat>::iterator f_it = vect.begin();
        
        // On parcour le vecteur contenant toute les instances de code_stat pour trouver celle qui ont le meme registre
	for (vector<code_stat>::iterator it = vect.begin(); it != vect.end(); ++it)
	{
                
                // pour chaque vecteur on verifie si les registres sont egaux
		for (; f_it != vect.end(); ++f_it)
		{
			if(f_it != it && (*f_it).registre == (*it).registre)
			{
				break;
			}
		}

                // si on trouve des vecteur en double
		if (f_it != vect.end())
		{
		        // on choisi quel class supprimer en fonction le l'erreur la plus grande
			if ((*f_it).err > (*it).err)
			{
				f_it = vect.erase(f_it);
			}
			else
			{
				f_it = vect.erase(it);
			}
		}
	}
}


// fonction qui calcule la distance de Hamming entre deux bitset
int hamming_dist(bitset<N> cod_out, bitset<N> mess)
{
	int distance = 0;

	int cod_size = cod_out.size();
	//pour chaque bit des mots 
	for (int i = 0; i < cod_size; i++) {

		//on augmente la distance de 1 si les bits sont differents 
		if (cod_out[i] != mess[i]) distance++;

	}

	return distance;
}



//////////////////////////////////////////////////////////////////
// vector< bitset<K> > GSM_decode(vector< bitset<N> > mess_tra) //
//                                                              //
//     Convolutional decoding of a message (GSM norm)           //
//////////////////////////////////////////////////////////////////

vector< bitset<K> > GSM_decode(vector< bitset<N> > mess_tra)
{

	bitset<R + 1> G0(25);
	bitset<R + 1> G1(27);
	bitset<N> cod_out;


	vector< bitset<K> > mess_dec;

	vector<code_stat> code_stat_vect;
	vector<code_stat> tmp_vect;

	//init vect
	code_stat initStat;
	initStat.registre.reset();
	initStat.err = 0;

	code_stat_vect.push_back(initStat);

        
	for (vector<bitset<N> >::iterator mess_it = mess_tra.begin(); mess_it != mess_tra.end(); ++mess_it)
	{
		//std::cout << (*mess_it) << endl;


		tmp_vect = code_stat_vect;	//on utilise un vecteur temporaire pour travailler
		code_stat_vect.clear();
		
		vector<code_stat>::iterator struct_it = tmp_vect.begin();
		
		
		for (; struct_it != tmp_vect.end(); ++struct_it)
		{
			//cout<<code_stat_vect.size()<<endl;

			//duplication instance car a chaque noeud on a deux nouvelle posibilites (0 ou 1)
			code_stat tmp0 = *struct_it;
			code_stat tmp1 = *struct_it;

			//mise a jour des code de registre courrant des struct
			tmp0.registre = tmp0.registre << 1;
			tmp1.registre = tmp1.registre << 1;
			tmp1.registre.set(0, true);
			tmp0.registre.set(0, false);

			//mise a jour du code courrant (code qui sera peut etre le code du message decode)
			tmp0.code.push_back(bitset<K>(0));
			tmp1.code.push_back(bitset<K>(1));

			//calcul de l'erreur courante
			//on recalcule le code GSM pour le message courant que l'on a dans la struct courante
			//et on prend le dernier bitset<2> qui contient la réponse de l'automate a un 0 ou un 1
			//dans l'etat courant contenu dans le registre
			encodage(tmp0.registre, G0, G1, cod_out);
			int dist = hamming_dist(cod_out, *mess_it);
			tmp0.err += dist;
                        
			dist = 0;
			
			// on encode ce code courant en fonction de l'etat de registre pour pouvoir avoir la sortie 
			// de "l'automate" et par la suite pouvoir calculer les erreurs avec la distance de Hamming
			encodage(tmp1.registre, G0, G1, cod_out);
			dist = hamming_dist(cod_out, *mess_it);
			tmp1.err += dist;

			//une fois que les struct sont mises a jour elles sont ajoutees au
			//vect qui contient toute les instances en cours
			code_stat_vect.push_back(tmp0);
			code_stat_vect.push_back(tmp1);

			//std::cout << "avant suppr double : " << code_stat_vect.size() << endl;
			
			suppr_double(code_stat_vect);

			//std::cout << "apres suppr double : " << code_stat_vect.size() << endl;
		}

	}
	
	// on va alors recuperer le code decode qui est la class restante dans le vecteur avec la plus petite erreur
	if( code_stat_vect.size() > 0 )
        {
                code_stat minPath = code_stat_vect[0];
                unsigned int min = code_stat_vect[0].err;
                 
                // on selectionne la class avec la plus petite erreur
                for (vector<code_stat>::iterator it = code_stat_vect.begin(); it != code_stat_vect.end(); ++it)
                {
                    if( (*it).err < min )
                    {
                        min = (*it).err;
                        minPath = (*it);
                    }
                }
                
                
                // on sauvgarde le message decode   
                for (vector<bitset<K> >::iterator it = minPath.code.begin(); it != minPath.code.end(); ++it)
                {
                    mess_dec.push_back((*it));
                }       
        }


	return mess_dec;
}





//////////////////////////////////////////////////////////////////
//                             MAIN                             //
//////////////////////////////////////////////////////////////////

int main()
{
	int NbMot = 12;

	vector< bitset<K> > mess;
	vector< bitset<N> > mess_cod;
	vector< bitset<N> > mess_tra;
	vector< bitset<K> > mess_dec;

	// Random initialization message
	srand((unsigned)time(NULL));
	for (int i = 0; i<NbMot; ++i)
		mess.push_back(randBitset<K>());
	for (int i = 0; i<R; ++i)
		mess.push_back(bitset<K>(0));

	// Coding of the message => mess_cod
	mess_cod = GSM_code(mess);

	// Simulation of a transmission (errors) => mess_tra
	mess_tra = GSM_transmission(mess_cod);

	// Decoding of the transmitted message => mess_dec
	mess_dec = GSM_decode(mess_tra);

	std::cout << "Source Message   : ";
	for (vector<bitset<K> >::iterator it = mess.begin(); it != mess.end(); ++it)
		std::cout << ' ' << *it;
	std::cout << '\n';

	std::cout << "Coded Message    : ";
	for (vector<bitset<N> >::iterator it = mess_cod.begin(); it != mess_cod.end(); ++it)
		std::cout << ' ' << *it;
	std::cout << '\n';

	std::cout << "Received Message : ";
	for (vector<bitset<N> >::iterator it = mess_tra.begin(); it != mess_tra.end(); ++it)
		std::cout << ' ' << *it;
	std::cout << '\n';

	std::cout << "Decoded Message  : ";
	for (vector<bitset<K> >::iterator it = mess_dec.begin(); it != mess_dec.end(); ++it)
		std::cout << ' ' << *it;
	std::cout << '\n';


	return 0;
}
