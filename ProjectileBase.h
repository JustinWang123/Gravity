#ifndef PROJECTILE_BASE_H
#define PROJECTILE_BASE_H

#include "SDLWrapper.h"
#include "Vector2df.h"
#include "EntityBase.h"
class Event;
class BaseGame;

typedef float 				Pos;

const Uint32 				BULLET_HEIGHT = 2;
const Uint32 				BULLET_WIDTH = 2;
const float 				BULLET_SPEED = 5.0f;
const Uint32				BULLET_DAMAGE = 10;
const float					BULLET_LIFE = 200;
const float					BULLET_HEALTH = 100;

const Uint32 				EXPLOSION_PARTICLE_HEIGHT = 16;
const Uint32 				EXPLOSION_PARTICLE_WIDTH = 16;
const float 				EXPLOSION_PARTICLE_SPEED = 2.0f;
const Uint32				EXPLOSION_PARTICLE_DAMAGE = 10;
const Uint32				EXPLOSION_PARTICLE_LIFE = 20;
const Uint32				EXPLOSION_PARTICLE_HEALTH = 100;

const float 				MINE_SPEED = 2.0f;
const float 				MINE_TURN_SPEED = 5.0f;
const Uint32 				MINE_LIFE = 500;
const Uint32				MINE_ACTIVATION_TIME = 300;
const Uint32 				MINE_HEALTH = 30;
const Uint32 				MINE_WIDTH = 16;
const Uint32 				MINE_HEIGHT = 16;
const Uint32 				MINE_RADIUS = 11;
const Uint32 				MINE_HOMING_RANGE = 200;

const float 				MISSILE_SPEED = 2.0f;
const float 				MISSILE_TURN_SPEED = 5.0f;
const Uint32 				MISSILE_LIFE = 200;
const Uint32 				MISSILE_HEALTH = 30;
const Uint32 				MISSILE_WIDTH = 16;
const Uint32 				MISSILE_HEIGHT = 16;
const Uint32 				MISSILE_RADIUS = 5;

const Uint32 				SMOKE_PARTICLE_HEIGHT = 16;
const Uint32 				SMOKE_PARTICLE_WIDTH = 16;
const float 				SMOKE_PARTICLE_SPEED = 0.1f;
const Uint32				SMOKE_PARTICLE_LIFE = 25;
const Uint32				SMOKE_PARTICLE_HEALTH = 100;

enum ProjectileType
{
    PROJECTILE_TYPE_BULLET,
    PROJECTILE_TYPE_EXPLOSION,
    PROJECTILE_TYPE_EXPLOSION_PARTICLE,
    PROJECTILE_TYPE_MISSILE,
    PROJECTILE_TYPE_MINE,
    PROJECTILE_TYPE_SMOKE
};

class ProjectileBase : public EntityBase
{
public:
    ProjectileBase(Uint32 setId, BaseGame* setGame);
    virtual                 ~ProjectileBase() {}

	virtual void			Update(float timeDelta);
	virtual void			Draw(Vector2df camPos) const;


	void					SetState(Uint32 setOwnerPlayerId, Vector2df setPos, Vector2df setHeading, float setSpeed, float setLife, Uint32 setHealth, bool isMoving);
	void					SetType(ProjectileType setType, float setScale, SDL_Surface* setSurface);

	// Packet:
	Uint32					WriteToPacket(Uint32 dataWritePos, Uint8 data[]);

    // Modifiers:
    void					SetType(ProjectileType setType);
    void					SetOwnerPlayerId(Uint32 id);
    void					SetLife(float setLife);
    void					DecreaseLife(float amount);
    void					SetSpeed(float setSpeed);
	void					SetHeading(Uint32 d);
    void					SetHeading(Vector2df heading);
    void					SetHealth(Uint32 setHealth);
	void					DecreaseHealth(Uint32 amount);
	void					SetScale(float setScale);
	void					SetIsMoving(bool b);

	// Event Modifiers:
	void					SetHitSolidEvent(Event* setEvent);
	void					SetHitCharEvent(Event* setEvent);
	void					SetOutOfLifeEvent(Event* setEvent);
	void					SetOutOfHealthEvent(Event* setEvent);
	void					SetUpdateEvent(Event* setEvent);

    // Accessors:
    ProjectileType			Type() const;
    Uint32					OwnerPlayerId() const;
    float					Life() const;
    float					Speed() const;
	Uint32					HeadingDeg() const;
    Vector2df				Heading() const;
    Uint32					Health() const;
	float					Scale() const;
	SDL_Surface*			Surface() const;
	bool					IsSolid(Vector2df atPos) const;
	Vector2df				HitPos() const;
	bool					IsMoving() const;

private:
	Event*					hitSolidEvent;
	Event*					hitCharEvent;
	Event*					outOfLifeEvent;
	Event*					outOfHealthEvent;
	Event*					updateEvent;



	ProjectileType			type;
    Uint32					headingDeg;
    Uint32					ownerPlayerId;
	float					life;
	float					speed;
	Uint32					health;
	float					scale;
	Uint32					radius;
	SDL_Surface* 			surface;
	Vector2df				hitPos;
	bool					isMoving;

};

#endif

