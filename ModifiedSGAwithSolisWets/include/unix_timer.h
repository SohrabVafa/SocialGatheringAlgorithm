/*

       AAAA    CCCC   OOOO   TTTTTT   SSSSS  PPPPP
      AA  AA  CC     OO  OO    TT    SS      PP  PP
      AAAAAA  CC     OO  OO    TT     SSSS   PPPPP
      AA  AA  CC     OO  OO    TT        SS  PP
      AA  AA   CCCC   OOOO     TT    SSSSS   PP

######################################################
##########    ACO algorithms for the TSP    ##########
######################################################

      Version: 1.0
      File:    times.h
      Author:  Thomas Stuetzle
      Purpose: routines for measuring elapsed time (CPU or real)
      Check:   README.txt and legal.txt
*/

#ifndef _TIMER
#define _TIMER

int time_expired();

void start_timers();

typedef enum type_timer {REAL, VIRTUAL} TIMER_TYPE;

double elapsed_time(TIMER_TYPE type);



#endif