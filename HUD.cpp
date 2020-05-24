// HUD.cpp

#include "HUD.h"

void HUDCamera::criaTexto()
{
	text = new osgText::Text;
	text->setDataVariance( osg::Object::DYNAMIC );
	text->setFont( g_font.get() );
	text->setCharacterSize( _width/50 );
	text->setAxisAlignment( osgText::TextBase::XY_PLANE );
	text->setBackdropType(osgText::Text::DROP_SHADOW_BOTTOM_RIGHT);
	text->setPosition( osg::Vec3(20, _height-20, -0.5f) ); // com 0 no final o menu tampa, com 0.1 ou 1 no final não aparece na tela, com -1 no final aparece atrás do menu
	text->setText( "Texto do HUD" );
	osg::ref_ptr<osg::Geode> textGeode = new osg::Geode;
	textGeode->addDrawable( text );
	addChild( textGeode.get() );
}
