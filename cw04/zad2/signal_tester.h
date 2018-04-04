//
// Created by Arkadiusz Placha on 03.04.2018.
//

#ifndef SYSOPS_SIGNAL_TESTER_H
#define SYSOPS_SIGNAL_TESTER_H


void set_parent_handlers();

void kill_children();

void child_work();

void set_child_handlers();

int main(int argc, char *argv[]);

#endif //SYSOPS_SIGNAL_TESTER_H
