#include "blackjack.h"

/* 
  Blackjack
 */

void deck_reset(void) {
  dealt_i = 0;
  already_dealt = 0;
  for(int i = 0; i < DECK_SIZE; i++) dealt[i] = -1;
  return;
}

card card_deal(void) {
  /* Deal new card that is still in deck */
  do {
    already_dealt = 0;
    dealt[dealt_i] = uniform_decision(DECK_SIZE);
    for (int i = 0; i <= dealt_i; i++) {
      if (dealt[i] == dealt[dealt_i]) already_dealt++;
    }
  } while(already_dealt > 1);

  /* Return dealt card */
  card c;
  c.n = deck_n[dealt[dealt_i]];
  strcpy(c.c, deck_s[dealt[dealt_i]%13]);
  
  /* Update deck */
  if (dealt_i++ > DECK_SIZE) deck_reset();
  return c;
}

int hand_add_card(hand* h) {
  if (h->L > HAND_SIZE) {
    hand_bust(h);
    return 1;
  }
  h->cards[h->L++] = card_deal();
  int bust_or_not = hand_calculate_value(h);
  return bust_or_not;
}

int hand_calculate_value(hand* h) {
  h->value = 0; /* Reset */
  if (h->L > 0) { /* Loop through entire hand */
    /* Add all card values */
    for (int i = 0; i < h->L; i++) h->value += h->cards[i].n;
    /* If hand is over 21, check for any aces and change them to 1 */
    if (h->value > 21) {
      for (int i = 0; i < h->L; i++) {
        if (h->cards[i].n == 11) {
          /* Change value of card in hand, to reflect if ace changed value */
          /* This shouldn't break anything */
          h->cards[i].n = 1;
          h->value -= 10;
        }
        if (h->value <= 21) break;
      }
    }
    if (h->value > 21) {
      hand_bust(h);
      return h->value;
    } else h->bust = 0;
  }
  return h->value;
}

player hand_bust(hand* h) {
  h->bust = 1;
  player winner = DEALER;
  switch (h->id) {
    case DEALER:
      winner = AGENT;
      break;
    case AGENT:
    case USER:
      winner = DEALER;
      break;
    case DRAW:
      winner = DRAW;
      break;
  }

  #if HAND_BUST_ENDS_GAME
  curses_end_game(winner); /* Remove this functionality for now */
  #endif
  
  return winner;
}

void hand_reset(hand* h) {
  h->L = 0;
  return;
}

int hand_dealer_decision(hand *dealer) {
  while (dealer->value < 17) {
    hand_add_card(dealer);
    hand_calculate_value(dealer);
  }
  if (dealer->value > 21) {
    hand_bust(dealer);
    return 1;
  } else {
    return 0;
  }
}

player hand_decide_winner(hand* agent, hand* dealer) {
  player winner;
  if ((agent->bust && dealer->bust) || (agent->value == dealer->value)) {
    /* If both players bust, it is a draw */
    winner = DRAW;
  } else if (agent->value > dealer->value) {
    /* Agent has greater value, if he hasn't busted, he wins */    
    if (!agent->bust)
      winner = AGENT;
    else
      winner = DEALER;
  } else if (agent->value < dealer->value) {
    /* Dealer has greater value, if he hasn't busted, he wins */    
    if (!dealer->bust)
      winner = DEALER;
    else
      winner = AGENT;
  }
  return winner;
}

/* Uniform decision */
int uniform_decision(int len) {
  double pi = 1/(double)len;
  double d = 0, p = 0;
  d = (double)rand()/RAND_MAX;
  for (int i = 0; i < len; i++)
    if ((d > p) && (d < (p+=pi))) return i;

  /* Should not reach here */
  return 0;
}

/* 
  Curses
 */

void curses_reset_screen(WINDOW* wnd) {
  /* Screen:
  0         10        20        30        40        50
  012345678901234567890123456789012345678901234567890123456789
  Player:  Cards:              Value:    Outcome:
  Agent
  Dealer
  
  */
  move(0,0);
  winsnstr(wnd, "Player:  Cards:              Value:    Outcome:\n", 128);
  move(1,0);
  winsnstr(wnd, "Agent                                          \n", 128);
  move(2,0);
  winsnstr(wnd, "Dealer                                         \n", 128);


  refresh();
  return;
}

void curses_update_hands(WINDOW* wnd, hand* agent, hand* dealer) {
  curses_reset_screen(wnd);
  /* Agent Cards */
  for (int i = 0; i < agent->L; i++) {
    move(1,9+i*3);
    draw(agent->cards[i].c[0]);
    move(1,9+i*3+1);
    draw(agent->cards[i].c[1]);
    move(1,9+i*3+2);
    draw(',');
  }
  delch(); /* delete final comma */
  /* Agent Value */
  move(1, 30);
  draw(48+agent->value/10);
  move(1, 31);
  draw(48+agent->value-10*(agent->value/10));

  /* Dealer Cards */
  for (int i = 0; i < dealer->L; i++) {
    move(2,9+i*3);
    draw(dealer->cards[i].c[0]);
    move(2,9+i*3+1);
    draw(dealer->cards[i].c[1]);
    move(2,9+i*3+2);
    draw(',');
  }
  delch(); /* delete final comma */
  /* Agent Value */
  move(2, 30);
  draw(48+dealer->value/10);
  move(2, 31);
  draw(48+dealer->value-10*(dealer->value/10));

  refresh();
  return;
}

void curses_end_game(player who_won) {
  int curses_row = 0;
  switch (who_won) {
    case DEALER:
      dealer_wins++;
      curses_row = 2;
      break;
    case AGENT:
    case USER:
    case DRAW:
      agent_wins++;
      curses_row = 1;
      break;
  }
  move(curses_row, 40);
  draw('W');
  move(curses_row, 41);
  draw('i');
  move(curses_row, 42);
  draw('n');
  return;
}

void draw(char c) {
  delch();
  insch(c);
  return;
}
