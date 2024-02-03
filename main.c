#include <stdlib.h>
#include <string.h>
#include <curses.h>
#include "blackjack.h"

/* Global (extern declaration in blackjack.h) */
int len = 52;
int deck_n[52] = {2,3,4,5,6,7,8,9,10,10,10,10,11,
                  2,3,4,5,6,7,8,9,10,10,10,10,11,
                  2,3,4,5,6,7,8,9,10,10,10,10,11,
                  2,3,4,5,6,7,8,9,10,10,10,10,11};

char* deck_s[13] = {" 2"," 3"," 4"," 5"," 6"," 7"," 8"," 9","10"," J"," Q"," K"," A"};
int dealt_i = 0;
int already_dealt = 0;
int dealt[52] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
                 -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
                 -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
                 -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
int agent_wins = 0;
int dealer_wins = 0;
int mc_debug = 0;

int main (void) {
  /*** BEGIN CURSES ***/
  WINDOW* wnd = initscr(); /* curses call to initialize window */
  cbreak(); /* curses call to set no waiting for Enter key */
  noecho(); /* curses call to set no echoing */
  clear(); /* curses call to clear screen, send cursor to position (0,0) */
  refresh(); /* curses call to implement all changes since last refresh */
  curs_set(0); /* display cursor or not */
  char c;
  
  hand agent;
  agent.id = AGENT;
  hand dealer;
  dealer.id = DEALER;
  do {
    /* Reset values */
    curses_reset_screen(wnd);
    hand_reset(&agent);
    hand_reset(&dealer);
    player winner = DEALER;
    deck_reset(); /* Reset deck */
    /* Deal two hands */
    hand_add_card(&agent);
    hand_add_card(&agent);
    hand_add_card(&dealer);
    curses_update_hands(wnd, &agent, &dealer);
    /* Get input and make decision to hit or stay */
    do {
      c = getch();
      /* Type 'h' to hit */
      if (c == 'h') {
        if (hand_add_card(&agent) > 21) break;
        curses_update_hands(wnd, &agent, &dealer);
      }
    } while (c == 'h'); /* Type anything else to stay */
    /* Deal second card to dealer */
    hand_add_card(&dealer);
    hand_dealer_decision(&dealer);
    /* Check who won */
    curses_update_hands(wnd, &agent, &dealer);
    winner = hand_decide_winner(&agent, &dealer);
    /* Display results */
    curses_end_game(winner);
    getch();
  } while (c != 'q');
  endwin();
  return 0;
}