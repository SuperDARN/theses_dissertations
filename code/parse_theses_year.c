/* parse_theses_year.c
   ===================
   Author: E.G.Thomas (2019)

   This program parses a text file containing SuperDARN thesis
   and dissertation information, builds html code to display the
   information in a more user-friendly format, and writes the
   output to stdout. Thesis/dissertation entries are first sorted
   by year and then alphabetically by author last name. The program
   can be compiled with:

        gcc -o parse_theses_year parse_theses_year.c

   and then executed using:

        ./parse_theses_year superdarn_theses.txt > output.html
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define STRLEN 512

struct thesis {
  char author[STRLEN];
  char year[STRLEN];
  char title[STRLEN];
  char advisor[STRLEN];
  char affiliation[STRLEN];
  char degree[STRLEN];
  char url[STRLEN];
};


struct thesis *parse_text(FILE *fp, int *num);
int compare(const void *s1, const void *s2);
int write_html(struct thesis *entry, int num);


int main(int argc, char *argv[]) {

  char fname[STRLEN];
  FILE *fp;

  struct thesis *entry=NULL;
  int num=0;

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
  entry = parse_text(fp, &num);

  /* Close input text file */
  fclose(fp);

  /* Check for error when parsing input text file */
  if (num == -1) {
    fprintf(stderr, "Failed to parse input text file.\n");
    return (-1);
  }

  /* Sort theses/dissertations first by year and then alphabetically by author last name
   * Note: this may not be necessary if the input text file was already sorted */
  qsort(entry, num, sizeof(struct thesis), compare);

  /* Build html and write to stdout */
  write_html(entry, num);

  return (0);
}


/* Function to parse a text file and store information about each
 * thesis/dissertation in the appropriate field of a structure and
 * return the number of entries found */
struct thesis *parse_text(FILE *fp, int *num) {

  struct thesis *entry=NULL;
  char line[STRLEN];
  int i=0, cnt=0;

  /* Read in each line of text file */
  while (fgets(line, STRLEN, fp) != NULL) {

    /* Trim \n at end of each line returned by fgets */
    line[strcspn(line, "\r\n")] = 0;

    /* Assign each line of text file to appropriate field */
    switch(i) {
      case 0:
        if (entry == NULL) entry = malloc(sizeof(struct thesis));
        else               entry = realloc(entry,sizeof(struct thesis)*(cnt+1));

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

    /* Advance to next field or reset to the beginning for a new entry */
    i++;
    if (i == 8) i=0;
  }

  /* Return the number of thesis/dissertation entries read from file */
  *num = cnt;

  return entry;
}


/* Function to sort theses/dissertations first by year and
 * then by author last name (for use with qsort) */
int compare(const void *s1, const void *s2) {
  struct thesis *t1 = (struct thesis *)s1;
  struct thesis *t2 = (struct thesis *)s2;

  int yearcompare = strcmp(t2->year, t1->year);

  if (yearcompare == 0) {
    /* Make sure first letter in author field is capitalized */
    char author1[STRLEN], author2[STRLEN];
    strcpy(author1, t1->author);
    strcpy(author2, t2->author);
    author1[0] = toupper(author1[0]);
    author2[0] = toupper(author2[0]);

    int authorcompare = strcmp(author1, author2);

    return authorcompare;
  } else {
    return yearcompare;
  }
}


/* Function to build the thesis/dissertation html and
 * write it to stdout */
int write_html(struct thesis *entry, int num) {

  int i, yearcompare;
  int ms_cnt=0, phd_cnt=0;
  char year[STRLEN];

  /* Start writing html output to stdout */
  fprintf(stdout, "<!-- *** BEGIN THESIS/DISSERTATION CONTENT HERE *** --!>\n");
  fprintf(stdout, "<div align=\"center\">\n\n");

  /* Build year links */
  fprintf(stdout, "  <div style=\"width:800px;\">\n");
  fprintf(stdout, "    <b>Jump to:</b>&nbsp;\n");
  strcpy(year, entry[0].year);
  fprintf(stdout, "    <a href=\"#%s\">%s</a>&nbsp;",year,year);
  for (i=0; i<num; i++) {
    yearcompare = strcmp(year, entry[i].year);
    if (yearcompare != 0) {
      strcpy(year, entry[i].year);
      fprintf(stdout, "|\n    <a href=\"#%s\">%s</a>&nbsp;",year,year);
    }
  }
  fprintf(stdout, "\n  </div>\n");
  fprintf(stdout, "  <br><br>\n\n");

  /* Get year of most recent thesis/dissertation */
  strcpy(year, entry[0].year);
  fprintf(stdout, "  <a name=%s></a>\n", year);
  fprintf(stdout, "  <center><b>%s</b></center><br>\n\n", year);

  /* Step through each thesis/dissertation */
  for (i=0; i<num; i++) {

    /* Count number of theses/dissertations by degree type */
    if (strcmp(entry[i].degree, "MS") == 0) ms_cnt++;
    else if (strcmp(entry[i].degree, "PhD") == 0) phd_cnt++;

    /* Insert year labels where necessary */
    yearcompare = strcmp(year, entry[i].year);
    if (yearcompare != 0) {
      strcpy(year, entry[i].year);
      fprintf(stdout, "  <a name=%s></a>\n", year);
      fprintf(stdout, "  <center><b>%s</b></center><br>\n\n", year);
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
  fprintf(stdout, "  <center>Number of items: <b>%d</b></center>\n", num);
  fprintf(stdout, "  <center>(%d MS | %d PhD)</center>\n\n",ms_cnt,phd_cnt);

  /* Finish writing html output to stdout */
  fprintf(stdout, "</div>\n");
  fprintf(stdout, "<!-- *** END THESIS/DISSERTATION CONTENT HERE *** --!>\n");

  return(0);
}
