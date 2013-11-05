#include "Event.h"
#include "ProjectileBase.h"
#include "GameBase.h"

// Event:
// ------------------------------------------------------------------------------------------------
Event :: Event(GameBase* setGame)
{
	game = setGame;
}

GameBase* Event :: Game() const
{
	return game;
}

// DoNothingEvent:
// ------------------------------------------------------------------------------------------------
DoNothingEvent :: DoNothingEvent(GameBase* setGame)
	:	Event(setGame)
{
}

void DoNothingEvent :: Process(ProjectileBase* subject)
{
	// Do nothing
}

// DeactivateEvent:
// ------------------------------------------------------------------------------------------------
DeactivateEvent :: DeactivateEvent(GameBase* setGame)
	:	Event(setGame)
{
}

void DeactivateEvent :: Process(ProjectileBase* subject)
{
	subject->Deactivate();
}

// BulletHitCharEvent:
// ------------------------------------------------------------------------------------------------
BulletHitCharEvent :: BulletHitCharEvent(GameBase* setGame)
	:	Event(setGame)
{
}

void BulletHitCharEvent :: Process(ProjectileBase* subject)
{
	subject->Deactivate();
	Game()->DamageCharsAtPos(subject->OwnerPlayerId(), BULLET_DAMAGE, subject->HitPos());
}

// CreateSmokeEvent:
// ------------------------------------------------------------------------------------------------
CreateSmokeEvent :: CreateSmokeEvent(GameBase* setGame)
	:	Event(setGame)
{
}

void CreateSmokeEvent :: Process(ProjectileBase* subject)
{
	if((int)subject->Life() % 5 == 0)
	{
		Game()->CreateProjectile(PROJECTILE_TYPE_SMOKE, subject->OwnerPlayerId(), subject->Pos(), subject->Heading());
	}
}

// SteerTowardsClosestCharEvent:
// ------------------------------------------------------------------------------------------------
SteerTowardsClosestCharEvent :: SteerTowardsClosestCharEvent(GameBase* setGame)
	:	Event(setGame)
{
}

void SteerTowardsClosestCharEvent :: Process(ProjectileBase* subject)
{
	if(subject->Life() < MINE_ACTIVATION_TIME)
	{
		Vector2df closestCharPos = Game()->GetClosestCharPos(subject->Pos());
		subject->SetHeading(closestCharPos - subject->Pos());
		subject->SetIsMoving(true);
	}
}

// StopMovingEvent:
// ------------------------------------------------------------------------------------------------
StopMovingEvent :: StopMovingEvent(GameBase* setGame)
	:	Event(setGame)
{
}

void StopMovingEvent :: Process(ProjectileBase* subject)
{
	subject->SetIsMoving(false);
}

// SelfDestructEvent:
// ------------------------------------------------------------------------------------------------
SelfDestructEvent :: SelfDestructEvent(GameBase* setGame)
	:	Event(setGame)
{
}

void SelfDestructEvent :: Process(ProjectileBase* subject)
{
	Game()->CreateExplosion(subject->OwnerPlayerId(), subject->Pos());
	subject->Deactivate();
}
