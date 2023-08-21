/**
 * @file checker.h
 * @author Javier Candales (javier_candales@yahoo.com.ar)
 * @brief Searches for possible movements and marks destination points.
 * @date 2023-08-21
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef CHECKER_H
#define CHECKER_H

#include <backgammon.h>

/**
 * @brief Searches for possible movements for the current player.
 * 
 * @param bg Backgammon instance.
 */
void check_selection(Backgammon *bg);

#endif
