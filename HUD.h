// HUD.h

#ifndef HUD_H
#define HUD_H

#include <string>
#include <osg/Camera>
#include <osgText/Font>
#include <osgText/Text>

class HUDCamera : public osg::Camera
{
public:
	HUDCamera(int left, int width, int top, int height)
	{
		g_font = osgText::readFontFile("fonts/Charter.ttf"); // https://practicaltypography.com/charter.html
		_width = width;
		_height = height;
		setReferenceFrame( osg::Transform::ABSOLUTE_RF );
		setClearMask( GL_DEPTH_BUFFER_BIT );
		setRenderOrder( osg::Camera::POST_RENDER );
		setAllowEventFocus( false );
		setProjectionMatrix( osg::Matrix::ortho2D(left, width, top, height) );
		//setProjectionMatrixAsOrtho( left, width, top, height, 100, -100 ); // não funcionou. Os valores Z (texto e menu do HUD) precisam ficar entre 0 e -1
		getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF ); // necessário aqui ??
		criaTexto();
		setName("cameraHUD");
	}
	void criaTexto();
protected:
	int _width, _height;
	osg::ref_ptr<osgText::Font> g_font;
	osg::ref_ptr<osgText::Text> text;
};

#endif
