//*****************************************************************************
//**
//**   File:               Spawnpoint.h
//**   Author:             $Author$
//**   Last Change:        $Date$
//**   Last Revision:      $Revision$
//**
//*****************************************************************************


#pragma once
#include "../WorldObject.h"
class ParserSection;
class Point3;

class Spawnpoint : public WorldObject
{
public:
	Spawnpoint();
	virtual ~Spawnpoint();

	void update(float dt, const Point3& playerPosition);

	void loadSettings(const ParserSection&);

private:
	int m_spawnType;
	int m_spawnCount;
	float m_timeBetweenInRangeAndFirstSession;
	float m_timeBetweenSessionStartAndFirstSpawn;
	float m_timeBetweenSpawns;
	float m_timeBetweenLastSpawnAndSessionEnd;
	float m_timeBetweenSessionEndAndSessionStart;
	float m_minimumPlayerDistance;
	float m_maximumPlayerDistance;


	enum State
	{
		WAITING_FOR_PLAYER,
		SESSION_STARTING,
		SPAWNING,
		SESSION_ENDING,
		WAITING_BETWEEN_SESSIONS,
	};

	State m_state;
	float m_timeTillNextEvent;
	int m_enemiesLeftToSpawnThisSession;

	void _spawnEnemy();
};
