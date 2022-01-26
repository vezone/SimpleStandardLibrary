#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>

/*

		 #####################################
		 #####################################
		       Logger.h (CONSOLE LOGGER)
		 #####################################
		 #####################################

*/

/*
  ==================
  ==================
  ==              ==
  ==    COLORS    ==
  ==              ==
  ==================
  ==================

  Name            BG  FG
  Black           30  40
  Red             31  41
  Green           32  42
  Yellow          33  43
  Blue            34  44
  Magenta         35  45
  Cyan            36  46
  White           37  47
  Bright Black    90  100
  Bright Red      91  101
  Bright Green    92  102
  Bright Yellow   93  103
  Bright Blue     94  104
  Bright Magenta  95  105
  Bright Cyan     96  106
  Bright White    97  107

  [a;b;cmTEXT\033[0m
  a - mode
  b - background color
  c - foreground color
  \033 - foreground & background

  examples:
  printf("\033[36mTexting\033[0m \n");

  //Background
  printf("\033[1;42;30mTexting\033[0m\t\t");
  printf("\033[3;44;30mTexting\033[0m\t\t");
  printf("\033[4;104;30mTexting\033[0m\t\t");
  printf("\033[5;100;30mTexting\033[0m\n");

  printf("\033[4;47;35mTexting\033[0m\t\t");
  printf("\033[3;47;35mTexting\033[0m\t\t");
  printf("\033[3;43;30mTexting\033[0m\t\t");
*/

#define BLACK(x) "\x1B[30m"x"\033[0m"
#define RED(x) "\x1B[31m"x"\033[0m"
#define GREEN(x) "\x1B[32m"x"\033[0m"
#define GREENQ(x) "\x1B[32m"x"\033[0m"
#define YELLOW(x) "\x1B[33m"x"\033[0m"
#define BLUE(x) "\x1B[34m"x"\033[0m"
#define MAGNETA(x) "\x1B[35m"x"\033[0m"
#define CYAN(x) "\x1B[36m"x"\033[0m"
#define WHITE(x) "\x1B[37m"x"\033[0m"

#define BRIGHTBLACK(x) "\x1B[90m"x"\033[0m"
#define BRIGHTRED(x) "\x1B[91m"x"\033[0m"
#define BRIGHTGREEN(x) "\x1B[92m"x"\033[0m"
#define BRIGHTYELLOW(x) "\x1B[93m"x"\033[0m"
#define BRIGHTBLUE(x) "\x1B[94m"x"\033[0m"
#define BRIGHTMAGNETA(x) "\x1B[95m"x"\033[0m"
#define BRIGHTCYAN(x) "\x1B[96m"x"\033[0m"
#define BRIGHTWHITE(x) "\x1B[97m"x"\033[0m"

#define RED5(x) "\033[5;1;31m"x"\033[0m"
#define REDBG5(x) "\033[5;101;30m"x"\033[0m"
#define GREEN5(x) "\033[5;1;32m"x"\033[0m"

#define GLOG(format, ...) printf("[LOG] file: %s, line: %d, message: ", __FILE__, __LINE__);printf(format, ##__VA_ARGS__)
#define GSUCCESS(format, ...) printf(GREENQ("[SUCCESS]")" file: %s, line: %d, message: ", __FILE__, __LINE__);printf(format, ##__VA_ARGS__)
#define GERROR(format, ...) printf(RED("[ERROR]")" file: %s, line: %d, message: ", __FILE__, __LINE__);printf(format, ##__VA_ARGS__)
#define GWARNING(format, ...) printf(YELLOW("[WARNING]")" file: %s, line: %d, message: ", __FILE__, __LINE__);printf(format, ##__VA_ARGS__)
#define GFORMAT(string, format, ...) sprintf(string, format, __VA_ARGS__)

#endif
