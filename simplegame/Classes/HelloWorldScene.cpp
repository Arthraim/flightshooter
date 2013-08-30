#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"

using namespace cocos2d;
using namespace CocosDenshion;

#define HERO_RECT       CCRectMake(0, 99, 102, 126)
#define ENEMY1_RECT     CCRectMake(534, 612, 57, 43)
#define BULLET_RECT     CCRectMake(1004, 987, 9, 21)

CCScene* HelloWorld::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    HelloWorld *layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !CCLayerColor::initWithColor( ccc4(255,255,255,255) ) )
    {
        return false;
    }
    
    // enable touch
    this->setTouchEnabled(true); //this->setIsTouchEnabled(true);

    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    this->player = CCSprite::create("shoot.png", HERO_RECT );
    this->player->setPosition( ccp(winSize.width/2, this->player->getContentSize().height/2) );
    this->addChild(this->player);
    
    this->schedule( schedule_selector(HelloWorld::addEnemies), 1.0 );
    this->schedule( schedule_selector(HelloWorld::fireBullets), 1.0/7.0 );
    
    return true;
}

void HelloWorld::menuCloseCallback(CCObject* pSender)
{
    CCDirector::sharedDirector()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

//
// enemy
//

void HelloWorld::addEnemies(float dt)
{
    this->addEnemy();
}

void HelloWorld::addEnemy()
{
    CCSprite *target = CCSprite::create("shoot.png", ENEMY1_RECT);
    
    // Determine where to spawn the target along the X axis
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    int minX = target->getContentSize().width/2;
    int maxX = winSize.width - target->getContentSize().width/2;
    int rangeX = maxX - minX;
    // srand( TimGetTicks() );
    int actualX = ( rand() % rangeX ) + minX;
    
    // Create the target slightly off-screen along the top edge,
    // and along a random position along the X axis as calculated
    target->setPosition( ccp(actualX, winSize.height + target->getContentSize().height/2) );
    this->addChild(target);
    
    // Determine speed of the target
    int minDuration = (int)3.0;
    int maxDuration = (int)6.0;
    int rangeDuration = maxDuration - minDuration;
    // srand( TimGetTicks() );
    int actualDuration = ( rand() % rangeDuration ) + minDuration;
    
    // Create the actions
    CCFiniteTimeAction* actionMove = CCMoveTo::create((float)actualDuration,
                                                      ccp( actualX, 0 - target->getContentSize().height/2) );
    CCFiniteTimeAction* actionMoveDone = CCCallFuncN::create(this,
                                                             callfuncN_selector(HelloWorld::enemyMoveFinished));
    target->runAction( CCSequence::create(actionMove, actionMoveDone, NULL) );
}

void HelloWorld::enemyMoveFinished(CCNode* sender)
{
    CCSprite *sprite = (CCSprite *)sender;
    this->removeChild(sprite, true);
}

//
// bullets
//

void HelloWorld::fireBullets(float dt)
{
    this->fireBullet();
}

void HelloWorld::fireBullet()
{
    CCSprite *bullet = CCSprite::create("shoot.png", BULLET_RECT);
    
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();

    CCPoint playerLocation = player->getPosition();
    bullet->setPosition(CCPoint(playerLocation.x, playerLocation.y + player->getContentSize().height/2));
    this->addChild(bullet);
    
    // Create the actions
    CCFiniteTimeAction* actionMove = CCMoveTo::create(1.0f,
                                                      ccp( bullet->getPosition().x, bullet->getPosition().y + winSize.height*3/4) );
    CCFiniteTimeAction* actionMoveDone = CCCallFuncN::create(this,
                                                             callfuncN_selector(HelloWorld::bulletMoveFinished));
    bullet->runAction( CCSequence::create(actionMove, actionMoveDone, NULL) );
}

void HelloWorld::bulletMoveFinished(CCNode* sender)
{
    CCSprite *sprite = (CCSprite *)sender;
    this->removeChild(sprite, true);
}

//
// hero
//

void HelloWorld::ccTouchesBegan(CCSet *touches, CCEvent *pEvent)
{
    CCTouch* touch = (CCTouch*)( touches->anyObject() );
    CCPoint location = touch->getLocationInView();
    location = CCDirector::sharedDirector()->convertToGL(location);
    CCPoint playerLocation = player->getPosition();
    
    this->touchOffset = CCPoint(playerLocation.x - location.x,
                                playerLocation.y - location.y);
}


void HelloWorld::ccTouchesMoved(CCSet *touches, CCEvent* event)
{
    CCTouch* touch = (CCTouch*)( touches->anyObject() );
    CCPoint location = touch->getLocationInView();
    location = CCDirector::sharedDirector()->convertToGL(location);

    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    CCSize playerSize = player->getContentSize();
    
    float newX = location.x + this->touchOffset.x;
    newX = newX < playerSize.width/2 ? playerSize.width/2 : newX;
    newX = newX > winSize.width - playerSize.width/2 ? winSize.width - playerSize.width/2 : newX;

    float newY = location.y + this->touchOffset.y;
    newY = newY < playerSize.height/2 ? playerSize.height/2 : newY;
    newY = newY > winSize.height - playerSize.height/2 ? winSize.height - playerSize.height/2 : newY;

    this->player->setPosition(CCPoint(newX, newY));
}

//void HelloWorld::ccTouchesEnded(CCSet* touches, CCEvent* event)
//{
//    // Choose one of the touches to work with
//    CCTouch* touch = (CCTouch*)( touches->anyObject() );
//    CCPoint location = touch->getLocationInView();
//    location = CCDirector::sharedDirector()->convertToGL(location);
//    
//    // Set up initial location of projectile
//    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
//    CCSprite *projectile = CCSprite::create("shoot.png", BULLET_RECT);
//    projectile->setPosition( ccp(20, winSize.height/2) );
//    
//    // Determinie offset of location to projectile
//    int offX = location.x - projectile->getPosition().x;
//    int offY = location.y - projectile->getPosition().y;
//    
//    // Bail out if we are shooting down or backwards
//    if (offX <= 0) return;
//    
//    // Ok to add now - we've double checked position
//    this->addChild(projectile);
//    
//    // Determine where we wish to shoot the projectile to
//    int realX = winSize.width
//    + (projectile->getContentSize().width/2);
//    float ratio = (float)offY / (float)offX;
//    int realY = (realX * ratio) + projectile->getPosition().y;
//    CCPoint realDest = ccp(realX, realY);
//    
//    // Determine the length of how far we're shooting
//    int offRealX = realX - projectile->getPosition().x;
//    int offRealY = realY - projectile->getPosition().y;
//    float length = sqrtf((offRealX * offRealX)
//                         + (offRealY*offRealY));
//    float velocity = 480/1; // 480pixels/1sec
//    float realMoveDuration = length/velocity;
//    
//    // Move projectile to actual endpoint
//    projectile->runAction( CCSequence::create(
//                                              CCMoveTo::create(realMoveDuration, realDest),
//                                              CCCallFuncN::create(this,
//                                                                  
//                                                                  callfuncN_selector(HelloWorld::spriteMoveFinished)), 
//                                              NULL) );
//}

