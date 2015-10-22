#include <vector>
#include <queue>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cmath>


using namespace std;


class Symbol
{
  public:
  string name;
  double freq;
  string code;
  bool leaf;
  Symbol *left, *right;

  Symbol(): name(""),freq(0),code(""),leaf(true), left(NULL), right(NULL){}

  Symbol(string newName, double newFreq, string newCode="", bool newLeaf=true, Symbol* newLeft=NULL, Symbol* newRight=NULL):name(newName), freq(newFreq),code(newCode), leaf(newLeaf), left(newLeft), right(newRight){}

  // To Do: Code Here
  // Complete the class
};


class compareSymbol
{
  public:
  bool operator()(const Symbol * a, const Symbol * b)
  {
    return a->freq > b->freq;
  }
};


void updateBinaryTree ( Symbol * sRoot, string prefix )
{
  sRoot->code = prefix;
  if(!sRoot->leaf)
  {
    updateBinaryTree(sRoot->left, prefix+"0");
    updateBinaryTree(sRoot->right, prefix+"1");
  }
}


Symbol * find(vector<Symbol*>& alphabet, string lettre)
{
  bool find = false;
  vector<Symbol*>::iterator it;
  for(it = alphabet.begin(); it != alphabet.end(); ++it)
  {
    if((*it)->name == lettre)
    {
      find = true;
      break;
    }
  }
  return find ? *it : NULL;
}


Symbol* CreateHuffmanCode(priority_queue<Symbol *,vector<Symbol *>, compareSymbol > & pq)
{
  while (pq.size() > 1)
  {
    Symbol * s1 = pq.top();
    pq.pop();
    Symbol * s2 = pq.top();
    pq.pop();
    Symbol * sFather = new Symbol(s2->name + s1->name, s2->freq + s1->freq, "", false, s2, s1);
    pq.push(sFather);
    s1->code += "1";
    s2->code += "0";
  }
  updateBinaryTree(pq.top(), "");
  return pq.top();
}


bool CreateAlphabet(vector<Symbol*>& alphabet, string file)
{
  if(file == "none")
  {
    // Probability of french letters
    alphabet.push_back(new Symbol("a",8.11));
    alphabet.push_back(new Symbol("b",0.81));
    alphabet.push_back(new Symbol("c",3.38));
    alphabet.push_back(new Symbol("d",4.28));
    alphabet.push_back(new Symbol("e",17.69));
    alphabet.push_back(new Symbol("f",1.13));
    alphabet.push_back(new Symbol("g",1.19));
    alphabet.push_back(new Symbol("h",0.74));
    alphabet.push_back(new Symbol("i",7.24));
    alphabet.push_back(new Symbol("j",0.18));
    alphabet.push_back(new Symbol("k",0.02));
    alphabet.push_back(new Symbol("l",5.99));
    alphabet.push_back(new Symbol("m",2.29));
    alphabet.push_back(new Symbol("n",7.68));
    alphabet.push_back(new Symbol("o",5.2));
    alphabet.push_back(new Symbol("p",2.92));
    alphabet.push_back(new Symbol("q",0.83));
    alphabet.push_back(new Symbol("r",6.43));
    alphabet.push_back(new Symbol("s",8.87));
    alphabet.push_back(new Symbol("t",7.44));
    alphabet.push_back(new Symbol("u",5.23));
    alphabet.push_back(new Symbol("v",1.28));
    alphabet.push_back(new Symbol("w",0.06));
    alphabet.push_back(new Symbol("x",0.53));
    alphabet.push_back(new Symbol("y",0.26));
    alphabet.push_back(new Symbol("z",0.12));
    double result = 0.0;
    for(size_t i = 0; i<alphabet.size(); ++i)
    {
      result += (alphabet[i]->freq * log2(alphabet[i]->freq))/100;
    }
    cout << "--------------------------------"<< endl;
    cout << "Antropie - S(X) = " << result << endl;
    cout << "--------------------------------"<< endl;

    return true;
  }
  else
  {
    unsigned int charCpt = 0;
    //ouverture du fichier
    ifstream fichier (file.c_str(), ios::in);
    //si ouverture possible
    if(fichier)
    {
      string lettre;
      char tmpChar;
      while(fichier.get(tmpChar))
      {
        lettre = "";
        lettre += tmpChar;
        //recherche de la lettre courante dans le vecteur, si on la trouve on renvoit
        //le Symbol * qui la contient sinon on revoit un pointeur null
        Symbol * currentSymbol = find(alphabet, lettre);
        //si le symbole n'a pas déj�  été trouvé dans le vector
        if(!currentSymbol)
        {
          alphabet.push_back(new Symbol(lettre, 1.0));
        }
        else
        {
          //incrémentation de l'argument comptant le nombre d'apparition de la lettre
          currentSymbol->freq += 1;
        }
        ++charCpt;
      }
      //convertion des occurences d'une lettre en pourcentage d'apparition
      for(size_t i = 0; i < alphabet.size(); ++i)
      {
        alphabet[i]->freq = ((alphabet[i]->freq)/charCpt)*100;
      }
      fichier.close();
      double result = 0.0;
      for(size_t i = 0; i<alphabet.size(); ++i)
      {
        result += (alphabet[i]->freq * log2(alphabet[i]->freq))/100;
      }
      cout << "--------------------------------"<< endl;
      cout << "Antropie - S(X) = " << result << endl;
      cout << "--------------------------------"<< endl;

      return true;
    }
    else
    {
      cout << "Impossible d'ouvrir le fichier" << endl;
      return false;
    }
  }
}


void DeleteHuffmanTree(Symbol * currentNode)
{
  if( currentNode->left != NULL )
  {
    DeleteHuffmanTree(currentNode->left);
  }
  if( currentNode->right != NULL )
  {
    DeleteHuffmanTree(currentNode->right);
  }
  delete currentNode;
}


void DeleteMemory(vector<Symbol*>& alphabet, Symbol* root)
{
  alphabet.erase(alphabet.begin(), alphabet.end());
  DeleteHuffmanTree(root);
}


using namespace std;
int main(int argc, char * argv[])
{
  vector<Symbol*> alphabet;
  if(argc >= 2)
  {
    if(!strcmp (argv[1], "alphabet"))
    {
      // Compute the frequencies of the symbol
      if(CreateAlphabet(alphabet, "none"))
      {
        //initialisation et remplissage de la priority_queue pour la creation du code Huffman
        priority_queue<Symbol *,vector<Symbol *>, compareSymbol > pq;
        for(unsigned int i = 0; i < alphabet.size();++i)
        {
          pq.push(alphabet[i]);
        }
        // Build the Huffman code tree
        Symbol* root = CreateHuffmanCode(pq);
        // Display the result
        for(size_t i=0;i<alphabet.size();++i)
        {
          cout << "Name : " << alphabet[i]->name << " ; Code : " << alphabet[i]->code << endl;
        }
        // Clear the memory
        DeleteMemory(alphabet,root);
      }
    }
    else
    {
      if(!strcmp (argv[1], "file") && argc == 3)
      {
        
        if(CreateAlphabet(alphabet, argv[2]))
        {
          //initialisation et remplissage de la priority_queue pour la creation du code Huffman
          priority_queue<Symbol *,vector<Symbol *>, compareSymbol > pq;
          for(unsigned int i = 0; i < alphabet.size();++i)
          {
            pq.push(alphabet[i]);
          }
          // Build the Huffman code tree
          Symbol* root = CreateHuffmanCode(pq);
          // Display the result
          for(size_t i=0;i<alphabet.size();++i)
          {
            cout << "Name : " << alphabet[i]->name << " ; Code : " << alphabet[i]->code << endl;
          }
          // Clear the memory
          DeleteMemory(alphabet,root);
        }
      }
      else
      {
        cout << "Les arguments que vous avez rentrez sont incorrects (entrez le nom du fichier)." << endl;
        return -1;
      }
    }
  }
  else
  {
    cout << "Usage : tp1 [OPTION] [FILE]" << endl << endl;
    cout << "[OPTION] : alphabet" << endl;
    cout << "[OPTION] : file" << endl;
    cout << "[FILE] : file of text to extract redundancy of letters (if [OPTION] = 'file)'" << endl;
  }
  return 0;
}
