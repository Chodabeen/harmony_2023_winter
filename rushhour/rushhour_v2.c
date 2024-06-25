#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <glib.h>

/* NOFIX --- */

typedef enum {
  start,
  left,
  right,
  up,
  down,
  quit,
  N_op
} commands ;

typedef enum {
  vertical,
  horizontal
} direction ;

typedef struct {
  int id ;
  int y1, y2 ;    // y1: the minimum of y, y2: the maximum of y
  int x1, x2 ;    // x1: the minimum of x, x2: the maximum of x
  int span ;        // the number of cells
  direction dir ;    // the direction of the car
} car_t ;

char * op_str[N_op] = {
  "start",
  "left",
  "right",
  "up",
  "down",
  "quit"
} ;

int n_cars = 0 ;
// cells[Y][X]
// A6 -> cells[5][0]
// F4 -> cells[3][5]
// F1 -> cells[0][5]

/* --- NOFIX */

commands
get_op_code (char * s)
{
  // return the corresponding number for the command given as s.
  // FIXME
  if (strcmp(strtok(s, " "), "start") == 0)
      return start;
  else if (strcmp(strtok(s, " "), "left") == 0)
      return left;
  else if (strcmp(strtok(s, " "), "right") == 0)
      return right;
  else if (strcmp(strtok(s, " "), "up") == 0)
      return up;
  else if (strcmp(strtok(s, " "), "down") == 0)
      return down;
  else if (strcmp(strtok(s, " "), "quit") == 0)
      return quit;
  else {
      fprintf(stderr, "invalid command\n");
      return N_op;
    }
}

typedef struct state state;

struct state {
  int cells[6][6];
  car_t * cars;
  state * from;
};

typedef struct {
  state * next;
} state_head;

state * make_state(state * from) {
  state * st = malloc(sizeof(state));
  st->cars = 0x0;
  st->from = from;
  
  return st;
}

int
load_game (char * filename, state * st)
{
  //FIXME
  // load_game returns 0 for a success, or return 1 for a failure.
  // Use fopen, getline, strtok, atoi, strcmp
  // Note that the last character of a line obtained by getline may be '\n'.
  char * str = NULL;
  size_t num = 0;
  ssize_t read;
  int i = 0, count_line = 0;
// printf("1");
  
  FILE * fp = fopen(filename, "r");
  // check if the file exists
  if (!fp) {
      fprintf(stderr, "Error opening file '%s'\n", filename);
      return 1;
  }

  // read the first line
  getline(&str, &num, fp);
  n_cars = atoi(str);
  // check the number of first line is valid
  if (isdigit(str[0]) && n_cars > 1 && n_cars < 37)
    st->cars = malloc(sizeof(car_t) * n_cars);
  else
    return 1;

  // save the information of the each car_t
  while ((read = getline(&str, &num, fp)) != -1 && !(isspace(str[0]))) {
    // check the exceed of information of the cars
    if (++count_line > n_cars)
      return 1;
    st->cars[i].id = i + 1;

    // split cell
    char *ptr = strtok(str, ":");
    char *pivot = ptr;
    char c1 = *pivot;     // c1 is column of pivot
    // check colum of pivot is between 'A' and 'F'
    if (c1 < 'A' || 'F' < c1) {
      printf("haha");
      return 1;
    }
    st->cars[i].x1 = (int)c1;
    st->cars[i].y2 = atoi(pivot + 1);
    // check the coordinate is out of the grid
    if (st->cars[i].x1 < 'A' || st->cars[i].x1 > 'F')
      return 1;
    // check the coordinate is out of the grid
    if (st->cars[i].y2 < 1 || st->cars[i].y2 > 6)
      return 1;

    // split direction
    ptr = strtok(NULL, ":");
    if (strcmp(ptr, "vertical") == 0)
        st->cars[i].dir = vertical;
    else if (strcmp(ptr, "horizontal") == 0)
        st->cars[i].dir = horizontal;

    // check the first car location(it has to be located in row4 && horizontal)
    if (st->cars[0].dir == vertical || st->cars[0].y2 != 4)
      return 1;

    // split span
    ptr = strtok(NULL, ":");
    st->cars[i].span = atoi(ptr);
    if (st->cars[i].span < 1 || 6 < st->cars[i].span)
      return 1;

    // set the value of y1, x2
    // cars[i].y1 = cars[i].dir == vertical ? cars[i].y2 - cars[i].span + 1 : cars[i].y2;
    // cars[i].x2 = cars[i].dir == vertical ? cars[i].x2 = cars[i].x1 : cars[i].x2 = cars[i].x1 + cars[i].span - 1;
    if (st->cars[i].dir == vertical) {
        st->cars[i].y1 = st->cars[i].y2 - st->cars[i].span + 1;
        st->cars[i].x2 = st->cars[i].x1;
    }
    else if (st->cars[i].dir == horizontal) {
        st->cars[i].y1 = st->cars[i].y2;
        st->cars[i].x2 = st->cars[i].x1 + st->cars[i].span - 1;
    }
    // check the coordinate is out of the grid
    if (st->cars[i].y1 < 1)
      return 1;
    // check the coordinate is out of the grid
    if (st->cars[i].x2 > 'F')
      return 1;

    // // check the overlapping areas of cars
    // for (int count = 0; count < i; count++) {
    // 	if (cars[i].x1 >= cars[count].x1 
    // })
    i++;
  }
  
  // check the lack of information of the cars
  if (count_line < n_cars) {
      return 1;
  }

  fclose(fp);
  free(str);
  return 0;
}


void
display (GtkWidget * buttons[][6], state * st)
{
  /* The beginning state of board1.txt must be shown as follows:
    + + 2 + + +
    + + 2 + + +
    1 1 2 + + +
    3 3 3 + + 4
    + + + + + 4
    + + + + + 4
  */

  //FIXME

  int (*cells)[6] = st->cells;

  GdkColor newColor;
  gdk_color_parse("#E0AED0", &newColor);
  GtkWidget *targetButton;

  for (int row = 0; row < 6; row++) {
      for (int col = 0; col < 6; col++) {
          if (cells[row][col] != 0) {
            targetButton = buttons[row][col];
            gtk_widget_modify_bg(targetButton, GTK_STATE_NORMAL, &newColor);
            
            // Convert integer to string before setting the label
            gchar *label_text = g_strdup_printf("%d", cells[row][col]);
            gtk_button_set_label(GTK_BUTTON(targetButton), label_text);
            g_free(label_text);
          }
      }
  }

}

int
update_cells (state * st)
{
  int (*cells)[6] = st->cells;
  car_t * cars = st->cars;

  memset(cells, 0, sizeof(int) * 36) ; // clear cells before the write.

  //FIXME
  // return 0 for sucess
  // return 1 if the given car information (cars) has a problem
  for (int id = 0; id < n_cars; id++) {
      for (int row = 0; row < 6; row++) {
          for (int col = 0; col < 6; col++) {
              if (cars[id].dir == vertical) {
                  if (row >= (6 - cars[id].y2) && row <= (6 - cars[id].y1) && col == (cars[id].x1 - 'A'))
                      cells[row][col] = id + 1;
              }
              else if (cars[id].dir == horizontal) {
                  if (row == (6 - cars[id].y1) && col >= (cars[id].x1 - 'A') && col <= (cars[id].x2 - 'A'))
                      cells[row][col] = id + 1;
              }
          }
      }
  }
  return 0;
}

int
move (int id, int op, state * st)
{
  //FIXME
  // move returns 1 when the given input is invalid.
  // or return 0 for a success.

  // Update cars[id].x1, cars[id].x2, cars[id].y1 and cars[id].y2
  //   according to the given command (op) if it is possible.

  // The condition that car_id can move left is when
  //  (1) car_id is horizontally placed, and
  //  (2) the minimum x value of car_id is greather than 0, and
  //  (3) no car is placed at cells[cars[id].y1][cars[id].x1-1].
  // You can find the condition for moving right, up, down as
  //   a similar fashion.

  int (*cells)[6] = st->cells;
  car_t * cars = st->cars;

  int index = id - 1;

  
  // check if the cars exist
  if  (cars == 0x0) {
    printf("here33\n");
      return 1;
  }
  else if (id <= 0 || id > n_cars)
      return 1;
  

  switch (op) {
    case left:
      // check the result is not in grid or car is overlapped
      // check the move is possible depending on its direction
      if (cars[index].x1 - 'A'- 1 >= 0 && cells[6 - cars[index].y1][cars[index].x1 - 'A' - 1] == 0
        && cars[index].dir == horizontal) {
        cars[index].x1--;
        cars[index].x2--;
      }
      else
        return 1;
      break;
    case right:
      // check the result is not in grid or car is overlapped
      // check the move is possible depending on its direction
      if (cars[index].x2 - 'A' + 1 <= 5 && cells[6 - cars[index].y1][cars[index].x2 - 'A' + 1] == 0
      && cars[id-1].dir == horizontal) {
            cars[index].x1++;
            cars[index].x2++;
          }
      else
        return 1;
      break;
    case up:
      // check the result is not in grid or car is overlapped
      // check the move is possible depending on its direction
      if (6 - cars[index].y2 - 1 >= 0 && cells[6 - cars[index].y2 - 1][cars[index].x1 - 'A'] == 0
      && cars[index].dir == vertical) {
        cars[index].y1++;
        cars[index].y2++;
      }
      else
        return 1;
      break;
    case down:
      // check the result is not in grid or car is overlapped
      // check the move is possible depending on its direction
      if (6 - cars[index].y1 + 1 <= 5 && cells[6 - cars[index].y1 + 1][cars[index].x1 - 'A'] == 0
      && cars[index].dir == vertical) {
        cars[index].y1--;
        cars[index].y2--;
      }
      else
        return 1;
      break;
    default:
      return 1;
  }
  return 0;
}


// int find_adjacency(GQueue * queue_state, state * popped_st) {

//   // move the cars
//   int i;
//   for (i = 0; i < n_cars; i++) {
//     if (popped_st->cars[i].dir == horizontal) {
//       // make right
//       state * right_st = make_state(popped_st);
//       if (move((popped_st->cars[i].id) + 1, right, right_st) == 1) {
//         free (right_st);
//       }
//       else {
//         printf("i: %d\n", i);
//         update_cells(right_st);
//         g_queue_push_tail(queue_state, (gpointer)right_st);
//       }
//       // make left
//       state * left_st = make_state(popped_st);
//       if (move((popped_st->cars[i].id) + 1, left, left_st) == 1) {
//         free (left_st);
//       }
//       else {
//         printf("i: %d\n", i);
//         update_cells(left_st);
//         g_queue_push_tail(queue_state, (gpointer)left_st);
//       }
//     }
//     else if (popped_st->cars[i].dir == vertical) {
//       // make up
//       state * up_st = make_state(popped_st);
//       if (move((popped_st->cars[i].id) + 1, up, up_st) == 1) {
//         free (up_st);
//       }
//       else {
//         printf("i: %d\n", i);
//         update_cells(up_st);
//         g_queue_push_tail(queue_state, (gpointer)up_st);
//       }
//       // make down
//       state * down_st = make_state(popped_st);
//       if (move((popped_st->cars[i].id) + 1, left, down_st) == 1) {
//         free (down_st);
//       }
//       else {
//         printf("i: %d\n", i);
//         update_cells(down_st);
//         g_queue_push_tail(queue_state, (gpointer)down_st);
//       } 
//     }
//   }

// }


int main(int argc, char *argv[]) {

  GtkWidget *window;
  GtkWidget *table;
  GtkWidget *bottomRightButton1;
  GtkWidget *bottomRightButton2;

  state_head checked_state;
  GQueue * queue_state = g_queue_new();
  checked_state.next = NULL; 

  gtk_init(&argc, &argv);

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size(GTK_WINDOW(window), 250, 180);
  gtk_window_set_title(GTK_WINDOW(window), "GtkTable");

  gtk_container_set_border_width(GTK_CONTAINER(window), 5);

  table = gtk_table_new(6, 6, TRUE);
  gtk_table_set_row_spacings(GTK_TABLE(table), 2);
  gtk_table_set_col_spacings(GTK_TABLE(table), 2);

  int i = 0;
  int j = 0;
  int pos = 0;

  GdkColor color;

  GtkWidget *buttons[6][6];

  for (i=0; i < 6; i++) {
    for (j=0; j < 6; j++) {

      gdk_color_parse("#AC87C5", &color);

      buttons[i][j] = gtk_button_new_with_label("");
      // button = gtk_button_new_with_label(values[pos]);
      gtk_widget_modify_bg(buttons[i][j], GTK_STATE_NORMAL, &color);
      gtk_table_attach_defaults(GTK_TABLE(table), buttons[i][j], j, j+1, i, i+1);
      pos++;
    }
  }

  gtk_container_add(GTK_CONTAINER(window), table);

  // Create buttons outside the table and add them to the window
  bottomRightButton1 = gtk_button_new_with_label("Back");
  gdk_color_parse("#756AB6", &color); // Orange color
  gtk_widget_modify_bg(bottomRightButton1, GTK_STATE_NORMAL, &color);
  gtk_table_attach_defaults(GTK_TABLE(table), bottomRightButton1, 6, 7, 6, 7);


  bottomRightButton2 = gtk_button_new_with_label("Go");
  gdk_color_parse("#DC84F3", &color); // Purple color
  gtk_widget_modify_bg(bottomRightButton2, GTK_STATE_NORMAL, &color);
  gtk_table_attach_defaults(GTK_TABLE(table), bottomRightButton2, 7, 8, 6, 7);


  g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);

  // gtk_widget_show_all(window);

  // gtk_main();

  char buf[128] ;
  int op ;
  int id ;

    scanf("%s", buf) ;

  switch (op = get_op_code(buf)) {
    // check the command is in op_str
    if (op == N_op)
      break;

    case start:
      scanf("%s", buf) ;
      state * made_st = make_state(NULL);
      if (load_game(buf, made_st) == 1) {
          fprintf(stderr, "invalid data\n");
          free (made_st);
          break;
      }
      g_queue_push_tail(queue_state, (gpointer)made_st);
  
      state * peeked_st = (state *)g_queue_peek_head(queue_state);
      update_cells(peeked_st) ;
      display(buttons, peeked_st) ;
      gtk_widget_show_all(window);
      gtk_main();
      break;

    case quit:
      exit(EXIT_SUCCESS);
  }

  state * popped_st;
  
  // while (!(g_queue_is_empty(queue_state))) {
  //   popped_st = (state *)g_queue_pop_head(queue_state);
  //   // insert to visit

  //   printf("%p\n", popped_st->cars);

  //   // check complete
  //   if (popped_st->cells[2][5] == 1) {
  //     printf("Problem solved!\n");
  //     break;
  //   }

  //   find_adjacency(queue_state, popped_st);

  //   // Get the length
  //   guint length = g_queue_get_length(queue_state);

  //   // Print the length
  //   g_print("Number of items in the queue: %u\n", length);

  //   break;

  // }

  return 0;
}
