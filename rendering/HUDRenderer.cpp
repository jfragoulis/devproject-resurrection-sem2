//*****************************************************************************
//**
//**   File:               HUDRenderer.cpp
//**   Author:             $Author: Joep.Moritz $
//**   Last Change:        $Date: 2008-03-09 14:34:51 +0000 (Sun, 09 Mar 2008) $
//**   Last Revision:      $Revision: 126 $
//**
//*****************************************************************************

#include "HUDRenderer.h"
#include "RenderEngine.h"
#include "../gfx/Shaders/ShaderManager.h"
#include "../gfx/Texture/Texture.h"
#include "../gfx/Texture/TextureIO.h"
#include "../GameLogic/GameLogic.h"
#include "../GameLogic/Objects/Playership.h"
#include "gl/glu.h"

HUDRenderer::HUDRenderer() : m_playership(0), m_currentLives(0), m_ebombType(EBOMB_TYPE_UNKNOWN)
{
	Texture *tex = TextureIO::instance()->getTexture("hudLifeIcon.bmp");
	m_textureList.push_back(tex);
	tex = TextureIO::instance()->getTexture("hudBar.bmp");
	m_textureList.push_back(tex);
	tex = TextureIO::instance()->getTexture("hudEnergyBg.bmp");
	m_textureList.push_back(tex);
	tex = TextureIO::instance()->getTexture("redBar.bmp");
	m_textureList.push_back(tex);
	tex = TextureIO::instance()->getTexture("yellowBar.bmp");
	m_textureList.push_back(tex);
	tex = TextureIO::instance()->getTexture("blueBar.bmp");
	m_textureList.push_back(tex);
	tex = TextureIO::instance()->getTexture("hudBar2.bmp");
	m_textureList.push_back(tex);
	tex = TextureIO::instance()->getTexture("hudRedBomb.bmp");
	m_textureList.push_back(tex);
	/*tex = TextureIO::instance()->getTexture("hudYellowBomb.bmp");
	m_textureList.push_back(tex);
	tex = TextureIO::instance()->getTexture("hudBlueBomb.bmp");
	m_textureList.push_back(tex);
	tex = TextureIO::instance()->getTexture("hudGreenBomb.bmp");
	m_textureList.push_back(tex);
	tex = TextureIO::instance()->getTexture("hudOrangeBomb.bmp");
	m_textureList.push_back(tex);
	tex = TextureIO::instance()->getTexture("hudPurpleBomb.bmp");
	m_textureList.push_back(tex);*/
}

HUDRenderer::~HUDRenderer()
{
}



void HUDRenderer :: render(Graphics& g) const
{
	

	// draw energy bars
	// draw number of lives
	// draw type of e-bomb. You could listen to events for e-bomb created/uncreated
	// but asking GameLogic about it is just as easy :)
	// if type is unknown, no ebomb exists
	

	int viewPortDims[4];
	RenderEngine::instance().getViewport(viewPortDims);
	int screenWidth = viewPortDims[2];
	int screenHeight = viewPortDims[3];


	//glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
		glLoadIdentity();
		gluOrtho2D(0, screenWidth, 0, screenHeight);
	glMatrixMode(GL_MODELVIEW);
	//here goes all the 2D rendering
	glPushMatrix();
		glLoadIdentity();

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		ShaderManager::instance()->begin("hudShader");
		m_textureList[1]->bind(0);
		CHECK_GL_ERROR();
		ShaderManager::instance()->setUniform1i("tex",0);
		CHECK_GL_ERROR();
		const GLfloat transparency = 0.6f;
		ShaderManager::instance()->setUniform1fv("transparency", &transparency);
		CHECK_GL_ERROR();

		if (m_playership != NULL) {
			int energyCapacity = m_playership->getEnergyCapacity();
			int redEnergyAmount = m_playership->getEnergy(ENERGY_TYPE_RED);
			int yellowEnergyAmount = m_playership->getEnergy(ENERGY_TYPE_YELLOW);
			int blueEnergyAmount = m_playership->getEnergy(ENERGY_TYPE_BLUE);

			//DRAW HUD BARS
			//life bar
			RenderEngine::drawTexturedQuad(Vector3(0.0f, 0, 0), Vector3(180.0f, 0, 0), Vector3(0, 50.0f, 0), Vector2(0,0), Vector2(1,1));
			m_textureList[2]->bind(0);
			//energy bar
			RenderEngine::drawTexturedQuad(Vector3((screenWidth/2.0f-(energyCapacity*10)/2)-10, 5, 0), Vector3(energyCapacity*10+20, 0, 0), Vector3(0, 40.0f, 0), Vector2(0,0), Vector2(1,1));
			m_textureList[6]->bind(0);
			RenderEngine::drawTexturedQuad(Vector3(screenWidth - 100.0f, 0, 0), Vector3(100.0f, 0, 0), Vector3(0, 50.0f, 0), Vector2(0,0), Vector2(1,1));

			const GLfloat transparency2 = 1.0f;
			ShaderManager::instance()->setUniform1fv("transparency", &transparency2);
			CHECK_GL_ERROR();

			m_textureList[0]->bind(0);
			//DRAW LIFES INFO
			for (int i = 0; i < m_currentLives; i++)
				RenderEngine::drawTexturedQuad(Vector3((float) (i*50.0f), 0, 0), Vector3(50.0f, 0, 0), Vector3(0, 50.0f, 0), Vector2(0,0), Vector2(1,1));

			

			//DRAW E-BOMB INFO
			switch (m_ebombType) {
				case EBOMB_TYPE_UNKNOWN:
					break;
				/*case EBOMB_TYPE_RED:
					m_textureList[7]->bind(0);
					RenderEngine::drawTexturedQuad(Vector3(screenWidth-70.0f, 10, 0), Vector3(30.0f, 0, 0), Vector3(0, 30.0f, 0), Vector2(0,0), Vector2(1,1));
					break;
				case EBOMB_TYPE_YELLOW:
					m_textureList[8]->bind(0);
					RenderEngine::drawTexturedQuad(Vector3(screenWidth-70.0f, 10, 0), Vector3(30.0f, 0, 0), Vector3(0, 30.0f, 0), Vector2(0,0), Vector2(1,1));
					break;
				case EBOMB_TYPE_BLUE:
					m_textureList[9]->bind(0);
					RenderEngine::drawTexturedQuad(Vector3(screenWidth-70.0f, 10, 0), Vector3(30.0f, 0, 0), Vector3(0, 30.0f, 0), Vector2(0,0), Vector2(1,1));
					break;
				case EBOMB_TYPE_GREEN:
					m_textureList[10]->bind(0);
					RenderEngine::drawTexturedQuad(Vector3(screenWidth-70.0f, 10, 0), Vector3(30.0f, 0, 0), Vector3(0, 30.0f, 0), Vector2(0,0), Vector2(1,1));
					break;
				case EBOMB_TYPE_ORANGE:
					m_textureList[11]->bind(0);
					RenderEngine::drawTexturedQuad(Vector3(screenWidth-70.0f, 10, 0), Vector3(30.0f, 0, 0), Vector3(0, 30.0f, 0), Vector2(0,0), Vector2(1,1));
					break;
				case EBOMB_TYPE_PURPLE:
					m_textureList[12]->bind(0);
					RenderEngine::drawTexturedQuad(Vector3(screenWidth-70.0f, 10, 0), Vector3(30.0f, 0, 0), Vector3(0, 30.0f, 0), Vector2(0,0), Vector2(1,1));
					break;*/
				default:
					m_textureList[7]->bind(0);
					RenderEngine::drawTexturedQuad(Vector3(screenWidth-70.0f, 10, 0), Vector3(30.0f, 0, 0), Vector3(0, 30.0f, 0), Vector2(0,0), Vector2(1,1));
					break;

					

			}


			//DRAW ENERGY BARS
			//RED BAR
			m_textureList[3]->bind(0);
			for (int i = 0; i < redEnergyAmount; i++) {
				RenderEngine::drawTexturedQuad(Vector3((float) (i*10.0f) + (screenWidth/2.0f-(energyCapacity*10)/2), 30.0f, 0), Vector3(10.0f, 0.0f, 0), Vector3(0, 10.0f, 0), Vector2(0,0), Vector2(1,1));
			}
			//YELLOW BAR
			m_textureList[4]->bind(0);
			for (int i = 0; i < yellowEnergyAmount; i++) {
				RenderEngine::drawTexturedQuad(Vector3((float) (i*10.0f) + (screenWidth/2.0f-(energyCapacity*10)/2), 20.0f, 0), Vector3(10.0f, 0, 0), Vector3(0, 10.0f, 0), Vector2(0,0), Vector2(1,1));
			}
			//BLUE BAR
			m_textureList[5]->bind(0);
			for (int i = 0; i < blueEnergyAmount; i++) {
				RenderEngine::drawTexturedQuad(Vector3((float) (i*10.0f) + (screenWidth/2.0f-(energyCapacity*10)/2), 10.0f, 0), Vector3(10.0f, 0, 0), Vector3(0, 10.0f, 0), Vector2(0,0), Vector2(1,1));
			}
		}


		ShaderManager::instance()->end();

		

		glDisable(GL_BLEND);

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_MODELVIEW);
	//glEnable(GL_LIGHTING);
}

void HUDRenderer :: update(float dt)
{
	m_playership = GameLogic::instance().getPlayership();
	m_currentLives = GameLogic::instance().getCurrentLives();
	m_ebombType = GameLogic::instance().getCurrentEbombType();
}


void HUDRenderer :: onEvent(Player_Spawned& evt)
{
	m_playership = evt.getValue();
}

void HUDRenderer :: onEvent(Player_Despawned&)
{
	m_playership = 0;
}