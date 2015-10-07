//
//  animationVideo.cpp
//  murmur
//
//  Created by Julien on 10/09/2015.
//
//

#include "animationVideo.h"

//--------------------------------------------------------------
AnimationVideo::AnimationVideo(string name) : Animation(name)
{
	m_bLoadVideo 		= false;
	m_bHasLoadedVideo	= false;
	mp_teVideoPath		= 0;
}

//--------------------------------------------------------------
AnimationVideo::~AnimationVideo()
{
}

//--------------------------------------------------------------
void AnimationVideo::createUICustom()
{
	if (mp_UIcanvas)
	{
		mp_teVideoPath = new ofxUITextInput("teVideoPath", "", 200, 16);
		mp_teVideoPath->setAutoUnfocus(true);
		mp_teVideoPath->setAutoClear(false);
	
		mp_UIcanvas->addWidgetDown(mp_teVideoPath);
		
		ofxUILabelButton* pBtnLoad = new ofxUILabelButton("btnLoadVideo", 100, false, OFX_UI_FONT_SMALL);
		mp_UIcanvas->addWidgetRight(pBtnLoad);
		pBtnLoad->getLabelWidget()->setLabel("Load");
	}
}

//--------------------------------------------------------------
void AnimationVideo::setVideo(string name)
{
	OFAPPLOG->begin("AnimationVideo::setVideo("+name+")");
	m_bHasLoadedVideo = m_player.loadMovie(name);
	OFAPPLOG->println(" - loading movie, m_bHasLoadedVideo="+ofToString(m_bHasLoadedVideo));
	if (m_bHasLoadedVideo)
	{
		OFAPPLOG->println(" - play movie");
		m_player.play();
	}
	OFAPPLOG->end();
}

//--------------------------------------------------------------
void AnimationVideo::VM_enter()
{
	m_player.play();
}

//--------------------------------------------------------------
void AnimationVideo::VM_exit()
{
	m_player.stop();
}

//--------------------------------------------------------------
void AnimationVideo::VM_update(float dt)
{
/*	if (!m_bLoadVideo)
	{
		m_bLoadVideo = true;
		setVideo(m_videoPath);
	}
*/

	m_player.update();
}

//--------------------------------------------------------------
void AnimationVideo::VM_draw(float w, float h)
{

	if (m_bHasLoadedVideo)
	{
		ofRectangle rectSurface(0,0,w,h);
		ofRectangle rectVideo(0,0,m_player.getWidth(),m_player.getHeight());
		rectVideo.scaleTo(rectSurface);

		ofSetColor(255,255);
		ofBackground(0);
		m_player.draw(rectVideo.x,rectVideo.y,rectVideo.width,rectVideo.height);
//		OFAPPLOG->println("player width="+ofToString(m_player.getWidth())+" ,height="+ofToString(m_player.getHeight()));
//		OFAPPLOG->println("player.isLoaded()="+ofToString(m_player.isLoaded()));
	}
	else
	{
		ofBackground(255,0,0);
	}
	
}

//--------------------------------------------------------------
void AnimationVideo::guiEvent(ofxUIEventArgs &e)
{
	OFAPPLOG->begin("AnimationVideo::guiEvent()");
	Animation::guiEvent(e);

	string name = e.getName();
	int kind = e.getKind();


	OFAPPLOG->println(" - name="+name);
	if (kind == OFX_UI_WIDGET_TEXTINPUT)
	{
		if (name == "teVideoPath")
		{
			if (mp_teVideoPath->getTextString() != "")
			{
				m_videoPath = "Video/"+mp_teVideoPath->getTextString();
				OFAPPLOG->println(" - m_videoPath = "+m_videoPath);

				m_bLoadVideo = false;
				m_bHasLoadedVideo = false;
				
				 setVideo(m_videoPath);

			}
		}
	}
	else
	if (kind == OFX_UI_WIDGET_LABELBUTTON)
	{
	   if (name == "btnLoadVideo")
	   {
			if (e.getButton()->getValue()>0)
			{
				ofFileDialogResult result = ofSystemLoadDialog("Load video", true, ofToDataPath("Video",true));
				ofLog() << " path= " << result.getPath();
				ofLog() << " name= " << result.getName();
				
				if (mp_teVideoPath && result.getName() != "")
				{
					m_videoPath = "Video/"+result.getName();
					mp_teVideoPath->setTextString( result.getName() );
					m_bLoadVideo = false;
					m_bHasLoadedVideo = false;
				}
			}
	   }
	}
	OFAPPLOG->end();
}

//--------------------------------------------------------------
void AnimationVideo::loadConfiguration(string filename)
{
	OFAPPLOG->begin("AnimationVideo::loadConfiguration("+filename+")");
	Animation::loadConfiguration(filename);

/*
	if (mp_teVideoPath)
	{
		m_bLoadVideo = false;
		m_bHasLoadedVideo = false;
		m_videoPath = "Video/"+mp_teVideoPath->getTextString();
		OFAPPLOG->println(" - m_videoPath = "+m_videoPath);
	}
	else
	{
		OFAPPLOG->println(" - mp_teVideoPath is NULL ");
	}
*/
	OFAPPLOG->end();
}



