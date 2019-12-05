/* parse_theses.c
   ==============
   Author: E.G.Thomas (2019)

   This program parses a text file containing SuperDARN thesis
   and dissertation information, builds html code to display the
   information in a more user-friendly format, and writes the
   output to stdout. Thesis/dissertation entries are first sorted
   alphabetically by author last name first and then by year if
   necessary. The program can be compiled with:

        gcc -o parse_theses parse_theses.c

   and then executed using:

        ./parse_theses superdarn_theses.txt > output.html
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define STRLEN 512
#define MAX_DOC 500

struct thesis {
  char author[STRLEN];
  char year[STRLEN];
  char title[STRLEN];
  char advisor[STRLEN];
  char affiliation[STRLEN];
  char degree[STRLEN];
  char url[STRLEN];
};


int parse_text(FILE *fp, struct thesis entry[MAX_DOC]);
int compare(const void *s1, const void *s2);
int write_html(struct thesis entry[MAX_DOC], int cnt);


int main(int argc, char *argv[]) {

  char fname[STRLEN];
  FILE *fp;

  struct thesis entry[MAX_DOC];
  int cnt=0;

  /* Get input filename */
  if (argc < 2) strcpy(fname, "superdarn_theses.txt");
  else strcpy(fname, argv[1]);

  /* Open input text file */
  fp = fopen(fname, "r");
  if (fp == NULL) {
    fprintf(stderr, "File not found: %s\n", fname);
    return (-1);
  }

  /* Parse input text file for information about each thesis/dissertation */
  cnt = parse_text(fp, entry);

  /* Close input text file */
  fclose(fp);

  /* Check for error when parsing input text file */
  if (cnt == -1) {
    fprintf(stderr, "Failed to parse input text file.\n");
    return (-1);
  }

  /* Sort theses/dissertations first alphabetically by author last name and then by year
   * Note: this may not be necessary if the input text file was already sorted */
  qsort(entry, cnt, sizeof(struct thesis), compare);

  /* Build html and write to stdout */
  write_html(entry, cnt);

  return (0);
}


/* Function to parse a text file and store information about each
 * thesis/dissertation in the appropriate field of a structure and
 * return the number of entries found */
int parse_text(FILE *fp, struct thesis entry[MAX_DOC]) {

  char line[STRLEN];
  int i=0, cnt=0;

  /* Read in each line of text file */
  while (fgets(line, STRLEN, fp) != NULL) {

    /* Trim \n at end of each line returned by fgets */
    line[strcspn(line, "\r\n")] = 0;

    /* Assign each line of text file to appropriate field */
    switch(i) {
      case 0:
        strcpy(entry[cnt].author, line);
        break;
      case 1:
        strcpy(entry[cnt].year, line);
        break;
      case 2:
        strcpy(entry[cnt].title, line);
        break;
      case 3:
        strcpy(entry[cnt].advisor, line);
        break;
      case 4:
        strcpy(entry[cnt].affiliation, line);
        break;
      case 5:
        strcpy(entry[cnt].degree, line);
        break;
      case 6:
        strcpy(entry[cnt].url, line);
        cnt++;
        break;
    }

    if (cnt >= MAX_DOC) {
      fprintf(stderr, "Too many entries, need to increase MAX_DOC\n");
      return (-1);
    }

    /* Advance to next field or reset to the beginning for a new entry */
    i++;
    if (i == 8) i=0;
  }

  /* Return the number of thesis/dissertation entries read from file */
  return cnt;
}


/* Function to sort theses/dissertations first by author last name
 * and then by year (for use with qsort) */
int compare(const void *s1, const void *s2) {
  struct thesis *t1 = (struct thesis *)s1;
  struct thesis *t2 = (struct thesis *)s2;
  char author1[STRLEN], author2[STRLEN];

  /* Make sure first letter in author field is capitalized */
  strcpy(author1, t1->author);
  strcpy(author2, t2->author);
  author1[0] = toupper(author1[0]);
  author2[0] = toupper(author2[0]);

  int authorcompare = strcmp(author1, author2);

  if (authorcompare == 0) {
    if (t1->year < t2->year) return -1;
    else if (t1->year > t2->year) return +1;
    else return 0;
  } else {
    return authorcompare;
  }
}


/* Function to build the thesis/dissertation html and
 * write it to stdout */
int write_html(struct thesis entry[MAX_DOC], int cnt) {

  int i, j=0;
  int ms_cnt=0, phd_cnt=0;
  char *alph[4] = {"A-G", "H-N", "O-U", "V-Z"};

  /* Start writing html output to stdout */
  fprintf(stdout, "<!-- *** BEGIN THESIS/DISSERTATION CONTENT HERE *** --!>\n");
  fprintf(stdout, "<div align=\"center\">\n\n");

  /* Build alphabetical links */
  fprintf(stdout, "  <b>Jump to:</b>&nbsp;\n");
  for (i=0; i<3; i++) {
    fprintf(stdout, "  <a href=\"#%s\">%s</a>&nbsp;|\n",alph[i],alph[i]);
  }
  fprintf(stdout, "  <a href=\"#%s\">%s</a>\n\n",alph[i],alph[i]);
  fprintf(stdout, "  <br><br>\n\n");

  /* Step through each thesis/dissertation */
  for (i=0; i<cnt; i++) {

    /* Count number of theses/dissertations by degree type */
    if (strcmp(entry[i].degree, "MS") == 0) ms_cnt++;
    else if (strcmp(entry[i].degree, "PhD") == 0) phd_cnt++;

    /* Insert alphabetical links where necessary */
    if ( (j < 4) && (toupper(entry[i].author[0]) >= alph[j][0]) ) {
      fprintf(stdout, "  <a name=%s></a>\n\n",alph[j]);
      j++;
    }
  
    /* Build html table for each thesis/dissertation */
    fprintf(stdout, "  <table style=\"border:1px solid black; width:600px;\">\n");
    fprintf(stdout, "    <tr><td><b>Author:</b> %s</td></tr>\n", entry[i].author);
    fprintf(stdout, "    <tr><td><b>Year:</b> %s</td></tr>\n", entry[i].year);
    fprintf(stdout, "    <tr><td><b>Title:</b> %s</td></tr>\n", entry[i].title);
    fprintf(stdout, "    <tr><td><b>Advisor:</b> %s</td></tr>\n", entry[i].advisor);
    fprintf(stdout, "    <tr><td><b>Affiliation:</b> %s</td></tr>\n", entry[i].affiliation);
    fprintf(stdout, "    <tr><td><b>Degree:</b> %s</td>", entry[i].degree);
    if (entry[i].url[0] == '\0') {
      fprintf(stdout,"</tr>\n");
    } else {
      fprintf(stdout, "<td align=\"right\"><a href=\"%s\" target=\"_blank\">URL</a></td></tr>\n", entry[i].url);
    }
    fprintf(stdout, "  </table><br>\n\n");
  }

  /* Print total number of items at bottom of page */
  fprintf(stdout, "  <center>Number of items: <b>%d</b></center>\n", cnt);
  fprintf(stdout, "  <center>(%d MS | %d PhD)</center>\n\n",ms_cnt,phd_cnt);

  /* Finish writing html output to stdout */
  fprintf(stdout, "</div>\n");
  fprintf(stdout, "<!-- *** END THESIS/DISSERTATION CONTENT HERE *** --!>\n");

  return(0);
}
