#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <ctype.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char** argv){
    DIR *dirp = opendir("/proc");

    int descriptor_var, descriptor = open("/proc/self/fd/0", O_RDONLY);
    char *tty = malloc(sizeof(char) * 1000);
    sprintf(tty, "%s", ttyname(descriptor));
    printf("%5s %s %8s %s\n", "CMD", "PID", "TTY", "TIME");

    struct dirent *dp;
    char flag;
    FILE* file;
    unsigned long time, stime;
    char *tty_var;
    char *time_s = malloc(sizeof(char) * 1000);
    char *cmd = malloc(sizeof(char) * 1000);
    char *path = malloc(sizeof(char) * 1000);
    int i;
    while ((dp = readdir(dirp)) != NULL) {
        flag = 1;
        for (i = 0; dp->d_name[i]; ++i)
            if (!isdigit(dp->d_name[i])) {
                flag = 0;
                break;
            }

        if (flag) {
            sprintf(path, "/proc/%s/fd/0", dp->d_name);
            descriptor_var = open(path, O_RDONLY);
            tty_var = ttyname(descriptor_var);

            if (tty_var && strcmp(tty_var, tty) == 0) {
                sprintf(path, "/proc/%s/stat", dp->d_name);
                file = fopen(path, "r");
                fscanf(file, "%d%s%c%c%c", &i, cmd, &flag, &flag, &flag);
                cmd[strlen(cmd) - 1] = '\0';

                for (i = 0; i < 11; ++i) {
                    fscanf(file, "%lu", &time);
                }

                fscanf(file, "%lu", &stime);
                time = (int)((double)(time + stime) / sysconf(_SC_CLK_TCK));
                sprintf(time_s, "%02lu:%02lu:%02lu", (time / 3600) % 3600, (time / 60) % 60, time % 60);
                printf("%5s %s %8s %s\n", cmd + 1, dp->d_name, tty_var + 5, time_s);
                fclose(file);
            }
            close(descriptor_var);
        }
    }
    close(descriptor);
    return 0;
}