#ifndef EVENT_H
#define EVENT_H

class BaseGame;
class ProjectileBase;

enum EventType
{
	DEACTIVATE,
	BULLET_HIT_CHAR,
	CREATE_SMOKE,
	STEER_TOWARDS_CLOSEST_CHAR,
	STOP_MOVING,
	SELF_DESTRUCT,
	NUM_OF_EVENT_TYPES
};

class Event
{
public:
	Event(BaseGame* setGame);
	virtual ~Event(){}

	virtual void Process(ProjectileBase* subject)=0;

	BaseGame*				Game() const;

private:
	BaseGame*				game;
};

class DeactivateEvent : public Event
{
public:
	DeactivateEvent(BaseGame* setGame);
	virtual void Process(ProjectileBase* subject);
};

class BulletHitCharEvent : public Event
{
public:
	BulletHitCharEvent(BaseGame* setGame);
	virtual void Process(ProjectileBase* subject);
};

class CreateSmokeEvent : public Event
{
public:
	CreateSmokeEvent(BaseGame* setGame);
	virtual void Process(ProjectileBase* subject);
};

class SteerTowardsClosestCharEvent : public Event
{
public:
	SteerTowardsClosestCharEvent(BaseGame* setGame);
	virtual void Process(ProjectileBase* subject);
};

class StopMovingEvent : public Event
{
public:
	StopMovingEvent(BaseGame* setGame);
	virtual void Process(ProjectileBase* subject);
};

class SelfDestructEvent : public Event
{
public:
	SelfDestructEvent(BaseGame* setGame);
	virtual void Process(ProjectileBase* subject);
};
#endif // EVENT_H
