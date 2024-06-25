#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <stdbool.h>
#include <pthread.h> 
#include <sys/socket.h> 
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ncurses.h>
#include "cJSON/cJSON.h" 
#include "cJSON/cJSON.c" 


#define CELL_SIZE 16


char ip[16];
int port;
int userid = 4;
int conn;

bool first_flag = false;    // used for identifying map loading whether it is first loading

char player_img[4][32] = {
    "image/player0.png",
    "image/player1.png",
    "image/player2.png",
    "image/player3.png"
};

char base_img[4][32] = {
    "image/base0.png",
    "image/base1.png",
    "image/base2.png",
    "image/base3.png"
};


GtkWidget *score_label;
int score = 0;
GtkWidget *window;
GtkWidget ***cell; 
GtkWidget *table;
GtkWidget *hbox;
GtkWidget *my_icon;

typedef struct User{
    int id;
    int user_x;
    int user_y;
    int base_x;
    int base_y;
    int score;
    char name[9];
} user;

typedef struct Pos{
    int x;
    int y;
} pos;

typedef struct Model{
    int row;
    int col;
    struct timeval timeout;
    user users[4];
    pos *obstacles;
    int n_obstacles;
    pos *balls;
    int n_balls;
} model;

model map;

//make new sock and connection with ip,port and return the sock_fd
// if it was succeed return 0, otherwise return 1
int makeConnection(){

   struct sockaddr_in serv_addr; 
   conn = socket(AF_INET, SOCK_STREAM, 0) ;
   
   if (conn <= 0) {
      fprintf(stderr,  " socket failed\n");
      return 1;
   } 

   memset(&serv_addr, '\0', sizeof(serv_addr)); 
   serv_addr.sin_family = AF_INET; 
   serv_addr.sin_port = htons(port); 
   if (inet_pton(AF_INET, ip, &serv_addr.sin_addr) <= 0) {
      fprintf(stderr, " inet_pton failed\n");
      return 1;
   }
   if (connect(conn, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
      fprintf(stderr, " connect failed\n");
      return 1;
   }

   return 0;
}

void update_score() {
    gchar *score_str = g_strdup_printf("Score: %d", score);
    gtk_label_set_text(GTK_LABEL(score_label), score_str);
    g_free(score_str);
}

void button_clicked(GtkWidget *widget, gpointer data) {
    const char *direction = (const char *)data;

    score++;
    update_score();
    
    g_print("Button clicked: %s\n", direction);
}

// if it was succeed return positive num, otherwise return -1
int find_id(char * nickname) {
    int i;
    for (i= 0; i < 4; i++) {
        if(strcmp(map.users[i].name, nickname)==0){
            return map.users[i].id;
        }
    }
    return -1;
}

// if (row,col) is in obstacle position return 0, otherwise return 1
int check_obstacle(int row, int col) {
    int i;
    for (i = 0; i < map.n_obstacles; i++) {
        if ((col == map.obstacles[i].x) && (row == map.obstacles[i].y)) {
            return 0;
        }
    }
    return 1;
}

// if (row,col) is in ball position return 0, otherwise return 1
int check_ball(int row, int col) {
    int i;
    for (i = 0; i < map.n_balls; i++) {
        if ((col == map.balls[i].x) && (row == map.balls[i].y)) {
            return 0;
        }
    }
    return 1;
}

// if (row,col) is in base position return 1
// if (row,col) is in user position return 2
// otherwise return 0
int check_userinfo(int row, int col) {
    int i;
    for (i = 0; i < 4; i++) {
        if ((col == map.users[i].base_x) && (row == map.users[i].base_y)) {
            return 1;
        }
        else if ((col == map.users[i].user_x) && (row == map.users[i].user_y)) {
            return 2;
        }
    }
    return 0;
}


// if it was succeed return 0, otherwise return 1
int send_bytes(int fd, void * buf, size_t len) {
    char * p = buf ;
    size_t acc = 0 ;

    while (acc < len) {
        size_t sent ;
        sent = send(fd, p, len - acc, MSG_NOSIGNAL) ;
        if (sent == 0)
            return 1 ;
        p += sent ;
        acc += sent ;
    }
    return 0 ;
}

// if it was succeed return 0, otherwise return 1
int recv_bytes(int fd, void * buf, size_t len){
    char * p = buf ;
    size_t acc = 0 ;

    while (acc < len) {
        size_t recved ;
        recved = recv(fd, p, len - acc, MSG_NOSIGNAL) ;
        if (recved == 0)
            return 1 ;
        p += recved ;
        acc += recved ;
    }
    return 0 ;
}


void on_key_press(GtkWidget *widget, GdkEventKey *event, gpointer user_data) {
    char cmd[3];
    printf("%d\n", userid);

    switch (event->keyval) {
        case GDK_KEY_Up:
            g_print("Up key pressed\n");
            snprintf(cmd, 3, "%dU", userid);
            break;
        case GDK_KEY_Down:
            g_print("Down key pressed\n");
            snprintf(cmd, 3, "%dD", userid);
            break;
        case GDK_KEY_Left:
            g_print("Left key pressed\n");
            snprintf(cmd, 3, "%dL", userid);
            break;
        case GDK_KEY_Right:
            g_print("Right key pressed\n");
            snprintf(cmd, 3, "%dR", userid);
            break;
        default:
            break;
    }

    // send cmd to server
    if (send_bytes(conn, cmd, 2) == 1) {
        fprintf(stderr, "send key failed\n");
    }
    printf("msg sent: %s\n", cmd);
}

// if it was succeed return 0, otherwise return 1
int read_mapfile(char * map_info) {
    // parse the JSON data 
    cJSON *json = cJSON_Parse(map_info); 

    if (json == NULL) { 
        const char *error_ptr = cJSON_GetErrorPtr(); 
        if (error_ptr != NULL) { 
            printf("Error: %s\n", error_ptr); 
        } 
        cJSON_Delete(json); 
        return 1; 
    }

    cJSON *mapsize = cJSON_GetObjectItem(json,"mapsize");
    cJSON *j_row = cJSON_GetObjectItem(mapsize, "rows");
    cJSON *j_col = cJSON_GetObjectItem(mapsize, "cols");
    map.row = j_row->valueint;
    map.col = j_col->valueint;

    cJSON *j_timeout = cJSON_GetObjectItemCaseSensitive(json, "timeout");
    gettimeofday(&map.timeout, NULL);

    int i;

    cJSON *j_user = cJSON_GetObjectItem(json,"user");
    cJSON *id, *j_base, *base_x, *base_y, *j2_user, *user_x, *user_y, *j_score, *j_name;
    cJSON * subitem;
    for (i = 0 ; i < cJSON_GetArraySize(j_user) ; i++){
        subitem = cJSON_GetArrayItem(j_user, i);
        id = cJSON_GetObjectItem(subitem, "id");
        j_base = cJSON_GetObjectItem(subitem, "base");
        base_x = cJSON_GetArrayItem(j_base, 0);
        base_y = cJSON_GetArrayItem(j_base, 1);
        j2_user = cJSON_GetObjectItem(subitem, "user");
        user_x = cJSON_GetArrayItem(j2_user, 0);
        user_y = cJSON_GetArrayItem(j2_user, 1);
        j_score = cJSON_GetObjectItem(subitem, "score"); 
        j_name = cJSON_GetObjectItem(subitem, "name"); 

        map.users[i].id = id->valueint;
        map.users[i].user_x = user_x->valueint;
        map.users[i].user_y = user_y->valueint;
        map.users[i].base_x = base_x->valueint;
        map.users[i].base_y = base_y->valueint;
        map.users[i].score = j_score->valueint;
        strcpy(map.users[i].name, j_name->valuestring);

    }

    cJSON * j_obstacle = cJSON_GetObjectItem(json,"obstacle");
    cJSON * x, * y;
    map.n_obstacles = cJSON_GetArraySize(j_obstacle);
    map.obstacles = malloc(sizeof(pos) * map.n_obstacles);
    
    printf("%d\n", map.n_obstacles);
    for (i = 0 ; i < cJSON_GetArraySize(j_obstacle) ; i++){
        subitem = cJSON_GetArrayItem(j_obstacle, i);
        x = cJSON_GetArrayItem(subitem, 0);
        y = cJSON_GetArrayItem(subitem, 1);

        map.obstacles[i].x = x->valueint;
        map.obstacles[i].y = y->valueint;
    }

    cJSON * j_ball = cJSON_GetObjectItem(json,"ball");
    map.n_balls = cJSON_GetArraySize(j_ball);
    map.balls = malloc(sizeof(pos) * map.n_balls);
    
    for (i = 0 ; i < cJSON_GetArraySize(j_ball) ; i++){
        subitem = cJSON_GetArrayItem(j_ball, i);
        x = cJSON_GetArrayItem(subitem, 0);
        y = cJSON_GetArrayItem(subitem, 1);

        map.balls[i].x = x->valueint;
        map.balls[i].y = y->valueint;
    }
    
    return 0;
}


void print_model() {
    printf("Row: %d\n", map.row);
    printf("Col: %d\n", map.col);

    printf("Timeout: %ld seconds\n", map.timeout.tv_sec);

    printf("Users:\n");
    for (int i = 0; i < 4; i++) {
        printf("  User %d:\n", i + 1);
        printf("    ID: %d\n", map.users[i].id);
        printf("    User X: %d\n", map.users[i].user_x);
        printf("    User Y: %d\n", map.users[i].user_y);
        printf("    Base X: %d\n", map.users[i].base_x);
        printf("    Base Y: %d\n", map.users[i].base_y);
        printf("    Score: %d\n", map.users[i].score);
        printf("    Name: %s\n", map.users[i].name);
    }

    printf("Obstacles:\n");
    for (int i = 0; i < map.n_obstacles; i++) {
        printf("  Obstacle %d: X: %d, Y: %d\n", i + 1, map.obstacles[i].x, map.obstacles[i].y);
    }

    printf("Balls:\n");
    for (int i = 0; i < map.n_balls; i++) {
        printf("  Ball %d: X: %d, Y: %d\n", i + 1, map.balls[i].x, map.balls[i].y);
    }
}


void update_score_label() {
    gchar *label_text = g_strdup_printf("%s: %d\n%s: %d\n%s: %d\n%s %d\n", map.users[0].name, map.users[0].score, 
    map.users[1].name, map.users[1].score, map.users[2].name, map.users[2].score, 
    map.users[3].name, map.users[3].score);
    gtk_label_set_text(GTK_LABEL(score_label), label_text);
    g_free(label_text);
}


void make_cell(int row, int col) {
    int i, j;
    cell = (GtkWidget ***)malloc(row * sizeof(GtkWidget **));
    for (i = 0; i < row; i++) {
        cell[i] = (GtkWidget **)malloc(col * sizeof(GtkWidget *));
        for (j = 0; j < col; j++) {
            cell[i][j] = NULL;  
        }
    }
}


// value of users means the index of ball which is changed (-1 means doesn't changed)
// value of ball means the index of ball which is changed (-1 means doesn't changed)
// prev_x and prev_y is info about user position (-1 for first load map)
void update_view(int user, int ball, int prev_x, int prev_y) {

    printf("user: %d, ball: %d, prev_x: %d, prev_y: %d\n\n\n\n", user, ball, prev_x, prev_y);
    GtkWidget *event_box;

    int i, j;
    bool prev_base = false;
    int random;
    int n_user = 0, n_base = 0;

    srand(time(NULL));

    if (first_flag == false) {
        for (i = 0; i < map.row; i++) {
            for (j = 0; j < map.col; j++) {
                event_box = gtk_event_box_new();

                random = rand() % 100;

                if (i == 0 || j == 0 || i == map.row - 1 || j == map.col - 1 || check_obstacle(i, j) == 0) {
                    // Create a GtkImage and set it to your image file
                    if (random % 3 == 0) {
                        cell[i][j] = gtk_image_new_from_file("image/building1.png"); 
                    }
                    else if (random % 4 == 0) {
                        cell[i][j] = gtk_image_new_from_file("image/building2.png");
                    }
                    else if (random % 5 == 0) {
                        cell[i][j] = gtk_image_new_from_file("image/building3.png");
                    }
                    else {
                        cell[i][j] = gtk_image_new_from_file("image/building4.png");
                    } 
                } else if (check_userinfo(i, j) == 1) {
                    // base
                    if (n_base == 0) {
                        cell[i][j] = gtk_image_new_from_file("image/base0.png");
                    }
                    else if (n_base == 1) {
                        cell[i][j] = gtk_image_new_from_file("image/base1.png");
                    }
                    else if (n_base == 2) {
                        cell[i][j] = gtk_image_new_from_file("image/base2.png");
                    }
                    else if (n_base == 3) {
                        cell[i][j] = gtk_image_new_from_file("image/base3.png");
                    }
                    n_base++;
                } else if (check_ball(i, j) == 0) {
                    cell[i][j] = gtk_image_new_from_file("image/banana16.png");
                } else if (check_userinfo(i, j) == 2) {
                    // user 
                    if (n_user == 0) {
                        cell[i][j] = gtk_image_new_from_file(player_img[0]);
                    }
                    else if (n_user == 1) {
                        cell[i][j] = gtk_image_new_from_file(player_img[1]);
                    }
                    else if (n_user == 2) {
                        cell[i][j] = gtk_image_new_from_file(player_img[2]);
                    }
                    else if (n_user == 3) {
                        cell[i][j] = gtk_image_new_from_file(player_img[3]);
                    }
                    n_user++;
                } else {
                    cell[i][j] = gtk_image_new_from_file("image/marble16.png");
                }

                gtk_container_add(GTK_CONTAINER(event_box), cell[i][j]);
                gtk_widget_set_size_request(event_box, CELL_SIZE, CELL_SIZE);
                gtk_table_attach_defaults(GTK_TABLE(table), event_box, j, j + 1, i, i + 1);
            }
        } 
        // update my icon
        gtk_image_set_from_file(GTK_IMAGE(my_icon), player_img[userid]);

        first_flag = true;
    } else {
        // update prev position of user 
        for (i = 0; i < 4; i++) {
            // case in prev postition is base
            if ((prev_x == map.users[i].base_x) && (prev_y == map.users[i].base_y)) {
                gtk_image_set_from_file(GTK_IMAGE(cell[prev_y][prev_x]), base_img[user]); 
                prev_base = true;
                break;
            }
        }
        if (prev_base == false) {
            gtk_image_set_from_file(GTK_IMAGE(cell[prev_y][prev_x]), "image/marble16.png"); 
        }
        
         // update current ball postion
        if (ball != -1) {
            gtk_image_set_from_file(GTK_IMAGE(cell[map.balls[ball].y][map.balls[ball].x]), "image/banana16.png"); 
        }

        // update current user postion
        gtk_image_set_from_file(GTK_IMAGE(cell[map.users[user].user_y][map.users[user].user_x]), player_img[user]); 

    }

    gtk_widget_show_all(window);
}


void * input(void * arg) {
    char buf[2];
    int result;
    int k, c, i;
    bool updated_flag;
    int ball = -1;
    int prev_x, prev_y;     // prev_x and prev_y is info about user position

    // receive echoed direction
    while ((result = recv_bytes(conn, buf, 2)) != 1) {
        // printf("<<<<<<<------k: %d      buf: %s------>>>>>>>>>\n", k++, buf);

        updated_flag = false;

        if (strncmp(buf, "FN", 2) == 0 || strncmp(buf, "TO", 2) == 0) {
            printf("TO saved\n");
            break;
        }

        prev_x = map.users[buf[0] - '0'].user_x;
        prev_y = map.users[buf[0] - '0'].user_y;

        // update user
        if (buf[1] == 'U') {
            // move user
            map.users[buf[0] - '0'].user_y--;
        }
        else if (buf[1] == 'L') {
            map.users[buf[0] - '0'].user_x--;
        }
        else if (buf[1] == 'R') {
            map.users[buf[0] - '0'].user_x++;
        }
        else if (buf[1] == 'D') {
            map.users[buf[0] - '0'].user_y++;
        }

        // update ball
        for (i = 0; i < map.n_balls; i++) {
            if ((map.users[buf[0] - '0'].user_x == map.balls[i].x) && (map.users[buf[0] - '0'].user_y == map.balls[i].y)) {
                // move ball
                if (buf[1] == 'U') {
                    map.balls[i].y--;
                }
                else if (buf[1] == 'L') {
                    map.balls[i].x--;
                }
                else if (buf[1] == 'R') {
                    map.balls[i].x++;
                }
                else if (buf[1] == 'D') {
                    map.balls[i].y++;
                }
                
                // if ball encouter in base
                for (c = 0; c < 4; c++) {
                    if ((map.users[c].base_x == map.balls[i].x) && (map.users[c].base_y == map.balls[i].y)) {
                        // update score;
                        map.users[c].score++;
                        map.balls[i].x = map.balls[map.n_balls - 1].x;
                        map.balls[i].y = map.balls[map.n_balls - 1].y;
                        // free(&map.balls[map.n_balls - 1]);
                        map.n_balls--;

                        update_view(buf[0] - '0', -1, prev_x, prev_y);
                        update_score_label(); 
                        updated_flag = true;
                        break;
                    }
                }
                if (updated_flag == false) {
                    update_view(buf[0] - '0', i, prev_x, prev_y);
                    updated_flag = true;
                }
                break;
            }
        }

        if (updated_flag == false) {
            update_view(buf[0] - '0', -1, prev_x, prev_y);
        }

        print_model();
        printf("<<<<<<<------k: %d      buf: %s------>>>>>>>>>\n", k++, buf);
    }

    if (result == 1) {
        fprintf(stderr, "receive echoed direction failed\n");
    }

}


int main(int argc, char *argv[]) {
    // --------------------------------map---------------------------------
    gtk_init(&argc, &argv);
    GdkColor color;
    gdk_color_parse("#F8EDFF", &color);
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_widget_modify_bg(window, GTK_STATE_NORMAL, &color);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_title(GTK_WINDOW(window), "PushPush");
    // --------------------------------map---------------------------------


    if (argc != 4) {
        fprintf(stderr, "Usage: ./%s <IP> <port> <nickname>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    else if (strlen(argv[3]) > 8) {
        fprintf(stderr, "Nicknames must be less than 9 characters\n");
        exit(EXIT_FAILURE);
    }

    strcpy(ip, argv[1]);
    port = atoi(argv[2]);

    char nickname[9];
    strcpy(nickname, argv[3]);

    // make connection
    if (makeConnection() == 1) {
        fprintf(stderr, "connect failed\n");
        exit(EXIT_FAILURE);
    }

    // send nickname to server
    if (send_bytes(conn, nickname, 9) == 1) {
        fprintf(stderr, "send nickname failed\n");
        exit(EXIT_FAILURE);
    }

    // receive map file size info
    int file_size;
    if (recv_bytes(conn, &file_size, 4) == 1) {
        fprintf(stderr, "receive file size failed\n");
        exit(EXIT_FAILURE);
    }

    // malloc memory to save serialized json file
    char *map_info = (char *)malloc(file_size);
    if (map_info == NULL) {
        fprintf(stderr, "Error allocating memory\n");
        return 1;
    }

    // recv serialized json file
    if (recv_bytes(conn, map_info, file_size) == 1) {
        fprintf(stderr, "read map failed\n");
        exit(EXIT_FAILURE);
    }

    printf("--------------\n");
    if (read_mapfile(map_info) != 0) {
        fprintf(stderr, "parsing map failed\n");
        exit(EXIT_FAILURE);
    }

    free (map_info);
    print_model();

    // recv timestamp
    if (recv_bytes(conn, (void *)&map.timeout, sizeof(struct timeval)) == 1) {
        fprintf(stderr, "read map failed\n");
        exit(EXIT_FAILURE);
    }

    // find id of user
    if ((userid = find_id(nickname)) == -1) {
        fprintf(stderr, "namelist error\n");
        exit(EXIT_FAILURE);
    }

    // printf("prev: %d\n", userid);

    // --------------------------------map---------------------------------
    GtkWidget *frame;

    // Create a hbox to hold all the widgets
    hbox = gtk_hbox_new(FALSE, 5);
    gtk_container_add(GTK_CONTAINER(window), hbox);

    // Create a table for the cells
    table = gtk_table_new(map.row, map.col, TRUE);
    gtk_container_set_border_width(GTK_CONTAINER(table), 0);  // Set padding to 0
    gtk_box_pack_start(GTK_BOX(hbox), table, TRUE, TRUE, 0);


    // Create a box to hold the content vertically
    GtkWidget *vbox = gtk_vbox_new(FALSE, 5); // Adjust spacing as needed
    gtk_box_pack_start(GTK_BOX(hbox), vbox, FALSE, FALSE, 0);

    // Add an empty, expandable space to center the vbox
    GtkWidget *spacer = gtk_label_new(NULL);
    gtk_box_pack_start(GTK_BOX(vbox), spacer, TRUE, TRUE, 0);

    // Create a label for "I am" and add it to the vbox
    GtkWidget *iam_label = gtk_label_new("I am");
    gtk_box_pack_start(GTK_BOX(vbox), iam_label, FALSE, FALSE, 0);

    // Create an image and add it to the vbox
    my_icon = gtk_image_new(); // Replace with the actual path
    gtk_box_pack_start(GTK_BOX(vbox), my_icon, FALSE, FALSE, 0);

    // Create a frame for additional content
    gdk_color_parse("#FFF6F6", &color);
    frame = gtk_frame_new("Score");
    gtk_widget_modify_bg(frame, GTK_STATE_NORMAL, &color);
    gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, FALSE, 0);

    // Create a label for the score and add it to the frame
    score_label = gtk_label_new(NULL);  // Create label without initial text
    update_score_label();  // Call the function to set the initial text
    gtk_container_add(GTK_CONTAINER(frame), score_label);

    // Add an empty, expandable space to center the vbox
    GtkWidget *spacer2 = gtk_label_new(NULL);
    gtk_box_pack_start(GTK_BOX(vbox), spacer2, TRUE, TRUE, 0);

    // Add an empty, expandable space to center the vbox
    GtkWidget *exit_button = gtk_button_new_with_label("Exit");
    gdk_color_parse("#AC87C5", &color);
    gtk_widget_modify_bg(exit_button, GTK_STATE_NORMAL, &color);
    g_signal_connect(G_OBJECT(exit_button), "clicked", G_CALLBACK(gtk_main_quit), NULL);
    gtk_box_pack_end(GTK_BOX(vbox), exit_button, FALSE, FALSE, 0);

    make_cell(map.row, map.col);

    update_view(-1, -1, -1, -1);

    // Connect the key press event to the on_key_press function
    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(G_OBJECT(window), "key-press-event", G_CALLBACK(on_key_press), NULL);
    // --------------------------------map---------------------------------
    

    // make thread
    pthread_t input_pid;
    if (pthread_create(&input_pid, NULL, (void *)input, NULL)) {
        perror("making thread failed\n");
        exit(EXIT_FAILURE);
    }

    // --------------------------------map---------------------------------
    gtk_main();
    // --------------------------------map---------------------------------

    pthread_join(input_pid, NULL);

    close (conn);

   printf("THE END\n");

    return 0;
}