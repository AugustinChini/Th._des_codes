#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <array>
#include <math.h>
#include <sstream>
#include <fstream>

using namespace std;

typedef array<pair<char, double>, 26> FreqArray;

class VigenereCryptanalysis
{
private:
	array<double, 26> targets;
	array<double, 26> sortedTargets;


public:
	VigenereCryptanalysis(const array<double, 26>& targetFreqs)
	{
		targets = targetFreqs;
		sortedTargets = targets;
		sort(sortedTargets.begin(), sortedTargets.end());
	}

	pair<string, string> analyze(string input)
	{
		string key = "ISIMA PERHAPS";
		string result = input;
		
		unsigned int  keyLength = getKeyLength(result);						//getting the key length 
		cout << "------------Key Length----------> " << keyLength << endl;
		
		key = getKey(result, keyLength);									//getting the key using the length
		
		result = decrypt(result, key);										//decryption of the text using the key found

		return make_pair(result, key);
	}
	
  string decrypt(string text, string key)
  {
    string out;
    
    unsigned int j = 0;
    
    for(unsigned int i = 0; i < text.size(); ++i)
    {   
		char current_code_char = text.at(i);
		
		char current_key_char = key.at(j);
		
		if(j != (key.size())-1)
		{
			++j;
		}
		else
		{
			j = 0;
		}
		
		char encrypt_index = ((current_code_char - current_key_char+26)%26)+'A';
		
		out.push_back(encrypt_index);
		
    }

    return out;
  }

	unsigned int getKeyLength(string encrypt)
	{
		string res = "";
		double ICsum = 0;
		vector<double> vectICAvg;

		// we try to find a key between 2 and 10 char
		for (unsigned int i = 1; i <= 10; ++i)
		{
			// get the char corresponding to the index
			// check if the string is shorter than the index
			if (encrypt.size() >= i)
			{
				string tmp = "";

				for (unsigned int k = 0; k < i; ++k)
				{
					unsigned int j = k;
					
					do
					{
						tmp += encrypt[j];
						j += i;

					} while (j + i < encrypt.size());
					
					
					ICsum += getIC(tmp);

					tmp = "";
				}
				
				cout << "IC average : "<< ICsum / (double)i << " - " << i << endl;

				vectICAvg.push_back( ICsum / (double)i );

				ICsum = 0;
			}
		}

		return findLenght(vectICAvg);

	}

	unsigned int findLenght(vector<double>& v)
	{
		unsigned int index = 0;
		bool find = false;

		for(unsigned int i = 0; i < v.size() && find == false; ++i)
		{
			if(0.06 <= v[i])		//we are looking for an IC value superior to 0.06
			{
				index = i;
				find = true;
			}

		}
		
		if(find)					//if there is one, we return the index of this IC value
			return index+1;
		else {
			unsigned int maxValIndex = (max_element(v.begin(), v.end()))-v.begin();	//if there is no IC value above 0.06
			return maxValIndex+1;													//we return the maximum IC value
		}
	}


	double getIC(string s)	//calculation of the index of coincidence
	{

		int size = s.size();

		double res = 0.0;

		for (char c = 'A'; c <= 'Z'; ++c)
		{
			int current_counter = counter(s, c);

			res += (current_counter*(current_counter - 1)) / (double)(size*(size - 1));
		}

		return res;

	}

	unsigned int counter(const string& s, char c)	//function to count the number of times a letter appears in a text
	{
		unsigned int cpt = 0;

		for (unsigned int i = 0; i<s.size(); ++i)
		{
			if (s[i] == c)
				++cpt;
		}

		return cpt;
	}
	
	
	string getKey(string s, int keyLength)	//function to get the key of the ciphertext
	{

		int size = s.size();
		string key = "";
		char b;
		int a;
		
		for(int i = 0; i < keyLength; i++){
			
			string sequence = "";
			
			for (int j = i; j < size; j+=keyLength){
				
				sequence+= s.at(j);		//extraction of a sequence from the ciphertext by getting a letter every "keylength" letter
				
			}

			a = chiSquarred(sequence); //we calculate the chiSquarred value of the sequence to have the index corresponding to the letter value

			b = (char)a;	//the index is converted into letter
			b += 'A';
			key += b;	//and added to the key
		}
		
		return key;

	}
	
	unsigned int chiSquarred(const string& s)
	{
		double chiMin = 0;
		unsigned int index = 0;
		
		int size = s.size();
		
		string tabString[26];
		tabString[0] = s;
		string newString = "";

		for(int h = 0; h < 26; h++){		//for the sequence we calculate 25 new sequences by shift the letters on by one
			if (h != 0){
				for (int j = 0; j < size; j++){
					char nextLetter = tabString[h-1].at(j);
					
					if (nextLetter == 'A')
						nextLetter = 'Z';
					else
						nextLetter = (char)(((int) nextLetter) - 1);
					
					
					newString += nextLetter;
				}
			}else{
				newString = s;
			}
			
			tabString[h] = newString;
			newString = "";
		}
		
		
		for (int i = 0; i < 26; i++){		//with every sequences we calculate the chiSquarred value
			
			double chi = 0;
		
			for (char c = 'A'; c <= 'Z'; ++c)
			{
				int current_counter = counter(tabString[i], c);
				
				int posLetter = c - 'A' + 1;
				
				double numberExpected = size * targets[posLetter-1];
				
				chi += (pow((current_counter - numberExpected), 2))/numberExpected;
				
			}
			
			if(i == 0) {				//if we have calculated chiSquarred value for the first sequence
				chiMin = chi;			//we initialize the minimum chiSquarred value with the current value
			}
				
			if(chi < chiMin) {			//if the current chiSquarred value calculated is smaller than chiMin
				chiMin = chi;			//the chiMin value is updated with the the current chiSquarred value
				index = i;
			}
			
			newString = "";
			
		}
		
		return index;
	}
	
	

};

string getTextToUpcase(string& text){	//function to convert the text to uppercase
	
	string upcasedText = "";
	
	for(unsigned int i = 0; i < text.size(); ++i)
    {
      if(text[i] >= 'A' && text[i] <= 'Z')
        upcasedText += text[i];
      else if(text[i] >= 'a' && text[i] <= 'z')
        upcasedText += text[i] + 'A' - 'a';
    }
    
    return upcasedText;
    
}

int main()
{
	//KDVTDFOEJJLNRHRTBNLDETKWPSSJRQTDQJAMUKHSXQEBYIXCVDRZYCVDVPMFIMKRUMUSXYXQYRXVFCXBIGMRHDOVRGSOYSEGCJRFPJECKV 'AZERTY'
	//PRINCIPEDEKERCKHOFFSTOUTELASECURITEDUNSYSTEMECRYPTOGRAPHIQUEDOITREPOSERSURLACLEFETPASSURLESYSTEMELUIMEME
	//BPSRAUNOHCWCBGITMPJQFMEXCXYCIAGPSXCPSXWWEROQCOPITRAEBENTGAYCPMSXPQNYWCDQEVJMAVIDQRZEQESBPCEWCXCYCVYGYCWI 'MYKEY'
	//string input = "KDVTDFOEJJLNRHRTBNLDETKWPSSJRQTDQJAMUKHSXQEBYIXCVDRZYCVDVPMFIMKRUMUSXYXQYRXVFCXBIGMRHDOVRGSOYSEGCJRFPJECKV";
	string input = "IEFOMNTUOHENWFWSJBSFFTPGSNMHZSBBIZAOMOSIUXYCQAELRWSKLQZEKJVWSIVIJMHUVASMVWJEWLZGUBZLAVCLHGMUHWHAKOOKAKKGMRELGEEFVWJELKSEDTYHSGGHBAMIYWEELJCEMXSOHLNZUJAGKSHAKAWWDXZCMVKHUWSWLQWTMLSHOJBSGUELGSUMLIJSMLBSIXUHSDBYSDAOLFATXZOFSTSZWRYHWJENUHGUKWZMSHBAGIGZZGNZHZSBTZHALELOSMLASJDTTQZESWWWRKLFGUZL";
	
	string nomFichier = "ciphertext_";
	
	
	int choix = 0;
	while (true) {
	   cout<<"Quel texte voulez-vous decrypter? Choix possible de 1 a 4"<<endl;
	   getline(cin, input);

	   stringstream myStream(input);
	   if (myStream >> choix){
		   if( choix > 0 && choix < 5) break;
	   }
	   cout << "Chiffre invalide, choisissez un chiffre entre 1 et 4" << endl;
	 }
	 
	 char c = choix +'0';
	 nomFichier += c;
	 nomFichier += ".txt";
	 
	 ifstream fichier(nomFichier, ios::in);  // we open the file in reading mode
 
	 if(fichier)  // if the openning went well
	 {
			getline(fichier, input);  // we get the ciphertext of the file

			fichier.close();		//and we close the file
	 }
	 else{
		 cerr << "Impossible d'ouvrir le fichier " + nomFichier + "!" << endl;	//if it was impossible to open the file, we warn the user
		 exit(1);																//and the program ends
	 }
	 
	input = getTextToUpcase(input);		//we convert the input text to uppercase
 
	cout << "Texte à decrypter: " << endl;
	cout << input << endl;
	cout << endl;
	

	array<double, 26> english = {
		0.08167, 0.01492, 0.02782, 0.04253, 0.12702, 0.02228,
		0.02015, 0.06094, 0.06966, 0.00153, 0.00772, 0.04025,
		0.02406, 0.06749, 0.07507, 0.01929, 0.00095, 0.05987,
		0.06327, 0.09056, 0.02758, 0.00978, 0.02360, 0.00150,
		0.01974, 0.00074 };

	array<double, 26> french = {
		0.0811,  0.0081,  0.0338,  0.0428,  0.1769,  0.0113,
		0.0119,  0.0074,  0.0724,  0.0018,  0.0002,  0.0599,
		0.0229,  0.0768,  0.0520,  0.0292,  0.0083,  0.0643,
		0.0887,  0.0744,  0.0523,  0.0128,  0.0006,  0.0053,
		0.0026,  0.0012 };

	cout << "Decryptage pour un texte anglais:" << endl;
	VigenereCryptanalysis vc_en(english);
	pair<string, string> output_en = vc_en.analyze(input);

	cout << "Key: " << output_en.second << endl;
	cout << "Text: " << output_en.first << endl << endl;

	cout << "Decryptage pour un texte francais:" << endl;
	VigenereCryptanalysis vc_fr(french);
	pair<string, string> output_fr = vc_fr.analyze(input);

	cout << "Key: " << output_fr.second << endl;
	cout << "Text: " << output_fr.first << endl;

}
