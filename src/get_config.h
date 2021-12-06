/* This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef GET_CONFIG_H
#define GET_CONFIG_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>

#define SPACE   32
#define HASH    35
#define EQU     61
#define LF      '\n' /* toDo: check if this works with DOS files! */
#define NTERM  '\0'
#define NL      10
#define HT      9    /*  Horizontal Tab */
#define LEFT_P  123
#define RIGHT_P 125

#define MOBSIZE 16

struct request {
  char* file;
  char* option;
};

struct option_values {
  char** opt_value;
  int count;
};

#ifndef HAVE_GETLINE
#include <stdlib.h>
#include <string.h>

#define BSIZE 256
/**
 * getline if not defined 
 */
ssize_t getline(char** lineptr, size_t* n, FILE* stream);
#endif

/**
 * read the values of a given option, specified by the option name
 *
 * @param: request contains the the option and file name
 * @param: values the return prameter contains the array of option values (new allocated space!), if found
 *
 * @return: -1 in case of an parser, file or out of memory error, 0 if the option was not 
 *          found, 1 if the option was found
 */
int read_option(struct request* req, struct option_values* values);

/**
 * check if the first character is a "#" -> comment
 * 
 * @param: the retrived line
 *
 * @return: 1 if the this line is a comment, 0 if not 
 */
int comment(char* line);

/**
 * checks the retrieved line for the option and it's type and takes care of it
 *
 * @param: stream the file handle
 * @param: line the retrived line
 * @param: len the lenght of the line
 * @param: req the requested option
 * @param: values the return prameter contains the array of option values (new allocated space!), if found
 *
 * @return: 1 if the option was found, 0 if not
 */
int type_select(FILE* stream, char* line, int len, struct request* req, struct option_values* values);

/**
 * retrieve the value of a single option
 *
 * @param: line the retrived line
 * @param: start points to the start of the option
 * @param: len the lenght of the line
 * @param: values the return prameter contains the array of option values (new allocated space!), if found
 *
 * @return: the lenght of the option value, -1 if an error occured and/or non chars copied
 */
int get_single_option(char* line, char* start, int len, struct option_values* values);

/**
 * retrive the values of a multi value option 
 *
 * @param: stream the file handle
 * @param: values (return parameter) contains the array of option values, if found
 *
 * @return: the number of lines printed, -1 if an error occured
 */
int get_multi_option(FILE* stream, struct option_values* values);

/**
 * copy the option value out of a given config file line
 * 
 * @param: line the retrived line
 * @param: len the lenght of the line
 * @param: start start position of the value in the line
 * @param: value the return parameter contains the option value (new allocated space!)
 *
 * @return: the number of copied chars (+1 = lenght of the value), -1 if something went wrong
 */
int copy_option_value(char* line, int len, char* start, char** value);

#endif /* GET_CONFIG_H */
