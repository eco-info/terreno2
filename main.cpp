// main.cpp
// g++ main.cpp cena.cpp OVNI.cpp HUD.cpp -losg -losgDB -losgGA -losgText -losgUtil -losgViewer -o main

#include "cena.h"
#include "OVNI.h"
#include "HUD.h"

float lonMin=-74, lonMax=-34.8, latMin=-33.8666667, latMax=5.3333333; // longitude/latitude mínima/máxima
float lonC = (lonMin+lonMax)/2; // longitude central -54.4
float latC = (latMin+latMax)/2; // latitude central -14.2666667
float rCeu = (lonMax-lonMin)*2; // "raio do céu" 78.4 (para calcular a posição inicial da câmera e a posição do "Sol")

int main( int argc, char** argv )
{
	osg::ref_ptr<Cena> cena = new Cena;
	osg::ref_ptr<OVNIManipulator> OVNI = new OVNIManipulator(&cena->viewer);
	cena->setupViewer(OVNI);
	OVNI->movePara(osg::Vec3(lonC,latC,rCeu));
	cena->defineJanela();
	OVNI->defineLimites(cena->windowX, cena->windowW, cena->windowY, cena->windowH);
	osg::ref_ptr<HUDCamera> HUD = new HUDCamera(cena->windowX, cena->windowW, cena->windowY, cena->windowH); // cria um HUD do tamanho da janela
	cena->addChild( HUD );

	while ( !cena->viewer.done() )
	{
		OVNI->calcAcc( &cena->viewer );
		cena->viewer.frame();
	}

	return 0;
}
