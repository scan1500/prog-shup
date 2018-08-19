#define AUTHOR "Andreas Schwarzmann"

#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define SPLSIZE 5
#define PRINTRS 2
#define TASKS 10

#define WAIT -1
#define SIGNAL 1

struct prnt_data
{
	int pge_cnt;
	int rndm_nbr;
} splr_buffer[SPLSIZE] , prntr_buffer[PRINTRS];
struct sembuf smphr;

int sem_splres_full, sem_splres_empty, sem_splres_mutex;		// SEM. FOR RES. IN SPOOLER BUFFER
int sem_prntres_full, sem_prntres_empty, sem_prntres_mutex;  // SEM. FOR RES. IN PRINTER BUFFER

int shm_splr_buffer_hndl, shm_prntr_buffer_hndl;				    // SHARED MEM. HANDLES FOR SPOOLER & PRINTER 
struct prnt_data *shm_splr_buffer_ptr, *shm_prntr_buffer_ptr;       // POINTERS TO THE SHARED MEM. AREA (STRUCTS)

int shm_nxt_set_hndl, shm_nxt_prntjob_hndl;		// SHARED MEM. HANDLES FOR BUFFER INDICES 
int *shm_nxt_set, *shm_nxt_prntjob;				// POINTERS TO THE SHARED MEM. AREA (INTEGERS)
int *shm_tsk_cnt, shm_tsk_cnt_hndl;

void task();
void spooler();
void printer();
void sem_op(int sem_id, int op);

void main(void) //COORDINATOR
{
	printf("\nThis solution has been created by %s\n\n", AUTHOR);

	int tsk_cnt, prntr_cnt, wpid, wstat;

	// GET SEMAPHORE HANDLES FOR SPOOLER-BUFFER
	sem_splres_full = semget(IPC_PRIVATE, 1, 0777 | IPC_CREAT);
	sem_splres_empty = semget(IPC_PRIVATE, 1, 0777 | IPC_CREAT);
	sem_splres_mutex = semget(IPC_PRIVATE, 1, 0777 | IPC_CREAT);

	// SET SEMAPHORE VALUES FOR SPOOLER-BUFFER
	semctl(sem_splres_full, 0, SETVAL, 0);
	semctl(sem_splres_empty, 0, SETVAL, 5);
	semctl(sem_splres_mutex, 0, SETVAL, 1);

	// GET SEMAPHORE HANDLES FOR PRINTER-BUFFER
	sem_prntres_full = semget(IPC_PRIVATE, 1, 0777 | IPC_CREAT);
	sem_prntres_empty = semget(IPC_PRIVATE, 1, 0777 | IPC_CREAT);
	sem_prntres_mutex = semget(IPC_PRIVATE, 1, 0777 | IPC_CREAT);

	// SET SEMAPHORE VALUES FOR PRINTER-BUFFER
	semctl(sem_prntres_full, 0, SETVAL, 0);
	semctl(sem_prntres_empty, 0, SETVAL, 2);
	semctl(sem_prntres_mutex, 0, SETVAL, 1);

	// GET SHARED-MEMORY HANDLES
	shm_splr_buffer_hndl = shmget(IPC_PRIVATE, SPLSIZE * sizeof(struct prnt_data), 0777 | IPC_CREAT);
	shm_prntr_buffer_hndl = shmget(IPC_PRIVATE, PRINTRS * sizeof(struct prnt_data), 0777 | IPC_CREAT);
	shm_nxt_set_hndl = shmget(IPC_PRIVATE, 1, 0777 | IPC_CREAT);
	shm_nxt_prntjob_hndl = shmget(IPC_PRIVATE, 1, 0777 | IPC_CREAT);
	shm_tsk_cnt_hndl = shmget(IPC_PRIVATE, 1, 0777 | IPC_CREAT);

	// GET POINTER TO ATTACHED SHARED-MEMORY
	shm_splr_buffer_ptr = shmat(shm_splr_buffer_hndl, NULL, 0);
	shm_prntr_buffer_ptr = shmat(shm_prntr_buffer_hndl, NULL, 0);
	shm_nxt_set = shmat(shm_nxt_set_hndl, NULL, 0);
	shm_nxt_prntjob = shmat(shm_nxt_prntjob_hndl, NULL, 0);
	shm_tsk_cnt = shmat(shm_tsk_cnt_hndl, NULL, 0);

	// CREATING ALL PROCESSES
	if (fork() == 0)
		spooler();

	for (prntr_cnt = 0; prntr_cnt < PRINTRS; prntr_cnt++)
		if (fork() == 0)
			printer();

	for (tsk_cnt = 0; tsk_cnt < TASKS; tsk_cnt++)
	{
		sleep(rand() % 5);
		if (fork() == 0)
			task();
	}

	// WAIT FOR ALL PROCESSES
	while ((wpid = wait(&wstat)) > 0);

	// REMOVE SEMAPHORES
	semctl(sem_splres_full, 0, IPC_RMID, NULL);
	semctl(sem_splres_empty, 0, IPC_RMID, NULL);
	semctl(sem_splres_mutex, 0, IPC_RMID, NULL);
	semctl(sem_prntres_full, 0, IPC_RMID, NULL);
	semctl(sem_prntres_empty, 0, IPC_RMID, NULL);
	semctl(sem_prntres_mutex, 0, IPC_RMID, NULL);

	// DETACH SHARED-MEMORY
	shmdt(shm_splr_buffer_ptr);
	shmdt(shm_prntr_buffer_ptr);
	shmdt(shm_nxt_set);
	shmdt(shm_nxt_prntjob);
	shmdt(shm_tsk_cnt);

	// REMOVE SHARED-MEMORY
	shmctl(shm_splr_buffer_hndl, IPC_RMID, NULL);
	shmctl(shm_prntr_buffer_hndl, IPC_RMID, NULL);
	shmctl(shm_nxt_set_hndl, IPC_RMID, NULL);
	shmctl(shm_nxt_prntjob_hndl, IPC_RMID, NULL);
	shmctl(shm_tsk_cnt_hndl, IPC_RMID, NULL);

	printf("\x1B[0m\n\n%d tasks have been printed\n", TASKS);
	printf("\x1B[0mprogram will terminate now\n\n");
}

void task() // PRODUCER
{
	int pge_cnt = (rand() % 5) + 2, rndm_nbr = rand();

	printf("\x1B[34m*NEW TASK PROCESS*\n");
	printf("\x1B[34mtask information:\n");
	printf("\x1B[34mpage number: %d\n", pge_cnt);
	printf("\x1B[34mrand. data: %d\n", rndm_nbr);

	sem_op(sem_splres_empty, WAIT); // DECREASE EMPTY SLOTS
	sem_op(sem_splres_mutex, WAIT);

	shm_splr_buffer_ptr[*shm_nxt_set].pge_cnt = pge_cnt;
	shm_splr_buffer_ptr[*shm_nxt_set].rndm_nbr = rndm_nbr;
	*shm_nxt_set = (*shm_nxt_set + 1) % SPLSIZE; // NEXT PROCESS WRITE IN NEXT BUFFERSLOT

	sem_op(sem_splres_mutex, SIGNAL);
	sem_op(sem_splres_full, SIGNAL); // INCREASE FULL SLOTS

	printf("\x1B[34mtask send to spooler\n");

	exit(0);
}

void spooler() // CONSUMER & PRODUCER
{
	struct prnt_data lcl_splr_data;
	int tsk_cnt, next_prntr = 0, next_splr_val = 0;

	printf("\x1B[35m\t*NEW SPOOLER PROCESS*\n");

	for (tsk_cnt = 0; tsk_cnt < TASKS; tsk_cnt++)
	{
		sem_op(sem_splres_full, WAIT);  // DECREASE FULL SLOTS
		printf("\x1B[35m\ttask received in spooler\n");
		// sem_op(sem_splres_mutex, -1); // GET MUTEX FOR BUFFER ELEMENT
		lcl_splr_data = shm_splr_buffer_ptr[next_splr_val]; // COPY BUFFER ELEMENT TO LOCAL VARIABLE
		// sem_op(sem_splres_mutex, 1);
		sem_op(sem_splres_empty, SIGNAL); // INCREASE EMPTY SLOTS | NEXT TASK CAN BE STORED

		printf("\x1B[35m\ttask removed from spooler\n");
		printf("\x1B[35m\ttask information:\n");
		printf("\x1B[35m\tpage number: %d\n", lcl_splr_data.pge_cnt);
		printf("\x1B[35m\trand. data: %d\n", lcl_splr_data.rndm_nbr);

		//---------------------------------------------------------------------------------------

		sem_op(sem_prntres_empty, WAIT);
		// sem_op(sem_prntres_mutex, -1);
		shm_prntr_buffer_ptr[next_prntr] = lcl_splr_data;
		// sem_op(sem_prntres_mutex, 1);
		sem_op(sem_prntres_full, SIGNAL);

		printf("\x1B[35m\t%d.task send to printers\n", tsk_cnt + 1);

		next_splr_val = (next_splr_val + 1) % SPLSIZE;  // SET INDEX TO NEXT BUFFER ELEMENT
		next_prntr = (next_prntr + 1) % PRINTRS;		// ALTERNATING PRINTERS
	}
	printf("\x1B[35m\tall jobs send\n");
	exit(0);
}

void printer() // CONSUMER
{
	int tsk_cnt;
	struct prnt_data lcl_prntr_data;
	printf("\x1B[36m\t\t*NEW PRINTER PROCESS*\n");
	
	while (1)
	{
		if (*shm_tsk_cnt == TASKS)
		{   // UNLOCK PRINTER WAITING FOR: sem_prntres_full
			sem_op(sem_prntres_full, SIGNAL);
			break;
		}

		sem_op(sem_prntres_full, WAIT);
		sem_op(sem_prntres_mutex, WAIT);

		if (*shm_tsk_cnt == TASKS)
		{   // UNLOCK NEXT PRINTER WAITING FOR: sem_prntres_full & sem_prntres_mutex
			// ESSENTIAL WHEN N>1 PRINTERS WANT LAST RESOURCE AT APPROX. THE SAME TIME
			sem_op(sem_prntres_mutex, SIGNAL);
			sem_op(sem_prntres_full, SIGNAL);
			break;
		}
			
		printf("\x1B[36m\t\tprinter: %d received data\n", getpid());

		lcl_prntr_data = shm_prntr_buffer_ptr[*shm_nxt_prntjob];
		*shm_nxt_prntjob = (*shm_nxt_prntjob + 1) % PRINTRS;
		tsk_cnt = *shm_tsk_cnt = *shm_tsk_cnt + 1;

		sem_op(sem_prntres_mutex, SIGNAL);

		printf("\x1B[36m\t\t%d.task removed from printbuffer\n", tsk_cnt);
		printf("\x1B[36m\t\ttask information:\n");
		printf("\x1B[36m\t\tpage number: %d\n", lcl_prntr_data.pge_cnt);
		printf("\x1B[36m\t\trand. data: %d\n", lcl_prntr_data.rndm_nbr);

		printf("\x1B[36m\t\t%d is printing for %ds\n", getpid(), lcl_prntr_data.pge_cnt);
		sleep(lcl_prntr_data.pge_cnt);
		printf("\x1B[31m\t\tprinting job done\n");

		sem_op(sem_prntres_empty, SIGNAL);
	}
	exit(0);
}

void sem_op(int sem_id, int op)
{
	smphr.sem_num = 0;
	smphr.sem_op = op;
	smphr.sem_flg = 0;
	if (semop(sem_id, &smphr, 1) == -1)
	{
		printf("error while semop");
		exit(1);
	}
}