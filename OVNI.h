// OVNI.h

#ifndef OVNI_H
#define OVNI_H

#include <iostream>
#include <osgGA/FirstPersonManipulator>
#include <osgViewer/Viewer>

class OVNIManipulator : public osgGA::FirstPersonManipulator
{
public:
	OVNIManipulator(osgViewer::Viewer *inputViewer) : _viewer(inputViewer)
	{
		// Start frame timer
		mainTimer.setStartTick();
		setAllowThrow(false); // não continua movendo ao soltar o botão do mouse
	}
	void movePara(osg::Vec3);
	void defineLimites(int left, int width, int top, int height);
	// Override handle function for keyboard input
	virtual bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &);
	virtual float pickDown( osg::Vec3d pos );
	virtual bool performMovement();
	virtual bool handleMouseMove( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us );
	void calcAcc(); // calcula aceleração do OVNI
protected:
	bool keyW=false,
		keyA=false,
		keyS=false,
		keyD=false,
		keyR=false,
		keyF=false;
	osg::Vec3d camSpeed;
	bool Warping=false,
		mouseFree=false;
	float velX=0,
		velY=0,
		velZ=0;	// XYZ: velocidades lateral, vertical e frontal
	const float acc = 0.1;
	const float maxVel = 1;
	const float inputTimeInterval = 0.02;
	double maxTick = inputTimeInterval;
	osgViewer::Viewer *_viewer;
	osg::Timer mainTimer;
	int _width, _height;
	osg::Vec3d fromQuat(const osg::Quat&, bool);
};

#endif
