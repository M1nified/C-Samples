#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <sys/time.h>
#include <time.h>

enum Direction
{
  DirectionLeft,
  DirectionRight,
  DirectionUp,
  DirectionDown,
  DirectionNone
};

void fire_sheep();
void spawn_alien();
void spawn_alien_auto();

int get_key();
void move_sheep_front();
void move_sheep_back();
void move_sheep_left();
void move_sheep_right();

void clear_screen();
void print_ship_auto();
void print_ship(int x, int y);
void print_at(int x, int y, char *string);
void print_missiles();
void print_aliens();
void print_score();

const char *CLEAR_SCREEN_ANSI = "\e[1;1H\e[2J";

int points = 0;

int width, height;

int sheep_x = 0, sheep_y = 0;

struct Missile
{
  int x;
  int y;
  int pace;
  struct timeval last_update_time;
};
#define MAX_MISSILES 1000
struct Missile missiles[MAX_MISSILES];
int initial_missile_pace = 50;

struct Alien
{
  bool enabled;
  int x;
  int y;
  int pace;
  enum Direction direction;
  struct timeval last_update_time;
  char *body;
};
#define MAX_ALIENS 1000
int min_alien_spawn_x;
int min_alien_spawn_y;
int max_alien_spawn_x;
int max_alien_spawn_y;
struct Alien aliens[MAX_ALIENS];
int initial_alien_pace = 100;

int min_alien_spawn_interval = 1000;
int max_alien_spawn_interval = 3000;
struct timeval last_alien_spawn_time;
struct timeval next_alien_spawn_time;

int main()
{
  struct timeval now;
  gettimeofday(&now, NULL);
  gettimeofday(&next_alien_spawn_time, NULL);

  struct Missile *missile;
  for (int i = 0; i < MAX_MISSILES; i++)
  {
    missile = &(missiles[i]);
    missile->x = -1;
    missile->y = -1;
    missile->pace = -1;
    missile->last_update_time = now;
  }

  struct Alien *alien;
  for (int i = 0; i < MAX_ALIENS; i++)
  {
    alien = &(aliens[i]);
    alien->x = -1;
    alien->y = -1;
    alien->pace = -1;
    alien->enabled = false;
    alien->last_update_time = now;
  }

  initscr();
  timeout(5);
  keypad(stdscr, TRUE);

  getmaxyx(stdscr, height, width);

  min_alien_spawn_x = 10;
  min_alien_spawn_y = 0;
  max_alien_spawn_x = width - 10;
  max_alien_spawn_y = height / 4;

  sheep_y = height - 10;
  sheep_x = width / 2;

  int key;
  while ((key = get_key()) != 27) // until escape
  {
    switch (key)
    {
      case KEY_UP:
      move_sheep_front();
      break;
      case KEY_DOWN:
      move_sheep_back();
      break;
      case KEY_LEFT:
      move_sheep_left();
      break;
      case KEY_RIGHT:
      move_sheep_right();
      break;
      case 32: //space
      fire_sheep();
      break;
    }
    
    clear();
    // mvprintw(1, 1, "Key: %d", key);
    spawn_alien_auto();
    print_missiles();
    print_aliens();
    print_ship_auto();
    print_score();
    // refresh();
    mvprintw(30, 2, "Next alien at: %d %d", next_alien_spawn_time.tv_sec, next_alien_spawn_time.tv_usec);
  }

  endwin();

  return 0;
}

// HELPERS

int time_interval_ms(struct timeval t1, struct timeval t2)
{
  int time_diff;
  time_diff = (t1.tv_sec - t2.tv_sec) * 1000;
  time_diff += (t1.tv_usec - t2.tv_usec) / 1000;
  return time_diff;
}

// ACTIONS

void fire_sheep()
{
  struct Missile *missile;
  int i;
  for (i = 0; i < MAX_MISSILES && missiles[i].pace != -1; i++)
    ;
  missile = &(missiles[i]);
  missile->pace = initial_missile_pace;
  missile->x = sheep_x;
  missile->y = sheep_y - 1;
}

void spawn_alien()
{
  struct Alien *alien;
  int i, x, y;
  for (i = 0; i < MAX_ALIENS && aliens[i].enabled != false; i++)
    ;
  alien = &(aliens[i]);
  time_t tt;
  srand(time(&tt));
  x = rand() % (max_alien_spawn_x - min_alien_spawn_x) + min_alien_spawn_x;
  y = rand() % (max_alien_spawn_y - min_alien_spawn_y) + min_alien_spawn_y;
  alien->x = (int)x;
  alien->y = (int)y;
  alien->enabled = true;
  alien->body = "#";
}

void spawn_alien_auto()
{
  int time_diff, ms_to_add;
  struct timeval now;
  gettimeofday(&now, NULL);
  time_diff = time_interval_ms(now, next_alien_spawn_time);
  if (time_diff >= 0)
  {
    spawn_alien();
    time_t tt;
    srand(time(&tt));
    ms_to_add = rand() % (max_alien_spawn_interval - min_alien_spawn_interval) + min_alien_spawn_interval;
    last_alien_spawn_time = now;
    next_alien_spawn_time = now;
    next_alien_spawn_time.tv_sec = ms_to_add / 1000;
    next_alien_spawn_time.tv_usec = (ms_to_add % 1000) * 1000;
  }
}

// MOVEMENT FUNCTIONS

int get_key()
{
  int key;
  key = getch();
  mvprintw(1, 1, "Key: %d", key);
  return key;
}

void move_sheep_front()
{
  sheep_y--;
}

void move_sheep_back()
{
  sheep_y++;
}

void move_sheep_left()
{
  sheep_x--;
}

void move_sheep_right()
{
  sheep_x++;
}

// SCREEN FUNCTIONS

void clear_screen()
{
  printf("%s", CLEAR_SCREEN_ANSI);
}

void print_at(int x, int y, char *string)
{
  // char *padd_x = (char *)malloc(WIDTH * sizeof(char));
  // sprintf(padd_x, "%%%ds%%-", x);
  // // printf("%-\n", padd_x);
  // for (int i = y; i < HEIGHT; i++)
  //   printf("\n");
  // printf("\r");
  // printf(padd_x, string);
  mvprintw(y, x, "%s", string);
}

void print_ship(int x, int y)
{
  print_at(x - 1, y, "xAx");
}

void print_ship_auto()
{
  print_ship(sheep_x, sheep_y);
}

void print_missiles()
{
  struct Missile *missile;
  struct timeval now;
  int time_diff;
  gettimeofday(&now, NULL);
  for (int i = 0; i < MAX_MISSILES; i++)
  {
    missile = &(missiles[i]);
    if (missile->pace > 0)
    {
      time_diff = time_interval_ms(now, missiles->last_update_time);
      // mvprintw(3 + i, 1, "%i %i %i", now.tv_usec, (missile->last_update_time).tv_usec, time_diff);
      if (time_diff > missile->pace)
      {
        missile->y--;
        gettimeofday(&(missile->last_update_time), NULL);
      }
      if (missile->y <= 0)
      {
        missile->pace = -1;
      }
      else
      {
        bool hit = false;
        struct Alien *alien;
        for (int j = 0; j < MAX_ALIENS; j++)
        {
          alien = &(aliens[j]);
          if (alien->enabled == true && alien->x == missile->x && alien->y == missile->y)
          {
            alien->enabled = false;
            missile->pace = -1;
            points++;
            hit = true;
            break;
          }
        }
        if (hit == false)
        {
          print_at(missile->x, missile->y, ".");
        }
      }
    }
  }
}

void print_aliens()
{
  struct timeval now;
  int time_diff;
  gettimeofday(&now, NULL);
  struct Alien *alien;
  for (int i = 0; i < MAX_ALIENS; i++)
  {
    alien = &(aliens[i]);
    if (alien->enabled == true)
    {
      print_at(alien->x, alien->y, alien->body);
    }
  }
}
void print_score()
{
  mvprintw(4, 2, "Points: %d", points);
}