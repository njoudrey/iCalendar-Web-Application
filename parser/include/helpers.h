#ifndef HELPERS_H
#define HELPERS_H

#include "LinkedListAPI.h"
#include "CalendarParser.h"

void fPrintPropertyList(List* props, FILE *fp);
void fPrintAlarmsList(List* alarms, FILE *fp);
void fPrintEventsList(List* events, FILE *fp);
char* dateTimeAssemble(DateTime dt);

int validCalProps(List* props);
int validEvents(List* props);
int validEventProps(List* props);
int validAlarms(List* alarms);
int validAlarmProps(List* props);

char* findSummary(List* props);
char* findOrg(List* props);
char* findLoc(List* props);
int getLengthOfInt(int num);
#endif