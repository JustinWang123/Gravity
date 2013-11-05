#ifndef PLAYER_CHARACTER_H
#define PLAYER_CHARACTER_H

#include "SDLWrapper.h"
#include "ProjectileBase.h"
class GameBase;

typedef Uint32 				Tile;
typedef float 				Pos;

const Uint32 				CHARACTER_WIDTH = 16;
const Uint32 				CHARACTER_HEIGHT = 16;
const float 				CHARACTER_GROUND_ACC = 0.4f;
const float 				CHARACTER_SPACE_ACC = 0.2f;
const float					CHARACTER_JUMP_ACC = 2.0f;
const float 				CHARACTER_SPACE_SPEED = 2.0f;
const float 				CHARACTER_GROUND_SPEED = 4.0f;
const float					CHARACTER_FRICTION = 0.8;
const Uint32 				CHARACTER_ATTACK_DELAY = 10;
const Uint32 				CHARACTER_HEALTH = 100;
const Uint32				AIMMER_DISTANCE = 32;
const float					CHARACTER_RADIUS = 16.0f;
const Uint32				NUM_OF_STARTING_BULLETS = 100;
const Uint32				CHARACTER_ROTATE_SPEED = 10;
const Uint32				CHARACTER_MAKE_SMOKE_TIME = 5;

struct ControlState
{

    ControlState()
    {
        upKeyIsDown = 0;
        downKeyIsDown = 0;
        leftKeyIsDown = 0;
        rightKeyIsDown = 0;
        mouse1IsDown = 0;
        mouse2IsDown = 0;
        mouse3IsDown = 0;
        headingDeg = 0;
    }

    bool operator==(const ControlState& rhs)
    {
        return 	upKeyIsDown == rhs.upKeyIsDown &&
                downKeyIsDown == rhs.downKeyIsDown &&
                leftKeyIsDown == rhs.leftKeyIsDown &&
                rightKeyIsDown == rhs.rightKeyIsDown &&
                mouse1IsDown == rhs.mouse1IsDown &&
                mouse2IsDown == rhs.mouse2IsDown &&
                mouse3IsDown == rhs.mouse3IsDown &&
                headingDeg == rhs.headingDeg;
        // sequenceId does not take part in equality
    }

    bool operator!=(const ControlState& rhs)
    {
        return 	!(upKeyIsDown == rhs.upKeyIsDown &&
                  downKeyIsDown == rhs.downKeyIsDown &&
                  leftKeyIsDown == rhs.leftKeyIsDown &&
                  rightKeyIsDown == rhs.rightKeyIsDown &&
                  mouse1IsDown == rhs.mouse1IsDown &&
                  mouse2IsDown == rhs.mouse2IsDown &&
                  mouse3IsDown == rhs.mouse3IsDown &&
                  headingDeg == rhs.headingDeg);
        // sequenceId does not take part in equality
    }
    bool upKeyIsDown;
    bool downKeyIsDown;
    bool leftKeyIsDown;
    bool rightKeyIsDown;
    bool mouse1IsDown;
    bool mouse2IsDown;
    bool mouse3IsDown;
    Uint16 headingDeg;
    Uint32 time;
    Uint32 length;
};


enum Facing
{
    FACING_RIGHT = 0,
    FACING_LEFT
};

class PlayerCharacter
{
public:
    PlayerCharacter(Uint32 setId, GameBase* setGameBase);
    virtual					~PlayerCharacter();
    void					Spawn(Vector2df atPos);

    virtual void 			Update(float timeDelta);
    void 					Draw(Vector2df camPos) const;

    Uint32                  WriteToPacket(Uint32 dataWritePos, Uint8 data[]);
    Uint32					ReadFromPacket(Uint32 dataReadPos, Uint8 data[]);

    // Modifiers:
    void					SetIsActive(bool b)
    {
        isActive = b;
    }
    void					DecreaseHealth(Uint32 attackerPlayerID, Uint32 amount);
    void					AddHealth(Uint32 amount);
    void					AddBullets(Uint32 amount);
    void					AddMissiles(Uint32 amount);
    void					AddMines(Uint32 amount);
    void					SetRespawnMe(bool b){respawnMe = b;}
    void					SetRespawnTime(Uint32 time)
    {
    	respawnTime = time;
	}
    virtual void			SetControlState(ControlState newState);
    void					AddPoint()
    {
    	score++;
	}

    // Accessors:
    bool					IsActive() const
    {
        return isActive;
    }
    Vector2df				GetPos() const
    {
        return pos;
    }
    Uint32					GetHealth() const
    {
        return health;
    }
    Uint32					GetNumOfBullets() const
    {
        return numOfBullets;
    }
    Uint32					GetNumOfMissiles() const
    {
        return numOfMissiles;
    }
    Uint32					GetNumOfMines() const
    {
        return numOfMines;
    }
    Uint32					GetScore() const
    {
        return score;
    }
    bool					IsSolid(Vector2df atPos);
    bool					IsDead() const
    {
        return health == 0;
    }
    Uint32					GetKillerId() const
    {
        return killerId;
    }
    bool					GetRespawnMe() const
    {
        return respawnMe;
    }
    Uint32					GetRespawnTime() const
    {
        return respawnTime;
    }
    ControlState			GetControlState() const;
    float					GetSpeed() const {return vel.Length();}

protected:
    Vector2df				Heading() const;

    bool					IsReadyToAttack() const
    {
        return attackDelay <= 0;
    }

    // Update subroutines:
    void 					UpdateVelocity(float timeDelta);
    void					UpdateJump(float timeDelta);
    void					UpdatePosition(float timeDelta);
    void					UpdateCollision(float timeDelta);
    void					UpdateAcceleration(float timeDelta);
    void					UpdateSpeed(float timeDelta);
    void					UpdateFriction(float timeDelta);
    void					UpdateAttack(float timeDelta);
    void					UpdateSmokeTrail(float timeDelta);
    void					Attack(ProjectileType type);

    ControlState 			controlState;

    // Movement data members:
    Vector2df				pos;
    Vector2df				vel;
    Vector2df				acc;
    float					speed;

    // Movement state:
    bool					collisionAbove;
    bool					collisionBelow;
    bool					collisionLeft;
    bool					collisionRight;
    bool					isOnGround;

    // Game data members:
    Uint32 					health;
    Uint32					numOfBullets;
    Uint32					numOfMissiles;
    Uint32					numOfMines;
    Uint32					score;
    float 					attackDelay;
    Uint32					id;
    bool					isActive;
    Uint32					killerId;
    float					makeSmokeTime;
    Uint32					respawnTime;
    bool					respawnMe;

    // Graphics data members:
    SDL_Surface* 			surfaceLeftIdle;
    SDL_Surface* 			surfaceRightIdle;
    SDL_Surface* 			surfaceLeftRun;
    SDL_Surface* 			surfaceRightRun;
    SDL_Surface*			surfaceHeading;
    SDL_Surface*			surfaceDead;

    // The game that the character belongs to:
    GameBase* 				game;
};

#endif
