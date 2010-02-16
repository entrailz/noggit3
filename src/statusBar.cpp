#include "statusBar.h"
#include "video.h"
#include "wowmapview.h"

statusBar::statusBar( float xPos, float yPos, float w, float h ) : window(xPos, yPos, w, h)
{
	mustresize = true;

	// create leftInfo text element
	leftInfo = new textUI( 8.0f, 7.0f, "", &arial16, eJustifyLeft );
	addChild( leftInfo );

	// create rightInfo text element
	rightInfo = new textUI( this->width - 8.0f, 7.0f, "", &arial16, eJustifyRight );
	addChild( rightInfo );
}

void statusBar::render( )
{
	glPushMatrix( );
	glTranslatef( x, y, 0.0f );

	glColor4f( 0.2f, 0.2f, 0.2f, 0.8f );
	glBegin( GL_TRIANGLE_STRIP );
	glVertex2f( 0.0f, 0.0f );
	glVertex2f( width, 0.0f );
	glVertex2f( 0.0f, height );
	glVertex2f( width, height );
	glEnd( );

	for( std::vector<frame*>::iterator child = children.begin( ); child != children.end( ); child++ )
		if( !( *child )->hidden )
			( *child )->render( );

	glColor3f( 0.7f, 0.7f, 0.7f );

	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, texture );
	glEnable( GL_TEXTURE_2D );

	//Draw Top Side
	glBegin( GL_TRIANGLE_STRIP );	
	glTexCoord2f( 0.375f, 1.0f );
	glVertex2f( 0.0f, 13.0f );	
	glTexCoord2f( 0.375f, 0.0f );
	glVertex2f( width, 13.0f );
	glTexCoord2f( 0.25f, 1.0f );
	glVertex2f( 0.0f, -3.0f );
	glTexCoord2f( 0.25f, 0.0f );
	glVertex2f( width, -3.0f );
	glEnd( );

	glDisable( GL_TEXTURE_2D );
	glPopMatrix( );
}

void statusBar::resize( )
{
	this->y = video.yres - 30.0f;
	this->width = video.xres;
	this->rightInfo->x = this->width - 8.0f;
}

void statusBar::setLeftInfo( std::string pText )
{
	this->leftInfo->setText( pText );
}

void statusBar::setRightInfo( std::string pText )
{
	this->rightInfo->setText( pText );
}