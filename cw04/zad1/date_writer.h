//
// Created by Arkadiusz Placha on 02.04.2018.
//

#ifndef SYSOPS_DATE_WRITER_H
#define SYSOPS_DATE_WRITER_H

void stop_signal_handler(int signal);

void interrupt_signal_handler(int signal);

void print_date();

void create_executor();

void set_interrupt_handler();

void set_stop_handler();

int main(int argc, char *argv[]);

#endif //SYSOPS_DATE_WRITER_H
