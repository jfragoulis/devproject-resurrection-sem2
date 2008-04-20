//*****************************************************************************
//**
//**   File:               Application.cpp
//**   Author:             $Author: Joep.Moritz $
//**   Last Change:        $Date: 2008-03-20 15:18:08 +0000 (Thu, 20 Mar 2008) $
//**   Last Revision:      $Revision: 212 $
//**
//*****************************************************************************


#include "Application.h"
#include "../ControllerManager.h"
#include "../MenuController.h"
#include "../LoadingController.h"
#include "../GameController.h"
#include "../../gfxutils/ConfParser/ConfParser.h"
#include "../../gfxutils/MemManager/MemMgr_RawData.h"
#include "../../GameLogic/GameLogic.h"
#include "../../rendering/RenderEngine.h"
#include "../../sound/SoundEngine.h"
#include "../../AI/AIEngine.h"
#include "../../Physics/PhysicsEngine.h"
#include "../../GameLogic/Enemies/EnemyFactory.h"
#include "../../GameLogic/Lasers/LaserFactory.h"
#include "../../GameLogic/Buffs/BuffFactory.h"
#include "../../GameLogic/Buffs/EnemyBuffCoupleManager.h"
#include "../../gfxutils/Misc/Logger.h"
#include "../../GameLogic/WorldObjectTypeManager.h"
#include "../../gfx/Texture/TextureMgr.h"
#include "../../gfx/Shaders/ShaderManager.h"
#include "../../utility/TimerManager.h"
#include "Input.h"
#include <al/alut.h>
#include <iostream>
#include <fstream>
using namespace std;

const float DELTA_TIME_MAX = 0.1f;
//const float DELTA_TIME_MIN = 0.5f;

Application :: Application()
{
}

Application :: ~Application()
{
}


//ofstream filestr;

bool Application :: init()
{
    alutInit(0,0);
    alGetError();

	//streambuf* psbuf;
	//filestr.open("cout.txt");
	//psbuf = filestr.rdbuf();
	//cout.rdbuf(psbuf);

	TimerManager::safeInstance();

	ConfParser* cp = new ConfParser("./config/config.gfx");
	TextureMgr::init(cp->getSection("Texture"));
	ShaderManager::init(cp->getSection("Shader"));
	MemMgrRaw::init(cp->getSection("MemManager:RawData"));

	LoadingController::safeInstance().load(this, &Application::load);

	return true;
}

void Application :: destroy()
{
	unload();

    alutExit();

	// This doesn't work!
	// LoadingController waits for 0.1ms till the loading screen is rendered,
	// before calling the callback. in that 0.1ms, no update is ever sent anymore
	// LoadingController::update never gets called anymore

	//LoadingController* lc = new LoadingController();
	//lc->setLoader(this, &Application::unload);

	//ControllerManager& cm = ControllerManager::safeInstance();
	//cm.activateController(lc);
}


void Application :: load()
{
	ConfParser cp("./config/config.txt");
	const ParserSection& ps = cp.rootSection();

	Logger::init(ps.getSection("Logger"));

	WorldObjectTypeManager::safeInstance().onApplicationLoad(ps);
	BuffFactory::safeInstance().onApplicationLoad(ps);
	LaserFactory::safeInstance().onApplicationLoad(ps);
	EnemyFactory::safeInstance().onApplicationLoad(ps);
	GameLogic::safeInstance().onApplicationLoad(ps);
	SoundEngine::safeInstance().onApplicationLoad(ps);
	RenderEngine::safeInstance().onApplicationLoad(ps);
	AIEngine::safeInstance().onApplicationLoad(ps);
	PhysicsEngine::safeInstance().onApplicationLoad(ps);

	MenuController::safeInstance().load();
    EnemyBuffCoupleManager::safeInstance();
}


void Application :: unload()
{
	RenderEngine::safeInstance().unloadAllRenderers();

	WorldObjectTypeManager::destroy();
	LaserFactory::destroy();
	EnemyFactory::destroy();
	BuffFactory::destroy();
	PhysicsEngine::destroy();
	AIEngine::destroy();
	ControllerManager::destroy();
	GameController::destroy();
	MenuController::destroy();
	LoadingController::destroy();
    SoundEngine::destroy();
	RenderEngine::destroy();
	GameLogic::destroy();
	MemMgrRaw::destroy();
	Logger::destroy();
	TimerManager::destroy();
    EnemyBuffCoupleManager::destroy();

	//filestr.close();
}

void Application :: render(Graphics& g) const
{
	RenderEngine::instance().render(g);
}
void Application :: update(float dt)
{
	if (dt > DELTA_TIME_MAX) dt = DELTA_TIME_MAX;
	//if (dt < DELTA_TIME_MIN) dt = DELTA_TIME_MIN; // <--- BAD!! Just a quick bugfix

	TimerManager::instance().update();
	Input::instance().update();
	ControllerManager::instance().update(dt);
}
