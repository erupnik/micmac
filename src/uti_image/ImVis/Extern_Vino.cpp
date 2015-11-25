/*Header-MicMac-eLiSe-25/06/2007

    MicMac : Multi Image Correspondances par Methodes Automatiques de Correlation
    eLiSe  : ELements of an Image Software Environnement

    www.micmac.ign.fr

   
    Copyright : Institut Geographique National
    Author : Marc Pierrot Deseilligny
    Contributors : Gregoire Maillet, Didier Boldo.

[1] M. Pierrot-Deseilligny, N. Paparoditis.
    "A multiresolution and optimization-based image matching approach:
    An application to surface reconstruction from SPOT5-HRS stereo imagery."
    In IAPRS vol XXXVI-1/W41 in ISPRS Workshop On Topographic Mapping From Space
    (With Special Emphasis on Small Satellites), Ankara, Turquie, 02-2006.

[2] M. Pierrot-Deseilligny, "MicMac, un lociel de mise en correspondance
    d'images, adapte au contexte geograhique" to appears in 
    Bulletin d'information de l'Institut Geographique National, 2007.

Francais :

   MicMac est un logiciel de mise en correspondance d'image adapte 
   au contexte de recherche en information geographique. Il s'appuie sur
   la bibliotheque de manipulation d'image eLiSe. Il est distibue sous la
   licences Cecill-B.  Voir en bas de fichier et  http://www.cecill.info.


English :

    MicMac is an open source software specialized in image matching
    for research in geographic information. MicMac is built on the
    eLiSe image library. MicMac is governed by the  "Cecill-B licence".
    See below and http://www.cecill.info.

Header-MicMac-eLiSe-25/06/2007*/

#include "Vino.h"

#if (ELISE_X11)

/*******************************************************************/
/*                                                                 */
/*    cPopUpMenuMessage                                            */
/*                                                                 */
/*******************************************************************/

void CorrectNonEmpty(int &aV0,int & aV1, const int & aVMax)
{
    if (aV0>aV1) ElSwap(aV0,aV1);

    if (aV0==aV1)
    {
         if (aV1<aVMax) 
            aV1++;
         else 
            aV0--;
    }
}

void CorrectRect(Pt2di &  aP0,Pt2di &  aP1,const Pt2di & aSz)
{
    aP0 = Inf(aSz,Sup(aP0,Pt2di(0,0)));
    aP1 = Inf(aSz,Sup(aP1,Pt2di(0,0)));

    CorrectNonEmpty(aP0.x,aP1.x,aSz.x);
    CorrectNonEmpty(aP0.y,aP1.y,aSz.y);
}


/*******************************************************************/
/*                                                                 */
/*    cPopUpMenuMessage                                            */
/*                                                                 */
/*******************************************************************/


cPopUpMenuMessage::cPopUpMenuMessage(Video_Win aW,Pt2di aSz) :
   PopUpMenuTransp(aW,aSz)
{
}

void cPopUpMenuMessage::ShowMessage(const std::string & aName, Pt2di aP,Pt3di aCoul)
{
     UpP0(aP);
     Pt2di aLarg = mW.SizeFixedString(aName);
     mW.fixed_string
     (
           Pt2dr(aP+ (mSz+Pt2di(-aLarg.x, aLarg.y))/2)  ,
           aName.c_str(), mW.prgb()(aCoul.x,aCoul.y,aCoul.z),
           true
     );
}

void cPopUpMenuMessage::Hide()
{
    Pop();
}

/*******************************************************************/
/*                                                                 */
/*    cStatImageRehauss                                            */
/*                                                                 */
/*******************************************************************/


void FillStat(cXml_StatVino & aStat,Flux_Pts aFlux,Fonc_Num aFonc)
{
   int aNbCh = aFonc.dimf_out();
   aStat.Soms().resize(aNbCh,0.0);
   aStat.Soms2().resize(aNbCh,0.0);
   aStat.ECT().resize(aNbCh,0.0);
   aStat.VLow().resize(aNbCh,0.0);
   aStat.VHigh().resize(aNbCh,0.0);
   Symb_FNum aSF(aFonc);

   ELISE_COPY
   (
        aFlux,
        Virgule(1.0,aSF,Square(aSF)),
        Virgule
        (
            sigma(aStat.Nb()),
            sigma(VData(aStat.Soms()),aNbCh)  | VMin(VData(aStat.VLow()),aNbCh) | VMax(VData(aStat.VHigh()),aNbCh),
            sigma(VData(aStat.Soms2()),aNbCh)
        )
   );

   double aNb = aStat.Nb();

   for (int aK=0 ; aK<aNbCh ; aK++)
   {
         aStat.Soms()[aK] /= aNb;
         aStat.Soms2()[aK] /= aNb;
         aStat.ECT()[aK] = sqrt(ElMax(0.0,aStat.Soms2()[aK]-ElSquare(aStat.Soms()[aK])));
   }
}

/****************************************/
/*                                      */
/*          STRING                      */
/*                                      */
/****************************************/

std::string StrNbChifSignNotSimple(double aVal,int aNbCh)
{
   if (aVal==1) return "1";
   if (aVal < 1)
   {
        if (aVal>0.1) return  ToString(aVal).substr(0,aNbCh+2);
        if (aVal>0.01) return  ToString(aVal).substr(0,aNbCh+3);

        double aLog10 = log(aVal) / log(10);
        int aLogDown =  round_down(ElAbs(aLog10));
        aVal = ElMin(1.0,aVal * pow(10,aLogDown));

        return ToString(aVal).substr(0,aNbCh+2) + "E-" + ToString(aLogDown);


   }

   if (aVal<100)
   {
       std::string aRes = ToString(aVal).substr(0,aNbCh+1);
       return aRes;
   }

   double aLog10 = log(aVal) / log(10);
   int aLogDown =  round_down(ElAbs(aLog10));
   aVal = ElMin(10.0,aVal / pow(10,aLogDown));
   return ToString(aVal).substr(0,aNbCh+2) +  "E" + ToString(aLogDown);
}

std::string StrNbChifSign(double aVal,int aNbCh)
{
    return SimplString(StrNbChifSignNotSimple(aVal,aNbCh));
}

std::string SimplString(std::string aStr)
{
   if (aStr.find('.') == std::string::npos)
      return aStr;
   int aK= aStr.size()-1;
   while ((aK>0) && (aStr[aK]=='0'))
     aK--;
   if (aStr[aK]=='.')
     aK--;
   aK++;
   return aStr.substr(0,aK);
}



#endif





/*Footer-MicMac-eLiSe-25/06/2007

Ce logiciel est un programme informatique servant à la mise en
correspondances d'images pour la reconstruction du relief.

Ce logiciel est régi par la licence CeCILL-B soumise au droit français et
respectant les principes de diffusion des logiciels libres. Vous pouvez
utiliser, modifier et/ou redistribuer ce programme sous les conditions
de la licence CeCILL-B telle que diffusée par le CEA, le CNRS et l'INRIA 
sur le site "http://www.cecill.info".

En contrepartie de l'accessibilité au code source et des droits de copie,
de modification et de redistribution accordés par cette licence, il n'est
offert aux utilisateurs qu'une garantie limitée.  Pour les mêmes raisons,
seule une responsabilité restreinte pèse sur l'auteur du programme,  le
titulaire des droits patrimoniaux et les concédants successifs.

A cet égard  l'attention de l'utilisateur est attirée sur les risques
associés au chargement,  à l'utilisation,  à la modification et/ou au
développement et à la reproduction du logiciel par l'utilisateur étant 
donné sa spécificité de logiciel libre, qui peut le rendre complexe à 
manipuler et qui le réserve donc à des développeurs et des professionnels
avertis possédant  des  connaissances  informatiques approfondies.  Les
utilisateurs sont donc invités à charger  et  tester  l'adéquation  du
logiciel à leurs besoins dans des conditions permettant d'assurer la
sécurité de leurs systèmes et ou de leurs données et, plus généralement, 
à l'utiliser et l'exploiter dans les mêmes conditions de sécurité. 

Le fait que vous puissiez accéder à cet en-tête signifie que vous avez 
pris connaissance de la licence CeCILL-B, et que vous en avez accepté les
termes.
Footer-MicMac-eLiSe-25/06/2007*/
