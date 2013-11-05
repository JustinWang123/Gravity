#ifndef EVENT_H
#define EVENT_H

class GameBase;
class ProjectileBase;

enum EventType
{
	DO_NOTHING,
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
	Event(GameBase* setGame);
	virtual ~Event(){}

	virtual void Process(ProjectileBase* subject)=0;

	GameBase*				Game() const;

private:
	GameBase*				game;
};

class DoNothingEvent : public Event
{
public:
	DoNothingEvent(GameBase* setGame);
	virtual void Process(ProjectileBase* subject);
};

class DeactivateEvent : public Event
{
public:
	DeactivateEvent(GameBase* setGame);
	virtual void Process(ProjectileBase* subject);
};

class BulletHitCharEvent : public Event
{
public:
	BulletHitCharEvent(GameBase* setGame);
	virtual void Process(ProjectileBase* subject);
};

class CreateSmokeEvent : public Event
{
public:
	CreateSmokeEvent(GameBase* setGame);
	virtual void Process(ProjectileBase* subject);
};

class SteerTowardsClosestCharEvent : public Event
{
public:
	SteerTowardsClosestCharEvent(GameBase* setGame);
	virtual void Process(ProjectileBase* subject);
};

class StopMovingEvent : public Event
{
public:
	StopMovingEvent(GameBase* setGame);
	virtual void Process(ProjectileBase* subject);
};

class SelfDestructEvent : public Event
{
public:
	SelfDestructEvent(GameBase* setGame);
	virtual void Process(ProjectileBase* subject);
};
#endif // EVENT_H
