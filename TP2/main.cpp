#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <cmath> 

#include "CImg-1.6.6/CImg.h"

#define ROUND( a ) ( ( (a) < 0 ) ? (int) ( (a) - 0.5 ) : (int) ( (a) + 0.5 ) )

using namespace std;
using namespace cimg_library;

double DCTCoefficient(int i)
{
    return i == 0 ? (1.0 / sqrt(2.0)) : 1.0;
}

void computeBlockDCT(vector<double> & cosVect, vector< CImg<double> >& bloc_image,vector< CImg<double> >& bloc_image_dct, size_t index )
{

    vector<double>::const_iterator it = cosVect.begin();

    for(int i = 0; i < 8; ++i ) 
    {
        for(int j = 0; j < 8; ++j)
        {
            double somme2 = 0.0;
            for(int x = 0; x < 8; ++x)
            {
                double somme1 = 0.0;
                for(int y = 0; y < 8; ++y)
                {
                     somme1 += (bloc_image[index](x, y)-128) * (*it);
                     ++it;
                }
                
                somme2 += somme1;
            }
            
            double dct = (2.0 / 8.0) * DCTCoefficient(i) * DCTCoefficient(j) * somme2;
            
            bloc_image_dct[index](i, j) = dct;
        }
    }

}


void computeBlockDCTinv(vector<double> & cosVect, vector< CImg<double> >& bloc_image_comp,vector< CImg<double> >& bloc_image_dct, size_t index )
{

    vector<double>::const_iterator it = cosVect.begin();

    for(int x = 0; x < 8; ++x ) 
    {
        for(int y = 0; y < 8; ++y)
        {
            double somme2 = 0.0;
            for(int i = 0; i < 8; ++i)
            {
                double somme1 = 0.0;
                for(int j = 0; j < 8; ++j)
                {
                     somme1 += DCTCoefficient(i) * DCTCoefficient(j)*(bloc_image_dct[index](i, j)) * (*it);
                     ++it;
                }
                
                somme2 += somme1;
            }
            
            double dctInv = (2.0 / 8.0)  * somme2;
            
            bloc_image_comp[index](x, y) = dctInv;
        }
    }

}

/*
 *Fonction principale d'encodage JPEG
 *Param : CImg<unsigned char>& image Image de base à encoder
 *Param : float quality qualité de l'encodage JPEG
 *Return : CImg<unsigned char> Image compressée
*/

CImg<double> JPEGEncoder(const int type, vector<double> & cosVect, vector<double> & cosVectInv, CImg<unsigned char>& image, float quality)
{
    
    CImg<double> comp(image.width(), image.height(), 1 ,1 ,0);

    comp = image;

    // Decoupage en bloc de l'image
    vector< CImg<double> > bloc_image;
    
    for(int x = 0; x < comp.width(); x += 8)
    {
        for(int y = 0; y < comp.height(); y += 8)
        {
            CImg<double> sub_image = comp.get_crop(x, y, x + 7, y + 7);
            bloc_image.push_back(sub_image);
        }
    }
    
    // Application de la DCT sur l'image decoupee
    vector< CImg<double> > bloc_image_dct(bloc_image);
    
    for( size_t index = 0; index < bloc_image_dct.size(); ++index )
    {
        computeBlockDCT(cosVect, bloc_image, bloc_image_dct, index);
    }

    int i = 0;
    

    if(type == 1)
    {

        // Reconstruction de l'image apres DCT pour l'affichage
        CImg<double> DCTImage(image.width(), image.height(), 1 ,1 ,0);

        for(int x = 0; x < comp.width(); x += 8)
        {
            for(int y = 0; y < comp.height(); y += 8)
            {
                DCTImage.draw_image(x, y, bloc_image_dct[i]);
                ++i;
            }
        }
        
        //return l'image DCT
        return DCTImage;
    }
    else
    {
        // Etape de quantification
        vector< CImg<double> > bloc_image_quantization(bloc_image);

        // Declaration de la Quantization matrix

        CImg<> Q(8,8);
        Q(0,0)=16;   Q(0,1)=11;   Q(0,2)=10;   Q(0,3)=16;   Q(0,4)=24;   Q(0,5)=40;   Q(0,6)=51;   Q(0,7)=61;
        Q(1,0)=12;   Q(1,1)=12;   Q(1,2)=14;   Q(1,3)=19;   Q(1,4)=26;   Q(1,5)=58;   Q(1,6)=60;   Q(1,7)=55;
        Q(2,0)=14;   Q(2,1)=13;   Q(2,2)=16;   Q(2,3)=24;   Q(2,4)=40;   Q(2,5)=57;   Q(2,6)=69;   Q(2,7)=56;
        Q(3,0)=14;   Q(3,1)=17;   Q(3,2)=22;   Q(3,3)=29;   Q(3,4)=51;   Q(3,5)=87;   Q(3,6)=80;   Q(3,7)=62;
        Q(4,0)=18;   Q(4,1)=22;   Q(4,2)=37;   Q(4,3)=56;   Q(4,4)=68;   Q(4,5)=109;  Q(4,6)=103;  Q(4,7)=77;
        Q(5,0)=24;   Q(5,1)=35;   Q(5,2)=55;   Q(5,3)=64;   Q(5,4)=81;   Q(5,5)=104;  Q(5,6)=113;  Q(5,7)=92;
        Q(6,0)=49;   Q(6,1)=64;   Q(6,2)=78;   Q(6,3)=87;   Q(6,4)=103;  Q(6,5)=121;  Q(6,6)=120;  Q(6,7)=101;
        Q(7,0)=72;   Q(7,1)=92;   Q(7,2)=95;   Q(7,3)=98;   Q(7,4)=112;  Q(7,5)=100;  Q(7,6)=103;  Q(7,7)=99;
        Q *= quality;

        for( size_t index = 0; index < bloc_image_dct.size(); ++index )
        {
            bloc_image_quantization[index] = bloc_image_dct[index].get_div(Q);
            for(int x = 0; x<8; ++x)
            {
                for(int y = 0; y<8; ++y)
                {
                    bloc_image_quantization[index](x,y) = ROUND(bloc_image_quantization[index](x,y));
                }
            }
        }
        
        /* Reconstruction de l'image après encodage
        CImg<double> quantImage(image.width(), image.height(), 1 ,1 ,0);
        index = 0;
        
        for(int x = 0; x < comp.width(); x += 8)
        {
            for(int y = 0; y < comp.height(); y += 8)
            {
                quantImage.draw_image(x, y, bloc_image_quantization[index]);
                ++index;
            }
        }
        
        // Affichage de l'image apres encodage
        CImgDisplay quant_disp(quantImage,"quant DCT Image");*/

        // Quantification inverse
        vector< CImg<double> > bloc_image_invQuant(bloc_image);
        for( size_t index = 0; index < bloc_image_quantization.size(); ++index )
        {
            bloc_image_invQuant[index] = bloc_image_quantization[index].get_mul(Q);
            for(int x = 0; x<8; ++x)
            {
                for(int y = 0; y<8; ++y)
                {
                    bloc_image_invQuant[index](x,y) = ROUND(bloc_image_invQuant[index](x,y));
                }
            }
        }

        /*CImg<double> quantInvImage(image.width(), image.height(), 1 ,1 ,0);
        index = 0;
        
        for(int x = 0; x < comp.width(); x += 8)
        {
            for(int y = 0; y < comp.height(); y += 8)
            {
                quantInvImage.draw_image(x, y, bloc_image_invQuant[index]);
                ++index;
            }
        }

        // Affichage de l'image apres quantification inverse
        CImgDisplay quantInv_disp(quantInvImage,"quant inv Image");*/
        

        // Etape de DCT inverse
        
        vector< CImg<double> > bloc_image_comp(bloc_image);
        
        for( size_t index = 0; index < bloc_image_invQuant.size(); ++index )
        {
            computeBlockDCTinv(cosVectInv, bloc_image_comp,  bloc_image_invQuant, index);
        }
        
        i = 0;

        //Reconstruction de l'image après decodage
        for(int x = 0; x < comp.width(); x += 8)
        {
            for(int y = 0; y < comp.height(); y += 8)
            {
                comp.draw_image(x, y, bloc_image_comp[i]);
                ++i;
            }
        }

        /* Affichage de l'image decodee
        CImgDisplay main_disp(comp,"comp Image");

        while (!main_disp.is_closed());
        {
            main_disp.wait();
        }*/
        
        return comp;
    }

}

void computCosBlock( vector<double> & cosVect, vector<double> & cosVectInv )
{
    //creation vecteur des cos       
    for(int i = 0; i < 8; ++i ) 
    {
        for(int j = 0; j < 8; ++j)
        {
            for(int x = 0; x < 8; ++x)
            {
                for(int y = 0; y < 8; ++y)
                {
                    cosVect.push_back(cos( ((2 * x + 1) * i * M_PI) / 16.0 ) * cos( ((2 * y + 1) * j * M_PI) / 16.0 ));
                }
                
            }
        }
    }

    //creation vecteur des cos inverse
    for(int x = 0; x < 8; ++x ) 
    {
        for(int y = 0; y < 8; ++y)
        {
            for(int i = 0; i < 8; ++i)
            {
                for(int j = 0; j < 8; ++j)
                {
                    cosVectInv.push_back(cos( ((2 * x + 1) * i * M_PI) / 16.0 ) * cos( ((2 * y + 1) * j * M_PI) / 16.0 ));
                }
            }
        }
    }
}


int main(int argc, char * argv[])
{

    if(argc == 3)
    {
        CImg<double> res_image;

        // Read the image "lena.bmp"
        CImg<unsigned char> my_image("lena.bmp");

        vector<double> cosVect;
        vector<double> cosVectInv;

        computCosBlock( cosVect, cosVectInv );

        // Take the luminance information 
        my_image.channel(0);

        if(!strcmp (argv[1], "dct"))
        {
            cout << "Appliquer la DCT et afficher le resultat..." << endl;

            res_image = JPEGEncoder(1 , cosVect, cosVectInv, my_image,atof(argv[2]));
        }
        else
        {
            cout << "Appliquer la DCT puis la DCT inverse et afficher l'image decompressee..." << endl;

            res_image = JPEGEncoder(2, cosVect, cosVectInv, my_image,atof(argv[2]));
        }


        // Display the bmp file
        CImgDisplay main_disp(my_image,"Initial Image");

        // Display the compressed file (by dct)

        CImgDisplay res_disp(res_image,"Compressed Image");

        while (!main_disp.is_closed());
        {
            main_disp.wait();
        }
        
    }
    else
    {
        cout << "Usage : tp2 [OPTION] [QUALITE]" << endl << endl;
        cout << "Appliquer la DCT et afficher le resultat" << endl;
        cout << "[OPTION] : dct" << endl << endl;
        cout << "Appliquer la DCT puis la DCT inverse et afficher l'image decompressee" << endl;
        cout << "[OPTION] : comp" << endl;
    }

    return 0;

}
