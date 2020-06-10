# To do:

    # Delete the first line of play_game() where the value is set to false.
        # This is just while developing in beta.
    
    # For the reading from file functions (for both C++ and Python), maybe
    # add a small delay (like 0.01 s) for each iteration of the while loop.
        # The point is to minimize the computational power, should the C++
        # engine be thinking at that time.
        
        # Think of any other ways the engine might be losing some of its
        # computational power to other processes.
    
    # Create a game loop in the Python program.
    
    # Keep testing my playing. I reviewed the Python and C++ code in parallel
    # for sending/receiving input while playing a game. It seems fine,
    # but there always could be an error.
    
    # User can't decide whether to play a new game after finishing one right now.
    
    # Have a better way for the Python and C++ programs to know whether the
    # other quit? Maybe have daemon threads write every 5 seconds, and have
    # other daemons read every 10 seconds. Doesn't take up much processing
    # power, and one program always stops 10 seconds after other does...
    
        # Main thing is that this doesn't depend on user input. If the
        # user doesn't input for a while, it's fine as long as both programs
        # are still running.
        
        # These daemons would write to specific, separate files, not on the standard
        # numbering system used for communication.
    
    # At the moment, nothing more needed to do in C++, unless writing some
    # code in Python requires it.
    
    # Implement a GUI.


# Bugs:

    # Possible bug: python program sometimes crashes when thinking_time is
    # really low (like 0.0001 s). Might be inherent to the C++ engine,
    # and if this is the case then it's fine.



import subprocess
import time
from pathlib import Path
import os
import threading
import sys

starting_num = 1587992
    
filename = str(starting_num) + ".txt"

kill_daemon = False

lowercase_list = ["a", "b", "c", "d", "e", "f", "g"]

uppercase_list = ["A", "B", "C", "D", "E", "F", "G"]

def convert_each_string_to_ints(lst):
    result = []
    
    for current_string in lst:
        value = int(current_string)
        result.append(value)
    
    return result

# Function also updates starting_num and filename.
def read_from_textfile(should_just_get_int):
    global starting_num
    global filename
    
    # Note - the "should_just_get_int" parameter stores true if this function
    # should just be returning an int, and false if the function should
    # return a whole list (such as when the C++ program sends a file with
    # 42 ints representing the piece to display for each square).
    
    while (True):
        # See if the textfile even exists yet - the C++ program may have not created it.
        
        file = Path(filename)
        
        if (file.is_file()):
            file_contents = open(filename, "r")
            lines = file_contents.readlines()
            if (len(lines) > 0):
                # But before doing this, increment the filename and starting_name.
                starting_num += 1
                filename = str(starting_num) + ".txt"
                file_contents.close()
                
                # Now to either return the first element/int of lines, or
                # to return the whole list. Depends on the function parameter:
                
                if (should_just_get_int):
                    return int(lines[0])
                else:
                    return convert_each_string_to_ints(lines)
                
            file_contents.close()

# Function updates starting_num and filename when done.
def write_int_to_textfile(value):
    global starting_num
    global filename
    
    file = open(filename, "w")
    
    file.write(str(value))
    
    file.close()

    starting_num += 1
    filename = str(starting_num) + ".txt"

def crash_program_if_subprocess_quits(p):
    while (True):
        time.sleep(1.0)
        if (kill_daemon):
            return
        if (p.poll() != None):
            # subprocess has crashed, so end the entire Python program.
            # Since this function is just called in a separate thread, use:
            os._exit(0)
    
def display_board(pieces_to_display):
    
    if (len(pieces_to_display) != 42):
        print("pieces_to_display only has these many elements:", len(pieces_to_display))
    
    print()
    
    print(" A   B   C   D   E   F   G\n")
    
    current_index = 0
    
    for r in range (6):
        print(" ", end = "")
        for c in range (7):
            if (pieces_to_display[current_index] == 0):
                print (" ", end = "")
            elif (pieces_to_display[current_index] == 1):
                print("X", end = "")
            else:
                print("O", end = "")
            print(" | ", end = "")
            
            current_index += 1
        print()
        print("----------------------------")
    
def is_valid_letter(letter):
    return ((letter in lowercase_list) or (letter in uppercase_list))

# Pre-condition: letter is a valid letter.
def convert_letter_to_index(letter):
    if (letter in lowercase_list):
        return (lowercase_list.index(letter))
    elif (letter in uppercase_list):
        return (uppercase_list.index(letter))
    else:
        sys.exit() # Raising an error.
    
def main():
    global kill_daemon
    
    p = subprocess.Popen([r"Version 1 - April 25, 2018.exe"])
    # Assigning the subprocess to var p in order to check if it's still running at any time,
    # using p.poll().
    
    t1 = threading.Thread(target=crash_program_if_subprocess_quits, args=(p,))
    
    t1.start()
    
    time.sleep(1.3) # to give the C++ program time to delete all the numbered textfiles.
    
    # I need the number of games the computer has won up to this point.
    # Get it from the textfile the C++ program has written to, containing this int.
    
    num_games_comp_won = read_from_textfile(True)
    
    print("Current ongoing match score:")
    
    print("Comp has won", num_games_comp_won, " games")
    
    num_games_user_won = read_from_textfile(True)
    
    print("You have won", num_games_user_won, "games")
    
    num_games_drawn = read_from_textfile(True)
    
    print("There have been", num_games_drawn, "draws")   
    
    thinking_time = float(input("Enter approximately how long you want the Engine to think on each move: "))
    
    # thinking_time is a floating point value storing the number of seconds for the thinking time.
    # Convert it to an int form storing the number of milliseconds.
    
    milliseconds_thinking_time = int(thinking_time * 1000)
    
    write_int_to_textfile(milliseconds_thinking_time)

    play_starting_position = input("Enter s to play the starting position: ")
    
    if (play_starting_position == "s" or play_starting_position == "S"):
        write_int_to_textfile(1)
    else:
        write_int_to_textfile(0)
    
    # Now figure out if this is the current game. Get this info from the C++ program.
    
    is_first_game = read_from_textfile(True)
    
    if (is_first_game != 1):
        # 1 would mean it is the first game. So since it isn't, ask the user
        # if they want to play again:
        
        play_again = input("Enter 1 to play again: ")
        
        if (play_again == "1"):
            write_int_to_textfile(1)
        else:
            write_int_to_textfile(0)
        
        if (play_again != "1"):
            # End the python program (the C++ program will be ending itself now as well).
            
            kill_daemon = True
            
            os._exit(0) # renders the above line kind of useless, since this
                        # should destroy the daemon as well.
    
    # Ask the user who should go first:
    
    is_user_turn = True
    # Variable that will alternate between True/False throughout the game.
    
    who_should_go_first = input("Enter y to go first: ")
    
    if (who_should_go_first == "y" or who_should_go_first == "Y"):
        write_int_to_textfile(1)
    else:
        write_int_to_textfile(0)
        is_user_turn = False
    
    # Ask the user to decide what piece they want to play:
    
    should_user_play_x = input("Enter X to play X; otherwise, enter O (or any other input): ")
    
    if (should_user_play_x == "x" or should_user_play_x == "X"):
        write_int_to_textfile(1)
    else:
        write_int_to_textfile(0)
    
    pieces_to_display = read_from_textfile(False)
    
    display_board(pieces_to_display)
    
    is_game_over = False
    
    # Game loop:
    
    while (not(is_game_over)):
        if (is_user_turn):
            
            letter = input("Enter the column to move in: ")
            
            while (not(is_valid_letter(letter))):
                letter = input("Invalid input, please try again: ")
            
            column_index = convert_letter_to_index(letter)
            
            write_int_to_textfile(column_index)
            
            is_valid_move = read_from_textfile(True)
            
            while (not(is_valid_move)):
                letter = input("You cannot move there, please try again: ")
                
                while (not(is_valid_letter(letter))):
                    letter = input("Invalid input, please try again: ")
                
                column_index = convert_letter_to_index(letter)
            
                write_int_to_textfile(column_index)
            
                is_valid_move = read_from_textfile(True)
            
            # At this point, the move sent to the C++ program was valid, and
            # now the C++ program is processing it.
        
  #     else:
  #         It is the computer's turn, so just wait until the pieces
  #         to display are sent over, just like what is done after the user
  #         makes a move in the above if statement.
        
        # Get the updated list of all the X and O pieces in the board:
        pieces_to_display = read_from_textfile(False)
        display_board(pieces_to_display)
        is_user_turn = not(is_user_turn)
        is_game_over = read_from_textfile(True)
    
    print("The game is over.")
    game_result = read_from_textfile(True)
    
    if (game_result == 1):
        print("The computer won.")
    elif (game_result == 2):
        print("You won!")
    else:
        print("The game was a draw.")
    
    # CONTINUE HERE - ASK THE USER IF THEY WANT TO PLAY AGAIN.
    # Loop all of main?
    
    
    
    
    # Now we're at the end of main(), so set the flag to kill the daemon thread:
    
    kill_daemon = True    
    
if __name__ == '__main__':
    main()
    
    
    
    
    
    