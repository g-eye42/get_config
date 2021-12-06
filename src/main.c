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

#include <getopt.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>

void usage();
void usage()
{
  printf("\nUsage: get_config [OPTIONS]\n");
  printf("Where OPTIONS are:\n");
  printf("  -f, --file\n");
  printf("     the configuration file\n");
  printf("  -o, --option\n");
  printf("     the configuration option\n"); 
  printf("  -h, --help\n");
  printf("     this help screen\n");
  printf("   -v, --version\n");
  printf("     get_config's version\n\n");
}

void version();
void version()
{
  printf("\nget_config is at version: %s\n\n", VERSION);
}

int print_option_values(struct option_values values);
int print_option_values(struct option_values values)
{
  int i=0;
  
  for (i=0 ; i<values.count; i++) {
    /* fprintf(stdout, "%d\n",i); */
    fprintf(stdout, "%s", values.opt_value[i]);
    free(values.opt_value[i]);
  }
  free(values.opt_value);
  return i;
} 

int
main (int argc, char **argv) 
{
  int c;
  int exit=0;
  int ret=0;
  int set=0;
  struct request req = { NULL, NULL };
  struct option_values values = { NULL, 0 };
  
  while (1) {
    int option_index=0;
    static struct option long_options[] = {
      {"file", required_argument, 0, 'f'},
      {"option", required_argument, 0, 'o'},
      {"number", no_argument, 0, 'n'},
      {"help", no_argument, 0, '?'},
      {"version", no_argument, 0, 'v'},
      {0, 0, 0, 0}
    };
    c=getopt_long(argc, argv, "f:o:?:v", 
		  long_options, 
		  &option_index);
    if (c==-1)
      break;
    switch (c) {
    case 'f':
      req.file=optarg;
      set++;
      break;
    case 'o':
      req.option=optarg;
      set++;
      break;
    case '?':
      usage();
      exit=1;
      break;
    case 'v':
      version();
      exit=1;
      break;
    default:
      printf("? getopt returned character code 0%o ?\n", c);
      ret=1;
    }
  }
  if (!exit) { 
    if (optind < argc) {
      usage();
    } else {
      if (set==2) {
	ret=read_option(&req, &values);
	if (ret>0) {         /* option found */
	  ret=print_option_values(values);
	} else if (ret==0) { /* option not found */
	  ret=1;
	} else if (ret<0) {  /* error ToDo!!!!*/
	  err(1, "%s", req.file);
	} 
      } else {
	usage();
      }
    }
  }
  return ret;
}


