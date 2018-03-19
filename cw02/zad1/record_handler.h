//
// Created by Arkadiusz Placha on 18.03.2018.
//

#ifndef SYSOPS_RECORD_HANDLER_H
#define SYSOPS_RECORD_HANDLER_H

void sys_generate(char *filePath, unsigned int recordLength, unsigned int recordsCount);

void sys_sort(char *filePath, unsigned int recordLength, unsigned int recordsCount);

void sys_copy(char *filePathFrom, char *filePathTo, unsigned int recordLength, unsigned int recordsCount);

void lib_generate(char *filePath, unsigned int recordLength, unsigned int recordsCount);

void lib_sort(char *filePath, unsigned int recordLength, unsigned int recordsCount);

void lib_copy(char *filePathFrom, char *filePathTo, unsigned int recordLength, unsigned int recordsCount);

#endif //SYSOPS_RECORD_HANDLER_H
