/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   globals.h
 * Author: adi
 *
 * Created on January 25, 2017, 4:32 PM
 */

#ifndef CONSTANTS_H
#define CONSTANTS_H

// Defined when the program is used to solve only a given part of the grid.
//#define SOLVE_PORTIONS

// Defined when we don't allow adding new blocked cells to the grid
//#define NO_BPS

#define CACHE_DEADLOCKS

static const int MAX_NR_ROWS = 13;
static const int MAX_NR_COLS = 13;
static const char BLANK = ' ';
static const char BLACKPOINT = '@';
static const char BLACKPOINT2 = '*';
static const int HORIZONTAL = 0;
static const int VERTICAL = 1;
static const int NOTHING = -1;
static const int MAX_BPS = 26;
static const int MAX_NR_POSITIONS = 15;

static const int DEADLOCK = 0;
static const int SUCCESS = 1;
static const int DONTCARE = 2;
static const int MATCH_DEADLOCK_DB = 3;
static const int RESOURCE_LIMIT = 4;
static const int REACHED_MAX_DEPTH = 5;

static const int NO_INDEX = -1;
static const int NR_CHARS = 'Z' - 'A' + 1;
static const int MAX_WORD_LENGTH = 14;
static const char WILDCARD = ' ';

static const int MAX_THEM_SCORE = MAX_NR_ROWS*MAX_NR_COLS;

/*
 * The min length of words whose number in a solution is limited to 
 * a max threshold given as an input argument to the program.
 */
static const int MIN_LENGTH_NTW = 3;

#endif /* CONSTANTS_H */

