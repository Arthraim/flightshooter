//
//  Enemy.cpp
//  simplegame
//
//  Created by Wang Xuyang on 13-08-30.
//
//

#include "Enemy.h"

using namespace cocos2d;

#define ENEMY1_RECT     CCRectMake(534, 612, 57, 43)

Enemy::Enemy(int type):_sprite(NULL),_destroyAnimation(NULL),_destroyAnimate(NULL)
{
    switch (type) {
        case 1:
        {
            _sprite = CCSprite::create("shoot.png", ENEMY1_RECT);
            _healthPoint = 1;
            _currentStatus = ENEMY_STATUS_ALIVE;
        }
            break;
            
        default:
            break;
    }
}

Enemy::~Enemy()
{
    if (_sprite) {
        _sprite->release();
        _sprite = NULL;
    }
}

CCSprite* Enemy::getSprite()
{
    return _sprite;
}

int Enemy::getHP()
{
    return _healthPoint;
}

int Enemy::minusHP()
{
    _healthPoint -= 1;
    return _healthPoint;
}

EnemyStatus Enemy::getCurrentStatus()
{
    return _currentStatus;
}

CCAnimate* Enemy::destroyAnimation()
{
    _currentStatus = ENEMY_STATUS_DESTROYING;
    _destroyAnimation = CCAnimation::create();
    _destroyAnimation->addSpriteFrame(CCSpriteFrame::create("shoot.png", CCRectMake(267, 347, 57, 51)));
    _destroyAnimation->addSpriteFrame(CCSpriteFrame::create("shoot.png", CCRectMake(873, 697, 57, 51)));
    _destroyAnimation->addSpriteFrame(CCSpriteFrame::create("shoot.png", CCRectMake(267, 296, 57, 51)));
    _destroyAnimation->addSpriteFrame(CCSpriteFrame::create("shoot.png", CCRectMake(930, 697, 57, 51)));
    _destroyAnimation->setDelayPerUnit(0.1);
    _destroyAnimate = CCAnimate::create(_destroyAnimation);
    return _destroyAnimate;
}
