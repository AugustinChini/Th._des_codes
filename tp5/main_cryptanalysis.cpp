#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <array>
#include <math.h>

using namespace std;

typedef array<pair<char, double>, 26> FreqArray;

class VigenereCryptanalysis
{
private:
	array<double, 26> targets;
	array<double, 26> sortedTargets;

	// TO COMPLETE

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
		
		unsigned int  keyLength = getKeyLength(result);
		cout << "------------Key Length----------> " << keyLength << endl;
		
		key = getKey(result, keyLength);
		
		result = decrypt(result, key);

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

		// we try to find a key between 2 and 16 char
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
					//cout << "String " << i << " " << tmp << endl;
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
			if(0.06 <= v[i])
			{
				index = i;
				find = true;
			}

		}
		
		if(find)
			return index+1;
		else {
			unsigned int maxValIndex = (max_element(v.begin(), v.end()))-v.begin();
			return maxValIndex+1;
		}
	}

	double getIC(string s)
	{

		int size = s.size();

		double res = 0.0;

		for (char c = 'A'; c <= 'Z'; ++c)
		{
			int current_counter = counter(s, c);

			//cout << c << ":" <<current_counter<< endl;

			res += (current_counter*(current_counter - 1)) / (double)(size*(size - 1));

			//cout << "res : " <<  (current_counter*(current_counter-1))/(double)(size*(size-1)) << endl;
		}

		//cout << res << endl;

		return res;

	}

	unsigned int counter(const string& s, char c)
	{
		unsigned int cpt = 0;

		for (unsigned int i = 0; i<s.size(); ++i)
		{
			if (s[i] == c)
				++cpt;
		}

		return cpt;
	}
	
	
	string getKey(string s, int keyLength)
	{

		int size = s.size();
		string key = "";
		char b;
		int a;
		
		for(int i = 0; i < keyLength; i++){
			
			string sequence = "";
			
			for (int j = i; j < size; j+=keyLength){
				
				sequence+= s.at(j);
				
			}
			//cout << "------------sequence----------> " << sequence << endl;
			a = chiSquarred(sequence); //recuperer la lettre correspondant à cet index, chaque lettre correspond à une lettre de la cle
			//cout << "------------index----------> " << a << endl;
			b = (char)a;
			b += 'A';
			//b = (char)('A' - 1 + a);
			key += b;
		}
		
		cout << "------------Key----------> " << key << endl;
		return key;

	}
	
	unsigned int chiSquarred(const string& s)
	{
		double chiMin = 0;
		unsigned int index = 0;
		
		int size = s.size();
		//cout << "------------string s----------> " << s << endl;
		
		string tabString[26];
		tabString[0] = s;
		string newString = "";
		//cout << "------------sequence ----------> " << s << endl;
		for(int h = 0; h < 26; h++){
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
			//cout << "------------string Chi-squarred----------> " << tabString[h] << endl;
		}
		
		
		for (int i = 0; i < 26; i++){
			
			double chi = 0;
		
			for (char c = 'A'; c <= 'Z'; ++c)
			{
				int current_counter = counter(tabString[i], c);
				//cout << "------------current_counter----------> " << current_counter << endl;
				
				int posLetter = c - 'A' + 1;
				//cout << "------------posLetter----------> " << posLetter << endl;
				double numberExpected = size * targets[posLetter-1];
				
				//cout << "------------c----------> " << c << endl;
				/*cout << "------------posLetter-1----------> " << posLetter-1 << endl;
				cout << "------------sortedTargets[posLetter-1]----------> " << targets[posLetter-1] << endl;
				cout << "------------numberExpected----------> " << numberExpected << endl;
				cout << "------------chi----------> " << chi << endl;*/
				chi += (pow((current_counter - numberExpected), 2))/numberExpected;
				
			}
			//cout << "------------chi----------> " << chi << endl;
			
			if(i == 0) {
				chiMin = chi;
			}
				
			if(chi < chiMin) {
				chiMin = chi;
				index = i;
			}
			newString = "";
			//cout << "------------chiMin----------> " << chiMin << endl;
		}
		//cout << "------------index----------> " << index << endl;
		//cout << "------------chiMin----------> " << chiMin << endl;

		return index;
	}
	
	

};

int main()
{
	//KDVTDFOEJJLNRHRTBNLDETKWPSSJRQTDQJAMUKHSXQEBYIXCVDRZYCVDVPMFIMKRUMUSXYXQYRXVFCXBIGMRHDOVRGSOYSEGCJRFPJECKV 'AZERTY'
	//PRINCIPEDEKERCKHOFFSTOUTELASECURITEDUNSYSTEMECRYPTOGRAPHIQUEDOITREPOSERSURLACLEFETPASSURLESYSTEMELUIMEME
	//BPSRAUNOHCWCBGITMPJQFMEXCXYCIAGPSXCPSXWWEROQCOPITRAEBENTGAYCPMSXPQNYWCDQEVJMAVIDQRZEQESBPCEWCXCYCVYGYCWI 'MYKEY'
	//string input = "KDVTDFOEJJLNRHRTBNLDETKWPSSJRQTDQJAMUKHSXQEBYIXCVDRZYCVDVPMFIMKRUMUSXYXQYRXVFCXBIGMRHDOVRGSOYSEGCJRFPJECKV";
	string input = "IEFOMNTUOHENWFWSJBSFFTPGSNMHZSBBIZAOMOSIUXYCQAELRWSKLQZEKJVWSIVIJMHUVASMVWJEWLZGUBZLAVCLHGMUHWHAKOOKAKKGMRELGEEFVWJELKSEDTYHSGGHBAMIYWEELJCEMXSOHLNZUJAGKSHAKAWWDXZCMVKHUWSWLQWTMLSHOJBSGUELGSUMLIJSMLBSIXUHSDBYSDAOLFATXZOFSTSZWRYHWJENUHGUKWZMSHBAGIGZZGNZHZSBTZHALELOSMLASJDTTQZESWWWRKLFGUZL";
	//Vigenere cipher(input);

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

	VigenereCryptanalysis vc_en(english);
	pair<string, string> output_en = vc_en.analyze(input);

	cout << "Key: " << output_en.second << endl;
	cout << "Text: " << output_en.first << endl << endl;

	VigenereCryptanalysis vc_fr(french);
	pair<string, string> output_fr = vc_fr.analyze(input);

	cout << "Key: " << output_fr.second << endl;
	cout << "Text: " << output_fr.first << endl;

	//unsigned int  keyLength = vc_fr.getKeyLength(input);
	//cout << "------------Key Length----------> " << keyLength << endl;
	
	/*vc_fr.getKey(input, keyLength);
	vc_en.getKey(input, keyLength);*/

}
