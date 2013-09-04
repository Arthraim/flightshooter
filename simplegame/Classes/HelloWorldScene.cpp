#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include "Enemy.h"

using namespace cocos2d;
using namespace CocosDenshion;

#define HERO1_RECT       CCRectMake(0, 99, 102, 126)
#define HERO2_RECT       CCRectMake(165, 360, 102, 126)
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
    this->player = CCSprite::create("shoot.png", HERO1_RECT );
    this->player->setPosition( ccp(winSize.width/2, this->player->getContentSize().height/2) );
    this->addChild(this->player);
    
    this->playerAnimation = CCAnimation::create();
    CCSpriteFrame *sframe = CCSpriteFrame::create("shoot.png", HERO2_RECT);
    this->playerAnimation->addSpriteFrame(sframe);
    CCSpriteFrame *sframe2 = CCSpriteFrame::create("shoot.png", HERO1_RECT);
    this->playerAnimation->addSpriteFrame(sframe2);
    this->playerAnimation->setDelayPerUnit(0.1);
    CCAnimate *animate = CCAnimate::create(this->playerAnimation);
    player->runAction(CCRepeatForever::create(animate));
    
    this->schedule( schedule_selector(HelloWorld::addEnemies), 1.0 );
    this->schedule( schedule_selector(HelloWorld::fireBullets), 1.0/7.0 );
    this->schedule( schedule_selector(HelloWorld::update) );
    
    _enemies = new CCArray;
    _bullets = new CCArray;
    
    return true;
}

HelloWorld::HelloWorld():_enemies(NULL),_bullets(NULL)
{
}

HelloWorld::~HelloWorld()
{
    if (_enemies)
    {
        _enemies->release();
        _enemies = NULL;
    }
    
    if (_bullets)
    {
        _bullets->release();
        _bullets = NULL;
    }
    
    // cpp don't need to call super dealloc
    // virtual destructor will do this
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
//    CCSprite *enemy = CCSprite::create("shoot.png", ENEMY1_RECT);
    Enemy *enemy = new Enemy(1);
    CCSprite *enemySprite = enemy->getSprite();
    
    // Determine where to spawn the target along the X axis
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    int minX = enemySprite->getContentSize().width/2;
    int maxX = winSize.width - enemySprite->getContentSize().width/2;
    int rangeX = maxX - minX;
    // srand( TimGetTicks() );
    int actualX = ( rand() % rangeX ) + minX;
    
    // Create the target slightly off-screen along the top edge,
    // and along a random position along the X axis as calculated
    enemySprite->setPosition( ccp(actualX, winSize.height + enemySprite->getContentSize().height/2) );
    this->addChild(enemySprite);
    
    // Determine speed of the target
    int minDuration = (int)3.0;
    int maxDuration = (int)6.0;
    int rangeDuration = maxDuration - minDuration;
    // srand( TimGetTicks() );
    int actualDuration = ( rand() % rangeDuration ) + minDuration;
    
    // Create the actions
    CCFiniteTimeAction* actionMove = CCMoveTo::create((float)actualDuration,
                                                      ccp( actualX, 0 - enemySprite->getContentSize().height/2) );
    CCFiniteTimeAction* actionMoveDone = CCCallFuncN::create(this,
                                                             callfuncN_selector(HelloWorld::removeEnemy));
    enemySprite->runAction( CCSequence::create(actionMove, actionMoveDone, NULL) );
    
    enemySprite->setTag(1);
    _enemies->addObject(enemy);
}

void HelloWorld::removeEnemy(CCNode *sender)
{
    CCSprite *sprite = dynamic_cast<CCSprite*>(sender);
    //    enemy->getSprite()->stopAllActions();
    //    this->removeChild(enemy->getSprite(), true);
    sprite->removeFromParentAndCleanup(true);
//    _enemies->removeObject(enemy);
#warning this must remove from array
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
    
    bullet->setTag(2);
    _bullets->addObject(bullet);
}

void HelloWorld::bulletMoveFinished(CCNode* sender)
{
    CCSprite *sprite = (CCSprite *)sender;
    this->removeChild(sprite, true);
    _bullets->removeObject(sprite);
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


void HelloWorld::update(float dt)
{
    CCArray *bulletsToDelete = new CCArray;
    CCArray* enemiesToDelete =new CCArray;
    CCObject* it = NULL;
    CCObject* jt = NULL;
    
    CCARRAY_FOREACH(_bullets, it)
    {
        CCSprite *bullet = dynamic_cast<CCSprite*>(it);
        CCRect bulletRect = CCRectMake(bullet->getPosition().x - (bullet->getContentSize().width/2),
                                       bullet->getPosition().y - (bullet->getContentSize().height/2),
                                       bullet->getContentSize().width,
                                       bullet->getContentSize().height);
        
        CCARRAY_FOREACH(_enemies, jt)
        {
            Enemy *enemy = dynamic_cast<Enemy*>(jt);
            
            if (!enemy) {
                continue;
            }
            if (!enemy->getSprite()) {
                continue;
            }
            if (enemy->getCurrentStatus() == ENEMY_STATUS_DESTROYING) {
                continue;
            }
            
            CCSprite *enemySprite = enemy->getSprite();
            CCRect enemyRect = CCRectMake(enemySprite->getPosition().x - (enemySprite->getContentSize().width/2),
                                          enemySprite->getPosition().y - (enemySprite->getContentSize().height/2),
                                          enemySprite->getContentSize().width,
                                          enemySprite->getContentSize().height);
            
            if (bulletRect.intersectsRect(enemyRect))
            {
                enemiesToDelete->addObject(enemy);
                bulletsToDelete->addObject(bullet);
            }
        }
    }
    
    CCARRAY_FOREACH(enemiesToDelete, jt)
    {
        Enemy *enemy = dynamic_cast<Enemy*>(jt);
        enemy->getSprite()->stopAllActions();
        
        CCAnimate *animAction = enemy->destroyAnimation();
        CCCallFuncN *callFunc = CCCallFuncN::create(this, callfuncN_selector(HelloWorld::removeEnemy));
        enemy->getSprite()->runAction(CCSequence::create(animAction, callFunc, NULL));
    }
    
    CCARRAY_FOREACH(bulletsToDelete, it)
    {
        CCSprite* bullet = dynamic_cast<CCSprite*>(it);
        _bullets->removeObject(bullet);
        this->removeChild(bullet, true);
    }
    
    bulletsToDelete->release();
    enemiesToDelete->release();
}

