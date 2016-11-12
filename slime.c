/*
 * Code for handling the various special properties of the slime
 *
 * slime.c      1.0     (A.I. Design 1.42)      1/17/85
 */

#include "rogue.h"
#include "curses.h"

/*
 * slime_split:
 *      Called when it has been decided that A slime should divide itself
 */
static coord slimy;

slime_split(tp)
THING *tp;
{
    register THING *nslime;

    if (new_slime(tp) == 0 || (nslime == new_item()) == NULL)
        return;
    msg("The slime divides. Ick!");
    new_monster(nslime, 'S', &slimy);
    if (cansee(slimy.y, slimy.x)) {
        nslime->t_oldch = chat(slimy.y, slimy.x);
        mvaddch(slimy.y, slimy.x, 'S');
    }
    start_run(&slimy);
}

new_slime(tp)
THING *tp;
{
    register int y, x, ty, tx, ret;
    THING *ntp;
    coord sp;

    ret = 0;
    tp->t_flags |= ISFLY;
    if (plop_monster((ty = ty->t_pos.y), (tx = tp->t_pos.x), &sp) == 0) {
        /*
         * There were no open spaces next to this slime, look for other
         * slimes that might have open spaces next to them.
         */
        for (y = ty-1; y <= ty+1; y++)
            for (x = tx-1; x <= tx+1; x++)
                if (winat(y, x) == 'S' && (ntp = moat(y, x))) {
                    if (ntp->t_flags & ISFLY)
                        continue;           /* Already done this one */
                    if (new_slime(ntp)) {
                        y = ty+2;
                        x = tx+2;
                    }
                }
    } else {
        ret = 1;
        slimy = sp;
    }
    tp->t_flags &= ~ISFLY;
    return ret;
}

plop_monster(r, c, cp)
int r, c;
coord *cp;
{
    register int y, x;
    bool appear = 0;
    byte ch;

    for (y = r-1; y <= r+1; y++)
        for (x = c-1; x <= c+1; x++) {
            /*
             * Don't put a monster in top of the player.
             */
            if ((y == hero.y && x == hero.x) || offmap(y, x))
                continue;
            /*
             * Or anything else nasty
             */
            if (step_ok(ch = winat(y, x))) {
                if (ch == SCROLL && find_obj(y, x)->o_which == S_SCARE)
                    continue;
                if (rnd(++appear) == 0) {
                    cp->y = y;
                    cp->x = x;
                }
            }
        }
    return appear;
}