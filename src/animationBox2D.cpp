//
//  animationBox2D.cpp
//  murmur
//
//  Created by Julien on 20/05/13.
//
//

#include "animationBox2D.h"
#include "globals.h"
#include "surface.h"


//--------------------------------------------------------------
AnimationBox2D::AnimationBox2D(string name) : Animation(name)
{
    m_volumeAccum = 0.0f;
    m_volumeAccumTarget = 0.0f;
    m_isBox2DCreated = false;
	
	m_isLeftWall 	= true;
	m_isRightWall	= true;
	m_isTopWall		= true;
	m_isBottomWall	= true;
}

//--------------------------------------------------------------
void AnimationBox2D::createBox2D(float gx, float gy, float fps, bool isBounds, ofRectangle bounds)
{
    if (!m_isBox2DCreated)
    {
        m_box2d.init();
        m_box2d.setGravity(gx, gy);
        if (isBounds)
			createBounds(bounds);
        m_box2d.setFPS(fps);
    
        m_isBox2DCreated = true;
    }
}

//--------------------------------------------------------------
void AnimationBox2D::createBounds(ofRectangle bounds)
{
	if (!m_box2d.bWorldCreated) return;
	
	if (m_box2d.ground)
		m_box2d.world->DestroyBody( m_box2d.ground );

	b2BodyDef bd;
	bd.position.Set(0, 0);
	m_box2d.ground = m_box2d.world->CreateBody(&bd);
	
	b2PolygonShape shape;
	
	ofRectangle rec(bounds.x/OFX_BOX2D_SCALE, bounds.y/OFX_BOX2D_SCALE, bounds.width/OFX_BOX2D_SCALE, bounds.height/OFX_BOX2D_SCALE);
	
	//right wall
	if (m_isRightWall)
	{
		shape.SetAsEdge(b2Vec2(rec.x+rec.width, rec.y), b2Vec2(rec.x+rec.width, rec.y+rec.height));
		m_box2d.ground->CreateFixture(&shape, 0.0f);
	}
	
	//left wall
	if (m_isLeftWall)
	{
		shape.SetAsEdge(b2Vec2(rec.x, rec.y), b2Vec2(rec.x, rec.y+rec.height));
		m_box2d.ground->CreateFixture(&shape, 0.0f);
	}
	
	// top wall
	if (m_isTopWall)
	{
		shape.SetAsEdge(b2Vec2(rec.x, rec.y), b2Vec2(rec.x+rec.width, rec.y));
		m_box2d.ground->CreateFixture(&shape, 0.0f);
	}
	
	// bottom wall
	if (m_isBottomWall)
	{
		shape.SetAsEdge(b2Vec2(rec.x, rec.y+rec.height), b2Vec2(rec.x+rec.width, rec.y+rec.height));
		m_box2d.ground->CreateFixture(&shape, 0.0f);
	}

}


//--------------------------------------------------------------
void AnimationBox2D::guiEvent(ofxUIEventArgs &e)
{
	Animation::guiEvent(e);

	string name = e.getName();
	if (name == "left wall" || name == "right wall" || name == "top wall" || name == "bottom wall")
	{
		Surface* pSurfaceMain = GLOBALS->getSurfaceMain();
		if (pSurfaceMain)
		{
			createBox2D(0,0*10,30,true,ofRectangle(0, 0, pSurfaceMain->getWidthPixels(), pSurfaceMain->getHeightPixels()));
			createBounds( ofRectangle(0.0f,0.0f,pSurfaceMain->getWidthPixels(),pSurfaceMain->getHeightPixels()) ); // TEMP, should be relative to surface
		}
	}
}

//--------------------------------------------------------------
AnimationBox2D_circles::AnimationBox2D_circles(string name ) : AnimationBox2D(name)
{
    m_gravity = 0.0f;
    m_isCircleInflating = false;
    m_circleSize = 0.0f;
    m_volTrigger = 0.2f;
    m_sizeMin = 10;
    m_sizeMax = 30;
    m_nbObjects = 400;
}

//--------------------------------------------------------------
void AnimationBox2D_circles::onVolumAccumEvent(string deviceId)
{
	Animation::onVolumAccumEvent(deviceId);
}

//--------------------------------------------------------------
void AnimationBox2D_circles::VM_update(float dt)
{
//	ofLog() << m_listSoundNames.size();

    if (m_isBox2DCreated){
        m_box2d.setGravity(0.0f, m_gravity);
        m_box2d.update();
    }
    
}

//--------------------------------------------------------------
void AnimationBox2D_circles::VM_draw(float w, float h)
{
 
    ofBackground(0);
    ofPushStyle();
    ofxBox2dCircle circle;
	for(int i=0; i<m_listCircles.size(); i++)
    {
        circle = m_listCircles[i];
        
		ofFill();
		ofSetColor(255);
        ofEllipse(circle.getPosition().x, circle.getPosition().y, circle.getRadius()*2,circle.getRadius()*2);
    }

    if (m_isCircleInflating)
    {
		ofSetColor(255,0,0);
        ofEllipse(m_posAnchor.x, m_posAnchor.y, m_circleSize,m_circleSize);
    }
    
    ofPopStyle();
}

//--------------------------------------------------------------
void AnimationBox2D_circles::onNewPacket(DevicePacket* pDevicePacket, string deviceId, float x, float y)
{
    if (pDevicePacket==0) return;
 
	accumulateVolume(pDevicePacket->m_volume, deviceId);
	
    m_posAnchor.set(x,y);
    
	if (pDevicePacket->m_volume>m_volTrigger)
	{
		// playSound(deviceId);

		m_volumeAccumTarget += pDevicePacket->m_volume;

        m_circleSize = ofRandom(m_sizeMin,m_sizeMax);

        //if (m_volumeAccumTarget>=10.5f)
        {
            ofxBox2dCircle circle;
            circle.setPhysics(3.0, 0.53, 0.1);
            circle.setup(m_box2d.getWorld(), x, y, m_circleSize);
            circle.setVelocity( ofRandom(-0.5,0.5), ofRandom(-0.5,0.5) );
            
            m_listCircles.push_back(circle);

            if (m_listCircles.size() > (int)m_nbObjects) {
                m_listCircles.begin()->destroy();
                m_listCircles.erase(m_listCircles.begin());
            }
            
            m_volumeAccumTarget = 0;
            m_isCircleInflating = false;
        }
    }
	else
	{
		m_volumeAccumTarget = 0;
        m_isCircleInflating = false;
	}
}

//--------------------------------------------------------------
void AnimationBox2D_circles::createUICustom()
{
    if (mp_UIcanvas)
    {
        mp_UIcanvas->addSlider("gravity", 			-10.0f, 10.0f, 	&m_gravity);
        mp_UIcanvas->addSlider("vol. trigger", 		0.0f, 1.0f, 	&m_volTrigger);
        mp_UIcanvas->addSlider("obj. size min", 	10, 20, 		&m_sizeMin);
        mp_UIcanvas->addSlider("obj. size max", 	10, 50, 		&m_sizeMax);
        mp_UIcanvas->addSlider("obj. number", 		100, 500, 		&m_nbObjects);

        mp_UIcanvas->addToggle("left wall", 	&m_isLeftWall);
        mp_UIcanvas->addToggle("right wall", 	&m_isRightWall);
        mp_UIcanvas->addToggle("top wall", 		&m_isTopWall);
        mp_UIcanvas->addToggle("bottom wall", 	&m_isBottomWall);
    }
}

//--------------------------------------------------------------
void AnimationBox2D_circles::registerSoundTags(vector<string>& soundTags)
{
	soundTags.push_back("hit");
	soundTags.push_back("bubble");
}


