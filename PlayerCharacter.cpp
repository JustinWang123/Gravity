#include "PlayerCharacter.h"
#include "GameBase.h"
#include "PacketProtocol.h"
#include <math.h>
#include <assert.h>



// ------------------------------------------------------------------------------------------------
PlayerCharacter :: PlayerCharacter(Uint32 setId, GameBase* setGameBase)
    :	attackDelay(0),
        id(setId),
        isActive(false),
        game(setGameBase)
{

    health = CHARACTER_HEALTH;
    numOfBullets = NUM_OF_STARTING_BULLETS;
    numOfMissiles = 0;
    numOfMines = 0;
    score = 0;
    surfaceLeftIdle = LoadSurface("Surfaces/Marine_Left.bmp");
    surfaceRightIdle = LoadSurface("Surfaces/Marine_Right.bmp");
    surfaceLeftRun = LoadSurface("Surfaces/Marine_Left_Run.bmp");
    surfaceRightRun = LoadSurface("Surfaces/Marine_Right_Run.bmp");
    surfaceHeading = LoadSurface("Surfaces/Bullet.bmp");
    surfaceDead = LoadSurface("Surfaces/Marine_Dead.bmp");
    respawnMe = false;
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
PlayerCharacter :: ~PlayerCharacter()
{
    FreeSurface(surfaceLeftIdle);
    FreeSurface(surfaceRightIdle);
    FreeSurface(surfaceLeftRun);
    FreeSurface(surfaceRightRun);
    FreeSurface(surfaceHeading);
    FreeSurface(surfaceDead);
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void PlayerCharacter :: Spawn(Vector2df atPos)
{
    isActive = true;
    pos = atPos;
    health = CHARACTER_HEALTH;
    attackDelay = 0;
    vel.Set(0,0);
    acc.Set(0,0);
    numOfBullets = NUM_OF_STARTING_BULLETS;
    numOfMissiles = 20;
    numOfMines = 20;
    makeSmokeTime = 0;
    respawnMe = false;
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void PlayerCharacter :: Update(float timeDelta)
{
    if(isActive && !IsDead())
    {
        UpdateVelocity(timeDelta);
        UpdateJump(timeDelta);
        UpdatePosition(timeDelta);
        UpdateCollision(timeDelta);
        UpdateAcceleration(timeDelta);
        UpdateSpeed(timeDelta);
        UpdateFriction(timeDelta);
        UpdateAttack(timeDelta);

    }
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void PlayerCharacter :: Draw(Vector2df camPos) const
{
    Vector2df charScreenPos = pos - camPos;

    if(!IsDead())
    {
        Facing facing;
        SDL_Surface* playerSurface = 0;
        int angle ;

        if(isOnGround)
        {
            if(collisionAbove)
            {
                angle = 180;
            }
            else if(collisionBelow)
            {
                angle = 0;
            }
            else if(collisionLeft)
            {
                angle = 270;
            }
            else
            {
                angle = 90;
            }
        }
        else
        {
            angle = vel.Deg();
        }

        if(angle < 45 || angle >= 315)
        {
            if(vel.x < 0)
            {
                facing = FACING_LEFT;
            }
            else
            {
                facing = FACING_RIGHT;
            }
        }
        else if(angle >= 45 && angle < 135)
        {
            if(vel.y < 0)
            {
                facing = FACING_RIGHT;
            }
            else
            {
                facing = FACING_LEFT;
            }
        }
        else if(angle >= 135 && angle < 225)
        {
            if(vel.x < 0)
            {
                facing = FACING_RIGHT;
            }
            else
            {
                facing = FACING_LEFT;
            }
        }
        else
        {
            if(vel.y < 0)
            {
                facing = FACING_LEFT;
            }
            else
            {
                facing = FACING_RIGHT;
            }
        }


        // If the player is moving then we use the running sprite:
        if(keysDown[SDLK_w] || keysDown[SDLK_s] || keysDown[SDLK_a] || keysDown[SDLK_d])
        {
            if(facing == FACING_RIGHT)
            {
                playerSurface = surfaceRightRun;
            }
            else
            {
                playerSurface = surfaceLeftRun;
            }
        }
        // Otherwise we use the idle sprite:
        else
        {
            if(facing == FACING_LEFT)
            {
                playerSurface = surfaceRightIdle;
            }
            else
            {
                playerSurface = surfaceLeftIdle;
            }
        }

        // Draw charcter alive sprite:
        DrawSurface(
            charScreenPos.x - (CHARACTER_WIDTH / 2),
            charScreenPos.y - (CHARACTER_HEIGHT / 2),
            rotozoomSurface(playerSurface, angle, 1, 0));

        // Draw aimmer:
        DrawSurface(
            charScreenPos.x + (Heading().x * AIMMER_DISTANCE) - 1,
            charScreenPos.y + (Heading().y * AIMMER_DISTANCE) - 1,
            surfaceHeading);

        // Draw health:
        DrawText(charScreenPos.x - 10, charScreenPos.y - 20, ToString(health));
    }
    else
    {
        // Draw dead sprite:
        DrawSurface(charScreenPos.x - (CHARACTER_WIDTH / 2), charScreenPos.y - (CHARACTER_HEIGHT / 2), surfaceDead);
    }
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void PlayerCharacter :: UpdateVelocity(float timeDelta)
{
    // Apply acceleration to velocity based on key states:
    if(controlState.upKeyIsDown)
    {
        vel.y += -acc.y * timeDelta;
    }
    else if(controlState.downKeyIsDown)
    {
        vel.y += acc.y * timeDelta;
    }

    if(controlState.leftKeyIsDown)
    {
        vel.x += -acc.x * timeDelta;
    }
    else if(controlState.rightKeyIsDown)
    {
        vel.x += acc.x * timeDelta;
    }

    // Add effect of gravity acceleration:
    vel += game->GetPosGravity(pos) * timeDelta;

    // Cap the velocity at the maximum speed:
    if(vel.Length()  > speed)
    {
        vel = vel.Normal() * speed;
    }
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void PlayerCharacter :: UpdateJump(float timeDelta)
{
    if(collisionAbove && controlState.downKeyIsDown)
    {
        vel.y += CHARACTER_JUMP_ACC * timeDelta;
    }

    if(collisionBelow && controlState.upKeyIsDown)
    {
        vel.y -= CHARACTER_JUMP_ACC * timeDelta;
    }

    if(collisionLeft && controlState.rightKeyIsDown)
    {
        vel.x += CHARACTER_JUMP_ACC * timeDelta;
    }

    if(collisionRight && controlState.leftKeyIsDown)
    {
        vel.x -= CHARACTER_JUMP_ACC * timeDelta;
    }
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void PlayerCharacter :: UpdatePosition(float timeDelta)
{
    float charTop = pos.y - CHARACTER_HEIGHT / 2;
    float charBottem = pos.y + CHARACTER_HEIGHT / 2;
    float charLeft = pos.x - CHARACTER_WIDTH / 2;
    float charRight = pos.x + CHARACTER_WIDTH / 2;

    // Handle velocity to the right:
    if(vel.x > 0)
    {
        if(!game->CheckCollisionWithLevel( Vector2df(charRight + vel.x * timeDelta, charTop + 2))
                && !game->CheckCollisionWithLevel( Vector2df(charRight + vel.x * timeDelta, charBottem - 2)))
        {
            pos.x += vel.x * timeDelta;
        }
        else
        {
            vel.x = 0;
        }
    }
    // Handle velocity to the left:
    else if(vel.x < 0)
    {
        if(!game->CheckCollisionWithLevel(Vector2df(charLeft + vel.x * timeDelta, charTop + 2))
                && !game->CheckCollisionWithLevel(Vector2df(charLeft + vel.x * timeDelta, charBottem - 2)))
        {
            pos.x += vel.x * timeDelta;
        }
        else
        {
            vel.x = 0;
        }
    }

    // Handle velocity down:
    if(vel.y > 0)
    {
        if(!game->CheckCollisionWithLevel(Vector2df(charRight - 2, charBottem + vel.y * timeDelta))
                && !game->CheckCollisionWithLevel(Vector2df(charLeft + 2, charBottem + vel.y * timeDelta)))
        {
            pos.y += vel.y * timeDelta;
        }
        else
        {
            vel.y = 0;
        }
    }
    // Handle velocity up
    else if(vel.y < 0)
    {
        if(!game->CheckCollisionWithLevel(Vector2df(charRight - 2, charTop + vel.y * timeDelta))
                && !game->CheckCollisionWithLevel(Vector2df(charLeft + 2, charTop + vel.y * timeDelta)))
        {
            pos.y += vel.y;
        }
        else
        {
            vel.y = 0;
        }
    }
} // ----------------------------------------------------------------------------------------------



// ------------------------------------------------------------------------------------------------
void PlayerCharacter :: UpdateCollision(float timeDelta)
{
    float charTop = pos.y - CHARACTER_HEIGHT / 2;
    float charBottem = pos.y + CHARACTER_HEIGHT / 2;
    float charLeft = pos.x - CHARACTER_WIDTH / 2;
    float charRight = pos.x + CHARACTER_WIDTH / 2;

    // check collision above:
    if(game->CheckCollisionWithLevel( Vector2df(charLeft + 2, charTop - 2))
            || game->CheckCollisionWithLevel( Vector2df(charRight - 2, charTop - 2)))
    {
        collisionAbove = true;
    }
    else
    {
        collisionAbove = false;
    }

    // check  collision below:
    if(game->CheckCollisionWithLevel( Vector2df(charLeft + 2, charBottem + 2))
            || game->CheckCollisionWithLevel( Vector2df(charRight - 2, charBottem + 2)))
    {
        collisionBelow = true;
    }
    else
    {
        collisionBelow = false;
    }

    // check collision to left:
    if(game->CheckCollisionWithLevel( Vector2df(charLeft - 2, charBottem - 2))
            || game->CheckCollisionWithLevel( Vector2df(charLeft - 2, charTop + 2)))
    {
        collisionLeft = true;
    }
    else
    {
        collisionLeft = false;
    }

    // check collision to right:
    if(game->CheckCollisionWithLevel( Vector2df(charRight + 2, charBottem - 2))
            || game->CheckCollisionWithLevel( Vector2df(charRight + 2, charTop + 2)))
    {
        collisionRight = true;
    }
    else
    {
        collisionRight = false;
    }

    if(collisionAbove || collisionBelow || collisionLeft || collisionRight)
    {
        isOnGround = true;
    }
    else
    {
        isOnGround = false;
    }
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void PlayerCharacter :: UpdateAcceleration(float timeDelta)
{
    if(collisionAbove || collisionBelow)
    {
        acc.x = CHARACTER_GROUND_ACC;
    }
    else
    {
        acc.x = CHARACTER_SPACE_ACC;
    }

    if(collisionLeft || collisionRight)
    {
        acc.y = CHARACTER_GROUND_ACC;
    }
    else
    {
        acc.y = CHARACTER_SPACE_ACC;
    }
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void PlayerCharacter :: UpdateSpeed(float timeDelta)
{
    if(isOnGround)
    {
        speed = CHARACTER_GROUND_SPEED;
    }
    else
    {
        speed = CHARACTER_SPACE_SPEED;
    }
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void PlayerCharacter :: UpdateFriction(float timeDelta)
{
    if( (collisionAbove || collisionBelow) && !game->IsPosGravityWell(pos) )
    {
        vel.x *= powf(CHARACTER_FRICTION, timeDelta);
    }

    if( (collisionLeft || collisionRight) && !game->IsPosGravityWell(pos) )
    {
        vel.y *= powf(CHARACTER_FRICTION, timeDelta);
    }
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void PlayerCharacter :: UpdateAttack(float timeDelta)
{
    if(controlState.mouse1IsDown && IsReadyToAttack())
    {
        Attack(PROJECTILE_TYPE_BULLET);
    }

    if(controlState.mouse3IsDown && IsReadyToAttack())
    {
        Attack(PROJECTILE_TYPE_MISSILE);
    }

    if(controlState.mouse2IsDown && IsReadyToAttack())
    {
        Attack(PROJECTILE_TYPE_MINE);
    }

    if(attackDelay > 0)
    {
        attackDelay -= timeDelta;
    }
} // ----------------------------------------------------------------------------------------------



// ------------------------------------------------------------------------------------------------
void PlayerCharacter :: UpdateSmokeTrail(float timeDelta)
{
    // Create smoke trail:
    if(keysDown[SDLK_w] || keysDown[SDLK_s] || keysDown[SDLK_a] || keysDown[SDLK_d])
    {
        if(makeSmokeTime <= 0)
        {
            game->CreateProjectile(PROJECTILE_TYPE_SMOKE, id, pos - vel.Normal() * 4.0f, Vector2df(controlState.headingDeg - 180));
            makeSmokeTime = CHARACTER_MAKE_SMOKE_TIME;
        }
    }
    makeSmokeTime -= timeDelta;
} // ----------------------------------------------------------------------------------------------



// ------------------------------------------------------------------------------------------------
Vector2df PlayerCharacter :: Heading() const
{
    return Vector2df(controlState.headingDeg);
} // ----------------------------------------------------------------------------------------------





// ------------------------------------------------------------------------------------------------
void PlayerCharacter :: DecreaseHealth(Uint32 attackerId, Uint32 amount)
{
    assert(attackerId < MAX_PLAYERS && "PlayerCharacter::DecreaseHealth attackerId < MAX_PLAYERS");

    if(amount >= health)
    {
        health = 0;
        killerId = attackerId;
    }
    else
    {
        health -= amount;
    }
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
bool PlayerCharacter :: IsSolid(Vector2df atPos)
{
    return 	atPos.x > (Uint32)pos.x - CHARACTER_WIDTH / 2
            &&		atPos.x < (Uint32)pos.x + CHARACTER_WIDTH / 2
            &&		atPos.y > (Uint32)pos.y - CHARACTER_HEIGHT / 2
            &&		atPos.y < (Uint32)pos.y + CHARACTER_HEIGHT / 2;
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void PlayerCharacter :: Attack(ProjectileType type)
{
    if(IsReadyToAttack())
    {
        if(type == PROJECTILE_TYPE_BULLET && numOfBullets > 0)
        {
            game->CreateProjectile(PROJECTILE_TYPE_BULLET, id, pos + Heading() * CHARACTER_RADIUS, Heading());
            numOfBullets--;
            attackDelay = CHARACTER_ATTACK_DELAY;
        }
        else if(type == PROJECTILE_TYPE_MISSILE && numOfMissiles > 0)
        {
            game->CreateProjectile(PROJECTILE_TYPE_MISSILE, id, pos + Heading() * CHARACTER_RADIUS, Heading());
            numOfMissiles--;
            attackDelay = CHARACTER_ATTACK_DELAY * 3.0f;
        }
        else if(type == PROJECTILE_TYPE_MINE && numOfMines > 0)
        {
            game->CreateProjectile(PROJECTILE_TYPE_MINE, id, pos + Heading() * CHARACTER_RADIUS,  Heading());
            numOfMines--;
            attackDelay = CHARACTER_ATTACK_DELAY * 3.0f;
        }
    }
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void PlayerCharacter :: AddHealth(Uint32 amount)
{
    health += amount;

    if(health > CHARACTER_HEALTH)
    {
        health = CHARACTER_HEALTH;
    }
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void PlayerCharacter :: AddBullets(Uint32 amount)
{
    numOfBullets += amount;
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void PlayerCharacter :: AddMissiles(Uint32 amount)
{
    numOfMissiles += amount;
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void PlayerCharacter :: AddMines(Uint32 amount)
{
    numOfMines += amount;
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
Uint32 PlayerCharacter :: WriteToPacket(Uint32 dataWritePos, Uint8 data[])
{
    memcpy(&data[dataWritePos + PACKET_WRITE_PLAYER_ID], &id,4 );
    memcpy(&data[dataWritePos + PACKET_WRITE_PLAYER_POSX], &pos.x, 4);
    memcpy(&data[dataWritePos + PACKET_WRITE_PLAYER_POSY], &pos.y, 4);
    memcpy(&data[dataWritePos + PACKET_WRITE_PLAYER_VELX], &vel.x, 4);
    memcpy(&data[dataWritePos + PACKET_WRITE_PLAYER_VELY], &vel.y, 4);
    memcpy(&data[dataWritePos + PACKET_WRITE_PLAYER_HEALTH], &health, 4);
    memcpy(&data[dataWritePos + PACKET_WRITE_PLAYER_NUM_BULLETS], &numOfBullets, 4);
    memcpy(&data[dataWritePos + PACKET_WRITE_PLAYER_NUM_MISSILE], &numOfMissiles, 4);
    memcpy(&data[dataWritePos + PACKET_WRITE_PLAYER_NUM_MINES], &numOfMines, 4);
    memcpy(&data[dataWritePos + PACKET_WRITE_PLAYER_SCORE], &score, 4);
    memcpy(&data[dataWritePos + PACKET_WRITE_PLAYER_ATTACK_DELAY], &attackDelay, 4);
    memcpy(&data[dataWritePos + PACKET_WRITE_PLAYER_CONTROL_STATE], &controlState, sizeof(ControlState));

    return PACKET_WRITE_PLAYER_LENGTH;
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
Uint32 PlayerCharacter :: ReadFromPacket(Uint32 dataReadPos, Uint8 data[])
{
    ControlState setControlState;

    memcpy(&pos.x, &data[dataReadPos + PACKET_WRITE_PLAYER_POSX], 4);
    memcpy(&pos.y, &data[dataReadPos + PACKET_WRITE_PLAYER_POSY], 4);
    memcpy(&vel.x, &data[dataReadPos + PACKET_WRITE_PLAYER_VELX], 4);
    memcpy(&vel.y, &data[dataReadPos + PACKET_WRITE_PLAYER_VELY], 4);
    memcpy(&health, &data[dataReadPos + PACKET_WRITE_PLAYER_HEALTH], 4);
    memcpy(&numOfBullets, &data[dataReadPos + PACKET_WRITE_PLAYER_NUM_BULLETS], 4);
    memcpy(&numOfMissiles, &data[dataReadPos + PACKET_WRITE_PLAYER_NUM_MISSILE], 4);
    memcpy(&numOfMines, &data[dataReadPos + PACKET_WRITE_PLAYER_NUM_MINES], 4);
    memcpy(&score, &data[dataReadPos + PACKET_WRITE_PLAYER_SCORE], 4);
	memcpy(&attackDelay, &data[dataReadPos + PACKET_WRITE_PLAYER_ATTACK_DELAY], 4);
    memcpy(&setControlState, &data[dataReadPos + PACKET_WRITE_PLAYER_CONTROL_STATE], sizeof(ControlState));

    SetControlState(setControlState);
    isActive = true;
    return PACKET_WRITE_PLAYER_LENGTH;

    /*
    if(playerID == myChannel) {
    	// Apply the remaining commands to the character:
    	while(commands.size() > 0) {
    		myPc->SetCommands(commands.front()->data);
    		myPc->UpdateVelocity();
    		myPc->UpdatePosition();

    		delete commands.front();
    		commands.pop_front();
    	}
    }*/
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void PlayerCharacter :: SetControlState(ControlState newState)
{
    controlState = newState;
} // ----------------------------------------------------------------------------------------------



// ------------------------------------------------------------------------------------------------
ControlState PlayerCharacter :: GetControlState() const
{
    return controlState;
} // ----------------------------------------------------------------------------------------------


