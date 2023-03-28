// MeetingTime.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>

using namespace std;

//that structure contains two variable "int" where it is stored the hour and minute from one tuple like [HH:MM-HH:MM]
struct timeClock {
    int hour, minute;
};

//here is a tuple like I explain above ([HH:MM-HH:MM]) which contains the start and the end times of one meeting
struct meet {
    timeClock begin, end;
};

//here I have a vector of tuples type "meet" and a variable "lenght" where it is stored the number of tuples
struct boookedCalendar {
    meet meet[500];
    int lenght;
};

//this function "translate" the input which is a sting(in a char vector) in the dataStructures which I declared above
void getCalendar(char calendar[], boookedCalendar *numberCalendar)
{
    int lenght = strlen(calendar), current = -1; //the lenght of the input string
    int minutes = 0, end = 0;                    //two truth variable -minutes- represent when it must store in " .minutes " field of the time
                                                 //                   -end- confirm if it's about the end time of one meetings
    for (int index = 0; index < lenght; index++) 
    {
        if (isdigit(calendar[index]))           
        {
            if (end) //if we are at end time of meeting                 
                if (minutes)
                    numberCalendar->meet[current].end.minute = numberCalendar->meet[current].end.minute * 10 + (calendar[index] - '0');  //convert a char number char in int
                else
                    numberCalendar->meet[current].end.hour = numberCalendar->meet[current].end.hour * 10 + (calendar[index] - '0');
            else    //if we are at start time of a meeting
                if (minutes)
                    numberCalendar->meet[current].begin.minute = numberCalendar->meet[current].begin.minute * 10 + (calendar[index] - '0');
                else
                    numberCalendar->meet[current].begin.hour = numberCalendar->meet[current].begin.hour * 10 + (calendar[index] - '0');
        }
        else
            if (calendar[index] == ':') //when we find this we know that must save in minutes filed the number
                minutes = 1;
            else
                if (calendar[index] == ',' && calendar[index+1] == '\'') //when it is this string  ",'" it means that we must save at end time of the tuple, and the hour filed (minutes = 0)
                {
                    index++; 
                    end = 1;
                    minutes = 0;
                }
                else
                    if (calendar[index] == '[' && calendar[index+1] == '\'') //when it is true it means that we must to save a new tupl, we put the "end" false,to save at start time
                                                                             //of the tuple, and also we advanced to the next one (current++)
                    {
                        index++;
                        minutes = 0;
                        end = 0;         
                        current++;
                    }
                    else
                        if (calendar[index] == '\'' && calendar[index + 1] == ']') //this is happened together with the next "if"; if one employee hawe two meetings one after another, whe save the both like 
                                                                                   //only one time when he is busy. "['12:30','14:30'],['14:30','15:00]" wil be store like "['12:30','15:00]
                        {
                            index++;
                            if (numberCalendar->meet[current].begin.hour == numberCalendar->meet[current - 1].end.hour && numberCalendar->meet[current].begin.minute == numberCalendar->meet[current - 1].end.minute)
                            {
                                numberCalendar->meet[current - 1].end.hour = numberCalendar->meet[current].end.hour;
                                numberCalendar->meet[current - 1].end.minute = numberCalendar->meet[current].end.minute;
                                numberCalendar->meet[current].end.hour = numberCalendar->meet[current].begin.hour = 0;
                                numberCalendar->meet[current].end.minute = numberCalendar->meet[current].begin.minute = 0;
                                current-=1;
                            }
                        }
    }
    numberCalendar->lenght = current; //when the input string is finished, whe save the number of the tuples in ".lenght" filed
}

//here i check if two meetings is begin simultanous(or have intervals when both employee is busy
int concurrent(meet m1, meet m2)
{
    if ((m1.end.hour > m2.begin.hour || m1.end.hour == m2.begin.hour && m1.end.minute > m2.begin.minute)
        && (m1.begin.hour < m2.end.hour || m1.begin.hour == m2.end.hour && m1.begin.minute < m2.end.minute))
        return 1;
    return 0;
}

//this function return the time when one of the employee start the program work - the second of them(the late time)
timeClock meetingStartTime(timeClock c1, timeClock c2)
{
    if (c1.hour > c2.hour)
        return c1;
    else
        if (c1.hour == c2.hour)
            if (c1.minute > c2.minute)
                return c1;
    return c2;
}

//this function return the time when one of the employee finish the program work - the first of them
timeClock meetingEndTime(timeClock c1, timeClock c2)
{
    if (c1.hour < c2.hour)
        return c1;
    else
        if (c1.hour == c2.hour)
            if (c1.minute < c2.minute)
                return c1;
    return c2;
}

//it is recive two time and return the time which is start/end last
//in meetingValidTime this function is called or begin times, or end times of a meet
timeClock maximTime(timeClock c1, timeClock c2)
{
    if (c1.hour > c2.hour)
        return c1;
    else
        if (c1.hour == c2.hour)
            if (c1.minute > c2.minute)
                return c1;
    return c2;
}

//it is recive two time and return the time which is start/end first
//in meetingValidTime this function is called or begin times, or end times of a meeting
timeClock minimTime(timeClock c1, timeClock c2)
{
    if (c1.hour < c2.hour)
        return c1;
    else
        if (c1.hour == c2.hour)
            if (c1.minute < c2.minute)
                return c1;
    return c2;
}

//this function is the "heart" of program.
// input:
//      -recives two list with all tuples of begin&end one meet of each employe(one for each)
//      -the work program of each (lmit time for start work and end it)
//      -an other list where it is store all the intervals in wich the both is available simultaneous
void meetingValidTime(boookedCalendar firstUser, boookedCalendar secondUser, boookedCalendar *possibleMeetingTime, boookedCalendar firstLimit, boookedCalendar secondLimit)
{
    int index1 = 0, index2 = 0; //the index for bookedCalendar of each employee
    int current = 0;            //the index for the new calendar with all free time simultaneous
    
    timeClock startMeet = meetingStartTime(firstLimit.meet[0].begin, secondLimit.meet[0].begin); //the start time of the employee which have a late start program
    timeClock endMeet = meetingEndTime(firstLimit.meet[0].end, secondLimit.meet[0].end);         //the end time of the employee which have a early end program
    

    //the next two "while" is if one of the employee end a meeting before the other start the work time. If it's happened, whe not consider the respective meetings
    //"[['9:00','9:30'], ['12:00','13:00'], ['16:00','18:00]]", and the start time late is "['10:00','18:30']", we skip the first, and we treat the next list:
    //"[['12:00','13:00'], ['16:00','18:00]]"
    while (firstUser.meet[index1].end.hour < startMeet.hour || firstUser.meet[index1].end.hour == startMeet.hour && firstUser.meet[index1].end.minute < startMeet.minute)
        index1++;
    while (secondUser.meet[index2].end.hour < startMeet.hour || secondUser.meet[index2].end.hour == startMeet.hour && secondUser.meet[index2].end.minute < startMeet.minute)
        index2++;

    //this "while" loop it's happend until the EndTime by one of bookedCalendar by one employee is smaller than the EndMeet(explained above
    //another condition wich must be true is both list must contains a tuple. When one of this is empty, the loop stops
    while (firstUser.lenght >= index1 && secondUser.lenght >= index2 && endMeet.hour > firstUser.meet[index1].end.hour || endMeet.hour == firstUser.meet[index1].end.hour && endMeet.minute > firstUser.meet[index1].end.minute &&
           endMeet.hour > secondUser.meet[index2].end.hour || endMeet.hour == secondUser.meet[index2].end.hour && endMeet.minute > secondUser.meet[index2].end.minute)
    {
        //the next 2 "if" statements is happened if one employee end a meeting when the another start. like  [[10:20-11:45]], and the other is [[11:45-13:20]] 
        if (firstUser.meet[index1].begin.hour == secondUser.meet[index1].end.hour && firstUser.meet[index1].begin.minute == secondUser.meet[index1].end.minute)
            index2++;
        if (secondUser.meet[index1].begin.hour == firstUser.meet[index1].end.hour && secondUser.meet[index1].begin.minute == firstUser.meet[index1].end.minute)
            index1++;

        //in this piece of code, we add at the possibleMeetingTime the free intervals of both employee
        if (concurrent(firstUser.meet[index1], secondUser.meet[index2]))
        {        
            cout << "da"<<endl;

                cout<< secondUser.meet[index2].end.hour  <<":"<< secondUser.meet[index2].end.minute << " " << index2 << endl;
                cout<< firstUser.meet[index1].end.hour  <<":"<< firstUser.meet[index1].end.minute << " " << index1 << endl<<endl;

            //if they have a simultaneous meeting, we use this instruction
            possibleMeetingTime->meet[current].begin = maximTime(firstUser.meet[index1].end, secondUser.meet[index2].end);
            index1++;
            index2++;
            possibleMeetingTime->meet[current].end = minimTime(firstUser.meet[index1].begin, secondUser.meet[index2].begin);
            current++;
            //the logic is that whe add at the start of one tuple from list the time of the employee which finished last that meeting
            //then we increase the both index, and whe save at end time of that tuple the time when start meeting the first of them
        }
        else
        {
            //else, the thinking is next:
            //we set the start time of the tuple from possibleMeeting with the mimim of end times of the meetings,
            //and the end time with the maxim from the start times of the meetings
            //the mimim represent the smallest time, and maxim represent the biggest time
            //then, we advanced in the list fro which we add the start time(of the possibleMeetingTime)
            possibleMeetingTime->meet[current].begin = minimTime(firstUser.meet[index1].end, secondUser.meet[index2].end);
            possibleMeetingTime->meet[current].end = maximTime(firstUser.meet[index1].begin, secondUser.meet[index2].begin);
            if (possibleMeetingTime->meet[current].begin.hour == firstUser.meet[index1].end.hour && possibleMeetingTime->meet[current].begin.minute == firstUser.meet[index1].end.minute)
                index1++;
            else
                index2++;
            current++;
        }

    }
    current--;
    if(possibleMeetingTime->meet[current].end.hour == 0)//ad the "endTime" to the termination of the last meet if that no have one
        possibleMeetingTime->meet[current].end = endMeet; 
    possibleMeetingTime->lenght = current;//set the lenght of calendar

    cout << endl << "Incepe:\n";
    for (int i = 0; i <= current; i++)
        cout << possibleMeetingTime->meet[i].begin.hour << ":" << possibleMeetingTime->meet[i].begin.minute << "-" << possibleMeetingTime->meet[i].end.hour << ":" << possibleMeetingTime->meet[i].end.minute << endl;
}

//this function check if is enough time for meet the emplyee in the free time of each
int enoughTime(meet m, int minutes)
{
    int startTime = 0, finalTime = 0;
    startTime = 60 * m.begin.hour + m.begin.minute;
    finalTime = 60 * m.end.hour + m.end.minute;
    if (finalTime - startTime >= minutes)
        return 1;
    return 0;
}

//this function recives the list of all possible meet time, an output wich is a string where the output is returned, and a meet time
void returnString(boookedCalendar possibleMeetingTime, char output[], int meetTime)
{
    output[0] = '[';
    int index = 0;
    for (int i = 0; i <= possibleMeetingTime.lenght; i++)
    {
        if (enoughTime(possibleMeetingTime.meet[i], meetTime))
        {
            int beginHour = possibleMeetingTime.meet[i].begin.hour;
            int endHour = possibleMeetingTime.meet[i].end.hour;
            int beginMinutes = possibleMeetingTime.meet[i].begin.minute;
            int endMinutes = possibleMeetingTime.meet[i].end.minute;

            output[++index] = '[';
            output[++index] = '\'';
            if (beginHour < 10)
                output[++index] = '0' + beginHour;
            else
            {
                output[++index] = '0' + beginHour / 10;
                output[++index] = '0' + beginHour % 10;
            }
            output[++index] = ':';
            output[++index] = beginMinutes / 10 + '0';
            output[++index] = beginMinutes % 10 + '0';           
            output[++index] = '\'';
            output[++index] = ',';
            output[++index] = '\'';
            if (endHour < 10)
                output[++index] = '0' + endHour;
            else
            {
                output[++index] = '0' + endHour / 10;
                output[++index] = '0' + endHour % 10;
            }
            output[++index] = ':';
            output[++index] = endMinutes / 10 + '0';
            output[++index] = endMinutes % 10 + '0';           
            output[++index] = '\'';
            output[++index] = ']';
            output[++index] = ',';
        }
    }
    if(index >0)
        index--;
    output[++index] = ']';
    output[++index] = '\0';
}

int main()
{
    //data used for store the input and for manipulate it more easy
    boookedCalendar firstUser = { 0 };
    boookedCalendar secondUser = { 0 };
    boookedCalendar firstLimit = { 0 };
    boookedCalendar secondLimit = { 0 };
    boookedCalendar possibleMeetingTime = { 0 };

    //input
    char s[100] = "[['9:00','10:30'], ['12:00','13:00'], ['16:00','18:00']]";
    char s2[100] = "[['10:00','11:30'], ['12:30','14:30'],['14:30','15:00'], ['16:00','18:30']]";
    char s3[18] = "['9:00','21:00']";
    char s4[18] = "['10:00','21:30']";
    int minutes = 29;

    //output
    char output[100];

    //logics of proram
    getCalendar(s, &firstUser);
    getCalendar(s2, &secondUser);
    getCalendar(s3, &firstLimit);
    getCalendar(s4, &secondLimit);
    meetingValidTime(firstUser, secondUser, &possibleMeetingTime, firstLimit, secondLimit);

    //create output as a string
    returnString(possibleMeetingTime, output, minutes);

    cout << output <<endl;

    return 0;
}

//Possible updates:
//the function returnString maybe could be written more beauty
//the size of the all vectors maybe could be alocated with malloc ad realloc depending of the input 
//
