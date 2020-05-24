// OVNI.cpp

#include "OVNI.h"

void OVNIManipulator::movePara(osg::Vec3 pos)
{
	osg::Quat quad0;
	setTransformation(pos, quad0 );
}

void OVNIManipulator::defineLimites(int left, int width, int top, int height)
{
	_width = width;
	_height = height;
}

bool OVNIManipulator::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
	if( ea.getEventType() == osgGA::GUIEventAdapter::PUSH )
		return handleMousePush( ea, aa );
	else
	if( ea.getEventType() == osgGA::GUIEventAdapter::RELEASE )
		return handleMouseRelease( ea, aa );
	else
	if( ea.getEventType() == osgGA::GUIEventAdapter::MOVE ||
		ea.getEventType() == osgGA::GUIEventAdapter::DRAG)
		return handleMouseMove( ea, aa ); // barra de espaços parou de levar pra coordenada inicial anterior
	if (!_viewer)
		return false;
	// Set the viewer's "eye" position, which is located at the center of the camera.
	osg::Vec3d eyePos;
	osg::Matrix matrix = _viewer->getCameraManipulator()->getMatrix();
	eyePos = matrix.getTrans();
	osg::Quat camRotation = matrix.getRotate();
	// Movement of the camera correlates with W, A, S, D
	if (ea.getEventType() == osgGA::GUIEventAdapter::KEYDOWN)
	{
		//std::cout << ea.getKey() << "\n";
		switch(ea.getKey())
		{
			case 'w':
			case 'W':
				keyW = true;
				break;
			case 'a':
			case 'A':
				keyA = true;
				break;
			case 's':
			case 'S':
				keyS = true;
				break;
			case 'd':
			case 'D':
				keyD = true;
				break;
			case 'r':
			case 'R':
				keyR = true;
				break;
			case 'f':
			case 'F':
				keyF = true;
				break;
		}
	}
	else
	if (ea.getEventType() == osgGA::GUIEventAdapter::KEYUP)
	{
		switch(ea.getKey())
		{
			case 'w':
			case 'W':
				keyW = false;
				break;
			case 'a':
			case 'A':
				keyA = false;
				break;
			case 's':
			case 'S':
				keyS = false;
				break;
			case 'd':
			case 'D':
				keyD = false;
				break;
			case 'r':
			case 'R':
				keyR = false;
				break;
			case 'f':
			case 'F':
				keyF = false;
				break;
		}
	}
	osg::Vec3 hpr = fromQuat(camRotation,true); // ângulos da câmera, em graus
	float chao = pickDown( eyePos );
	if( chao > 0 && eyePos.z() < chao+0.2 ) // altura mínima do OVNI
	{
		eyePos = osg::Vec3( eyePos.x(), eyePos.y(), chao+0.2 );
	}
	eyePos += camRotation * camSpeed * eyePos.z()/20; // * eyePos.z()/20 faz a velocidade aumentar com a altitude
	matrix.setTrans(eyePos);
	// Check [mainTimer.time % (time divisor) == 0]
	if (mainTimer.time_s() >= maxTick) // ??
	{
		_viewer->getCameraManipulator()->setByMatrix(matrix); // muda posição, não o ângulo
		maxTick += inputTimeInterval;
	}

	return false;
}

// https://groups.google.com/forum/#!searchin/osg-users/LineSegmentIntersector|sort:date/osg-users/f4WZnzr8X5w/37pAZ4QqAwAJ
// src/osgUtil/LineSegmentIntersector.cpp
float OVNIManipulator::pickDown( osg::Vec3d pos ) // retorna altitude do terreno (className() == "Geometry") naquele ponto, ou -1 se estiver fora do terreno
{
	osgUtil::LineSegmentIntersector::Intersections intersections;
	osg::Vec3d pos1( pos.x(), pos.y(), pos.z()+100 );
	osg::Vec3d pos2( pos.x(), pos.y(), -100 );
	osg::ref_ptr<osgUtil::LineSegmentIntersector> intsec = new osgUtil::LineSegmentIntersector( osgUtil::Intersector::MODEL, pos1, pos2 );
	osgUtil::IntersectionVisitor iv( intsec.get() );
	_viewer->getSceneData()->accept( iv );
	if( intsec->containsIntersections() )
	{
		for(osgUtil::LineSegmentIntersector::Intersections::iterator hitr = intsec->getIntersections().begin();
			hitr != intsec->getIntersections().end();
			++hitr)
		{
			if ( strcmp(hitr->drawable->className(),"Geometry") == 0 )
			{
				return hitr->getWorldIntersectPoint().z();
			}
		}
		return -1;
	}
	else
		return -1;
}

bool OVNIManipulator::performMovement()
{
	// return if less then two events have been added
	if( _ga_t0.get() == NULL || _ga_t1.get() == NULL )
		return false;
	if( mouseFree )
	{
		if( Warping )
			mouseFree = false;
		return false;
	}
	if( Warping )
		Warping = false;
	// get delta time
	double eventTimeDelta = _ga_t0->getTime() - _ga_t1->getTime();
	if( eventTimeDelta < 0. )
	{
		OSG_WARN << "Manipulator warning: eventTimeDelta = " << eventTimeDelta << std::endl;
		eventTimeDelta = 0.;
	}
	// get deltaX and deltaY
	float dx = _ga_t0->getXnormalized() - _ga_t1->getXnormalized();
	float dy = _ga_t0->getYnormalized() - _ga_t1->getYnormalized();
	// return if there is no movement.
	if( dx == 0. && dy == 0. )
		return false;
	// call appropriate methods
	unsigned int modKeyMask = _ga_t1->getModKeyMask();
	unsigned int buttonMask = _ga_t1->getButtonMask();
	if( buttonMask == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON )
		return performMovementLeftMouseButton( eventTimeDelta, dx, dy );
	else if( ( buttonMask == osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON ) ||
			( buttonMask == osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON && modKeyMask & osgGA::GUIEventAdapter::MODKEY_CTRL ) ||
			( buttonMask == (osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON | osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON) ) )
		return performMovementMiddleMouseButton( eventTimeDelta, dx, dy );
	else if( buttonMask == osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON )
		return performMovementRightMouseButton( eventTimeDelta, dx, dy );
	else
		return performMovementLeftMouseButton( eventTimeDelta, dx, dy );
	return false;
}

bool OVNIManipulator::handleMouseMove(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
	addMouseEvent( ea );
	if ( performMovement() )
		aa.requestRedraw();
	aa.requestContinuousUpdate( false );
	_thrown = false;
	if( !mouseFree )
	{
		// Warp horizontal
		if( _ga_t0->getX() > _width*0.9 )
		{
			Warping = true;
			mouseFree = true;
			aa.requestWarpPointer( _width*0.2, _ga_t0->getY() );
		}
		else
		if( _ga_t0->getX() < _width*0.1 )
		{
			Warping = true;
			mouseFree = true;
			aa.requestWarpPointer( _width*0.8, _ga_t0->getY() );
		}
		// Warp vertical
		if( _ga_t0->getY() > _height*0.9 )
		{
			Warping = true;
			mouseFree = true;
			aa.requestWarpPointer( _ga_t0->getX(), _height*0.2 );
		}
		else
		if( _ga_t0->getY() < _height*0.1 )
		{
			Warping = true;
			mouseFree = true;
			aa.requestWarpPointer( _ga_t0->getX(), _height*0.8 );
		}
	}
	return true;
}

osg::Vec3d OVNIManipulator::fromQuat(const osg::Quat& quat, bool degrees)
{
	// From: http://guardian.curtin.edu.au/cga/faq/angles.html
	// Except OSG exchanges pitch & roll from what is listed on that page
	double qx = quat.x();
	double qy = quat.y();
	double qz = quat.z();
	double qw = quat.w();

	double sqx = qx * qx;
	double sqy = qy * qy;
	double sqz = qz * qz;
	double sqw = qw * qw;

	double term1 = 2 * (qx*qy + qw*qz);
	double term2 = sqw + sqx - sqy - sqz;
	//double term3 = -2 * (qx*qz - qw*qy);
	double term4 = 2 * (qw*qx + qy*qz);
	double term5 = sqw - sqx - sqy + sqz;

	double heading = atan2(term1, term2);
	double pitch = atan2(term4, term5);
	//double roll = asin(term3);

	//Return values in degrees if requested, else its radians
	if (degrees)
	{
		heading = osg::RadiansToDegrees(heading);
		pitch = osg::RadiansToDegrees(pitch);
		//roll = osg::RadiansToDegrees(roll);
	}
	return osg::Vec3d(heading, pitch, 0); // (não usa roll)
}

void OVNIManipulator::calcAcc(osgViewer::Viewer* viewer) // calcula aceleração do OVNI
{
	// velocidade frontal
	if (!keyW && !keyS)
	{
		velZ *= 0.9;
		if( abs(velZ) < 0.01 )
			velZ = 0;
	}
	else
	{
		if (keyW && velZ > -maxVel)
			velZ -= acc;
		if (keyS && velZ < maxVel)
			velZ += acc;
	}
	// velocidade lateral
	if (!keyA && !keyD)
	{
		velX *= 0.9;
		if( abs(velX) < 0.01 )
			velX = 0;
	}
	else
	{
		if (keyA && velX > -maxVel)
			velX -= acc;
		if (keyD && velX < maxVel)
			velX += acc;
	}
	// velocidade vertical
	if (!keyR && !keyF)
	{
		velY *= 0.9;
		if( abs(velY) < 0.01 )
			velY = 0;
	}
	else
	{
		if (keyR && velY < maxVel)
			velY += acc;
		if (keyF && velY > -maxVel)
			velY -= acc;
	}
	camSpeed.set(velX, velY, velZ);
}
