#pragma once
#include "../Movable.h"
#include "../../math/Point3.h"
#include "../../math/Vector3.h"
class ParserSection;

/**
 * All lasers are considered to be lines.
 * They have the following properties:
 * age, life time, max length, width, speed, direction, back point and state
 * A laser starts at a specific point and grows until it reaches the max length.
 * When it dies it shrinks again.
 */
class Laser : public WorldObject
{
public:
	Laser(int type);
	virtual ~Laser();

	float getLength() const { return getPosition().distance(m_backPoint); }
	float getMaxLength() const { return m_maxLength; }
	float getWidth() const { return m_width; }
	const Vector3& getDirection() const { return m_direction; }
	const Point3& getFrontPoint() const { return getPosition(); }
	const Point3& getBackPoint() const { return m_backPoint; }
	bool isToBeDeleted() const { return m_state == TO_BE_DELETED; }
	bool isEnding() const { return m_state == ENDING; }

	void setMaxLength(float l) { m_maxLength = l; }
	void setWidth(float w) { m_width = w; }

	float getPower() const { return m_power; }
	void setPower(float p) { m_power = p; }


	//! Starts this laser at given position
	void start(const Point3& pos, const Vector3& direction);

	//! Laser will shrink at current position
	void die();

	//! Moves laser, updates state
	void update(float dt);

	//! Loads lifetime, maxLength, speed and width
	void loadSettings(const ParserSection& ps);

private:
	enum State {
		STARTING,
		ALIVE,
		ENDING,
		TO_BE_DELETED,
	};

	float m_age;
	float m_lifetime;
	float m_maxLength;
	float m_width;
	float m_speed;
	Vector3 m_direction;
	Point3 m_backPoint;
	State m_state;

	// gameplay data
	float m_power;
};
