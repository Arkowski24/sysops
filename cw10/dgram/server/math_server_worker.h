//
// Created by farald on 10.06.18.
//

#ifndef SYSOPS_MATH_SERVER_WORKER_H
#define SYSOPS_MATH_SERVER_WORKER_H

#include "../fifo/circular_fifo.h"

void insert_worker(int type, int index);

void delete_worker(int type, int index);

Client_t *fetch_worker();

void reinsert_worker(Client_t *worker);

void discard_worker(Client_t *worker);

#endif //SYSOPS_MATH_SERVER_WORKER_H
