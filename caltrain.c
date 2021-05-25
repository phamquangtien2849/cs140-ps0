#include "pintos_thread.h"

struct station {
    int num_waiting, num_seats, num_boarding, num_boarded;

    struct lock lock;
    struct condition open;
    struct condition closed;
};

void
station_init(struct station *station)
{
    station->num_waiting = 0;
    station->num_seats = 0;
    station->num_boarding = 0;
    station->num_boarded = 0;

    lock_init(&station->lock);
    cond_init(&station->open);
    cond_init(&station->closed);
}

void
station_load_train(struct station *station, int count)
{
    lock_acquire(&station->lock);
    station->num_seats = count;
    cond_broadcast(&station->open, &station->lock);
    while (station->num_boarded < station->num_seats && (station->num_waiting > 0 || station->num_boarding > 0))
        cond_wait(&station->closed, &station->lock);
    station->num_seats = 0;
    station->num_boarded = 0;
    lock_release(&station->lock);
}

void
station_wait_for_train(struct station *station)
{
    lock_acquire(&station->lock);
    station->num_waiting++;
    while (station->num_boarding + station->num_boarded >= station->num_seats)
        cond_wait(&station->open, &station->lock);
    station->num_boarding++;
    station->num_waiting--;
    lock_release(&station->lock);
}

void
station_on_board(struct station *station)
{
    lock_acquire(&station->lock);
    station->num_boarded++;
    station->num_boarding--;
    cond_signal(&station->closed, &station->lock);
    lock_release(&station->lock);
}