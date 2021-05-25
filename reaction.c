#include "pintos_thread.h"

// Forward declaration. This function is implemented in reaction-runner.c,
// but you needn't care what it does. Just be sure it's called when
// appropriate within reaction_o()/reaction_h().
void make_water();

struct reaction {
    int h_excess, h_needed;

    struct lock lock;
    struct condition h2o_reaction;
    struct condition h_creation;
};

void
reaction_init(struct reaction *reaction)
{
    reaction->h_excess = 0;
    reaction->h_needed = 0;

    lock_init(&reaction->lock);
    cond_init(&reaction->h2o_reaction);
    cond_init(&reaction->h_creation);
}

void
reaction_h(struct reaction *reaction)
{
    lock_acquire(&reaction->lock);
    reaction->h_excess++;
    cond_signal(&reaction->h_creation, &reaction->lock);
    while (reaction->h_needed == 0)
        cond_wait(&reaction->h2o_reaction, &reaction->lock);
    reaction->h_needed--;
    lock_release(&reaction->lock);
}

void
reaction_o(struct reaction *reaction)
{
    lock_acquire(&reaction->lock);
    while (reaction->h_excess < 2)
        cond_wait(&reaction->h_creation, &reaction->lock);
    reaction->h_needed += 2;
    reaction->h_excess -= 2;
    cond_broadcast(&reaction->h2o_reaction, &reaction->lock);
    make_water();
    lock_release(&reaction->lock);
}