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
	cout << "-------------------- Debug Informations (Coding) --------------------" << endl << endl;
	cout << "Initial register ( u(i-4)  u(i-3)  u(i-2)  u(i-1)  u(i)  ): " << reg << endl;
	cout << "Polynom G0       ( g0(i-4) g0(i-3) g0(i-2) g0(i-1) g0(i) ): " << G0 << endl;
	cout << "Polynom G1       ( g1(i-4) g1(i-3) g1(i-2) g1(i-1) g1(i) ): " << G1 << endl << endl;
#endif

	for (vector<bitset<K> >::iterator it = mess.begin(); it != mess.end(); ++it)
	{
		reg = reg << 1;
		reg.set(0, (*it).count());

		cod_out.reset();

		encodage(reg, G0, G1, cod_out);

		mess_out.push_back(cod_out);

#ifdef DEBUG
		cout << "Block number: " << ++i << " - In frame: " << *it << endl;
		cout << "\t Current status of registers: " << reg << endl;
		cout << "\t Out : " << cod_out << endl;
#endif
	}
#ifdef DEBUG
	cout << "------------------------------------------------------------------" << endl << endl;
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

	return mess_tra;
}

class code_stat
{
	public :
		bitset<R + 1> registre;
		vector< bitset<K> > code;
		unsigned int err;

		inline bool operator==(bitset<R + 1> &rcs)
		{
			return registre == rcs;
		}
};

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

	//init vect
	code_stat initStat;
	initStat.registre.reset();
	initStat.err = 0;

	code_stat_vect.push_back(initStat);


	for (vector<bitset<N> >::iterator mess_it = mess_tra.begin(); mess_it != mess_tra.end(); ++mess_it)
	{
		cout << (*mess_it) << endl;

		for (vector<code_stat>::iterator struct_it = code_stat_vect.begin(); struct_it != code_stat_vect.end(); ++struct_it)
		{
			//duplication instance
			code_stat tmp0 = *struct_it;
			code_stat tmp1 = *struct_it;

			//mise a jour des code de registre courrant des struct
			tmp0.registre = tmp0.registre >> 1;
			tmp1.registre = tmp1.registre >> 1;
			tmp1.registre.set(3, true);

			//mise a jour du code courrant (code qui sera peut etre le code du message decode)
			tmp0.code.push_back(bitset<K>(0));
			tmp1.code.push_back(bitset<K>(1));

			//calcul de l'erreur courrante
			//on recalcul le code GSM pour le message courrant que l'on a dans la struct courrante
			//et on prend le dernier bitset<2> qui contient la r�ponse de l'automate a un 0 ou un 1
			//dans l'etat courrant contenu dans le registre
			encodage(tmp0.registre, G0, G1, cod_out);
			int dist = hamming_dist(cod_out, *mess_it);
			tmp0.err += dist;

			dist = 0;

			encodage(tmp1.registre, G0, G1, cod_out);
			dist = hamming_dist(cod_out, *mess_it);
			tmp1.err += dist;

			//une fois que les struct sont mises a jours elles sont ajoutees au
			//vect qui contient toute les instances en cours
			*struct_it = tmp0;
			code_stat_vect.push_back(tmp1);

			suppr_double(code_stat_vect);

		}

	}

	/////////// TO DELETE AND MODIFY ///////////
	for (unsigned int i = 0; i<mess_tra.size(); ++i)
		mess_dec.push_back(randBitset<K>());
	////////////////////////////////////////////

	return mess_dec;
}



void suppr_double(vector<code_stat> &vect)
{
	for (vector<code_stat>::iterator it = vect.begin(); it != vect.end(); ++it)
	{
		vector<code_stat>::iterator f_it = find(vect.begin(), vect.end(), (*it).registre);

		if (it != vect.end())
			std::cout << "Element found in myvector:\n";
		else
			std::cout << "Element not found in myvector\n";
	}
}



int hamming_dist(bitset<N> cod_out, bitset<N> mess)
{
	int distance = 0;

	int cod_size = cod_out.size();
	//pour chaque bit des mots 
	for (int i = 0; i < cod_size; i++) {

		//on augmente la distance de 1 si les bits sont differents 
		if (cod_out[i] != mess[i]) distance++;

	}
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

	cout << "Source Message   : ";
	for (vector<bitset<K> >::iterator it = mess.begin(); it != mess.end(); ++it)
		cout << ' ' << *it;
	cout << '\n';

	cout << "Coded Message    : ";
	for (vector<bitset<N> >::iterator it = mess_cod.begin(); it != mess_cod.end(); ++it)
		cout << ' ' << *it;
	cout << '\n';

	cout << "Received Message : ";
	for (vector<bitset<N> >::iterator it = mess_tra.begin(); it != mess_tra.end(); ++it)
		cout << ' ' << *it;
	cout << '\n';

	cout << "Decoded Message  : ";
	for (vector<bitset<K> >::iterator it = mess_dec.begin(); it != mess_dec.end(); ++it)
		cout << ' ' << *it;
	cout << '\n';

	system("pause");

	return 0;
}