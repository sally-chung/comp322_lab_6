#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <syslog.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

// terminal > parentp > childp

pid_t mole;


void term_signal_handler(int signum);
void usr_signal_handler(int signum);

int main()
{
    pid_t pid = 0;
    pid_t sid = 0;
    int i, fd0, fd1, fd2;
    struct rlimit r_lim;

    // clear file creation mask
    umask(0);

    pid = fork();

    // become a session leader to lose control TTY
    // call fork and have parent exit
    if (pid > 0) //parent
        exit(EXIT_SUCCESS);

    //creates a new session
    setsid();

    // change working directory to the root directory
    /* if (chdir("/") < 0)
        err_quit("%s: can't change directory to /", cmd); */

    // unneeded file descriptors should be closed
    getrlimit(RLIMIT_NOFILE, &r_lim);
    if (r_lim.rlim_max == RLIM_INFINITY)
        r_lim.rlim_max = 1024;
    for (i = 0; i < r_lim.rlim_max; i++)
        close(i);

    // some daemons open file descriptors 0, 1, and 2 to /dev/null
    // so that any library routines that try to read from std input
    // or write to std output or std error will have no effect
    fd0 = open("/dev/null", O_RDWR);
    fd1 = dup(0);
    fd2 = dup(0);

    signal(SIGTERM, term_signal_handler);
    signal(SIGUSR1, usr_signal_handler);
    signal(SIGUSR2, usr_signal_handler);

    mole = pid;
}

void usr_signal_handler(int signum)
{
    if (mole == 0)
    {
        char *mole_name;
        char *write_path[3];

        int rand_num = rand() % 2;

        // char buffer[1024];
        // char *path_name = getcwd(buffer, 1024);
        // strcat(path_name, "/mole");
        write_path[0] = "/mole";

        if (rand_num == 1)
            mole_name = "mole1";
        else
            mole_name = "mole2";

        write_path[1] = mole_name;
        write_path[2] = NULL;
        execv(write_path[0], write_path);
    }
}
void term_signal_handler(int signum)
{
    // shut down any and all child of the daemon process then exit
    if (mole != 0)
        kill(mole, SIGKILL); // current child process killed
    kill(getpid(), SIGKILL); // kill daemon
}

// when your system is running properly, you will have at most
// three process running: the daemon and the one child