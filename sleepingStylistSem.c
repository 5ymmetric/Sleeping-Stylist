/*
 * Author: Karthik Reddy Pagilla
 * CSE username: kpagilla
 * Description: Sleeping Barber Problem 1
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define CHAIRS 7
#define DELAY 10000
#define NUMBER_OF_STYLISTS 1
#define NUMBER_OF_CUSTOMERS 120

sem_t mutex, stylistSem, customerSem;
int pshared = 0;
int waiting = 0;
int customersServed = 0;
int salonFull = 0;
int salonEmpty = 0;

/*
 * Function definitions
 */
void init();
void customer(void *i);
void stylist(void *i);

/*
 * Stylist and customer threads are created
 * And joined here
 */
void main(void) {

  init();

  int i;
  pthread_t stylistThreads[NUMBER_OF_STYLISTS];
  pthread_t customerThreads[NUMBER_OF_CUSTOMERS];

  for(i = 0; i < NUMBER_OF_STYLISTS; i++){
    // passing i + 1 as arg to keep track of stylist number and type casted as long to avoid size mismatch warnings
    pthread_create(&stylistThreads[i], NULL, (void *) stylist, (void *) (long) i + 1);
  }


  for(i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
    // passing i + 1 as arg to keep track of customer number and type casted as long to avoid size mismatch warnings
    pthread_create(&customerThreads[i], NULL, (void *) customer, (void *) (long) i + 1);
  }

  for(i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
    pthread_join(customerThreads[i], NULL);
  }

  for(i = 0; i < NUMBER_OF_STYLISTS; i++) {
    pthread_join(stylistThreads[i], NULL);
  }

  return;
}

/*
 * This function initializes the semaphores
 */
void init() {
  sem_init(&mutex, pshared, 1);
  sem_init(&stylistSem, pshared, 0);
  sem_init(&customerSem, pshared, 0);
}

/*
 * This function is the start_routine for the stylist thread
 * and this function serves the customer when the stylist is available
 */
void stylist(void *i) {
  int j;
  int *stylistNumber = (int *) i;

  while(1) {

    if(customersServed == NUMBER_OF_CUSTOMERS) {
      printf("\nFinished serving all customers!\n");
      printf("Salon full = %d times\n", salonFull);
      printf("Salon empty = %d times\n", salonEmpty);
      break;
    }

    sem_wait(&mutex);

    if(waiting == 0){
      printf("Stylist is Sleeping\n");
      salonEmpty++;
    }

    sem_post(&mutex);

    sem_wait(&customerSem); //Waiting for customer to become available
    sem_wait(&mutex); // Acquire access for waiting

    waiting = waiting - 1;

    sem_post(&stylistSem); // signal customer the stylist is ready
    sem_post(&mutex); // release waiting

    printf("\nStylist Number %d is serving a customer....\n", stylistNumber);
    for(j = 0; j < DELAY; j++);  // Cut hair
    ++customersServed;
    printf("Total customers served: %d\n", customersServed);
  }
}

/*
 * This fucntion is the start_routine for the customer threads
 * and this function either adds the customer to the waiting queue or
 * sends them for shopping
 */
void customer(void *i) {
  int j;
  int *customerNumber = (int *) i;

  while(1) {
    sem_wait(&mutex); // Acquire access for waiting

    if(waiting < CHAIRS) {
      printf("Customer Number %d is added to the waiting queue\n", customerNumber);
      waiting = waiting + 1;

      if(waiting == CHAIRS) {
        salonFull++;
      }

      sem_post(&customerSem); // signal that customer is ready
      sem_post(&mutex); // release waiting
      sem_wait(&stylistSem); // wait for stylist

      printf("Customer Number %d is getting a haircut\n", customerNumber);
      break;
    } else {
      printf("Customer Number %d leaving...\n", customerNumber);
      sem_post(&mutex); // release waiting
      for(j = 0; j < DELAY; j++); // go shopping
    }
      printf("Customer Number %d reattempting...\n", customerNumber);
  }
}
