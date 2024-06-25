#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>

int execute_html() {

    DIR * dir_ptr;
    struct dirent * file;
    char filename[512], sys_cmd[256];
    int fd;


    if ((dir_ptr = opendir("test/HTML")) != NULL) {
        while ((file = readdir(dir_ptr)) != NULL) {

            if ((file->d_name[0] == '.') || (file->d_type == DT_DIR)) {
            continue;
            }

            sprintf(filename, "test/HTML/%s", file->d_name);
            sprintf(sys_cmd, "./testHTML %s", filename);


            if (system(sys_cmd) != 0) {
                fprintf(stderr, "execute_html failed\n");
                return 1;
            }
        }


        fd = open("info_total", O_WRONLY);
        // dup2(1, fd);

        // make .gcov file
        if (system("gcov -i -b -c testHTML.c > info_total") != 0) {
            fprintf(stderr, "\"gcov -i -b -c testHTML.c\" failed\n");
            return 1;
        }

        close(fd);
        closedir(dir_ptr);
        return 0;
    }
    return 1;
}

int print() {
    FILE * fp;
    char line[256];
    char * percent, * cnt;
    float f_percent, f_cnt, num;

    // open .gcov file
    if ((fp = fopen("info_total", "r")) == NULL) {
        fprintf(stderr, "open info_total failed\n");
        return 1;
    }

    while ((fgets(line, 256, fp)) != NULL) {
        line[strlen(line) - 1] = '\0';
        if (line[0] == 'L' || line[0] == 'T') {
            cnt = strrchr(line, ' ') + 1;
            f_cnt = atoi(cnt);

            percent = strchr(line, ':') + 1;
            strtok(percent, "%");
            f_percent = atof(percent);

            num = f_percent / 100 * f_cnt;

            printf("%d\n", (int)round(num));
        }
    }
    fclose(fp);
    return 0;
}

int main() {

    if (execute_html() == 1) {
        fprintf(stderr, "execute_html() failed\n");
        exit(EXIT_FAILURE);
    }

    printf("\n\n");

    if (print() == 1) {
        fprintf(stderr, "print() failed\n");
        exit(EXIT_FAILURE);
    }
    


}
