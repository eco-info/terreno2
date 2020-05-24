// cena.h

#ifndef cena_H
#define cena_H

#include <osgDB/ReadFile>
#include <osgViewer/Viewer>

class OVNIManipulator;

class Cena : public osg::Group // o que esse public faz ??
{
public:
	Cena()
	{
		setName("grpCena");
		nodeBR = osgDB::readNodeFile("map/BR.osgb");
		nodeBR->setName("nodeBR");
		addChild( nodeBR );
		viewer.setSceneData( this ); // "this->get()" ??
	}
	void setupViewer(OVNIManipulator*);
	void defineJanela();
	osgViewer::Viewer viewer;
	int windowX, windowY, windowW, windowH;
protected:
	osg::ref_ptr<osg::Node> nodeBR;
};

#include "OVNI.h"

inline void Cena::setupViewer(OVNIManipulator* ovni) // tem que ser inline senão "source [ovni] is a pointer to incomplete type"
{
	viewer.setCameraManipulator(dynamic_cast<osgGA::CameraManipulator*>(ovni));
	viewer.setUpViewInWindow(0, 0, 800, 600); // deprecated ?? osgViewer::View::apply( ViewConfig* config )
	viewer.realize();
}

#endif
