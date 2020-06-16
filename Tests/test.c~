#include <string_funcs.h>
#include <ctest.h>
#include <stdio.h>


CTEST (check, tabulation) // probel
{
	int i,expected[10],check[10];
	char word[10][10];
	FILE* file = fopen ("topics/Animals.txt","r");
	for (i = 0; i < 10; ++i){
		fscanf (file,"%s",word[i]);	
	}
	fclose (file);
	for (i = 0; i < 10; ++i) {
		check[i] = space_suff(word[i]);
	}
	expected[0] = 10;
	expected[1] = 7;
	expected[2] = 9;
	expected[3] = 8;
	expected[4] = 9;
	expected[5] = 8;
	expected[6] = 9;
	expected[7] = 8;
	expected[8] = 9;
	expected[9] = 10;
	ASSERT_EQUAL(expected[0], check[0]);
	ASSERT_EQUAL(expected[1], check[1]);
	ASSERT_EQUAL(expected[2], check[2]);
	ASSERT_EQUAL(expected[3], check[3]);
	ASSERT_EQUAL(expected[4], check[4]);
	ASSERT_EQUAL(expected[5], check[5]);
	ASSERT_EQUAL(expected[6], check[6]);
	ASSERT_EQUAL(expected[7], check[7]);
	ASSERT_EQUAL(expected[8], check[8]);
	ASSERT_EQUAL(expected[9], check[9]);
}

// simbol
CTEST (check, simbol) // check_simbol
{
	int i = 0,kod;
	int expected[26];
	int check[26];
	char input = 'a';
	char simbol[26] = {0};
	
	for (i = 0; i < 26; ++i){
		simbol[i] = 0;	
	}
	printf ("\n");
	for (i = 0; i < 26; ++i) {
		check[i] = used_simbol_check(simbol, input);
		++input;
		expected[i] = 1;
		ASSERT_EQUAL(expected[i], check[i]);
	}	
	kod = 96;
	for (i = 0; i < 26; ++i){
		simbol[i] = ++kod;	
	}
	input = 'a';
	for (i = 0; i < 26; ++i) {
		check[i] = used_simbol_check(simbol, input);
		++input;
		expected[i] = 0;
		ASSERT_EQUAL(expected[i], check[i]);
	}	
	
}

CTEST (difficult, tryed) // difficult
{	
	int i,tryed = 0, difficult[3],check[3],expected[3];
	difficult[0] = 1;
	difficult[1] = 2;
	difficult[2] = 3;
	expected[0] = 0;
	expected[1] = 0;
	expected[2] = 0;
	check[0] = Difficult_output (difficult[0],&tryed);
	check[1] = Difficult_output (difficult[1],&tryed);
	check[2] = Difficult_output (difficult[2],&tryed);
	ASSERT_EQUAL(expected[0], check[0]);
	ASSERT_EQUAL(expected[1], check[1]);
	ASSERT_EQUAL(expected[2], check[2]);
}

CTEST (topic, across) // data stream
{
	int i,check[4],expected[4],topic[4];
	FILE* file;
	for (i = 0; i < 4; ++i) {
		topic[i] = i + 1;
		check[i] = Topic_output (topic[i], &file);
		expected[i] = 0;
		ASSERT_EQUAL(expected[i], check[i]);
	}
	fclose (file);
}

