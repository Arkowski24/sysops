//
// Created by Arkadiusz Placha on 18.03.2018.
//

#ifndef SYSOPS_RECORD_HANDLER_H
#define SYSOPS_RECORD_HANDLER_H

void sys_generate(char *filePath, unsigned int recordsCount, unsigned int recordLength);

void sys_sort(char *filePath, unsigned int recordsCount, unsigned int recordLength);

void sys_copy(char *filePathFrom, char *filePathTo, unsigned int recordsCount, unsigned int recordLength);

void lib_generate(char *filePath, unsigned int recordsCount, unsigned int recordLength);

void lib_sort(char *filePath, unsigned int recordsCount, unsigned int recordLength);

void lib_copy(char *filePathFrom, char *filePathTo, unsigned int recordsCount, unsigned int recordLength);

unsigned char *create_buffer(unsigned int size);

#endif //SYSOPS_RECORD_HANDLER_H
