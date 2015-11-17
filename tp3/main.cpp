#include <iostream>      
#include <fstream>
#include <string>         
#include <bitset>        
#include <vector>
#include <algorithm>

/**
 * Hamming (7,4)
 **/ 
#define N 4
#define HAMMING_7 7

/**
 * Debug macros
 **/ 
#define DEBUG_RF false // Debug Information: Read File
#define DEBUG_HE true // Debug Information: Hamming Encoding

using namespace std; 

/**
 * vector<bitset<N> > readFile(string filename)
 * Read a file in binary and create a vector of bitset wih a width of 4 for each bitset
 * Return a vector bitset
 **/ 
vector<bitset<N> > readFile(string filename)
{
    vector<bitset<N> > content;
    ifstream reader;
    char buffer;
    reader.open(filename.c_str(), ios::binary|ios::in);

    if(DEBUG_RF)
        cout << "Read : \t";

    if(reader != NULL && reader.is_open())
    {
        while(!reader.eof())
        {
            reader.read(&buffer, 1);
            bitset<N> bsBufferLSB(buffer);
            bitset<N> bsBufferMSB(buffer>>4);
            
            content.push_back(bsBufferMSB);
            content.push_back(bsBufferLSB);
    
            if(DEBUG_RF)
            {
                cout << " |" << bsBufferMSB.to_string();
                cout << " |" << bsBufferLSB.to_string();
            }
        }
    }
    
    if(DEBUG_RF)
        cout << endl;

    reader.close();
    return content;
} 

/**
 * vector<bitset<HAMMING_7> > HammingEncoding(vector<bitset<N> > bitsetVector)
 * Convert a vector of bitset<4> into a hamming vector of bitset<7>
 **/ 
vector<bitset<HAMMING_7> > HammingEncoding(const vector<bitset<N> > &bitsetVector)
{
    vector<bitset<HAMMING_7> > encodedBitset;
    
    if(DEBUG_HE)
        std::cout << "----------Encode-------------" << endl;
        
    for(vector<bitset<N> >::const_iterator i = bitsetVector.begin(); i != bitsetVector.end();++i)	//on parcourt le vecteru de bitset
    {
        // Code to modify (sample)      
        bitset<N> inBuffer = *i;
        bitset<HAMMING_7> outBuffer;

        outBuffer[0] = (inBuffer[0]+inBuffer[1]+inBuffer[3])%2;		//on convertit le bitset<4> en bitset<HAMMING_7>  
        outBuffer[1] = (inBuffer[0]+inBuffer[2]+inBuffer[3])%2;		//pour cela, on recupere les 4 bits de donnees en position 2,4,5 et 6
        outBuffer[2] = inBuffer[0];									//et on ajoute 3 bits de redondance qui sont calcules en ajoutant les bits de donnees ensemble
        outBuffer[3] = (inBuffer[1]+inBuffer[2]+inBuffer[3])%2;		//et en converstissant les valeurs obtenues en binaire
        
        outBuffer[4] = inBuffer[1];
        outBuffer[5] = inBuffer[2];
        outBuffer[6] = inBuffer[3];
        
                string tmp = outBuffer.to_string();					//on convertit le bitset obtenu en string
                std::reverse(tmp.begin(), tmp.end());
                
        if(DEBUG_HE)
            cout << " | " << tmp;									//et on l'affiche
        
        encodedBitset.push_back(outBuffer);
    }
    
    if(DEBUG_HE)
        cout << endl;
    
    return encodedBitset;											//on retourne le vecteur de bitset<4>
}

vector<bitset<N> > HammingDecoding(const vector<bitset<HAMMING_7> > &bitsetVector)
{
        vector<bitset<N> > decodedBitset;
    
    if(DEBUG_HE)
        std::cout << "----------Decode-------------" << endl;
        
    for(vector<bitset<HAMMING_7> >::const_iterator i = bitsetVector.begin(); i != bitsetVector.end();++i)	//on parcourt le vecteur
    {     
        bitset<HAMMING_7> inBuffer = *i;
        bitset<N> outBuffer;

        outBuffer[0] = inBuffer[2];			//on sait à quelles positions
        outBuffer[1] = inBuffer[4];			//se trouvent les bits correspondants aux données
        outBuffer[2] = inBuffer[5];			//on les recupere
        outBuffer[3] = inBuffer[6];
        
                string tmp = outBuffer.to_string();	//on les convertit en string
                
        if(DEBUG_HE)
            cout << " | " << tmp;					//et on les affiche
        
        decodedBitset.push_back(outBuffer);
    }
    
    if(DEBUG_HE)
        cout << endl;
                
        
    return decodedBitset;					//on retourne le vecteur decode
}

vector<unsigned int> verificationCode(vector<bitset<HAMMING_7> > &bitsetVector)
{
    vector<bitset<HAMMING_7> >::const_iterator it = bitsetVector.begin();

    vector<unsigned int> errorVector;

    std::cout << "----------Code erreur -------------" << endl;

    while(it != bitsetVector.end())
    {
        bitset<HAMMING_7> inBuffer = *it;
        bitset<3> outBuffer;

        outBuffer[0] = (inBuffer[3]+inBuffer[4]+inBuffer[5]+inBuffer[6])%2;	//au lieu de multiplier le bitset par la matrice de verification V
        outBuffer[1] = (inBuffer[1]+inBuffer[2]+inBuffer[5]+inBuffer[6])%2;	//on simplifie les operations pour avoir seulement des additions et gagner en performances
        outBuffer[2] = (inBuffer[0]+inBuffer[2]+inBuffer[4]+inBuffer[6])%2;	//     | 0 0 0 1 1 1 1 |
																			// V = | 0 1 1 0 0 1 1 |
        bitset<3> bitsetRef(0ul);											//     | 1 0 1 0 1 0 1 |

        //si le bitSet courant est different du bitSet de reference (0 0 0) il y a donc une erreur
        if(outBuffer != bitsetRef)
        {
            errorVector.push_back((unsigned int)outBuffer.to_ulong());
            errorVector.push_back(it - bitsetVector.begin());
        }

        if(DEBUG_HE)
            cout << outBuffer[0] << " " << outBuffer[1] << " " << outBuffer[2] << "|";
        
        ++it;
    }

    cout << endl;

    return errorVector;
}


void insertError(vector<bitset<HAMMING_7> > &bitsetVector)
{

    //On rajoute 4 erreurs

    cout << "On ajoute les erreurs ..." << endl;

    bitset<HAMMING_7> tmp =  bitsetVector[3];

    cout << "err 1 -->" << "Lettre numero 3 : " <<bitsetVector[3] << "---" << tmp.set(1, true) << endl;

    bitsetVector[3] = tmp;



    tmp =  bitsetVector[7];

    cout << "err 1 -->" << "Lettre numero 7 : " <<bitsetVector[7] << "---" << tmp.set(2, true)<< endl;

    bitsetVector[7] = tmp;



    tmp =  bitsetVector[11];

    cout << "err 1 -->" << "Lettre numero 11: " <<bitsetVector[11] << "---" << tmp.set(4, false)<< endl;

    bitsetVector[11] = tmp;



    tmp =  bitsetVector[26];

   cout << "err 1 -->" << "Lettre numero 26: " <<bitsetVector[26] << "---" << tmp.set(5, false)<< endl;

    bitsetVector[26] = tmp;

}

void HammingDistance2Words(const vector<bitset<HAMMING_7> > &bitsetVector, int posWord1, int posWord2)
{

	bitset<HAMMING_7> word1, word2;
	int distance = 0;
	int vectorSize = bitsetVector.size();
	if(posWord1 < vectorSize && posWord2 < vectorSize){	//on vérifie que les index des mots a comparer sont plus petits que la taille du vecteur

		word1 = bitsetVector[posWord1];					//on récupère les deux mots a comparer
		word2 = bitsetVector[posWord2];
		
		int wordSize = word1.size();
		for(int i = 0; i < wordSize; i++){				//pour chaque bit des mots
			if(word1[i] != word2[i]) distance ++;		//on augmente la distance de 1 si les bits sont differents
		}

	}  
	cout << "La distance de Hamming entre deux mots est de " << distance << endl;	//on affiche la distance calculee
    
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                     Main                                                       //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main()
{
    vector< bitset<N> > input_data;
    vector< bitset<HAMMING_7> > encode_data; 

    // Read data to encode
    input_data = readFile("test.txt");

    // Encode by Hamming (7,4) coding
    encode_data = HammingEncoding(input_data);

    //return vector of bitset<4>
    HammingDecoding(encode_data);
    
    //sauvegarde des donnees encodees par Hamming avant l'injection d'erreur pour calculer la distance plus tard
    vector< bitset<HAMMING_7> > encoded_data = encode_data;

	// on insère des erreurs dans les donnees
    insertError(encode_data);

    vector<unsigned int> errorVector = verificationCode(encode_data);

    //affichage des erreurs ...

    cout << "affichage des erreurs" << endl;

    vector<unsigned int>::const_iterator it = errorVector.begin();

    while(it != errorVector.end())
    {
        cout << "erreur au bit numero : " << *it;
        ++it;
        cout << ", a la lettre : " << *it << endl;
        ++it;
    }
    
    // calcul de la distance de hamming entre deux mots, ici pour le code de Hamming (7,4)
    
    HammingDistance2Words(encoded_data, 0, 1);

}
