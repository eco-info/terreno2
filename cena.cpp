// cena.cpp

#include "cena.h"

void Cena::defineJanela()
{
	osgViewer::Viewer::Windows windows;
	viewer.getWindows(windows);
	osgViewer::Viewer::Windows::iterator window;
	window = windows.begin();
	(*window)->useCursor(false);
	(*window)->getWindowRectangle(windowX, windowY, windowW, windowH);
	std::cout << "Tamanho da janela: " << windowW << "x" << windowH << "\n";
}
