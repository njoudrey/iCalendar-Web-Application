#include "CalendarParser.h"
#include "helpers.h"

void deleteEvent(void* toBeDeleted)
{
	Event *tmpEvent = NULL;
	List* eventProps = NULL;
	List* alarms = NULL;

	if(toBeDeleted == NULL)
	{
		return;
	}

	tmpEvent = (Event*)toBeDeleted;
	eventProps = tmpEvent->properties;
	alarms = tmpEvent->alarms;

	//freelist
	if(eventProps != NULL)
	{
		freeList(eventProps);
	}
	if(alarms != NULL)
	{
		freeList(alarms);
	}
	free(tmpEvent);
}
int compareEvents(const void* first, const void* second)
{
	return -1;
}
char* printEvent(void* toBePrinted)
{
	char *tmpStr = NULL;
	char *propHolder = NULL;
	Event *tmpEvent = NULL;
	int len = 0;
	tmpEvent = (Event*)toBePrinted;

	len = strlen(tmpEvent->UID)+strlen(tmpEvent->creationDateTime.date)+strlen(tmpEvent->creationDateTime.time) + strlen(tmpEvent->startDateTime.date)+strlen(tmpEvent->startDateTime.time) + 44;
	tmpStr = (char*)malloc(sizeof(char)*len);

	sprintf(tmpStr, "Event:\nUID: %s Creation Date: %s %s Start Date: %s %s", tmpEvent->UID, tmpEvent->creationDateTime.date, tmpEvent->creationDateTime.time, tmpEvent->startDateTime.date, tmpEvent->startDateTime.time);

	propHolder = toString(((Event*)toBePrinted)->properties);
	tmpStr = (char*)realloc(tmpStr, sizeof(char)*(strlen(tmpStr) + strlen(propHolder)+1));
	strcat(tmpStr, propHolder);
	strcpy(propHolder, "");
	free(propHolder);

	propHolder = toString(((Event*)toBePrinted)->alarms);
	tmpStr = (char*)realloc(tmpStr, sizeof(char)*(strlen(tmpStr) + strlen(propHolder)+1));
	strcat(tmpStr, propHolder);
	free(propHolder);
	return tmpStr;
}

void deleteAlarm(void* toBeDeleted)
{
	Alarm* tmpAlarm = NULL;
	List* alarmProps = NULL;

	if(toBeDeleted == NULL)
	{
		return;
	}

	tmpAlarm = (Alarm*)toBeDeleted;
	alarmProps = tmpAlarm->properties;

	//freelist
	freeList(alarmProps);
	free(tmpAlarm->trigger);
	free(tmpAlarm);
}
int compareAlarms(const void* first, const void* second)
{
	return -1;
}
char* printAlarm(void* toBePrinted)
{
	char *tmpStr = NULL;
	char *propHolder = NULL;
	Alarm *tmpAlarm = NULL;
	int len = 0;

	tmpAlarm = (Alarm*)toBePrinted;

	len = strlen(tmpAlarm->action)+strlen(tmpAlarm->trigger) + 27;
	tmpStr = (char*)malloc(sizeof(char)*len);

	sprintf(tmpStr, "Alarm:\nAction: %s, Trigger: %s", tmpAlarm->action, tmpAlarm->trigger);

	propHolder = toString(((Alarm*)toBePrinted)->properties);
	tmpStr = (char*)realloc(tmpStr, sizeof(char)*(strlen(tmpStr) + strlen(propHolder)+1));
	strcat(tmpStr, propHolder);
	free(propHolder);

	return tmpStr;
}

void deleteProperty(void* toBeDeleted)
{
	Property *tmpProp = NULL;

	if(toBeDeleted == NULL)
	{
		return;
	}

	tmpProp = (Property*)toBeDeleted;

	free(tmpProp);
}
int compareProperties(const void* first, const void* second)
{
	Property *prop1 = NULL;
	Property *prop2 = NULL;

	if(first == NULL || second == NULL)
	{
		return 0;
	}

	prop1 = (Property*)first;
	prop2 = (Property*)second;

	return strcmp((char*)prop1->propName, (char*)prop2->propName);
}
char* printProperty(void* toBePrinted)
{
	char *tmpStr = NULL;
	Property *tmpProp = NULL;
	int len = 0;

	tmpProp = (Property*)toBePrinted;

	if(toBePrinted == NULL)
	{
		return NULL;
	}

	len = strlen(tmpProp->propName)+strlen(tmpProp->propDescr) + 22;
	tmpStr = (char*)malloc(sizeof(char)*len);

	sprintf(tmpStr, "Property: %s Contents: %s", tmpProp->propName, tmpProp->propDescr);

	return tmpStr;
}

void deleteDate(void* toBeDeleted)
{

}
int compareDates(const void* first, const void* second)
{
	return -1;
}
char* printDate(void* toBePrinted)
{
	return NULL;
}

ICalErrorCode createCalendar(char* fileName, Calendar** obj)
{
	//file stuff
	FILE *fp = NULL;
	char c = 0;

	//parser stuff
	char line[2000] = "";
	char property[2000] = "";
	char content[2000] = "";
	int seperatorPos = 0;
	int lineIndex = 0;
	int linesRead = 0;

	//flags
	bool rFlag = false;
	bool nFlag = false;
	bool objCal = false;
	bool objEvent = false;
	bool objAlarm = false;

	bool prodIDFound = false;
	bool versionFound = false;
	bool eventFound = false;
	bool calcScaleFound = false;
	bool methodFound = false;

	bool dTStampFound = false;
	bool uIDFound = false;
	bool dTStartFound = false;
	bool rRuleFound = false;
	bool dTendDurFound = false;
	bool classFound = false;
	bool createdFound = false;
	bool descriptionFound = false;
	bool geoFound = false;
	bool lastModFound = false;
	bool locationFound = false;
	bool organizerFound = false;
	bool priorityFound = false;
	bool seqFound = false;
	bool statusFound = false;
	bool summaryFound = false;
	bool transpFound = false;
	bool urlFound = false;
	bool recuridFound = false;

	bool triggerFound = false;
	bool actionFound = false;
	bool alarmDurFound = false;
	bool alarmRepFound = false;
	bool alarmAttFound = false;
	bool alarmSumFound = false;
	bool alarmDescFound = false;


	//calendar tings
	Property *tmpProp = NULL;
	Event *tmpEvent = NULL;
	Alarm *tmpAlarm = NULL;
	List* list = NULL;
	List* events = NULL;
	List* eventProps = NULL;
	List* alarms = NULL;
	List* alarmProps = NULL;


	if(fileName == NULL)
	{
		*obj = NULL;
		return INV_FILE;
	}
	if(strstr(fileName,".ics") == NULL || strlen(fileName) < 5)
	{
		*obj = NULL;
		return INV_FILE;
	}

	fp = fopen(fileName, "r");
	if(fp == NULL)
	{
		*obj = NULL;
		return INV_FILE;
	}
	else
	{
		*obj = (Calendar*)malloc(sizeof(Calendar));
		list = initializeList(&printProperty, &deleteProperty, &compareProperties);
		events = initializeList(&printEvent, &deleteEvent, &compareProperties);
		while((c=getc(fp)) != EOF)
		{
			if(c == '\r')
			{
				rFlag = true;
			}
			else if(rFlag && c == '\n')
			{
				rFlag = false;
				nFlag = true;
			}
			else if(rFlag == false && c == '\n')
			{
				//free up
				deleteCalendar(*obj);
				return INV_FILE;
			}
			else if(rFlag && c != '\n')
			{
				//free up
				deleteCalendar(*obj);
				return INV_FILE;
			}
			else if(nFlag)
			{
				if(c == ' ' || c == '\t')
				{
					
				}
				else
				{
					//ANALYZE LINES
					//FIRST LINE IS BEGIN:VCALENDAR AKA VALID START
					if(linesRead == 0 && strcmp(line, "BEGIN:VCALENDAR")==0)
					{
							objCal = true;
					}
					if(objCal == false)
					{
						deleteCalendar(*obj);
						return INV_CAL;
					}
					//ENTER VEVENT
					if(strcmp(line, "BEGIN:VEVENT")==0 && objCal == true)
					{
						objEvent = true;
						tmpEvent = (Event*)malloc(sizeof(Event));
						eventProps = initializeList(&printProperty, &deleteProperty, &compareProperties);
						alarms = initializeList(&printAlarm, &deleteAlarm, &compareProperties);
						eventFound = true;
					}
					//EXIT VEVENT
					if(strcmp(line, "END:VEVENT")==0 && objEvent == true)
					{
						objEvent = false;
						tmpEvent->properties = eventProps;
						tmpEvent->alarms = alarms;
						insertBack(events, (void*)tmpEvent);
						if(uIDFound == false || dTStampFound == false)
						{
							freeList(events);
							deleteCalendar(*obj);
							return INV_EVENT;
						}
						dTStampFound = false;
						uIDFound = false;
						dTStartFound = false;
						rRuleFound = false;
						dTendDurFound = false;
						classFound = false;
						createdFound = false;
						descriptionFound = false;
						geoFound = false;
						lastModFound = false;
						locationFound = false;
						organizerFound = false;
						priorityFound = false;
						seqFound = false;
						statusFound = false;
						summaryFound = false;
						transpFound = false;
						urlFound = false;
						recuridFound = false;
					}

					if(strcmp(line, "BEGIN:VALARM") == 0 && objEvent == true && objAlarm == false)
					{
						tmpAlarm = (Alarm*)malloc(sizeof(Alarm));
						alarmProps = initializeList(&printProperty, &deleteProperty, &compareProperties);
						objAlarm = true;
					}
					if(strcmp(line, "END:VALARM") == 0 && objAlarm == true)
					{
						tmpAlarm->properties = alarmProps;
						insertBack(alarms, (void*)tmpAlarm);
						objAlarm = false;
						triggerFound = false;
						actionFound = false;
						alarmAttFound = false;
						alarmDurFound = false;
						alarmRepFound = false;
						alarmSumFound = false;
						alarmDescFound = false;
						if(strlen(tmpAlarm->action) == 0 || tmpAlarm->trigger == NULL)
						{
							deleteEvent(tmpEvent);
							deleteCalendar(*obj);
							return INV_ALARM;
						}
					}

					//BREAK INTO PROPERTY AND CONTENT
					for(int i = 0; i < strlen(line); i++)
					{
						if(line[i] == ':' || line[i] == ';')
						{
							seperatorPos = i;
							break;
						}
					}
					strncpy(property, line, seperatorPos);
					strcpy(content, &line[seperatorPos+1]);

					if(objCal == true && objEvent == false && objAlarm == false)
					{
						if(strcmp(property, "PRODID") == 0 && prodIDFound == true)
						{
							deleteCalendar(*obj);
							return DUP_PRODID;
						}

						if(strcmp(property, "PRODID") == 0 && prodIDFound == false)
						{
							strcpy((*obj)->prodID,content);
							prodIDFound = true;
							if(strlen(content) == 0)
							{
								deleteCalendar(*obj);
								return INV_PRODID;
							}
						}


						if(strcmp(property, "VERSION") == 0 && versionFound == true)
						{
							deleteCalendar(*obj);
							return DUP_VER;
						}

						if(strcmp(property, "VERSION") == 0 && versionFound == false)
						{
							(*obj)->version = (float)atoll(content);
							versionFound = true;
							if(strlen(content) == 0)
							{
								deleteCalendar(*obj);
								return INV_VER;
							}
						}



						if(strcmp(property, "CALSCALE") == 0 && calcScaleFound == false)
						{
							tmpProp = (Property*)malloc(sizeof(Property)+((strlen(content)+1)*(sizeof(char))));
							strcpy(tmpProp->propName, property);
							strcpy(tmpProp->propDescr, content);
							insertBack(list, (void*)tmpProp);
							calcScaleFound = true;
						}

						if(strcmp(property, "METHOD") == 0 && methodFound == false)
						{
							tmpProp = (Property*)malloc(sizeof(Property)+((strlen(content)+1)*(sizeof(char))));
							strcpy(tmpProp->propName, property);
							strcpy(tmpProp->propDescr, content);
							insertBack(list, (void*)tmpProp);
							methodFound = true;
						}

						if(strcmp(property, "X-PROP") == 0 || strcmp(property, "IANA-PROP") == 0)
						{
							tmpProp = (Property*)malloc(sizeof(Property)+((strlen(content)+1)*(sizeof(char))));
							strcpy(tmpProp->propName, property);
							strcpy(tmpProp->propDescr, content);
							insertBack(list, (void*)tmpProp);
						}
					}
					//LOOKING FOR VEVENT PROPERTIES
					if(objCal == true && objEvent == true && objAlarm == false)
					{
						if(strcmp(property, "UID") == 0 && uIDFound == false)
						{
							strcpy(tmpEvent->UID, content);
							uIDFound = true;
						}
						if(strcmp(property, "DTSTAMP") == 0 && dTStampFound == false)
						{
							DateTime tmp;
							if(content[8] != 'T')
							{
								deleteEvent(tmpEvent);
								deleteCalendar(*obj);
								return INV_DT;
							}
							strncpy(tmp.date, content, 8);
							(tmp.date)[8] = '\0';
							strncpy(tmp.time, &content[9], 6);
							(tmp.time)[6] = '\0';
							if(content[15] == 'Z')
							{
								tmp.UTC = true;
							}
							else
							{
								tmp.UTC = false;
							}
							tmpEvent->creationDateTime = tmp;
							dTStampFound = true;
							
						}
						if(strcmp(property, "DTSTART") == 0 && dTStartFound == false)
						{
							DateTime tmp;
							strncpy(tmp.date, content, 8);
							(tmp.date)[8] = '\0';
							strncpy(tmp.time, &content[9], 6);
							(tmp.time)[6] = '\0';
							if(content[15] == 'Z')
							{
								tmp.UTC = true;
							}
							else
							{
								tmp.UTC = false;
							}
							tmpEvent->startDateTime = tmp;
							dTStartFound = true;
						}

						if(strcmp(property, "ATTACH") == 0 || strcmp(property, "ATTENDEE") == 0 || strcmp(property, "CATEGORIES") == 0 || strcmp(property, "COMMENT") == 0 || strcmp(property, "CONTACT") == 0 || strcmp(property, "EXDATE") == 0 || strcmp(property, "RSTATUS") == 0 || strcmp(property, "RELATED") == 0 || strcmp(property, "RESOURCES") == 0 || strcmp(property, "RDATE") == 0 || strcmp(property, "X-PROP") == 0 || strcmp(property, "IANA-PROP") == 0)
						{
							tmpProp = (Property*)malloc(sizeof(Property)+((strlen(content)+1)*(sizeof(char))));
							strcpy(tmpProp->propName, property);
							strcpy(tmpProp->propDescr, content);
							insertBack(eventProps, (void*)tmpProp);
						}
						if(strcmp(property, "RRULE") == 0 && rRuleFound == false)
						{
							tmpProp = (Property*)malloc(sizeof(Property)+((strlen(content)+1)*(sizeof(char))));
							strcpy(tmpProp->propName, property);
							strcpy(tmpProp->propDescr, content);
							insertBack(eventProps, (void*)tmpProp);
							rRuleFound = true;
						}
						if((strcmp(property, "DTEND") == 0 || strcmp(property, "DURATION") == 0) && dTendDurFound == false)
						{
							tmpProp = (Property*)malloc(sizeof(Property)+((strlen(content)+1)*(sizeof(char))));
							strcpy(tmpProp->propName, property);
							strcpy(tmpProp->propDescr, content);
							insertBack(eventProps, (void*)tmpProp);
							dTendDurFound = true;
						}
						if(strcmp(property, "CLASS") == 0 && classFound == false)
						{
							tmpProp = (Property*)malloc(sizeof(Property)+((strlen(content)+1)*(sizeof(char))));
							strcpy(tmpProp->propName, property);
							strcpy(tmpProp->propDescr, content);
							insertBack(eventProps, (void*)tmpProp);
							classFound = true;
						}
						if(strcmp(property, "CREATED") == 0 && createdFound == false)
						{
							tmpProp = (Property*)malloc(sizeof(Property)+((strlen(content)+1)*(sizeof(char))));
							strcpy(tmpProp->propName, property);
							strcpy(tmpProp->propDescr, content);
							insertBack(eventProps, (void*)tmpProp);
							createdFound = true;
						}
						if(strcmp(property, "DESCRIPTION") == 0 && descriptionFound == false)
						{
							tmpProp = (Property*)malloc(sizeof(Property)+((strlen(content)+1)*(sizeof(char))));
							strcpy(tmpProp->propName, property);
							strcpy(tmpProp->propDescr, content);
							insertBack(eventProps, (void*)tmpProp);
							descriptionFound = true;
						}
						if(strcmp(property, "GEO") == 0 && geoFound == false)
						{
							tmpProp = (Property*)malloc(sizeof(Property)+((strlen(content)+1)*(sizeof(char))));
							strcpy(tmpProp->propName, property);
							strcpy(tmpProp->propDescr, content);
							insertBack(eventProps, (void*)tmpProp);
							geoFound = true;
						}
						if(strcmp(property, "LAST-MODIFIED") == 0 && lastModFound == false)
						{
							tmpProp = (Property*)malloc(sizeof(Property)+((strlen(content)+1)*(sizeof(char))));
							strcpy(tmpProp->propName, property);
							strcpy(tmpProp->propDescr, content);
							insertBack(eventProps, (void*)tmpProp);
							lastModFound = true;
						}
						if(strcmp(property, "LOCATION") == 0 && locationFound == false)
						{
							tmpProp = (Property*)malloc(sizeof(Property)+((strlen(content)+1)*(sizeof(char))));
							strcpy(tmpProp->propName, property);
							strcpy(tmpProp->propDescr, content);
							insertBack(eventProps, (void*)tmpProp);
							locationFound = true;
						}
						if(strcmp(property, "ORGANIZER") == 0 && organizerFound == false)
						{
							tmpProp = (Property*)malloc(sizeof(Property)+((strlen(content)+1)*(sizeof(char))));
							strcpy(tmpProp->propName, property);
							strcpy(tmpProp->propDescr, content);
							insertBack(eventProps, (void*)tmpProp);
							organizerFound = true;
						}
						if(strcmp(property, "PRIORITY") == 0 && priorityFound == false)
						{
							tmpProp = (Property*)malloc(sizeof(Property)+((strlen(content)+1)*(sizeof(char))));
							strcpy(tmpProp->propName, property);
							strcpy(tmpProp->propDescr, content);
							insertBack(eventProps, (void*)tmpProp);
							priorityFound = true;
						}
						if(strcmp(property, "SEQUENCE") == 0 && seqFound == false)
						{
							tmpProp = (Property*)malloc(sizeof(Property)+((strlen(content)+1)*(sizeof(char))));
							strcpy(tmpProp->propName, property);
							strcpy(tmpProp->propDescr, content);
							insertBack(eventProps, (void*)tmpProp);
							seqFound = true;
						}
						if(strcmp(property, "STATUS") == 0 && statusFound == false)
						{
							tmpProp = (Property*)malloc(sizeof(Property)+((strlen(content)+1)*(sizeof(char))));
							strcpy(tmpProp->propName, property);
							strcpy(tmpProp->propDescr, content);
							insertBack(eventProps, (void*)tmpProp);
							statusFound = true;
						}
						if(strcmp(property, "SUMMARY") == 0 && summaryFound == false)
						{
							tmpProp = (Property*)malloc(sizeof(Property)+((strlen(content)+1)*(sizeof(char))));
							strcpy(tmpProp->propName, property);
							strcpy(tmpProp->propDescr, content);
							insertBack(eventProps, (void*)tmpProp);
							summaryFound = true;
						}
						if(strcmp(property, "TRANSP") == 0 && transpFound == false)
						{
							tmpProp = (Property*)malloc(sizeof(Property)+((strlen(content)+1)*(sizeof(char))));
							strcpy(tmpProp->propName, property);
							strcpy(tmpProp->propDescr, content);
							insertBack(eventProps, (void*)tmpProp);
							transpFound = true;
						}
						if(strcmp(property, "URL") == 0 && urlFound == false)
						{
							tmpProp = (Property*)malloc(sizeof(Property)+((strlen(content)+1)*(sizeof(char))));
							strcpy(tmpProp->propName, property);
							strcpy(tmpProp->propDescr, content);
							insertBack(eventProps, (void*)tmpProp);
							urlFound = true;
						}
						if(strcmp(property, "RECURRENCE-ID") == 0 && recuridFound == false)
						{
							tmpProp = (Property*)malloc(sizeof(Property)+((strlen(content)+1)*(sizeof(char))));
							strcpy(tmpProp->propName, property);
							strcpy(tmpProp->propDescr, content);
							insertBack(eventProps, (void*)tmpProp);
							recuridFound = true;
						}
						if(strlen(content) == 0)
						{
							freeList(eventProps);
							deleteCalendar(*obj);
							return INV_EVENT;
						}
						if(strlen(property) == 0 && line[0] != ';')
						{
							freeList(eventProps);
							deleteCalendar(*obj);
							return INV_EVENT;
						}
					}
					//ALARM PROPERTIES
					if(objCal == true && objEvent == true && objAlarm == true)
					{
						if(strcmp(property, "TRIGGER") == 0 && triggerFound == false)
						{
							tmpAlarm->trigger = (char*)malloc((strlen(content)+1)*sizeof(char));
							strcpy(tmpAlarm->trigger, content);
							triggerFound = true;
						}
						if(strcmp(property, "ACTION") == 0 && actionFound == false)
						{
							strcpy(tmpAlarm->action, content);
							actionFound = true;
						}
						//both need to occur
						if(strcmp(property, "DURATION") == 0 && alarmDurFound == false)
						{
							tmpProp = (Property*)malloc(sizeof(Property)+((strlen(content)+1)*(sizeof(char))));
							strcpy(tmpProp->propName, property);
							strcpy(tmpProp->propDescr, content);
							insertBack(alarmProps, (void*)tmpProp);
							alarmDurFound = true;
							if(strlen(property) == 0 || strlen(content) == 0)
							{
								freeList(alarmProps);
								deleteAlarm(tmpAlarm);
								deleteEvent(tmpEvent);
								deleteCalendar(*obj);
								return INV_ALARM;
							}
						}
						if(strcmp(property, "REPEAT") == 0 && alarmRepFound == false)
						{
							tmpProp = (Property*)malloc(sizeof(Property)+((strlen(content)+1)*(sizeof(char))));
							strcpy(tmpProp->propName, property);
							strcpy(tmpProp->propDescr, content);
							insertBack(alarmProps, (void*)tmpProp);
							alarmRepFound = true;
							if(strlen(content) == 0)
							{
								freeList(alarmProps);
								deleteAlarm(tmpAlarm);
								deleteEvent(tmpEvent);
								deleteCalendar(*obj);
								return INV_ALARM;
							}
						}
						if(strcmp(property, "ATTACH") == 0 && alarmAttFound == false)
						{
							tmpProp = (Property*)malloc(sizeof(Property)+((strlen(content)+1)*(sizeof(char))));
							strcpy(tmpProp->propName, property);
							strcpy(tmpProp->propDescr, content);
							insertBack(alarmProps, (void*)tmpProp);
							alarmAttFound = true;
							if(strlen(property) == 0 || strlen(content) == 0)
							{
								freeList(alarmProps);
								deleteAlarm(tmpAlarm);
								deleteEvent(tmpEvent);
								deleteCalendar(*obj);
								return INV_ALARM;
							}
						}
						if(strcmp(property, "DESCRIPTION") == 0 && alarmDescFound == false)
						{
							tmpProp = (Property*)malloc(sizeof(Property)+((strlen(content)+1)*(sizeof(char))));
							strcpy(tmpProp->propName, property);
							strcpy(tmpProp->propDescr, content);
							insertBack(alarmProps, (void*)tmpProp);
							alarmDescFound = true;
							if(strlen(property) == 0 || strlen(content) == 0)
							{
								freeList(alarmProps);
								deleteAlarm(tmpAlarm);
								deleteEvent(tmpEvent);
								deleteCalendar(*obj);
								return INV_ALARM;
							}
						}
						if(strcmp(property, "SUMMARY") == 0 && alarmSumFound == false)
						{
							tmpProp = (Property*)malloc(sizeof(Property)+((strlen(content)+1)*(sizeof(char))));
							strcpy(tmpProp->propName, property);
							strcpy(tmpProp->propDescr, content);
							insertBack(alarmProps, (void*)tmpProp);
							alarmSumFound = true;
							if(strlen(property) == 0 || strlen(content) == 0)
							{
								freeList(alarmProps);
								deleteAlarm(tmpAlarm);
								deleteEvent(tmpEvent);
								deleteCalendar(*obj);
								return INV_ALARM;
							}
						}
						if(strcmp(property, "X-PROP") == 0 || strcmp(property, "IANA-PROP") == 0 || strcmp(property, "ATTENDEE") == 0)
						{
							tmpProp = (Property*)malloc(sizeof(Property)+((strlen(content)+1)*(sizeof(char))));
							strcpy(tmpProp->propName, property);
							strcpy(tmpProp->propDescr, content);
							insertBack(alarmProps, (void*)tmpProp);
							if(strlen(property) == 0 || strlen(content) == 0)
							{
								freeList(alarmProps);
								deleteAlarm(tmpAlarm);
								deleteEvent(tmpEvent);
								deleteCalendar(*obj);
								return INV_ALARM;
							}
						}
					}
					//END OF ANALYZATION
					memset(property,0,sizeof(property));
					memset(content,0,sizeof(content));
					memset(line,0,sizeof(line));
					linesRead++;
					line[0] = c;
					lineIndex = 1;
					seperatorPos = 0;
				}
				nFlag = false;
			}
			else
			{
				line[lineIndex] = c;
				lineIndex++;
			}
		}
		//LAST LINE IS END:VCALENDAR AKA VALID ENDING
		((*obj)->properties) = list;
		((*obj)->events) = events;
		if(objEvent == true)
		{
			tmpEvent->properties = initializeList(&printProperty, &deleteProperty, &compareProperties);
			tmpEvent->alarms = initializeList(&printAlarm, &deleteAlarm, &compareProperties);
			freeList(eventProps);
			freeList(alarms);
			deleteEvent(tmpEvent);
			deleteCalendar(*obj);
			fclose(fp);
			return INV_EVENT;
		}
		if(strcmp(line, "END:VCALENDAR")==0)
		{
			objCal = false;
		}
		if(objCal == true || prodIDFound == false || versionFound == false || eventFound == false)
		{
			deleteCalendar(*obj);
			fclose(fp);
			return INV_CAL;
		}
	}
	fclose(fp);
	return OK;
}

void deleteCalendar(Calendar* obj)
{
	if(obj != NULL)
	{
		freeList(obj->properties);
		freeList(obj->events);
		free(obj);
	}
}

char* printCalendar(const Calendar* obj)
{
	char *returnStr = NULL;
	char *temp = NULL;

	if(obj != NULL)
	{
		if(strlen(obj->prodID) > 0)
		{
		returnStr = (char*)malloc(sizeof(char)*(24+4+14+strlen(obj->prodID)+1));
		sprintf(returnStr, "Calendar Info:\nVersion->%.2lf, Product ID->%s", obj->version, obj->prodID);
		}
		if(obj->properties->length > 0)
		{
			temp = toString(obj->properties);
			returnStr = (char*)realloc(returnStr, strlen(returnStr) + strlen(temp)+1);
			strcat(returnStr, temp);
			free(temp);
		}
		if(obj->events->length > 0)
		{
			temp = toString(obj->events);
			returnStr = (char*)realloc(returnStr, strlen(returnStr) + strlen(temp)+1);
			strcat(returnStr, temp);
			free(temp);
		}
	}
	return returnStr;
}

char* printError(ICalErrorCode err)
{
	switch(err)
	{
		case OK: return "OK\n";
		case INV_FILE: return "INV_FILE\n";
		case INV_CAL: return "INV_CAL\n";
		case INV_VER: return "INV_VER\n";
		case DUP_VER: return "DUP_VER\n";
		case INV_PRODID: return "INV_PRODID\n";
		case DUP_PRODID: return "DUP_PRODID\n";
		case INV_EVENT: return "INV_EVENT\n";
		case INV_DT: return "INV_DT\n";
		case INV_ALARM: return "INV_ALARM\n";
		case WRITE_ERROR: return "WRITE_ERROR\n";
		case OTHER_ERROR: return "OTHER_ERROR\n";
		default: return "default\n";
	}
}

ICalErrorCode writeCalendar(char* fileName, const Calendar* obj)
{
	//initiate
	FILE *fp = NULL;

	if(fileName == NULL)
	{
		return WRITE_ERROR;
	}
	if(strstr(fileName,".ics") == NULL)
	{
		return WRITE_ERROR;
	}
	if(obj == NULL)
	{
		return WRITE_ERROR;
	}

	fp = fopen(fileName, "w");
	if(fp == NULL)
	{
		return WRITE_ERROR;
	}
	//cal start
	fprintf(fp, "BEGIN:VCALENDAR\r\nVERSION:%.1f\r\nPRODID:%s\r\n",obj->version,obj->prodID);
	//cal props
	fPrintPropertyList(obj->properties, fp);
	fPrintEventsList(obj->events, fp);
	//cal end
	fprintf(fp, "END:VCALENDAR\r\n");

	fclose(fp);
	return OK;
}

/** Function to validating an existing a Calendar object
 *@pre Calendar object exists and is not NULL
 *@post Calendar has not been modified in any way
 *@return the error code indicating success or the error encountered when validating the calendar
 *@param obj - a pointer to a Calendar struct
 **/
ICalErrorCode validateCalendar(const Calendar* obj)
{
	//invalid calendar checks
	if(obj == NULL)
	{
		return INV_CAL;
	}
	if(obj->version == 0 || (strlen(obj->prodID) == 0 || strlen(obj->prodID) > 999))
	{
		return INV_CAL;
	}
	if(obj->events == NULL)
	{
		return INV_CAL;
	}
	if(getLength(obj->events) == 0)
	{
		return INV_CAL;
	}
	if(obj->properties == NULL)
	{
		return INV_CAL;
	}
	if(validCalProps(obj->properties))
	{
		return INV_CAL;
	}
	if(validEvents(obj->events) == 1)
	{
			return INV_EVENT;
	}
	else if(validEvents(obj->events) == 2)
	{
		return INV_ALARM;
	}
	return OK;
}

/** Function to converting a DateTime into a JSON string
 *@pre N/A
 *@post DateTime has not been modified in any way
 *@return A string in JSON format
 *@param prop - a DateTime struct
 **/
char* dtToJSON(DateTime prop)
{
	char* str = NULL;
	if(prop.UTC == true)
	{
		str = (char*)malloc(49*sizeof(char));
		sprintf(str, "{\"date\":\"%s\",\"time\":\"%s\",\"isUTC\":true}", prop.date, prop.time);
	}
	else
	{
		str = (char*)malloc(50*sizeof(char));
		sprintf(str, "{\"date\":\"%s\",\"time\":\"%s\",\"isUTC\":false}", prop.date, prop.time);
	}
	return str;
}

char* propertyToJSON(const Property* prop)
{
	char* str = NULL;

	if(prop == NULL)
	{
		str = (char*)(malloc(3*sizeof(char)));
		strcpy(str, "{}");
		return str;
	}

	str = (char*)malloc((9+strlen(prop->propName)+11+strlen(prop->propDescr)+2+1)*sizeof(char));
	sprintf(str, "{\"name\":\"%s\",\"descr\":\"%s\"}",prop->propName, prop->propDescr);
	return str;
}

//action
//triger
//#ofproperties
char* alarmToJSON(const Alarm* alarm)
{
	char* str = NULL;

	if(alarm == NULL)
	{
		str = (char*)(malloc(3*sizeof(char)));
		strcpy(str, "{}");
		return str;
	}

	str = (char*)malloc((10+strlen(alarm->action)+11+strlen(alarm->trigger)+13+getLengthOfInt(getLength(alarm->properties)+2)+5)*sizeof(char));
	sprintf(str, "{\"action\":\"%s\",\"trigger\":\"%s\",\"numProps\":%d}",alarm->action, alarm->trigger, getLength(alarm->properties)+2);
	return str;
}

/** Function to converting an Event into a JSON string
 *@pre Event is not NULL
 *@post Event has not been modified in any way
 *@return A string in JSON format
 *@param event - a pointer to an Event struct
 **/
char* eventToJSON(const Event* event)
{
	char* dtHolder = NULL;
	char* sumHolder = NULL;
	char* str = NULL;

	if(event == NULL)
	{
		str = (char*)(malloc(3*sizeof(char)));
		strcpy(str, "{}");
		return str;
	}

	dtHolder = dtToJSON(event->startDateTime);
	sumHolder = findSummary(event->properties);


	if(sumHolder == NULL)
	{
		str = (char*)malloc((11+strlen(dtHolder)+12+getLengthOfInt(getLength(event->properties)+3)+13+getLengthOfInt(getLength(event->alarms))+14+1)*sizeof(char));
		sprintf(str, "{\"startDT\":%s,\"numProps\":%d,\"numAlarms\":%d,\"summary\":\"\"}",dtHolder, getLength(event->properties)+3, getLength(event->alarms));
	}
	else
	{
		str = (char*)malloc((11+strlen(dtHolder)+12+getLengthOfInt(getLength(event->properties)+3)+13+getLengthOfInt(getLength(event->alarms))+14+strlen(sumHolder)+1)*sizeof(char));
		sprintf(str, "{\"startDT\":%s,\"numProps\":%d,\"numAlarms\":%d,\"summary\":\"%s\"}",dtHolder, getLength(event->properties)+3, getLength(event->alarms), sumHolder);
	}
	free(dtHolder);
	free(sumHolder);
	return str;
}

char* eventToJSON2(const Event* event)
{
	char* dtHolder = NULL;
	char* sumHolder = NULL;
	char* alHolder = NULL;
	char* orgHolder = NULL;
	char* locHolder = NULL;
	char* str = NULL;

	if(event == NULL)
	{
		str = (char*)(malloc(3*sizeof(char)));
		strcpy(str, "{}");
		return str;
	}

	dtHolder = dtToJSON(event->startDateTime);
	sumHolder = findSummary(event->properties);
	alHolder = alarmListToJSON(event->alarms);
	orgHolder = findOrg(event->properties);
	locHolder = findLoc(event->properties);


	if(sumHolder == NULL && orgHolder == NULL && locHolder == NULL)
	{
		str = (char*)malloc((11+strlen(dtHolder)+12+getLengthOfInt(getLength(event->properties)+3)+13+getLengthOfInt(getLength(event->alarms))+14+1+10+29+strlen(alHolder))*sizeof(char));
		sprintf(str, "{\"startDT\":%s,\"numProps\":%d,\"numAlarms\":%d,\"summary\":\"\",\"location\":\"\",\"organizer\":\"\",\"alarms\":%s}",dtHolder, getLength(event->properties)+3, getLength(event->alarms), alHolder);
	}
	else if(sumHolder != NULL && orgHolder == NULL && locHolder == NULL)
	{
		str = (char*)malloc((11+strlen(dtHolder)+12+getLengthOfInt(getLength(event->properties)+3)+13+getLengthOfInt(getLength(event->alarms))+14+strlen(sumHolder)+1+10+29+strlen(alHolder))*sizeof(char));
		sprintf(str, "{\"startDT\":%s,\"numProps\":%d,\"numAlarms\":%d,\"summary\":\"%s\",\"location\":\"\",\"organizer\":\"\",\"alarms\":%s}",dtHolder, getLength(event->properties)+3, getLength(event->alarms), sumHolder, alHolder);
	}
	else if(sumHolder == NULL && orgHolder != NULL && locHolder == NULL)
	{
		str = (char*)malloc((11+strlen(dtHolder)+12+getLengthOfInt(getLength(event->properties)+3)+13+getLengthOfInt(getLength(event->alarms))+14+1+10+29+strlen(orgHolder)+strlen(alHolder))*sizeof(char));
		sprintf(str, "{\"startDT\":%s,\"numProps\":%d,\"numAlarms\":%d,\"summary\":\"\",\"location\":\"\",\"organizer\":\"%s\",\"alarms\":%s}",dtHolder, getLength(event->properties)+3, getLength(event->alarms), orgHolder, alHolder);
	}
	else if(sumHolder == NULL && orgHolder == NULL && locHolder != NULL)
	{
		str = (char*)malloc((11+strlen(dtHolder)+12+getLengthOfInt(getLength(event->properties)+3)+13+getLengthOfInt(getLength(event->alarms))+14+1+10+29+strlen(locHolder)+strlen(alHolder))*sizeof(char));
		sprintf(str, "{\"startDT\":%s,\"numProps\":%d,\"numAlarms\":%d,\"summary\":\"\",\"location\":\"%s\",\"organizer\":\"\",\"alarms\":%s}",dtHolder, getLength(event->properties)+3, getLength(event->alarms), locHolder, alHolder);
	}
	else if(sumHolder != NULL && orgHolder != NULL && locHolder == NULL)
	{
		str = (char*)malloc((11+strlen(dtHolder)+12+getLengthOfInt(getLength(event->properties)+3)+13+getLengthOfInt(getLength(event->alarms))+14+1+10+29+strlen(sumHolder)+strlen(orgHolder)+strlen(alHolder))*sizeof(char));
		sprintf(str, "{\"startDT\":%s,\"numProps\":%d,\"numAlarms\":%d,\"summary\":\"%s\",\"location\":\"\",\"organizer\":\"%s\",\"alarms\":%s}",dtHolder, getLength(event->properties)+3, getLength(event->alarms), sumHolder, orgHolder, alHolder);
	}
	else if(sumHolder != NULL && orgHolder == NULL && locHolder != NULL)
	{
		str = (char*)malloc((11+strlen(dtHolder)+12+getLengthOfInt(getLength(event->properties)+3)+13+getLengthOfInt(getLength(event->alarms))+14+1+10+29+strlen(sumHolder)+strlen(locHolder)+strlen(alHolder))*sizeof(char));
		sprintf(str, "{\"startDT\":%s,\"numProps\":%d,\"numAlarms\":%d,\"summary\":\"%s\",\"location\":\"%s\",\"organizer\":\"\",\"alarms\":%s}",dtHolder, getLength(event->properties)+3, getLength(event->alarms), sumHolder, locHolder, alHolder);
	}
	else if(sumHolder == NULL && orgHolder != NULL && locHolder != NULL)
	{
		str = (char*)malloc((11+strlen(dtHolder)+12+getLengthOfInt(getLength(event->properties)+3)+13+getLengthOfInt(getLength(event->alarms))+14+1+10+29+strlen(orgHolder)+strlen(locHolder)+strlen(alHolder))*sizeof(char));
		sprintf(str, "{\"startDT\":%s,\"numProps\":%d,\"numAlarms\":%d,\"summary\":\"\",\"location\":\"%s\",\"organizer\":\"%s\",\"alarms\":%s}",dtHolder, getLength(event->properties)+3, getLength(event->alarms), locHolder, orgHolder, alHolder);
	}
	else
	{
		str = (char*)malloc((11+strlen(dtHolder)+12+getLengthOfInt(getLength(event->properties)+3)+13+getLengthOfInt(getLength(event->alarms))+14+strlen(sumHolder)+1+10+29+strlen(locHolder)+strlen(orgHolder)+strlen(alHolder))*sizeof(char));
		sprintf(str, "{\"startDT\":%s,\"numProps\":%d,\"numAlarms\":%d,\"summary\":\"%s\",\"location\":\"%s\",\"organizer\":\"%s\",\"alarms\":%s}",dtHolder, getLength(event->properties)+3, getLength(event->alarms), sumHolder, locHolder, orgHolder, alHolder);
	}
	//\"location\":\"%s\",\"organizer\":\"%s\", 29
	free(dtHolder);
	free(sumHolder);
	free(alHolder);
	free(orgHolder);
	free(locHolder);

	return str;
}

char* propListToJSON(const List* eventList)
{
	Property* tmpEvent;
	char* str = NULL;
	char* tmp = NULL;
	int size = 1;

	if(eventList == NULL)
	{
		str = (char*)(malloc(sizeof(char)*3));
		strcpy(str, "[]");
		return str;
	}
	Node* tmpNode = eventList->head;
	size++;
	str = (char*)malloc(sizeof(char)*size);
	strcpy(str, "[");

	for(int i = 0; i < getLength((List*)eventList); i++)
	{
		tmpEvent = tmpNode->data;
		tmp = propertyToJSON(tmpEvent);
		size = size + strlen(tmp);
		str = (char*)realloc(str, size);
		strcat(str,tmp);
		free(tmp);
		tmpNode = tmpNode->next;
		if(tmpNode != NULL)
		{
			size++;
			str = (char*)realloc(str, size);
			strcat(str,",");
		}
	}
	size++;
	str = (char*)realloc(str, size);
	strcat(str,"]\0");
	return str;
}

//action
//triger
//#ofproperties
char* alarmListToJSON(const List* eventList)
{
	Alarm* tmpEvent;
	char* str = NULL;
	char* tmp = NULL;
	int size = 1;

	if(eventList == NULL)
	{
		str = (char*)(malloc(sizeof(char)*3));
		strcpy(str, "[]");
		return str;
	}
	Node* tmpNode = eventList->head;
	size++;
	str = (char*)malloc(sizeof(char)*size);
	strcpy(str, "[");

	for(int i = 0; i < getLength((List*)eventList); i++)
	{
		tmpEvent = tmpNode->data;
		tmp = alarmToJSON(tmpEvent);
		size = size + strlen(tmp);
		str = (char*)realloc(str, size);
		strcat(str,tmp);
		free(tmp);
		tmpNode = tmpNode->next;
		if(tmpNode != NULL)
		{
			size++;
			str = (char*)realloc(str, size);
			strcat(str,",");
		}
	}
	size++;
	str = (char*)realloc(str, size);
	strcat(str,"]\0");
	return str;
}

/** Function to converting an Event list into a JSON string
 *@pre Event list is not NULL
 *@post Event list has not been modified in any way
 *@return A string in JSON format
 *@param eventList - a pointer to an Event list
 **/
char* eventListToJSON(const List* eventList)
{
	Event* tmpEvent;
	char* str = NULL;
	char* tmp = NULL;
	int size = 1;

	if(eventList == NULL)
	{
		str = (char*)(malloc(sizeof(char)*3));
		strcpy(str, "[]");
		return str;
	}
	Node* tmpNode = eventList->head;
	size++;
	str = (char*)malloc(sizeof(char)*size);
	strcpy(str, "[");

	for(int i = 0; i < getLength((List*)eventList); i++)
	{
		tmpEvent = tmpNode->data;
		tmp = eventToJSON(tmpEvent);
		size = size + strlen(tmp);
		str = (char*)realloc(str, size);
		strcat(str,tmp);
		free(tmp);
		tmpNode = tmpNode->next;
		if(tmpNode != NULL)
		{
			size++;
			str = (char*)realloc(str, size);
			strcat(str,",");
		}
	}
	size++;
	str = (char*)realloc(str, size);
	strcat(str,"]\0");
	return str;
}

char* eventListToJSON2(const List* eventList)
{
	Event* tmpEvent;
	char* str = NULL;
	char* tmp = NULL;
	int size = 1;

	if(eventList == NULL)
	{
		str = (char*)(malloc(sizeof(char)*3));
		strcpy(str, "[]");
		return str;
	}
	Node* tmpNode = eventList->head;
	size++;
	str = (char*)malloc(sizeof(char)*size);
	strcpy(str, "[");

	for(int i = 0; i < getLength((List*)eventList); i++)
	{
		tmpEvent = tmpNode->data;
		tmp = eventToJSON2(tmpEvent);
		size = size + strlen(tmp);
		str = (char*)realloc(str, size);
		strcat(str,tmp);
		free(tmp);
		tmpNode = tmpNode->next;
		if(tmpNode != NULL)
		{
			size++;
			str = (char*)realloc(str, size);
			strcat(str,",");
		}
	}
	size++;
	str = (char*)realloc(str, size);
	strcat(str,"]\0");
	return str;
}

/** Function to converting a Calendar into a JSON string
 *@pre Calendar is not NULL
 *@post Calendar has not been modified in any way
 *@return A string in JSON format
 *@param cal - a pointer to a Calendar struct
 **/
char* calendarToJSON(const Calendar* cal)
{
	char* str = NULL;
	int ver = 0;

	if(cal == NULL)
	{
		str = (char*)malloc(sizeof(char)*3);
		strcpy(str, "{}");
		return str;
	}

	ver = (int)cal->version;
	str = (char*)malloc((11+1+11+strlen(cal->prodID)+13+getLengthOfInt(getLength(cal->properties)+2)+13+getLengthOfInt(getLength(cal->events))+2)*sizeof(char));
	sprintf(str, "{\"version\":%d,\"prodID\":\"%s\",\"numProps\":%d,\"numEvents\":%d}", ver, cal->prodID, getLength(cal->properties)+2, getLength(cal->events));
	return str;
}

/** Function to converting a JSON string into a Calendar struct
 *@pre JSON string is not NULL
 *@post String has not been modified in any way
 *@return A newly allocated and partially initialized Calendar struct
 *@param str - a pointer to a string
 **/
Calendar* JSONtoCalendar(const char* str)
{
	Calendar* obj = NULL;

	bool ver = false;

	bool flag1 = false;
	bool flag2 = false;

	bool verDone = false;
	char version[20];
	char prodID[200];
	int verCount = 0;
	int proCount = 0;
	if(str == NULL)
	{
		return NULL;
	}
	for(int i = 0; i < strlen(str); i++)
	{
		if(str[i] == ',' && ver == true)
		{
			verDone = true;
			ver = false;
		}
		if(ver == true)
		{
			version[verCount++] = str[i];
		}
		if(str[i] == ':' && verDone == false && ver == false)
		{
			ver = true;
		}


		if(flag2 == true && str[i] == '\"')
		{
			flag2 = false;
		}

		if(flag2 == true)
		{
			prodID[proCount++] = str[i];
		}

		if(flag1 == true)
		{
			flag2 = true;
			flag1 = false;
		}
		if(verDone == true && str[i] == ':')
		{
			flag1 = true;
		}
	}
	prodID[proCount] = '\0';
	version[verCount] = '\0';

	if(strlen(prodID) == 0 ||strlen(version) == 0)
	{
		return NULL;
	}

	obj = (Calendar*)malloc(sizeof(Calendar));
	obj->version = atof(version);
	strcpy(obj->prodID, prodID);
	obj->properties = initializeList(&printProperty, &deleteProperty, &compareProperties);
	obj->events = initializeList(&printEvent, &deleteEvent, &compareProperties);
		
	return obj;
}

/** Function to converting a JSON string into an Event struct
 *@pre JSON string is not NULL
 *@post String has not been modified in any way
 *@return A newly allocated and partially initialized Event struct
 *@param str - a pointer to a string
 **/
Event* JSONtoEvent(const char* str)
{
	Event* event = NULL;
	Property* tmpProp = NULL;
	char UID[200];
	char startDT[200];
	char creationDT[200];
	char summary[200];
	bool parse1 = false;
	bool parse2 = false;
	bool parseUID = true;
	bool parseStart = false;
	bool parseCreat = false;
	bool parseSummary = false;
	bool parseEnd = false;
	int place = 0;

	if(str == NULL)
	{
		return NULL;
	}

	for(int i = 0; i < strlen(str); i++)
	{
		//{"uid":"NICK","startDT":{"date":"33445566","isUTC":false,"time":"445566"},"creationDT":{"date":"22334455","isUTC":true,"time":"223344"}}
		if(str[i] == '\"' && parse2 == true && parseUID == true)
		{
			parse2 = false;
			parseUID = false;
			parseStart = true;
			UID[place] = '\0';
			place = 0;
		}
		if(parse2 == true && parseUID == true)
		{
			UID[place++] = str[i];
		}
		if(str[i] == '\"' && parse1 == true && parseUID == true)
		{
			parse2 = true;
			parse1 = false;
		}
		if(str[i] == ':' && parseUID == true)
		{
			parse1 = true;
		}
		//////////////":{"date":"33445566","isUTC":false,"time":"445566"},
		if(str[i] == ',' && parse2 == true && parseStart == true && parseEnd == true)
		{
			parse2 = false;
			parseStart = false;
			parseCreat = true;
			startDT[place] = '\0';
			place = 0;
			parseEnd = false;
		}
		if(parse2 == true && parseStart == true)
		{
			startDT[place++] = str[i];
		}
		if(str[i] == '}' && parse2 == true && parseStart == true)
		{
			parseEnd = true;
		}
		if(str[i] == ':' && parse1 == true && parseStart == true)
		{
			parse2 = true;
			parse1 = false;
		}
		if(str[i] == '\"' && parseStart == true)
		{
			parse1 = true;
		}

		////////
		if(((str[i] == '}') || (str[i] == ',')) && parse2 == true && parseCreat == true && parseEnd == true)
		{
			parse2 = false;
			parseCreat = false;
			creationDT[place] = '\0';
			place = 0;
			if(str[i] == ',')
			{
				parseSummary = true;
				parse1 = false;
				parse2 = false;
			}
		}
		if(parse2 == true && parseCreat == true)
		{
			creationDT[place++] = str[i];
		}
		if(str[i] == '}' && parse2 == true && parseCreat == true)
		{
			parseEnd = true;
		}
		if(str[i] == ':' && parse1 == true && parseCreat == true)
		{
			parse2 = true;
			parse1 = false;
		}
		if(str[i] == '\"' && parseCreat == true)
		{
			parse1 = true;
		}
		/////
		if(str[i] == '\"' && parse2 == true && parseSummary == true)
		{
			parse2 = false;
			parseSummary = false;
			summary[place] = '\0';
			place = 0;
		}
		if(parse2 == true && parseSummary == true)
		{
			summary[place++] = str[i];
		}
		if(str[i] == '\"' && parse1 == true && parseSummary == true)
		{
			parse2 = true;
			parse1 = false;
		}
		if(str[i] == ':' && parseSummary == true && parse1 == false && parse2 == false)
		{
			parse1 = true;
		}

	}
	if(strlen(UID) == 0)
	{
		return NULL;
	}

	event = (Event*)malloc(sizeof(Event));
	strcpy(event->UID, UID);
	event->alarms = initializeList(&printAlarm, &deleteAlarm, &compareProperties);
	event->properties = initializeList(&printProperty, &deleteProperty, &compareProperties);
	if(strlen(summary) > 0)
	{
		tmpProp = (Property*)malloc(sizeof(Property)+((strlen(summary)+1)*(sizeof(char))));
		strcpy(tmpProp->propName, "SUMMARY\0");
		strcpy(tmpProp->propDescr, summary);
		insertBack(event->properties, (void*)tmpProp);
	}
	memset(event->creationDateTime.date,0,sizeof(event->creationDateTime.date));
	memset(event->creationDateTime.time,0,sizeof(event->creationDateTime.time));
	event->creationDateTime.UTC = false;
	memset(event->startDateTime.date,0,sizeof(event->startDateTime.date));
	memset(event->startDateTime.time,0,sizeof(event->startDateTime.time));
	event->startDateTime.UTC = false;
	
	event->creationDateTime = JSONtoAlarm(creationDT);
	event->startDateTime = JSONtoAlarm(startDT);
	return event;
}

DateTime JSONtoAlarm(char* str)
{
	DateTime newDT;
	char date[9]; 
	//hhmmss.   Must not exceed the array length.   Must not be an empty string.
	char time[7]; 
	//indicates whether this is UTC time
	char isUTC[6];
	bool parse1 = false;
	bool parse2 = false;
	bool parseDate = true;
	bool parseTime = false;
	bool parseUTC = false;
	int place = 0;

	for(int i = 0; i < strlen(str); i++)
	{
		//{"uid":"NICK","startDT":{"date":"33445566","isUTC":false,"time":"445566"},"creationDT":{"date":"22334455","isUTC":true,"time":"223344"}}
		if(str[i] == '\"' && parse2 == true && parseDate == true)
		{
			parse2 = false;
			parseDate = false;
			parseUTC = true;
			date[place] = '\0';
			place = 0;
		}
		if(parse2 == true && parseDate == true)
		{
			date[place++] = str[i];
		}
		if(str[i] == '\"' && parse1 == true && parseDate == true)
		{
			parse2 = true;
			parse1 = false;
		}
		if(str[i] == ':' && parseDate == true)
		{
			parse1 = true;
		}


		//------------
		if(str[i] == ',' && parse2 == true && parseUTC == true)
		{
			parse2 = false;
			parseUTC = false;
			parseTime = true;
			isUTC[place] = '\0';
			place = 0;
		}
		if(parse2 == true && parseUTC == true)
		{
			isUTC[place++] = str[i];
		}
		if(str[i] == ':' && parse1 == true && parseUTC == true)
		{
			parse2 = true;
			parse1 = false;
		}
		if(str[i] == '\"' && parseUTC == true)
		{
			parse1 = true;
		}

		//---------

		if(str[i] == '\"' && parse2 == true && parseTime == true)
		{
			parse2 = false;
			parseUTC = false;
			parseTime = true;
			time[place] = '\0';
			place = 0;
		}
		if(parse2 == true && parseTime == true)
		{
			time[place++] = str[i];
		}
		if(str[i] == '\"' && parse1 == true && parseTime == true)
		{
			parse2 = true;
			parse1 = false;
		}
		if(str[i] == ':' && parseTime == true)
		{
			parse1 = true;
		}

	}
	strcpy(newDT.date, date);
	strcpy(newDT.time, time);
	if(strcmp(isUTC, "false") == 0)
	{
		newDT.UTC = false;
	}
	else if(strcmp(isUTC, "true") == 0)
	{
		newDT.UTC = true;
	}

	return newDT;
	
}

/** Function to adding an Event struct to an ixisting Calendar struct
 *@pre arguments are not NULL
 *@post The new event has been added to the calendar's events list
 *@return N/A
 *@param cal - a Calendar struct
 *@param toBeAdded - an Event struct
 **/
void addEvent(Calendar* cal, Event* toBeAdded)
{
	if(cal != NULL && toBeAdded != NULL)
	{
		insertBack(cal->events, (void*)toBeAdded);
	}
}

char* createCalToJSON(char *fileName)
{
	Calendar* iCal = NULL;
	char* calInfo = NULL;
	if(createCalendar(fileName, &iCal) == OK)
	{
		calInfo = calendarToJSON(iCal);
		deleteCalendar(iCal);
		return calInfo;
	}
	return NULL;
}

char* createEventListToJSON(char *fileName)
{
	Calendar* iCal = NULL;
	char* eventsInfo = NULL;
	if(createCalendar(fileName, &iCal) == OK)
	{
		eventsInfo = eventListToJSON(iCal->events);
		deleteCalendar(iCal);
		return eventsInfo;
	}
	return NULL;
}

char* createEventListToJSON2(char *fileName)
{
	Calendar* iCal = NULL;
	char* eventsInfo = NULL;
	if(createCalendar(fileName, &iCal) == OK)
	{
		eventsInfo = eventListToJSON2(iCal->events);
		deleteCalendar(iCal);
		return eventsInfo;
	}
	return NULL;
}

char* createAlarmListToJSON(char *fileName, int evtNum)
{
	Calendar* iCal = NULL;
	char* eventsInfo = NULL;
	int iter = evtNum-1;
	Node* events = NULL;
	Event* tmp = NULL;
	if(createCalendar(fileName, &iCal) == OK && evtNum > 0)
	{
		events = iCal->events->head;
		for(int i = 0; i < iter; i++)
		{
			events = events->next;
		}
		tmp = (Event*)events->data;
		eventsInfo = alarmListToJSON(tmp->alarms);
		deleteCalendar(iCal);
		return eventsInfo;
	}
	return NULL;
}

char* createPropListToJSON(char *fileName, int evtNum)
{
	Calendar* iCal = NULL;
	char* eventsInfo = NULL;
	int iter = evtNum-1;
	Node* events = NULL;
	Event* tmp = NULL;
	if(createCalendar(fileName, &iCal) == OK && evtNum > 0)
	{
		events = iCal->events->head;
		for(int i = 0; i < iter; i++)
		{
			events = events->next;
		}
		tmp = (Event*)events->data;
		eventsInfo = propListToJSON(tmp->properties);
		deleteCalendar(iCal);
		return eventsInfo;
	}
	return NULL;
}

void createCalFile(char *fileName, char *calInfo, char *event)
{
	Calendar* iCal = JSONtoCalendar(calInfo);
	addEvent(iCal, JSONtoEvent(event));
	if(validateCalendar(iCal) == OK)
	{
		writeCalendar(fileName, iCal);
	}
	deleteCalendar(iCal);
}

void addEventToCal(char *fileName, char*event)
{
	Calendar* iCal = NULL;
	if(createCalendar(fileName, &iCal) == OK)
	{
		addEvent(iCal, JSONtoEvent(event));
	}
	if(validateCalendar(iCal) == OK)
	{
		writeCalendar(fileName, iCal);
	}
	if(iCal != NULL)
	{
		deleteCalendar(iCal);
	}
}

