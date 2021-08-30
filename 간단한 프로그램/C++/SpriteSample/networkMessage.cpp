#include "framework.h"
#include "queue.h"
#include "Image.h"
#include "Animation.h"
#include "SpriteData.h"
#include "Sprite.h"
#include "ringBuffer.h"
#include "network.h"
#include "networkMessage.h"
#include "protocolBuffer.h"
#include "common.h"
#include "recv.h"

bool CProxyFunc::SC_MoveStartProxy(int id, char direction, short x, short y) {

    CSprite** userListEnd = user + 50;

    for (CSprite** userIter = user; userIter != userListEnd; ++userIter) {

        CSprite* ptrUser = *userIter;
        if (id == ptrUser->_id) {

            ptrUser->_msg = (INPUT_MESSAGE)direction;
            ptrUser->_x = x;
            ptrUser->_y = y;
            return true;
        }

    }
    return false;
}
bool CProxyFunc::SC_CreateMyCharacterProxy(int id, char direction, short x, short y, char hp) {
    USHORT idx;
    userIndex->front(&idx);
    userIndex->pop();

    CSprite* newUser = user[idx];
    new (newUser) CSprite(spriteData, userUpdate);
    newUser->_x = x;
    newUser->_y = y;
    newUser->_nowHp = hp;
    newUser->_maxHp = hp;
    newUser->_seeRight = direction == 4;
    newUser->_id = id;

    newUser->_shadow = _shadow;
    newUser->_isLive = true;
    newUser->_hpBar = _hpBar;
    newUser->_effect = *_effect;

    mySprite = newUser;

    return true;
}

bool CProxyFunc::SC_OtherMyCharacterProxy(int id, char direction, short x, short y, char hp) {
    USHORT idx;
    userIndex->front(&idx);
    userIndex->pop();

    CSprite* newUser = user[idx];
    new (newUser) CSprite(spriteData, otherUserUpdate);
    newUser->_x = x;
    newUser->_y = y;
    newUser->_nowHp = hp;
    newUser->_maxHp = hp;
    newUser->_seeRight = direction == 4;
    newUser->_shadow = _shadow;
    newUser->_isLive = true;
    newUser->_id = id;
    newUser->_hpBar = _hpBar;
    newUser->_effect = *_effect;

    return true;
}

bool CProxyFunc::SC_DeleteCharacterProxy(int id) {
    CSprite** userListEnd = user + 50;

    for (CSprite** userIter = user; userIter != userListEnd; ++userIter) {

        CSprite* ptrUser = *userIter;
        if (id == ptrUser->_id) {

            ptrUser->_isLive = false;
            userIndex->push(userIter - user);
            return true;
        }

    }
    return false;
}

bool CProxyFunc::SC_MoveStopProxy(int id, char direction, short x, short y) {

    CSprite** userListEnd = user + 50;

    for (CSprite** userIter = user; userIter != userListEnd; ++userIter) {

        CSprite* ptrUser = *userIter;

        if (id == ptrUser->_id) {

            ptrUser->_x = x;
            ptrUser->_y = y;
            ptrUser->_msg = INPUT_MESSAGE::NONE;
            ptrUser->_seeRight = direction == 4;
            return true;
        }

    }
    return false;

}

bool CProxyFunc::SC_Attack1Proxy(int id, char direction, short x, short y) {
    CSprite** userListEnd = user + 50;

    for (CSprite** userIter = user; userIter != userListEnd; ++userIter) {

        CSprite* ptrUser = *userIter;
        if (id == ptrUser->_id) {

            ptrUser->_x = x;
            ptrUser->_y = y;
            ptrUser->_msg = INPUT_MESSAGE::ATTACK1;
            ptrUser->_seeRight = (direction == 4);
            return true;
        }
    }
    return false;
}

bool CProxyFunc::SC_Attack2Proxy(int id, char direction, short x, short y) {
    CSprite** userListEnd = user + 50;

    for (CSprite** userIter = user; userIter != userListEnd; ++userIter) {

        CSprite* ptrUser = *userIter;
        if (id == ptrUser->_id) {

            ptrUser->_x = x;
            ptrUser->_y = y;
            ptrUser->_msg = INPUT_MESSAGE::ATTACK2;
            ptrUser->_seeRight = (direction == 4);
            return true;
        }
    }
    return false;
}

bool CProxyFunc::SC_Attack3Proxy(int id, char direction, short x, short y) {
    CSprite** userListEnd = user + 50;

    for (CSprite** userIter = user; userIter != userListEnd; ++userIter) {

        CSprite* ptrUser = *userIter;
        if (id == ptrUser->_id) {

            ptrUser->_x = x;
            ptrUser->_y = y;
            ptrUser->_msg = INPUT_MESSAGE::ATTACK3;
            ptrUser->_seeRight = (direction == 4);
            return true;
        }

    }
    return false;
}

bool CProxyFunc::SC_DamageProxy(int attackId, int damageId, char damageHp) {
    CSprite* attackSprite = nullptr;
    CSprite* damageSprite = nullptr;

    CSprite** userListEnd = user + 50;

    for (CSprite** userIter = user; userIter != userListEnd; ++userIter) {

        CSprite* ptrUser = *userIter;
        if (attackId == ptrUser->_id) {

            attackSprite = ptrUser;
        }

        if (damageId == ptrUser->_id) {

            damageSprite = ptrUser;
        }

    }

    damageSprite->_nowHp = damageHp;
    damageSprite->_effect._doneSingleTime = false;
    damageSprite->_effect._currentImageIndex = 0;
    return true;
}
/*
void CNetwork::recv_DAMAGE(UINT attackId, UINT damageId, BYTE damageHp) {

    

}
*/