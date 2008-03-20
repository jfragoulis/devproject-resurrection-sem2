//*****************************************************************************
//**
//**   File:               Enemyship.cpp
//**   Author:             $Author: Joep.Moritz $
//**   Last Change:        $Date: 2008-03-13 00:18:54 +0000 (Thu, 13 Mar 2008) $
//**   Last Revision:      $Revision: 156 $
//**
//*****************************************************************************


#include "Enemyship.h"
#include "../../gfxutils/ConfParser/ParserSection.h"
#include "../../gfxutils/Misc/utils.h"

Enemyship :: Enemyship() : m_state(ALIVE)
{
}

Enemyship :: Enemyship(int type)
{
	m_state = ALIVE;
	setType(type);
}

Enemyship :: ~Enemyship()
{
}


void Enemyship :: loadSettings( const ParserSection& ps )
{
	m_energyType = EnergyTypeFromString(ps.getVal("EnergyType"));
	m_collisionPower = FromString<int>(ps.getVal("CollisionPower"));
	m_hitPoints = FromString<int>(ps.getVal("HitPoints"));
	m_energyPoints = FromString<int>(ps.getVal("EnergyPoints"));
	Spaceship::loadSettings(ps);
}