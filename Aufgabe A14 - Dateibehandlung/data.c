#define AUTHOR "Andreas Schwarzmann"
#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>

struct Element
{
	char frst_name[32];
	char scnd_name[32];
	struct Element *next;
	struct Element *prev;
};

struct Element *head;
struct Element *tail;

void main(void)
{
	printf("\nThis solution has been created by %s\n\n", AUTHOR);

	int temp_asci, i;
	
	FILE *fp_srce = fopen("quelle.txt", "r");
	if (fp_srce == NULL)
		exit(1);
	else
	{
		head = tail = NULL;
		while ((temp_asci = getc(fp_srce)) != EOF)
		{
			struct Element *e = (struct Element *) malloc(sizeof(struct Element));
			if (e == NULL)
				exit(1);
			else
			{
				for (i = 0; temp_asci != 32; temp_asci = getc(fp_srce), e->frst_name[i] = '\0')
					e->frst_name[i++] = temp_asci;

				for (i = 0, temp_asci = getc(fp_srce); temp_asci != 10; temp_asci = getc(fp_srce), e->scnd_name[i] = '\0')
					e->scnd_name[i++] = temp_asci;

				if (head == NULL)
				{
					e->next = NULL;
					e->prev = NULL;
					head = e;
					tail = e;
				}
				else
				{
					head->prev = e;
					e->next = head;
					head = e;
					head->prev = NULL;
				}
			}
		}
	}
	fclose(fp_srce);

	FILE *fp_dest = fopen("umgekehrte-reihenfolge.txt", "w+");
	if (fp_dest == NULL)
		exit(1);
	else
	{
		struct Element *temp_elmt;
		for (temp_elmt = head; temp_elmt != NULL; temp_elmt = temp_elmt->next)
			fprintf(fp_dest, "%s %s\n", temp_elmt->frst_name, temp_elmt->scnd_name);
	}
	fclose(fp_dest);
}