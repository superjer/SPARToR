// DO NOT USE INCLUDE GUARDS HERE

#define TYPE mother
#include BEGIN
HIDE(
        char edit;
)
#include END

#define TYPE ghost
#include BEGIN
EXPOSE(V, pos, )
EXPOSE(V, vel, )
EXPOSE(V, hull, [2])
EXPOSE(int, model, )
HIDE(
        int client;
        int avatar;
        int clipboard_x;
        int clipboard_y;
        int clipboard_z;
        CB *clipboard_data;
)
#include END

#define TYPE dummy
#include BEGIN
EXPOSE(V, pos, )
EXPOSE(V, vel, )
EXPOSE(V, hull, [2])
EXPOSE(int, model, )
#include END

#define TYPE player
#include BEGIN
EXPOSE(V, pos, )
EXPOSE(V, vel, )
EXPOSE(V, hull, [2])
EXPOSE(V, pvel, )
EXPOSE(int, model, )
HIDE(
        int ghost;
        char goingl;
        char goingr;
        char goingu;
        char goingd;
        char jumping;
        char firing;
        char cooldown;
        char projectiles;
        char gunback;
        char gundown;
        char grounded;
        char facingr;
        char turning;
        char stabbing;
        char hovertime;
        char walkcounter;
)
#include END

#define TYPE bullet
#include BEGIN
EXPOSE(V, pos, )
EXPOSE(V, vel, )
EXPOSE(V, hull, [2])
EXPOSE(int, model, )
HIDE(
        int owner;
        Uint8 ttl;
        char dead;
)
#include END

#define TYPE slug
#include BEGIN
EXPOSE(V, pos, )
EXPOSE(V, vel, )
EXPOSE(V, hull, [2])
EXPOSE(int, model, )
HIDE(
        int dead;
        int spawner;
)
#include END

#define TYPE amigo
#include BEGIN
EXPOSE(V, pos, )
EXPOSE(V, vel, )
EXPOSE(V, hull, [2])
EXPOSE(int, model, )
HIDE(
        char state;
        int statetime;
        int hatcounter;
        int sword;
        V sword_dist;
)
#include END

#define TYPE amigosword
#include BEGIN
EXPOSE(V, pos, )
EXPOSE(V, vel, )
EXPOSE(V, hull, [2])
EXPOSE(int, model, )
HIDE(
        int owner;
        int spincounter;
)
#include END
