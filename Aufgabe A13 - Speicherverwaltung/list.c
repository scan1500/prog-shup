#define AUTHOR "Andreas Schwarzmann"
#include <stdlib.h>
#include <stdio.h>

struct Element
{
	char frst_name[16];
	char scnd_name[16];
	struct Element *next;
	struct Element *prev;
};

struct Element *head;
struct Element *tail;

void main(int argc, char *argv[])
{
	printf("\nThis solution has been created by %s\n\n", AUTHOR);

	head = tail = NULL;
	int i, entries = *argv[1] - 48;

	for (i = 0; i < entries; i++)
	{
		struct Element *e = (struct Element *) malloc(sizeof(struct Element));
		if (e == NULL)
			exit(1);
		else
		{
			printf("%d.entry: (in)first name: ", i + 1);
			fgets(e->frst_name, 16, stdin);
			printf("%d.entry: (in)second name: ", i + 1);
			fgets(e->scnd_name, 16, stdin);

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

	struct Element *temp;
	for (temp = head; temp != NULL; temp = temp->next)
		printf("(out)first name : %s\n (out)second name : %s\n", temp->frst_name, temp->scnd_name);
	for (temp = tail; temp != NULL; temp = temp->prev)
		printf("(out)first name : %s\n (out)second name : %s\n", temp->frst_name, temp->scnd_name);
}