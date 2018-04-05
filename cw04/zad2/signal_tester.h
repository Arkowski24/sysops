//
// Created by Arkadiusz Placha on 03.04.2018.
//

#ifndef SYSOPS_SIGNAL_TESTER_H
#define SYSOPS_SIGNAL_TESTER_H

#define TEST_PRINT_CREATION 1

#define TEST_PRINT_REQUEST 1

#define TEST_PRINT_APPROVAL 1

#define TEST_PRINT_RTS 1

#define TEST_PRINT_CHILD_EXIT 1

void kill_children();

void child_work();

void parent_work();

int main(int argc, char *argv[]);

#endif //SYSOPS_SIGNAL_TESTER_H
