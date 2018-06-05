//
// Created by Arkadiusz Placha on 31.05.18.
//

#ifndef SYSOPS_PROD_CONS_H
#define SYSOPS_PROD_CONS_H

void initialize_queue(unsigned int qSize);

void initialize_mutex();

void kill_other_threads();

void *producer_routine(void *arg);

void *consumer_routine(void *arg);

#endif //SYSOPS_PROD_CONS_H
