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

void daemonp(const char *cmd);
void signal_handler(int signum);

int main() {
    signal(SIGTERM, signal_handler);

    return 0;
}

void daemonp(const char *cmd)
{
    pid_t pid = 0;
    pid_t sid = 0;
    int i, fd0, fd1, fd2;
    struct rlimit r_lim;
    struct sigaction sig_act;

    // clear file creation mask
    umask(0);

    // become a session leader to lose control TTY
    // call fork and have parent exit
    /* if ((pid = fork()) < 0)
        err_quit("%s: can't fork", cmd);
    else */
    if (pid != 0) //parent
        exit(0);

    //creates a new session
    setsid();

    // ensure future opens won't allocate controlling TTYs
    sig_act.sa_handler = SIG_IGN;
    sigemptyset(&sig_act.sa_mask);
    sig_act.sa_flags = 0;
    /* if (sigaction(SIGHUP, &sa, NULL) < 0)
        err_quit("%s: can't ignore SIGHUP", cmd);
    if ((pid = fork()) < 0)
        err_quit("%s: can't fork", cmd);
    else */
    if (pid != 0) // parent
        exit(0);

    // change working directory to the root directory
    /* if (chdir("/") < 0)
        err_quit("%s: can't change directory to /", cmd); */

    // unneeded file descriptors should be closed
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

    //initialize the log file
    openlog(cmd, LOG_CONS, LOG_DAEMON);
    if (fd0 != 0 || fd1 != 1 || fd2 != 2)
    {
        syslog(LOG_ERR, "unexpected file descriptors %d %d %d", fd0, fd1, fd2);
        exit(1);
    }
}

void signal_handler(int signum)
{
    switch(signum) {
        case SIGTERM:
        // shut down any and all child of the daemon process then exit
            break;
        case SIGUSR1:
        // terminate child process #1 and spawn a new child process
            break;
        case SIGUSR2:
        // terminate child process #2 and spawn a new child process
            break;

        // when your system is running properly, you will have at most
        // three process running: the daemon and the one child 
    }
}