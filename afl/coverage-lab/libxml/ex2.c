#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>

int count_line(char * filename) {
    char sys_cmd[128], line[256], write_line[128];
    int line_cnt = 0, branch_cnt = 0;
    FILE * fp, * info_fp;

    sprintf(sys_cmd, "./testHTML %s", filename);

    // make .gcov file
    if (system("gcov -i -b -c testHTML.c") != 0) {
        fprintf(stderr, "\"gcov -i -b -c testHTML.c\" failed\n");
        return 1;
    }

    // open .gcov file
    if ((fp = fopen("testHTML.c.gcov", "r")) == NULL) {
        fprintf(stderr, "open gcov file failed\n");
        return 1;
    }

    while ((fgets(line, 256, fp)) != NULL) {
        line[strlen(line) - 1] = '\0';
        if (line[0] == 'l' && atoi(strchr(line, ',') + 1) > 0) {
            line_cnt++;
        }
        else if (line[0] == 'b' && strcmp(strchr(line, ',') + 1, "taken") == 0) {
            branch_cnt++;
        }
    }

    // remove .gcda and .gcov file
    if (system("rm testHTML.c.gcov testHTML.gcda") != 0) {
        fprintf(stderr, "\"rm testHTML.c.gcov testHTML.gcda\" failed\n");
    }

    // open .gcov file
    if ((info_fp = fopen("info", "a")) == NULL) {
        fprintf(stderr, "open info_fpfile failed\n");
        return 1;
    }

    sprintf(write_line, "%s     line_cnt: %d    branch_cnt: %d\n", filename, line_cnt, branch_cnt);
    fputs(write_line, info_fp);

    fclose(info_fp);
    fclose(fp);
}

int execute_html() {

    DIR * dir_ptr;
    struct dirent * file;
    char filename[512], sys_cmd[256];


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

            if (count_line(filename) == 1) {
                fprintf(stderr, "make_file failed\n");
                return 1;
            }
        }

        closedir(dir_ptr);
        return 0;
    }
    return 1;
}

int main() {

    if (execute_html() == 1) {
        fprintf(stderr, "execute_html() failed\n");
        exit(EXIT_FAILURE);
    }
    


}
