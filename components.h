#include <iostream>

// The structure of a player
struct Player {
    // The symbol of the player that can be 'B' or 'R'
    char symbol;

    // The name of the player specified by the user
    std::string name;

    // The number of each piece
    int pieces[3] = {2, 2, 2};
};