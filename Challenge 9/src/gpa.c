#include "gpa.h"

// return: struct person
// param: (struct person p1, struct person p2)
// TODO: create a function that returns the person who has a higher GPA.
// TODO: if GPAs are equal, then return the first person (the first parameter).
// TODO: this function can only be called in the file where it is declared.
struct person compareTo(struct person p1, struct person p2)
{
  if (p2.GPA > p1.GPA)
  {
    return p2;
  }
  else
  {
    return p1;
  }
}


// return: void
// param: (struct person persons[], int len)
// TODO: create a function that returns the person who has the highest GPA.
// TODO: if more than one person has the highest GPA, return the *first*
//       one (in order of index values into the array) with that GPA
// TODO: update the value of the_person in the main.c file to this person.
// TODO: this function must be callable from other modules.
extern void find_the_person(struct person persons[], int len)
{
  struct person smartest;
  smartest = persons[0];
  for (int i = 1; i <= len; i++)
  {
    smartest = compareTo(smartest, persons[i]);
  }
  extern struct person the_person;
  the_person = smartest;
}
