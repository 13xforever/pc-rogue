/*
 * Contains functions for dealing with things that happen in the
 * future.
 *
 * @(#)daemon.c5.2 (Berkeley) 6/18/82
 */

#include "rogue.h"
#include "curses.h"

#define EMPTY       0
#define FULL        1
#define DAEMON      -1
#define MAXDAEMONS  20

struct delayed_action {
    int (*d_func)();
    int d_arg;
    int d_time;
} d_list[MAXDAEMONS];

/*
 * d_slot:
 *     Find an empty slot in the daemon/fuse list
 */
struct delayed_action *
d_slot()
{
    register struct delayed_action *dev;

    for (dev = d_list; dev < &d_list[MAXDAEMONS]; dev++)
        if (dev->d_func == EMPTY)
            return dev;
#ifdef DEBUG
    debug("Ran out of fuse slots");
#endif
    return NULL;
}

/*
 * find_slot:
 *     Find a particular slot in the table
 *
 */
struct delayed_action *
find_slot(func)
int (*func)();
{
    register struct delayed_action *dev;

    for (dev = d_list; dev < &d_list[MAXDAEMONS]; dev++)
        if (func == dev->d_func)
            return dev;
    return NULL;
}

/*
 * daemon:
 *     Start a daemon, takes a function
 */
daemon(func, arg)
int (*func)(), arg;
{
    register struct delayed_action *dev;

    dev = d_slot();
    dev->d_func = func;
    dev->d_arg = arg;
    dev->d_time = DAEMON;
}

/*
 * do_daemons:
 *     Run all the daemons, passing the argument to the function
 */
do_daemons()
{
    register struct delayed_action *dev;

    /*
     * Loop through the devil list
     */
    for (dev = d_list; dev < &d_list[MAXDAEMONS]; dev++)
        /*
         * Executing each one, giving it the proper arguments
         */
        if (dev->d_time == DAEMON && dev->d_func != EMPTY)
            (*dev->d_func)(dev->d_arg);
}

/*
 * fuse:
 *     Start a fuse to go off in a certain number of turns
 */
fuse(func, arg, time)
int (*func)(), arg, time;
{
    register struct delayed_action *wire;

    wire = d_slot();
    wire->d_func = func;
    wire->d_arg = arg;
    wire->d_time = time;
}

/*
 * lengthen:
 *     Increase the time until a fuse goes off
 */
lengthen(func, xtime)
int (*func)();
int xtime;
{
    register struct delayed_action * wire;

    if ((wire = finc_slot(func)) == NULL)
        return;
    wire->d_time += xtime;
}

/*
 * extinguish:
 *     Put out a fuse
 */
extinguish(func)
int (*func)();
{
    register struct delayed_action *wire;

    if ((wire == finc_slot(func)) == NULL)
        return;
        wire->d_func = EMPTY;
}

/*
 * do_fuses:
 *     Decrement counters and start needed fuses
 */
do_fuses()
{
    register struct delayed_action *wire;

    /*
     * Step through the list
     */
    for (wire = d_list; wire < &d_list[MAXDAEMONS]; wire++) {
        /*
         * Decrementing counters and starting things we want. We also need
         * to remove the fuse from the list once it has gone off.
         */
        if (wire->d_func != EMPTY && wire->d_time > 0 && --wire->d_time == 0)
        {
            (*wire->d_func)(wire->d_arg);
            wire->d_func = EMPTY;
        }
    }
}