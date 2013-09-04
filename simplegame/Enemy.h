//
//  Enemy.h
//  simplegame
//
//  Created by Wang Xuyang on 13-08-30.
//
//

#ifndef __simplegame__Enemy__
#define __simplegame__Enemy__

#include <cocos2d.h>

typedef enum EnemyStatus {
    ENEMY_STATUS_ALIVE,
    ENEMY_STATUS_DEAD,
    ENEMY_STATUS_DESTROYING
} EnemyStatus;

class Enemy : public cocos2d::CCSprite {
    EnemyStatus _currentStatus;

    int _healthPoint;
    
    cocos2d::CCAnimation *_destroyAnimation;
    cocos2d::CCAnimate *_destroyAnimate;
    
public:
    Enemy(int type);
    ~Enemy();

    int getHP();
    int minusHP();
    EnemyStatus getCurrentStatus();
    
    cocos2d::CCAnimate* destroyAnimation();
};

#endif /* defined(__simplegame__Enemy__) */
