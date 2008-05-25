#include "Application.h"
#include "../GameCube/gc_new_delete.h"
#include "Input.h"
#include "../ControllerManager.h"
#include "../LoadingController.h"
//#include "../MenuController.h"
#include "../GameController.h"
#include "../../gfxutils/ConfParser/ConfParser.h"
//#include "../../gfxutils/MemManager/MemMgr_RawData.h"
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
//#include <al/alut.h>

// TEMP FILES

// ~TEMP FILES

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
	Input::create();
	RenderEngine::safeInstance().init();

	LoadingController::safeInstance().load(this, &Application::load);

	return true;
}


void Application :: load()
{
	OSReport("Application :: load()\n");

	ConfParser cp("config/config.txt");
	const ParserSection& ps = cp.rootSection();

	Logger::init(ps.getSection("Logger"));

	//MenuController::safeInstance().onApplicationLoad(ps);
	WorldObjectTypeManager::safeInstance().onApplicationLoad(ps);
	BuffFactory::safeInstance().onApplicationLoad(ps);
	LaserFactory::safeInstance().onApplicationLoad(ps);
	EnemyFactory::safeInstance().onApplicationLoad(ps);
	GameLogic::safeInstance().onApplicationLoad(ps);
	SoundEngine::safeInstance().onApplicationLoad(ps);
	RenderEngine::safeInstance().onApplicationLoad(ps);
	AIEngine::safeInstance().onApplicationLoad(ps);
	PhysicsEngine::safeInstance().onApplicationLoad(ps);

	//MenuController::safeInstance().load();
    EnemyBuffCoupleManager::safeInstance();

	GameController::safeInstance().loadLevel("Level08");
}


void Application :: destroy()
{
	unload();

    //alutExit();

	// This doesn't work!
	// LoadingController waits for 0.1ms till the loading screen is rendered,
	// before calling the callback. in that 0.1ms, no update is ever sent anymore
	// LoadingController::update never gets called anymore

	//LoadingController* lc = new LoadingController();
	//lc->setLoader(this, &Application::unload);

	//ControllerManager& cm = ControllerManager::safeInstance();
	//cm.activateController(lc);
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
	//MenuController::destroy();
	LoadingController::destroy();
	SoundEngine::destroy();
	RenderEngine::destroy();
	GameLogic::destroy();
	//MemMgrRaw::destroy();
	Logger::destroy();
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

	//cout << "Application :: update(" << dt << ")" << endl;

	Input::instance().update();
	ControllerManager::instance().update(dt);
	RenderEngine::instance().update(dt);
}
