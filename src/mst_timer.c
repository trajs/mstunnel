#include "mstunnel.h"
#include "mst_network.h"
#include "memmgmt.h"
#include "mst_timer.h"

// system timer
mst_timer_data_t *sys_td = NULL;

extern mst_event_base_t meb;

void mst_timer(evutil_socket_t fd, short event, void *arg)
{
    mst_timer_data_t *td = (mst_timer_data_t *)arg;
//    fprintf(stderr, "Timer triggered\n======================\n");
    if (!arg) {
        return;
    }
    switch(td->type) {
        case MST_SYS:
            fprintf(stderr, "System timer\n");
            break;
        case MST_MNP:
            fprintf(stderr, "MNP timer\n");
            if (-1 == mst_link_status(td->data)) {
                return;
            }
            break;
        default:
            fprintf(stderr, "Unknown timer\n");
            return;
    }

    evtimer_add(td->te, &td->timeo);
    return;
}

void *mst_loop_timer(void *arg)
{
    int rv = -1;
    rv = event_base_dispatch(meb.Teb);
    fprintf(stderr, "exit status timer... base: %p, rv: %d\n", meb.Teb, rv);
    return NULL;
}

int mst_timer_init(void)
{
    pthread_t pt_status_timer;

    sys_td = (mst_timer_data_t *)__mst_malloc(sizeof(mst_timer_data_t));
    assert(sys_td);

    sys_td->type = MST_SYS;
    sys_td->timeo.tv_sec = 1;
    sys_td->timeo.tv_usec = 0;
    sys_td->data = NULL;

    // Create event base for Timer events
    meb.Teb = event_base_new();
    assert(meb.Teb);

    sys_td->te = evtimer_new(meb.Teb, mst_timer, sys_td);
    fprintf(stderr, "Looping timer... base: %p\n", meb.Teb);
    evtimer_add(sys_td->te, &sys_td->timeo);

    fprintf(stderr, "status timer... base: %p\n", meb.Teb);
    pthread_create(&pt_status_timer, NULL, mst_loop_timer, NULL);

    return 0;
}
