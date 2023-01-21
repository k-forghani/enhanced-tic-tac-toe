#include <iostream>
#include <fstream>
#include <stdio.h>
#include "constants.h"
#include "components.h"

using namespace std;



/* Global Variables */

Player players[2];
string board[3][3];



/* Functions Declarations */

void welcome_scene();
void stage_scene();
void history_scene();

void handle_choices(int);



/* General Functions */

// This function clears the entire screen.
inline void clear () {
    // Use "CLS" instead of "clear" in Windows.
    system("clear"); 
}

// This function gets an integer value from the user.
// Inputs:
//    message : the message that will be shown before getting the value (default: >>)
// Outputs:
//    int : the value passed by user
inline int input_int (string message = ">>") {
    cout << message << " ";
    int n;
    cin >> n;
    return n;
}

// This function capitalize all the letters inside a string.
// Inputs:
//    content : the initial string
// Outputs:
//    string : the capitalized string
inline string toupper (string content) {
    for (int i = 0; i < content.length(); i++) {
        content[i] = toupper(content[i]);
    }
    return content;
}

// This function converts the ordinal numbers to words
// Inputs:
//    times : the ordinal number
// Outputs:
//    string : the ordinal number in words
string timize (int times) {
    if (times == 1) {
        return "once";
    } else if (times == 2) {
        return "twice";
    } else {
        return to_string(times) + " times";
    }
}



/* Operating Functions */

// This function decides that who must start the game.
// Inputs:
//    n1 : the name of player one
//    n2 : the name of player two
// Outputs:
//    string : the name of player who must start the game
int discover_starter (string n1, string n2) {
    ifstream fin(HISTORY_PATH);

    if (!fin.is_open()) {
        return -1;
    }

    int winner = -1;

    while (!fin.eof()) {
        string p1, p2;
        int w1, w2;
        fin >> p1 >> p2;
        fin >> w1 >> w2;
        
        if (p1 == n1 && p2 == n2) {
            if (w1 > w2) {
                winner = 0;
            } else if (w2 > w1) {
                winner = 1;
            }
        }

        if (p1 == n2 && p2 == n1) {
            if (w1 > w2) {
                winner = 1;
            } else if (w2 > w1) {
                winner = 0;
            }
        }
    }

    fin.close();

    return winner;
}

// This function updates the history file of wins.
// Inputs:
//    n1 : the name of player one
//    n2 : the name of player two
//    winner : the index of winner
void update_history (string n1, string n2, int winner) {
    int nw1 = 0, nw2 = 0;

    if (winner == 0) {
        nw1 = 1;
    } else if (winner == 1) {
        nw2 = 1;
    }

    ofstream fout(HISTORY_BUFFER_PATH);
    ifstream fin(HISTORY_PATH);

    if (!fin.is_open()) {
        fout << n1 << " " << n2 << " " << nw1 << " " << nw2;
    } else {
        bool does_exist = false;

        while (!fin.eof()) {
            string p1 = "", p2 = "";
            int w1, w2;
            fin >> p1 >> p2;
            fin >> w1 >> w2;
            if (n1 == p1 && n2 == p2) {
                w1 += nw1;
                w2 += nw2;
                does_exist = true;
            } else if (n1 == p2 && n2 == p1) {
                w1 += nw2;
                w2 += nw1;
                does_exist = true;
            }
            if (p1 != "" && p2 != "") {
                fout << p1 << " " << p2 << " " << w1 << " " << w2 << endl;
            }
        }

        if (!does_exist) {
            fout << n1 << " " << n2 << " " << nw1 << " " << nw2 << endl;
        }
    }

    fin.close();
    fout.close();

    remove(HISTORY_PATH.c_str());
    rename(HISTORY_BUFFER_PATH.c_str(), HISTORY_PATH.c_str());
}

// This function initializes all cells of the board with empty string.
void initialize_board () {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            board[i][j] = "";
        }
    }
}

// This function initializes the default details of players.
void initialize_players () {
    players[0].symbol = 'B';
    players[1].symbol = 'R';

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 3; j++) {
            players[i].pieces[j] = 2;
        }
    }
}

// This function clears the entire screen and displays the current situation of the board.
void display_board () {
    clear();
    cout << "   ";
    for (int i = 0; i < 3; i++) {
        cout << "  0" << i + 1 << " ";
    }
    cout << endl;
    cout << "   " << string(3 * 4 + 4, '-') << endl;
    for (int i = 0; i < 3; i++) {
        cout << "0" << i + 1 << " | ";
        for (int j = 0; j < 3; j++) {
            string pieces = board[i][j];
            if (pieces.length() >= 2) {
                cout << pieces.substr(pieces.length() - 2, 2);
            } else {
                cout << "  ";
            }
            cout << " | ";
        }
        cout << endl;
        cout << "   ";
        if (i != 2) {
            for (int i = 0; i < 3 * 4 + 4; i++) {
                if (i % 5 == 0) {
                    cout << "|";
                } else {
                    cout << "-";
                }
            }
            cout << endl;
        }
    }
    cout << string(3 * 4 + 4, '-') << endl;
}

// This function displays the menu and gets the choice of the user.
void handle_menu () {
    cout << "Available Commands:" << endl;
    cout << "(1) Start a New Game" << endl;
    cout << "(2) See the History" << endl;
    cout << "(3) Exit the Game" << endl;
    int choice = input_int("Please choose a command:");
    switch (choice) {
        case 1:
            stage_scene();
            break;
        case 2:
            history_scene();
            break;
        case 3:
            exit(0);
            break;
        default:
            cout << "Error: Invalid command!" << endl;
            cin.clear();
            cin.ignore(1000, '\n');
            handle_menu();
            break;
    }
}

// This function checks whether the game must be finished or not.
// Outputs:
//    string : the symbol of the winner if someone won, or ' ' if nothing happened
char check_board () {
    string top_board[3][3];

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j].length() < 2) {
                top_board[i][j] = "  ";
            } else {
                top_board[i][j] = board[i][j].substr(board[i][j].length() - 2, 2);
            }
        }
    }

    for (int i = 0; i < 3; i++) {
        if (top_board[i][0][0] == top_board[i][1][0] && top_board[i][1][0] == top_board[i][2][0]) {
            if (top_board[i][0][0] != ' ') {
                return top_board[i][0][0];
            }
        }
    }
    for (int j = 0; j < 3; j++) {
        if (top_board[0][j][0] == top_board[1][j][0] && top_board[1][j][0] == top_board[2][j][0]) {
            if (top_board[0][j][0] != ' ') {
                return top_board[0][j][0];
            }
        }
    }
    if (top_board[0][0][0] == top_board[1][1][0] && top_board[1][1][0] == top_board[2][2][0]) {
        if (top_board[0][0][0] != ' ') {
            return top_board[0][0][0];
        }
    }
    if (top_board[0][2][0] == top_board[1][1][0] && top_board[1][1][0] == top_board[2][0][0]) {
        if (top_board[0][2][0] != ' ') {
            return top_board[0][2][0];
        }
    }

    return ' ';
}

// This function clears the entire screen and gets the details of the players.
void handle_players () {
    clear();
    cin.ignore();
    
    while (true) {
        cout << "Please enter details of players: " << endl << endl;
        cout << "[1] First Player" << endl;
        cout << "    Symbol: " << players[0].symbol << endl;
        cout << "    Name: ";
        cin >> players[0].name;
        cout << endl;
        cout << "[2] Second Player" << endl;
        cout << "    Symbol: " << players[1].symbol << endl;
        cout << "    Name: ";
        cin >> players[1].name;
        cout << endl;

        if (players[0].name == players[1].name) {
            cout << endl << "Error: Names can't be the same!" << endl;
            continue;
        }
        
        break;
    }
}

// This function ends the game by displaying the winner and then the menu to handle further actions.
// Inputs:
//    winner : the player who won the game
void end_game (char winner) {
    display_board();

    cout << endl;

    Player player;
    for (int i = 0; i < 2; i++) {
        if (players[i].symbol == winner) {
            player = players[i];
            update_history(players[0].name, players[1].name, i);
            break;
        }
    }
    cout << "Congratulations to Player '" << player.symbol << "'!" << endl;
    cout << player.name << " won the game." << endl;
    
    cout << endl;
    
    handle_menu();
}

// This function handles placing a piece on the board by a specific player.
// Inputs:
//    player : the id of the player who wants to place a piece (default: 0)
void handle_placing (int player = 0) {
    int row = -1;
    int column = -1;
    string piece = "";
    int size_of_choice = 0;

    while (true) {
        cin.clear();
        cin.ignore(1000, '\n');
        
        row = input_int("Please enter the row:") - 1;
        column = input_int("Please enter the column:") - 1;

        if (row < 0 || row > 2 || column < 0 || column > 2) {
            cout << "Error: Row or column is out of range! Please try another." << endl;
            continue;
        }

        string pieces = board[row][column];
        int size_of_prepiece = 0;

        if (pieces.length() >= 2) {
            size_of_prepiece = pieces[pieces.length() - 1] - '0';
        }

        if (size_of_prepiece > 2) {
            cout << "Error: You can't place any piece on this cell." << endl;
        } else {
            break;
        }

        cin.ignore();
    }

    while (true) {
        cin.ignore();
        cout << "Please enter the piece: ";
        cin >> piece;
        piece = toupper(piece);

        if (piece.length() != 2) {
            cout << "Error: Invalid choice of piece!" << endl;
            continue;
        }

        size_of_choice = piece[1] - '0';

        if (piece[0] != players[player].symbol || size_of_choice < 1 || size_of_choice > 3) {
            cout << "Error: Invalid choice of piece!" << endl;
            continue;
        }

        if (players[player].pieces[size_of_choice - 1] < 1) {
            cout << "Error: You don't have this piece" << endl;
            continue;
        }
        
        string pieces = board[row][column];
        int size_of_prepiece = 0;

        if (pieces.length() >= 2) {
            size_of_prepiece = pieces[pieces.length() - 1] - '0';
        }

        if (size_of_choice <= size_of_prepiece) {
            cout << "Error: You can place only a larger piece on a smaller piece!" << endl;
            continue;
        }

        break;
    }

    board[row][column] += piece;

    players[player].pieces[size_of_choice - 1]--;

    display_board();
    if (player == 0) {
        handle_choices(1);
    } else {
        handle_choices(0);
    }
}

// This function handles moving a piece across the board by a specific player.
// Inputs:
//    player : the id of the player who wants to move a piece (default: 0)
void handle_moving (int player = 0) {
    int source_row = -1;
    int source_column = -1;
    int target_row = -1;
    int target_column = -1;
    int size_of_source = 0;

    while (true) {
        cin.clear();
        cin.ignore(1000, '\n');

        source_row = input_int("Source Row:") - 1;
        source_column = input_int("Source Column:") - 1;

        if (source_row < 0 || source_row > 2 || source_column <  0 || source_column > 2) {
            cout << "Error: Source row or column is out of range!" << endl;
            continue;
        }

        string pieces = board[source_row][source_column];

        if (pieces.length() < 2) {
            cout << "Error: There is no piece to move!" << endl;
            continue;
        }

        size_of_source = pieces[pieces.length() - 1] - '0';

        if (pieces[pieces.length() - 2] != players[player].symbol) {
            cout << "Error: You can't move the piece that is not yours!" << endl;
            continue;
        }

        bool applicable = false;

        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                int size = 0;
                if (board[i][j].length() >= 2) {
                    size = board[i][j][board[i][j].length() - 1] - '0';
                }
                if (size < size_of_source) {
                    applicable = true;
                    break;
                }
            }
            if (applicable) break;
        }

        if (!applicable) {
            cout << "Error: You can't move this piece to any cell!" << endl;
            continue;
        }

        break;
    }

    while (true) {
        cin.clear();
        cin.ignore(1000, '\n');

        target_row = input_int("Target Row:") - 1;
        target_column = input_int("Target Column:") - 1;

        if (target_row < 0 || target_row > 2 || target_column < 0 || target_column > 2) {
            cout << "Error: Target row or column is out of range!" << endl;
            continue;
        }

        string pieces = board[target_row][target_column];

        int size_of_target = 0;

        if (pieces.length() >= 2) {
            size_of_target = pieces[pieces.length() - 1] - '0';
        }

        if (size_of_source <= size_of_target) {
            cout << "Error: You can move only a larger piece onto a smaller piece!" << endl;
            continue;
        }

        break;
    }

    string source_pieces = board[source_row][source_column];
    string piece = source_pieces.substr(source_pieces.length() - 2, 2);
    board[source_row][source_column] = source_pieces.substr(0, source_pieces.length() - 2);

    string target_pieces = board[target_row][target_column];
    board[target_row][target_column] = target_pieces.substr(0, target_pieces.length() - 2);
    board[target_row][target_column] += piece;

    display_board();
    if (player == 0) {
        handle_choices(1);
    } else {
        handle_choices(0);
    }
}

// This function handles choices of each player on each turn.
// It will lead calling "handle_placing" or "handle_moving".
// Inputs:
//    player : the id of the player who must start the game (default: -1)
//             -1 means that the starter will be chosen based on the wins history.
void handle_choices (int player = -1) {
    if (player == -1) {
        player = discover_starter(players[0].name, players[1].name);
        if (player == -1) {
            player = 0;
        }
    }

    char result = check_board();
    if (result != ' ') {
        end_game(result);
        return;
    }

    cout << players[player].name << ", it is your turn." << endl;
    cout << "Your symbol is '" << players[player].symbol << "'";
    int npieces = 0;
    for (int i = 0; i < 3; i++) {
        npieces += players[player].pieces[i];
    }
    if (npieces != 0) {
        cout << ", and you have these pieces:" << endl;
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < players[player].pieces[i]; j++) {
                cout << players[player].symbol << i + 1 << " ";
            }
        }
        cout << endl;

        bool accessible = false;

        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (board[i][j] != "") {
                    if (board[i][j][board[i][j].length() - 2] == players[player].symbol) {
                        accessible = true;
                        break;
                    }
                }
            }
            if (accessible) break;
        }
        
        if (accessible) {
            cout << "What do you want to do?" << endl;
            cout << "(1) Place a New Piece" << endl;
            cout << "(2) Move an Existing Piece across the Board" << endl;
            while (true) {
                cin.clear();
                cin.ignore(1000, '\n');

                int choice = input_int("Please enter your choice:");
                if (choice == 1) {
                    handle_placing(player);
                    break;
                } else if (choice == 2) {
                    handle_moving(player);
                    break;
                } else {
                    cout << "Invalid choice!" << endl;
                }
            }
        } else {
            handle_placing(player);
        }
    } else {
        cout << ", and you don't have any pieces!" << endl;
        cout << "So you can only move the pieces." << endl;
        handle_moving(player);
    }
}




/* Scenes */

// This function displays the welcome scene (including menu).
void welcome_scene () {
    cout << "Enhanced Tic Tac Toe 1.0" << endl;
    cout << "Developed by: Kazem Forghani" << endl;
    cout << "Updated at: 2022 Jan 20 22:40" << endl;
    handle_menu();
}

// This function displays all the necessary scenes to start the game sequentially.
void stage_scene () {
    initialize_players();
    handle_players();
    initialize_board();
    display_board();
    handle_choices();
}

// This function displays the scene of the wins history.
void history_scene () {
    clear();

    cout << "History" << endl;
    
    ifstream fin(HISTORY_PATH);

    if (!fin.is_open()) {
        cout << "There is no history!" << endl;
        return;
    }

    int counter = 0;

    while (!fin.eof()) {
        string p1, p2;
        int w1, w2;
        fin >> p1 >> p2;
        fin >> w1 >> w2;
        if (p1 != "" and p2 != "") {
            counter++;
            string all_times = timize(w1 + w2);
            string w1_times = timize(w1);
            string w2_times = timize(w2);
            cout << counter << ". " << p1 << " and " << p2 << " have played " << all_times << "." << endl;
            cout << "   " << p1 << " won " << w1_times << " and " << p2 << " won " << w2_times << "." << endl;
        }
    }

    if (counter == 0) {
        cout << "There is no history!" << endl;
    }

    cout << endl;

    fin.close();

    handle_menu();
}

/* Main */

// This is the main function.
int main () {
    welcome_scene();
    
    return 0;
}