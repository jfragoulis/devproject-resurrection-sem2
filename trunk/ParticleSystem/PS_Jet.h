//*****************************************************************************
//**
//**   File:               PS_Jet.h
//**   Author:             $Author: Manuele.Bonanno $
//**   Last Change:        $Date: 2008-03-09 14:34:51 +0000 (dom, 09 mar 2008) $
//**   Last Revision:      $Revision: 126 $
//**
//*****************************************************************************

#pragma once

#include <string>
#include "PS_Base.h"
#include "../GameLogic/Objects/Playership.h"

class Texture;
struct VAttribStatus;

/*
	This Particle system uses positions & velocity.
	In _generateData function, these 2 should be used.
	In the ctor for the copies, the m_usedAttribs should be modified appropriately
*/

class PS_Jet : public PS_Base
{
private:
	Playership *m_emitterShip;

public :
	// 2 Ctors : Template & copy 
	// Template gets the vbo, copy gets the model
	PS_Jet(const std::string& name,
			Model * model,
			const float psize,
			const float syslife,
			const float plife,
			const unsigned pnum,
			const int shindex,
			const VAttribStatus& status);

	PS_Jet(const std::string& name,
			VBO * vbo,
			const float psize,
			const float syslife,
			const float plife,
			const unsigned pnum,
			const int shindex,
			const std::string& texture);

	void  _generateData(VBO * vbo,Texture * tex);			// The texture for the gen model

	virtual void render() const;		
	virtual void update(const float delta);		
	virtual void reset();						
	virtual PS_Base * clone() const;	

	void setEmitterShip(Playership *emitterShip) { m_emitterShip = emitterShip; };

};