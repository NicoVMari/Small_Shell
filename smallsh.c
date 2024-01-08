#include "smallsh.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

//char *prompt = "Scrivere un comando>";

void procline(void) 	/* tratta una riga di input */
{

  char *arg[MAXARG+1];	/* array di puntatori per runcommand */
  int toktype;  	/* tipo del simbolo nel comando */
  int narg;		/* numero di argomenti considerati finora */

  narg=0;

  do {

    /* mette un simbolo in arg[narg]
       ed esegue un'azione a seconda del tipo di simbolo */

    switch (toktype = gettok(&arg[narg])) {
      case ARG:
        /* se argomento: passa al prossimo simbolo */
        if (narg < MAXARG) narg++;
	      break;
      case AMPERSAND:
        if (narg != 0) {
          arg[narg] = NULL;
          runcommand(arg,1); //caso background
        }
        if (toktype != EOL)  narg = 0;
        break;
      case EOL:
      case SEMICOLON:
         /* se fine riga o ';' esegue il comando ora contenuto in arg,
	 mettendo NULL per indicare la fine degli argomenti:
         serve a execvp */
        if (narg != 0) {
          arg[narg] = NULL;
          runcommand(arg,0); //caso forground
        }
	/* se non fine riga (descrizione comando finisce con ';')
           bisogna ricominciare a riempire arg dall'indice 0 */
        if (toktype != EOL)  narg = 0;
        break;
     }
  }

  while (toktype != EOL);  /* fine riga, procline finita */

}

void runcommand(char **cline, int where)	/* esegue un comando */
{
  pid_t pid;
  int exitstat,ret;

  pid = fork();
  if (pid == (pid_t) -1) {
     perror("smallsh: fork fallita");
     return;
  }

  if (pid == (pid_t) 0) {
    execvp(*cline,cline);
    perror(*cline);
    exit(1);
  }

  if (!where) {
    ret = waitpid(pid,&exitstat,0);
    //ret = wait(&exitstat);
    if (ret == -1) perror("waitpid");
  }
}

int main()
{
  while(userin() != EOF)
    procline();
  return 0;
}
