#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <array>

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
		string result = "I CAN NOT DECRYPT THIS TEXT FOR NOW :-)" + input;

		return make_pair(result, key);
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

		return index+1;
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

};

int main()
{
	//KDVTDFOEJJLNRHRTBNLDETKWPSSJRQTDQJAMUKHSXQEBYIXCVDRZYCVDVPMFIMKRUMUSXYXQYRXVFCXBIGMRHDOVRGSOYSEGCJRFPJECKV 'AZERTY'
	//PRINCIPEDEKERCKHOFFSTOUTELASECURITEDUNSYSTEMECRYPTOGRAPHIQUEDOITREPOSERSURLACLEFETPASSURLESYSTEMELUIMEME
	//BPSRAUNOHCWCBGITMPJQFMEXCXYCIAGPSXCPSXWWEROQCOPITRAEBENTGAYCPMSXPQNYWCDQEVJMAVIDQRZEQESBPCEWCXCYCVYGYCWI 'MYKEY'
	string input = "KDVTDFOEJJLNRHRTBNLDETKWPSSJRQTDQJAMUKHSXQEBYIXCVDRZYCVDVPMFIMKRUMUSXYXQYRXVFCXBIGMRHDOVRGSOYSEGCJRFPJECKV";

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
	cout << "Text: " << output_en.first << endl;

	VigenereCryptanalysis vc_fr(french);
	pair<string, string> output_fr = vc_fr.analyze(input);

	cout << "Key: " << output_fr.second << endl;
	cout << "Text: " << output_fr.first << endl;

	cout << "------------Key Length----------> " << vc_fr.getKeyLength(input) << endl;

}