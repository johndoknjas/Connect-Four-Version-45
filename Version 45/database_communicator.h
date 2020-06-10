#pragma once

#include "sqlite3.h"
#include <stdexcept>
#include <cstdlib>
#include <string>
#include <vector>

using namespace std;

class Wrapper
{
    // By wrapping the sqlite3* pointer in this Wrapper class, I can automatically
    // initialize it (with sqlite3_open) and close it (with sqlite3_close), using
    // the constructor and destructor.

    // This allows me to create a static object of Wrapper, the Database_Functions class,
    // without extra hassle. If I had created a static sqlite3* pointer in the
    // Database_Functions class instead, it would be trickier to open and close
    // the associated database, as it's a static object (with no constructor/destructor).

public:
    Wrapper();
    ~Wrapper();

    sqlite3* DB_pointer;
};

Wrapper::Wrapper()
{
    int signal;

    signal = sqlite3_open("Database_C.db", &DB_pointer);

    if (signal)
    {
        throw runtime_error("Problem opening database C.\n");
    }
}

Wrapper::~Wrapper()
{
    sqlite3_close(DB_pointer);
}

class Database_Functions
{
public:
    static Wrapper DB;

    static int callback(void* data, int argc, char** argv, char** azColName);

    static void get_info_for_position(const vector<vector<char>>& boardP,
                                      bool is_comp_turnP, int& evaluation,
                                      int& best_move_col_index);

    static string convert_board_to_string(const vector<vector<char>>& boardP);

};

Wrapper Database_Functions::DB; // calls the default constructor of Wrapper.

int Database_Functions::callback(void* data, int argc, char** argv, char** azColName)
{
    vector<string> row_being_examined;

    for (int i = 0; i < argc; i++) {

        row_being_examined.push_back(argv[i]);
    }

    ((vector<vector<string>>*)data)->push_back(row_being_examined);

    return 0; // Necessary to avoid sql saying there is an error.
              // The zero says that the function worked fine.
}

void Database_Functions::get_info_for_position(const vector<vector<char>>& boardP,
                                               bool is_comp_turnP, int& evaluationP,
                                               int& best_move_col_index)
{
    string string_rep = convert_board_to_string(boardP);

    char whose_turn = is_comp_turnP ? 'C' : 'U';

    string select_statement = "";
    select_statement += "SELECT evaluation, best_move FROM Positions ";
    select_statement += "WHERE string_rep = '";
    select_statement += string_rep;
    select_statement += "' and whose_turn = '";
    select_statement += whose_turn;
    select_statement += "';";

    vector<vector<string>> result_from_select;

    vector<vector<string>>* vec_ptr = &result_from_select;

    int signal;

    char* error_message;

    signal = sqlite3_exec(DB.DB_pointer, select_statement.c_str(), callback,
                          vec_ptr, &error_message);

    if (signal != SQLITE_OK)
    {
        cout << "Error - from sqlite: " << error_message << "\n";

        throw runtime_error("Something went wrong in get_info_for_position()\n");
    }

    if (result_from_select.size() != 1 || result_from_select[0].size() != 2)
    {
        throw runtime_error("Problem with the size of the SELECT\n");
    }

    evaluationP = stoi(result_from_select[0][0]);

    best_move_col_index = stoi(result_from_select[0][1]);
}

string Database_Functions::convert_board_to_string(const vector<vector<char>>& boardP)
{
    string result;

    for (int r = 0; r < 6; r++)
    {
        for (int c = 0; c < 7; c++)
        {
            result += boardP[r][c];
        }
    }

    return result;
}

