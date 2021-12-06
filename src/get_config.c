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

#include "get_config.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#ifndef HAVE_GETLINE
ssize_t getline(char** lineptr, size_t* n, FILE* stream)
{
  unsigned char **buff=NULL;
  int next_char=0;
  int i=0;
  int j=0;
  int m=0;
  int ret;

  /* Initialize line -> empty line */
  *lineptr=calloc(sizeof(char*), 1);
  if ( *lineptr==NULL ) {
    printf("Oh dear, something went wrong with calloc()! %s\n", strerror(errno));
    return -1;
  } 
  *lineptr[0]=NL;
  /* check for NULL parameters */
  if (lineptr == NULL || n == NULL) { return -1; }
  /* check for end of line */
  if (feof(stream)) { return -1; }
  /* allocate a BSIZExBSIZE buffer */ 
  buff=calloc(sizeof(unsigned char*), BSIZE);
  if ( buff==NULL ) {
    printf("Oh dear, something went wrong with calloc()! %s\n", strerror(errno));
    return -1;
  }
  next_char=getc(stream);
  while (next_char != NL && i<BSIZE && next_char != EOF) {
    /* New buffer needed? */
    if (j%(BSIZE)==0 || i==0) {
      j=0;
      buff[i]=calloc(sizeof(unsigned char*), BSIZE); /* switched calloc parameters! */
      if ( buff[i]==NULL ) {
	printf("Oh dear, something went wrong with calloc()! %s\n", strerror(errno));
	return -1;
      }
      i++;
    }
    buff[i-1][j]=next_char;
    next_char=getc(stream);
    j++;
  }
  if (i!=0) { /* rebuild line */
    /* buffersize*full buffers+last buffer+new line */
    *n=BSIZE*(i-1)+j+1;
    free(*lineptr);
    *lineptr=calloc(sizeof(char*), *n+1);  /* switched calloc parameters! */
    if ( *lineptr==NULL ) {
      printf("Oh dear, something went wrong with calloc()! %s\n", strerror(errno));
      return -1;
    }
    for (m=0 ; m<i ; m++) {
      strncat(*lineptr, (char*)buff[m], BSIZE);
      free(buff[m]);
    }
    (*lineptr)[(*n)-1]=NL;
    (*lineptr)[*n]=0;
    ret=(*n);
  } else { /* return 1 -> empty line */
    ret=1;
  }
  return ret;
}
#endif

int read_option(struct request* req, struct option_values* values)
{
  int result=0;
  FILE *config;
  char *line=NULL;
  size_t len=0;
  ssize_t read=0;
  int stop=0;
  
  config=fopen(req->file, "r");
  if (config != NULL) {
    while (stop==0 && read !=-1) {
      read=getline(&line, &len, config);
      if (read > 0 && !comment(line) && line[0] != NL) {
	stop=type_select(config, line, len, req, values);
      } 
    }
    /* toDo: check return value */
    if (stop) {
      result=1;
    }
    if (line) {
      free(line);
    }
    fclose(config);
  } else {
    result=-1;
  }
  return result;
}

int comment(char* line)
{
  int ret=0;
  char* pos;
  
  pos=strchr(line, '#');
  if (pos) {
    ret=1;
    while (pos > line) {
      pos=pos-sizeof(char);
      if (*pos != SPACE) {
	ret=0;
	break;
      }
    }
  }
  return ret;
}

int type_select(FILE* stream, char* line, int len, struct request* req, struct option_values* values)
{
  int result=0;
  char* start;

  /* find option start pos */
  start=strstr(line, req->option);
  if (start) {
    /* move pointer to the end of the option */ 
    start=start+strlen(req->option)*sizeof(char);
    /* move over spaces */
    while (*start == SPACE) {
      start=start+sizeof(char);
    }
    /* find "=" sign */
    if (*start == EQU) {
      start=start+sizeof(char);
      /* again, move over spaces */
      while (*start == SPACE) {
	start=start+sizeof(char);
      }
      /* check type */
      if (*start == LEFT_P) {
	result=get_multi_option(stream, values);
      } else {
	result=get_single_option(line, start, len, values);
      }
    }
  }
  return result;
}  

int get_single_option(char* line, char* start, int len, struct option_values* values)
{
  int result;
  int copy;
  char* value;
  char** parray;

  result=-1;
  /* allocate space for two char pointers, since this is only one value and a terminating '\0' */
  parray=(char**)calloc(1, sizeof(char*));
  if (parray == NULL) {
    printf("Oh dear, something went wrong with calloc()! %s\n", strerror(errno));
    return -1;
  }
  copy=copy_option_value(line, len, start, &value);	
  if (copy > 0) {
    /* Copy to return parameter */
    parray[0]=value;
    values->opt_value=parray;
    values->count=1; 
    result=copy;
  }
  return result;
}

int get_multi_option(FILE* stream, struct option_values* values)
{
  int result;
  unsigned int i,j=0;
  int stop=0;
  int copy;
  int allocsize;
  ssize_t read=0;
  size_t len=0;
  char *line=0;
  char** parray=NULL;
  char *start;
  char* value;

  result=-1;
  while (stop == 0 && read !=-1) {
    read=getline(&line, &len, stream);
     /* move over spaces and tabs */
    start=line;
    i=0;
    while ( i<len && (line[i]==SPACE || line[i]==HT) ) {
      start=start+sizeof(char);
      i++;
    }
    /* allocate resp reallocate space for MOBSIZE (Multi Option Block SIZE) char pointers,
       which equates the max. number of options stored in one block */
    if (j%MOBSIZE == 0) {
      if (j == 0 ) {
	allocsize=MOBSIZE;
      } else {
	allocsize=MOBSIZE+j;
      }
      /* printf("allocate new block of %d pointers\n", allocsize); */
      parray=(char**)realloc(parray, (allocsize*sizeof(char*)));
      if (parray == NULL) {
	printf("Oh dear, something went wrong with realloc()! %s\n", strerror(errno));
	return -1;
      }
    }
    /* Check if this line is the end of a multi line option */
    if (*start==RIGHT_P) {
      stop=1;
      /* Add terminator / check if allways is enough allocated space / make a own "termAdd" function */   
    } else {
      copy=copy_option_value(line, len, start, &value);
      if (copy > 0) {
	parray[j]=value;
	j++;
      } else {
	return -1;
      }
    }
    result=j;
  }
  /* right now no check for '}' at end of line!
     no error if '}' is missing! */
  values->opt_value=parray;
  values->count=j; 
  return result;
}

int copy_option_value(char* line, int len, char* start, char** value)
{
  int copy;
  char* end;
  
   /* start points to start of option value -> end is: start pointer + len of line */
  end=line+sizeof(char)*len;
  /* calculate copy len = len of option value */ 
  copy=end-start;	
  /* allocate space for the option value */
  *(value)=calloc(copy+1, sizeof(char));
  if ( *(value) == NULL ) {
    printf("Oh dear, something went wrong with calloc()! %s\n", strerror(errno));
    return -1;
  }
  /* copy option value to return parameter */
  if (copy != 0) {
    strncpy(*(value), start, copy);
  }
  return copy;
}
