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
        
    for(vector<bitset<N> >::const_iterator i = bitsetVector.begin(); i != bitsetVector.end();++i)
    {
        // Code to modify (sample)      
        bitset<N> inBuffer = *i;
        bitset<HAMMING_7> outBuffer;

        outBuffer[0] = (inBuffer[0]+inBuffer[1]+inBuffer[3])%2;
        outBuffer[1] = (inBuffer[0]+inBuffer[2]+inBuffer[3])%2;
        outBuffer[2] = inBuffer[0];
        outBuffer[3] = (inBuffer[1]+inBuffer[2]+inBuffer[3])%2;
        
        outBuffer[4] = inBuffer[1];
        outBuffer[5] = inBuffer[2];
        outBuffer[6] = inBuffer[3];
        
                string tmp = outBuffer.to_string();
                std::reverse(tmp.begin(), tmp.end());
                
        if(DEBUG_HE)
            cout << " | " << tmp;
        
        encodedBitset.push_back(outBuffer);
    }
    
    if(DEBUG_HE)
        cout << endl;
    
    return encodedBitset;
}

vector<bitset<N> > HammingDecoding(const vector<bitset<HAMMING_7> > &bitsetVector)
{
        vector<bitset<N> > decodedBitset;
    
    if(DEBUG_HE)
        std::cout << "----------Decode-------------" << endl;
        
    for(vector<bitset<HAMMING_7> >::const_iterator i = bitsetVector.begin(); i != bitsetVector.end();++i)
    {     
        bitset<HAMMING_7> inBuffer = *i;
        bitset<N> outBuffer;

        outBuffer[0] = inBuffer[2];
        outBuffer[1] = inBuffer[4];
        outBuffer[2] = inBuffer[5];
        outBuffer[3] = inBuffer[6];
        
                string tmp = outBuffer.to_string();
                
        if(DEBUG_HE)
            cout << " | " << tmp;
        
        decodedBitset.push_back(outBuffer);
    }
    
    if(DEBUG_HE)
        cout << endl;
                
        
    return decodedBitset;
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

        outBuffer[0] = (inBuffer[3]+inBuffer[4]+inBuffer[5]+inBuffer[6])%2;
        outBuffer[1] = (inBuffer[1]+inBuffer[2]+inBuffer[5]+inBuffer[6])%2;
        outBuffer[2] = (inBuffer[0]+inBuffer[2]+inBuffer[4]+inBuffer[6])%2;

        bitset<3> bitsetRef(0ul);

        //si le bitSet courrant est diffèrent du bitSet de reference (0 0 0) il y a doonc une erreur
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

}
