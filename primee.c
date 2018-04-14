#include <pthread.h>		
#include <math.h>		
#include <stdlib.h>		
#include <assert.h>

typedef struct
{

  pthread_cond_t start_working;

  pthread_mutex_t cond_mutex;

  long max_search;
} thread_parameters_t;

typedef struct
{
  long f[2];
} factor_t;


void
print_factorization (long n, factor_t * an)
{
  int i;
  if (an[n].f[0])
    {
      for (i = 0; i < 2; ++i)
	if (an[n].f[i])
	  print_factorization (an[n].f[i], an);
    }
  else
    printf (" %ld ", n);
}


void *
primes_computer_runner (void *param)
{
  factor_t *an;
  long i, prime, limit;
  thread_parameters_t *thread_parameters = (thread_parameters_t *) param;
  pthread_mutex_lock (&thread_parameters->cond_mutex);
  pthread_cond_wait (&thread_parameters->start_working,
		     &thread_parameters->cond_mutex);

  printf ("Thread woken up to find primes less than %ld.\n",
	  thread_parameters->max_search);

  an =
    (factor_t *) calloc (thread_parameters->max_search, sizeof (factor_t));
  assert (an != NULL);

  limit = (long) sqrt (thread_parameters->max_search) + 1;
  
  for (prime = 2; prime <= limit; ++prime)
    for (i = 2; i * prime < thread_parameters->max_search; ++i)
      {
	an[i * prime].f[0] = i;
	an[i * prime].f[1] = prime;

      }
	 
  for (i = 2; i < thread_parameters->max_search; ++i)
    if (!an[i].f[0])
      printf ("* %ld is prime.\n", i);
#ifdef SHOW_NONPRIME
    else
      {
	printf ("  %ld is nonprime, factorization: (", i);
	print_factorization (i, an);
	puts (")");
      }
#endif

  free (an);

  return NULL;
}

int
main (int argc, char *argv[])
{
  
  thread_parameters_t thread_parameters;
 
  pthread_cond_init (&thread_parameters.start_working, NULL);
  pthread_mutex_init (&thread_parameters.cond_mutex, NULL);

  
  pthread_t computational_thread;

  
  pthread_create (&computational_thread, NULL, primes_computer_runner,
		  (void *) &thread_parameters);

  puts (
	"Enter an integer and the program will print all primes less than\n"
	"the integer you enter. (^D or ^C to quit.)\n"
#ifdef SHOW_NONPRIME
	
#endif
    );

  
  if (!scanf ("%ld", &thread_parameters.max_search))
    return 0;
  
  pthread_cond_broadcast (&thread_parameters.start_working);
  
  pthread_join (computational_thread, NULL);

  return 0;
}
