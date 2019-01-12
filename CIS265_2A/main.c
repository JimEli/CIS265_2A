/*************************************************************************
* Title: (CIS265_2A) 13 Digit ISBN Decomposer
* File: main.c
* Author: James Eli
* Date: 8/27/2017
*
* Books are identified by an International Standard Book Number (ISBN). 
* ISBNs assigned after January 1, 2007 contain 13 digits, arranged in five 
* groups, such as 978-0-393-97950-3, (Older ISBNs use 10 digits.). The 
* first group (the GSI prefix) is currently either 978 or 979. The group 
* identifier specifies the language or country of origin (for example, 0 
* and 1 are used in English-speaking countries). The publisher code 
* identifies the publisher (393 is the code for W. W. Norton). The item 
* number is assigned by the publisher to identify a specific book (97950 
* is the code for this book). An ISBN ends with a check digit that’s used 
* to verify the accuracy of the preceding digits. Write a program that 
* breaks down an ISBN entered by the user:
*   Enter ISBN: 978-0-393-97950-3
*   GSI prefix: 978
*   Group identifier: 0
*   Publisher code: 393
*   Item number: 97950
*   Check digit: 3
* Note: The number of digits in each group may vary; you can’t assume that 
* groups have the lengths shown in this example. Test your program with 
* actual ISBN values (usually found on the back cover of a book and on the 
* copyright page).
*
* Notes:
*  (1) Compiled with MS Visual Studio 2017 Community (v141), using C
*      language options.
*  (2) ISBN code groups can be delimited by any of the following:
*	   !#$%&*+,-./:;@\\| and the space character.
*  (3) Description of the 13-digit ISBN checksum was retrieved from
*      en.wikipedia.org/wiki/International_Standard_Book_Number on 8/28/17.
*  (4) Uses secure version of strtok (strtok_s).
*
* Submitted in partial fulfillment of the requirements of PCC CIS-265.
*************************************************************************
* Change Log:
*   08/27/2017: Initial release. JME
*************************************************************************/
#include <assert.h> 
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Definitions.
#define INVALID_ISBN_CHARACTER 1 // Flag for an invalid charater inside ISBN string.
#define INVALID_ISBN_CHECKSUM  2 // Flag for an invalid isbn checksum.

/*************************************************************************
 * Calculate & validate ISBN 13-digit checksum.  
 * From Wikipedia: The calculation of an ISBN - 13 check digit begins with 
 * the first 12 digits of the 13-digit ISBN(thus excluding the check digit 
 * itself).Each digit, from left to right, is alternately multiplied by 1 
 * or 3, then those products are summed modulo 10 to give a value ranging 
 * from 0 to 9. Subtracted from 10, that leaves a result from 1 to 10.
 *************************************************************************/
bool checksum(char *group[], const char *checkDigit) 
{
	unsigned int digit = 0; // Counts digits in ISBN and used to toggle multiplier between 1 and 3.
	unsigned int sum = 0;   // Checksum.

	// Loop through ISBN digits and calculate checksum.
	for (int i = 0; i < 4; i++) 
		for (char *pos = group[i]; *pos; pos++) 
			if (isdigit(*pos))
				sum += (*pos - '0') * ((digit++ & 1) ? 3 : 1);

	// Compare checksum with check digit.
	if (10 - sum%10 == *checkDigit - '0')
		return true; // Checksum valid.
	
	return false;
}

// Validate string characters as numeric.
bool validate(const char* s) 
{
	// Empty?
	if (s == NULL || *s == '\0')
		return false;

	// Check for non-numeric chars.
	while (*s != '\0' && *s != '\n')
		if (!isdigit(*s++))
			return false;

	// Arriving here means only valid chars found.
	return true;
}

// Trim whitespace from string.
void trim(char* src) 
{
	static const char *whiteSpace = " \n\r\t";
	char *dest = src;

	assert(src != NULL);

	// Iterate through string. 
	while (*src != '\0') {
		if (!strchr(whiteSpace, *src))
			*dest++ = *src; // Skip this if whitespace.
		src++;
	}

	*dest = '\0'; // Terminate string.
}

// Program starts here.
int main(void) 
{
	char isbn[24];	    // Holds user entered isbn.
	char *isbnGroup[5]; // ISBN separate group codes.
	// ISBN group titles.
	static const char *groupTitle[5] = { "GSI prefix", "Group identifier", "Publisher code", "Item number", "Check digit" };
	// Typical characters used as delimiters.
	static const char *delimiter = " !#$%&*+,-./:;@\\|";
	int error = 0;      // Flags error when processing isbn.

	// Enter an isbn.
	fputs("Enter an ISBN (i.e. 978-0-393-97950-3): ", stdout);
	if (!fgets(isbn, sizeof isbn, stdin)) {
		fputs("Fatal program error!\n", stderr);
		exit(EXIT_FAILURE);
	}
	else if (!strchr(isbn, '\n')) 
	{
		// Input too long, eat remainder.
		while (fgets(isbn, sizeof isbn, stdin) && !strchr(isbn, '\n'))
			; // Empty body.
		fputs("Too many characters input.\n", stdout);
	}
	else 
	{
		// Trim whitespace from isbn string.
		trim(isbn);

		char *context = NULL;
		// Tokenize isbn string.
		isbnGroup[0] = strtok_s(isbn, delimiter, &context);
		for (int i = 1; i < 5; i++)
			isbnGroup[i] = strtok_s(NULL, delimiter, &context);

		// Validate isbn groups as only numeric values.
		for (int i = 0; i < 5; i++)
			if (!validate(isbnGroup[i]))
				error = INVALID_ISBN_CHARACTER;

		// Validate isbn checksum.
		if (!error)
			if (!checksum(isbnGroup, isbnGroup[4]))
				error = INVALID_ISBN_CHECKSUM;

		// Check if an error was caught, and display isbn groups.
		switch (error) 
		{
			case 0:
				// No error, display isbn group info.
				for (int i = 0; i < 5; i++)
					if (isbnGroup[i] != NULL)
						fprintf(stdout, "%s: %s\n", groupTitle[i], isbnGroup[i]);
				break;
		
			case INVALID_ISBN_CHARACTER:
				fputs("Invalid character(s) found in ISBN.\n", stdout);
				break;
		
			case INVALID_ISBN_CHECKSUM:
				fputs("An invalid ISBN checksum was found.\n", stdout);
				break;
		
			default:
				fputs("An unknown error occurred. Try again.\n", stderr);
				break;
		}
	}
}

