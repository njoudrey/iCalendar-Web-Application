#include "helpers.h"

void fPrintPropertyList(List* props, FILE *fp)
{
	Node* tmp = props->head;
	Property* tmpProp;
	for(int i = 0; i < getLength(props); i++)
	{
		tmpProp = (Property*)(tmp->data);
		fprintf(fp, "%s:%s\r\n",tmpProp->propName,tmpProp->propDescr);
		tmp = tmp->next;
	}
}

void fPrintAlarmsList(List* alarms, FILE *fp)
{
	Node* alarmNode = alarms->head;
	Alarm* tmpAlarm;
	for(int i = 0; i < getLength(alarms); i++)
	{
		tmpAlarm = (Alarm*)(alarmNode->data);

		fprintf(fp, "BEGIN:VALARM\r\nACTION:%s\r\nTRIGGER:%s\r\n", tmpAlarm->action,tmpAlarm->trigger);
		fPrintPropertyList(tmpAlarm->properties, fp);
		fprintf(fp, "END:VALARM\r\n");
		alarmNode = alarmNode->next;
	}
}

void fPrintEventsList(List* events, FILE *fp)
{
	Node* eventNode = events->head;
	Event* tmpEvent;
	char* tmp = NULL;
	for(int i = 0; i < getLength(events); i++)
	{
		tmpEvent = (Event*)(eventNode->data);
		//musthaves
		fprintf(fp, "BEGIN:VEVENT\r\nUID:%s\r\n", tmpEvent->UID);
		tmp = dateTimeAssemble(tmpEvent->creationDateTime);
		if(strlen(tmp) == 15 || strlen(tmp) == 16)
		{
			fprintf(fp, "DTSTAMP:%s\r\n", tmp);
		}
		free(tmp);
		tmp = dateTimeAssemble(tmpEvent->startDateTime);
		if(strlen(tmp) == 15 || strlen(tmp) == 16)
		{
			fprintf(fp, "DTSTART:%s\r\n", tmp);
		}
		free(tmp);
		//props
		fPrintPropertyList(tmpEvent->properties, fp);
		//alarms
		fPrintAlarmsList(tmpEvent->alarms, fp);
		fprintf(fp, "END:VEVENT\r\n");
		eventNode = eventNode->next;
	}
}

char* dateTimeAssemble(DateTime dt)
{
	char* dateTime = NULL;
	if(dt.UTC == true)
	{
		dateTime = (char*)malloc(17*sizeof(char));
		sprintf(dateTime, "%sT%sZ", dt.date, dt.time);
	}
	else
	{
		dateTime = (char*)malloc(16*sizeof(char));
		sprintf(dateTime, "%sT%s", dt.date, dt.time);
	}
	return dateTime;
}
//return 0 if valid
int validCalProps(List* props)
{	
	Node* tmp = props->head;
	Property* tmpProp;
	bool calscale = false;
	bool method = false;
	for(int i = 0; i < getLength(props); i++)
	{
		tmpProp = (Property*)(tmp->data);
		//empty fields
		if((strlen(tmpProp->propName) == 0 || strlen(tmpProp->propName) > 199)|| strlen(tmpProp->propDescr) == 0)
		{
			return 1;
		}
		if(strcmp(tmpProp->propName, "CALSCALE") != 0 && strcmp(tmpProp->propName, "METHOD") != 0)
		{
			return 1;
		}
		else
		{
			if(strcmp(tmpProp->propName, "CALSCALE") == 0 && calscale == true)
			{
				return 1;
			}
			if(strcmp(tmpProp->propName, "METHOD") == 0 && method == true)
			{
				return 1;
			}
			if(strcmp(tmpProp->propName, "CALSCALE") == 0)
			{
				calscale = true;
			}
			if(strcmp(tmpProp->propName, "METHOD") == 0)
			{
				method = true;
			}

		}
		tmp = tmp->next;
	}
	return 0;
}
//return 0 if good
int validEvents(List* events)
{
	Node* eventNode = events->head;
	Event* tmpEvent;
	char* tmp = NULL;
	for(int i = 0; i < getLength(events); i++)
	{
		tmpEvent = (Event*)(eventNode->data);
		if(strlen(tmpEvent->UID) == 0 || strlen(tmpEvent->UID) > 999)
		{
			return 1;
		}
		tmp = dateTimeAssemble(tmpEvent->creationDateTime);
		if(strlen(tmp) != 15 && strlen(tmp) != 16)
		{
			free(tmp);
			return 1;
		}
		free(tmp);

		tmp = dateTimeAssemble(tmpEvent->startDateTime);
		if(strlen(tmp) != 15 && strlen(tmp) != 16)
		{
			free(tmp);
			return 1;
		}
		free(tmp);
		if(tmpEvent->properties == NULL)
		{
			return 1;
		}
		if(validEventProps(tmpEvent->properties))
		{
			return 1;
		}
		if(tmpEvent->alarms == NULL)
		{
			return 1;
		}
		if(validAlarms(tmpEvent->alarms))
		{
			return 2;
		}
		eventNode = eventNode->next;
	}
	return 0;
}

int validEventProps(List* props)
{
	Node* tmp = props->head;
	Property* tmpProp;
	bool class = false;
	bool created = false;
	bool desc = false;
	bool geo = false;
	bool lastmod = false;
	bool location = false;
	bool organizer = false;
	bool priority = false;
	bool seq = false;
	bool status = false;
	bool summary = false;
	bool transp = false;
	bool url = false;
	bool recurid = false;

	bool dtenddur = false;

	for(int i = 0; i < getLength(props); i++)
	{
		tmpProp = (Property*)(tmp->data);
		//empty fields
		if(strlen(tmpProp->propName) == 0 || strlen(tmpProp->propName) > 199 || strlen(tmpProp->propDescr) == 0)
		{
			return 1;
		}
		if(strcmp(tmpProp->propName, "CLASS") != 0 && strcmp(tmpProp->propName, "CREATED") != 0 && strcmp(tmpProp->propName, "DESCRIPTION") != 0 && strcmp(tmpProp->propName, "GEO") != 0 && strcmp(tmpProp->propName, "LAST-MODIFIED") != 0 && strcmp(tmpProp->propName, "LOCATION") != 0 && strcmp(tmpProp->propName, "ORGANIZER") != 0 && strcmp(tmpProp->propName, "PRIORITY") != 0 && strcmp(tmpProp->propName, "SEQUENCE") != 0 && strcmp(tmpProp->propName, "STATUS") != 0 && strcmp(tmpProp->propName, "SUMMARY") != 0 && strcmp(tmpProp->propName, "TRANSP") != 0 && strcmp(tmpProp->propName, "URL") != 0 && strcmp(tmpProp->propName, "RECURRENCE-ID") != 0 && strcmp(tmpProp->propName, "RRULE") != 0 && strcmp(tmpProp->propName, "DTEND") != 0 && strcmp(tmpProp->propName, "DURATION") != 0 && strcmp(tmpProp->propName, "ATTACH") != 0 && strcmp(tmpProp->propName, "ATTENDEE") != 0 && strcmp(tmpProp->propName, "CATEGORIES") != 0 && strcmp(tmpProp->propName, "COMMENT") != 0 && strcmp(tmpProp->propName, "CONTACT") != 0 && strcmp(tmpProp->propName, "EXDATE") != 0 && strcmp(tmpProp->propName, "RSTATUS") != 0 && strcmp(tmpProp->propName, "RELATED") != 0 && strcmp(tmpProp->propName, "RESOURCES") != 0 && strcmp(tmpProp->propName, "RDATE") != 0)
		{
			return 1;
		}
		else
		{
			if(strcmp(tmpProp->propName, "CLASS") == 0 && class == true)
			{
				return 1;
			}
			if(strcmp(tmpProp->propName, "CREATED") == 0  && created == true)
			{
				return 1;
			}
			if(strcmp(tmpProp->propName, "DESCRIPTION") == 0  && desc == true)
			{
				return 1;
			}
			if(strcmp(tmpProp->propName, "GEO") == 0  && geo == true)
			{
				return 1;
			}
			if(strcmp(tmpProp->propName, "LAST-MODIFIED") == 0  && lastmod == true)
			{
				return 1;
			}
			if(strcmp(tmpProp->propName, "LOCATION") == 0  && location == true)
			{
				return 1;
			}
			if(strcmp(tmpProp->propName, "ORGANIZER") == 0  && organizer == true)
			{
				return 1;
			}
			if(strcmp(tmpProp->propName, "PRIORITY") == 0  && priority == true)
			{
				return 1;
			}
			if(strcmp(tmpProp->propName, "SEQUENCE") == 0  && seq == true)
			{
				return 1;
			}
			if(strcmp(tmpProp->propName, "STATUS") == 0  && status == true)
			{
				return 1;
			}
			if(strcmp(tmpProp->propName, "SUMMARY") == 0  && summary == true)
			{
				return 1;
			}
			if(strcmp(tmpProp->propName, "TRANSP") == 0 && transp == true)
			{
				return 1;
			}
			if(strcmp(tmpProp->propName, "URL") == 0 && url == true)
			{
				return 1;
			}
			if(strcmp(tmpProp->propName, "RECURRENCE-ID") == 0  && recurid == true)
			{
				return 1;
			}
			if((strcmp(tmpProp->propName, "DTEND") == 0 || strcmp(tmpProp->propName, "DURATION") == 0)  && dtenddur == true)
			{
				return 1;
			}


			if(strcmp(tmpProp->propName, "CLASS") == 0)
			{
				class = true;
			}
			if(strcmp(tmpProp->propName, "CREATED") == 0)
			{
				created = true;
			}
			if(strcmp(tmpProp->propName, "DESCRIPTION") == 0)
			{
				desc = true;
			}
			if(strcmp(tmpProp->propName, "GEO") == 0)
			{
				geo = true;
			}
			if(strcmp(tmpProp->propName, "LAST-MODIFIED") == 0)
			{
				lastmod = true;
			}
			if(strcmp(tmpProp->propName, "LOCATION") == 0)
			{
				location = true;
			}
			if(strcmp(tmpProp->propName, "ORGANIZER") == 0)
			{
				organizer = true;
			}
			if(strcmp(tmpProp->propName, "PRIORITY") == 0)
			{
				priority = true;
			}
			if(strcmp(tmpProp->propName, "SEQUENCE") == 0)
			{
				seq = true;
			}
			if(strcmp(tmpProp->propName, "STATUS") == 0)
			{
				status = true;
			}
			if(strcmp(tmpProp->propName, "SUMMARY") == 0)
			{
				summary = true;
			}
			if(strcmp(tmpProp->propName, "TRANSP") == 0)
			{
				transp = true;
			}
			if(strcmp(tmpProp->propName, "URL") == 0)
			{
				url = true;
			}
			if(strcmp(tmpProp->propName, "RECURRENCE-ID") == 0)
			{
				recurid = true;
			}
			if(strcmp(tmpProp->propName, "DTEND") == 0 || strcmp(tmpProp->propName, "DURATION") == 0)
			{
				dtenddur = true;
			}
		}
		tmp = tmp->next;
	}
	return 0;
}

int validAlarms(List* alarms)
{
	Node* alarmNode = alarms->head;
	Alarm* tmpAlarm;
	for(int i = 0; i < getLength(alarms); i++)
	{
		tmpAlarm = (Alarm*)(alarmNode->data);
		if(strlen(tmpAlarm->action) == 0 || strlen(tmpAlarm->action) > 199)
		{
			return 1;
		}
		if(tmpAlarm->trigger == NULL)
		{
			return 1;
		}
		if(strlen(tmpAlarm->trigger) == 0)
		{
			return 1;
		}
		if(validAlarmProps(tmpAlarm->properties))
		{
			return 1;
		}
		alarmNode = alarmNode->next;
	}
	return 0;
}

int validAlarmProps(List* props)
{
	Node* tmp = props->head;
	Property* tmpProp;
	bool dur = false;
	bool rep = false;
	bool attach = false;
	for(int i = 0; i < getLength(props); i++)
	{
		tmpProp = (Property*)(tmp->data);
		//empty fields
		if(strlen(tmpProp->propName) == 0 || strlen(tmpProp->propName) > 199 || strlen(tmpProp->propDescr) == 0)
		{
			return 1;
		}
		if(strcmp(tmpProp->propName, "DURATION") != 0 && strcmp(tmpProp->propName, "REPEAT") != 0 && strcmp(tmpProp->propName, "ATTACH") != 0)
		{
			return 1;
		}
		else
		{
			if(strcmp(tmpProp->propName, "DURATION") == 0 && dur == true)
			{
				return 1;
			}
			if(strcmp(tmpProp->propName, "REPEAT") == 0 && rep == true)
			{
				return 1;
			}
			if(strcmp(tmpProp->propName, "ATTACH") == 0 && attach == true)
			{
				return 1;
			}
			if(strcmp(tmpProp->propName, "DURATION") == 0)
			{
				dur = true;
			}
			if(strcmp(tmpProp->propName, "REPEAT") == 0)
			{
				rep = true;
			}
			if(strcmp(tmpProp->propName, "ATTACH") == 0)
			{
				attach = true;
			}

		}
		tmp = tmp->next;
	}
	if((dur == false && rep == true)||(dur == true && rep == false))
	{
		return 1;
	}
	return 0;
}

//return content if found
char* findSummary(List* props)
{
	char* tmpStr = NULL;
	Node* tmp = props->head;
	Property* tmpProp;
	for(int i = 0; i < getLength(props); i++)
	{
		tmpProp = (Property*)(tmp->data);
		if(strcmp("SUMMARY", tmpProp->propName) == 0)
		{
			tmpStr = (char*)malloc((strlen(tmpProp->propDescr)+1)*sizeof(char));
			strcpy(tmpStr, tmpProp->propDescr);
		}
		tmp = tmp->next;
	}
	return tmpStr;
}

char* findOrg(List* props)
{
	char* tmpStr = NULL;
	Node* tmp = props->head;
	Property* tmpProp;
	for(int i = 0; i < getLength(props); i++)
	{
		tmpProp = (Property*)(tmp->data);
		if(strcmp("ORGANIZER", tmpProp->propName) == 0)
		{
			tmpStr = (char*)malloc((strlen(tmpProp->propDescr)+1)*sizeof(char));
			strcpy(tmpStr, tmpProp->propDescr);
		}
		tmp = tmp->next;
	}
	return tmpStr;
}

char* findLoc(List* props)
{
	char* tmpStr = NULL;
	Node* tmp = props->head;
	Property* tmpProp;
	for(int i = 0; i < getLength(props); i++)
	{
		tmpProp = (Property*)(tmp->data);
		if(strcmp("LOCATION", tmpProp->propName) == 0)
		{
			tmpStr = (char*)malloc((strlen(tmpProp->propDescr)+1)*sizeof(char));
			strcpy(tmpStr, tmpProp->propDescr);
		}
		tmp = tmp->next;
	}
	return tmpStr;
}

int getLengthOfInt(int num)
{
	char numTmp[100];
	sprintf(numTmp,"%d",num);
	return strlen(numTmp);
}