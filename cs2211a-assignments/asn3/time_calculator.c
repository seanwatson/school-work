
/*
 * Program to compute the time of day
 * after a specified duration.
 *
 * Author: Sean Watson
 * Date: Oct. 25, 2011
 */

#include <stdio.h>	//Include the in/out library for reading input and printing output

int main(void){

	//Initialize variables to store the current time, duration, and new time
	int start_time=0, duration=0, new_time=0, new_mins=0, new_hrs=0, start_mins =0, start_hrs=0, duration_mins=0, duration_hrs=0;

	//Prompt for a starting time of day
	printf("Please enter a time of day:");
	scanf("%d", &start_time);

	//Verify the input is a valid time of day
	while (start_time > 2400 || start_time < 0 || (start_time - (start_time/100*100)) >= 60){
		printf("Invalid time. Please enter a new time of day:");	//If it is invalid prompt for another input
		scanf("%d", &start_time);
	}

	//Prompt for a duration to increment or decrement the current time
	printf("Please enter a duration:");
	scanf("%d", &duration);

	//Verify the duration is a valid time
	while ((duration - (duration/100*100)) >= 60 || (duration - (duration/100*100)) <= -60){
		printf("Invalid duration. Please enter a new duration:");	//If it is invalid prompt for another input
		scanf("%d", &duration);
	}

	start_mins = start_time - (start_time/100*100);	//Extract the minutes value of start time
	duration_mins = duration - (duration/100*100);	//Extract the minutes value of duration
	start_hrs = start_time/100;		//Extract the hours value of start time
	duration_hrs = duration/100;	//Extract the hours value of duration

	//Add the duration minutes to the start minutes
	new_mins = start_mins + duration_mins;

	//Increase the hours if the minutes are over or equal to 60
	if (new_mins >= 60){
		new_hrs += 1;
		new_mins -= 60;
	}

	//Decrease the hours if the minutes are under 0
	if (new_mins < 0){
		new_hrs -= 1;
		new_mins += 60;
	}

	//Add the duration hours to the start hours
	new_hrs += start_hrs + duration_hrs;
	new_time = (new_hrs*100)+new_mins;	//Put the new hours and minutes together into one time

	//Adjust the new time to be within a 24 hour day
	while (new_time > 2400){
		new_time -= 2400;
	}

	//Adjust the new time to be within a 24 hour day
	while (new_time < 0){
		new_time += 2400;
	}

	//Print the new time
	printf("The new time is: %d\n", new_time);
	return 0;
}
