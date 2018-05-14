//
// Created by Arkadiusz Placha on 14.05.2018.
//

#ifndef SYSOPS_CLIENT_H
#define SYSOPS_CLIENT_H

void load_fifo();

void initialize_resources();

void create_personal_semaphore();

void delete_personal_semaphore();

void free_resources();

void main_task();

void execute_tasks(int s);

int main(int argc, char *argv[]);

#endif //SYSOPS_CLIENT_H
