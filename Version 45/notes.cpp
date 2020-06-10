/* Version 45


- In the think_ function, after the while loop, see if the DB is going to be used
  instead. If so, reset the TT. This ensures "bad data" in the TT, from when the DB was preferred,
  won't be used in the future (don't know why it is beneficial to do this).

- However, if the DB is not going to be used after the while loop, don't clear the TT.
  The current TT is good, and will be used for the rest of the game. Set a new static
  variable called surpassed_DB to true, since then
  in each subsequent call to think_, surpassed_DB = true, telling the engine
  never to clear the TT in the current game.

- In the if block after the while loop in think_, for deciding whether to use the DB,
  change the < 18 to <= 18. Because it's a bit inconsistent right now. In the while loop
  header, if it's depth_limit and the max depth won't EXCEED the DB, skip the while loop
  and use the DB. But here in this if statement, use the DB only if the max depth
  is LESS than the DB.
     - In addition, can change the < 16 to <= 16, in the condition in the while loop
       for breaking. Since now the engine has to reach at least 19 ply in its think to be used,
       instead of just 18 ply. So the condition to break from the loop can be eased up.
       If the ply is <= 16 after at least a third of the thinking time, most likely won't
       be making it to 19 ply.

- Note that clearing the TT if the DB is used (in order to use a new, empty
  TT to the next call to think_) could happen during a generate_best_move think
  (or on an actual think, right before generate_best_move), only
  against the user. However, this should be fine.

Done coding this version, now just running match tests in the Versus Sim.

Match Results:

- Match against V.41 (500 trials, thinking_time = 1.0)
   - V.45 won 502 games, lost 372, drew 126. Ratio of 1.298... ratio. Roughly
     what V.44 got against V.41 in a depth_limit match, so good - this meets (and slightly exceeds) expectations.
   - 149 trials where V.45 won, 76 trials where V.41 won.
   - V.45 thought at an average rate of 0.686...s/move, while V.41 thought
     at an average rate of 0.8219...s/move.
   - So V.45 thought for about 83.53...% as long.

- Match against V.43 (500 trials, thinking_time = 1.0)

   - V.45 won 439 games, lost 408, drew 153. Ratio of 1.0639...
	- Not that high, but I suppose it's sufficient.
        - Expectation waws some ratio that shows at least some superiority. Say, at least 1.07? So this is okay.
   - V.45 won 81 trials, lost 64 trials.
   - V.45 spent an average of 0.682s/move, while V.43 spent around 0.73s/move.
   - On zero thinks, V.45 slightly less than V.43 (both were around 0.02 to 0.03).

   - Note that this match is against V.43, and not V.44, since V.44 only tried to improve on
     the depth_limit mode. On the small chance V.44 accidentally messed something up
     for the time_limit mode, this match here may point it out.

- Match against V.44 (500 trials, depth_limit = 9)
   - TO BE PLAYED.
   - Only purpose of this match is to ensure V.45 didn't make anything worse.
   - The engines are very slightly different, since if the ply reached is now <= 18, the DB is used.
   - But the score ratio should be very close to 1.0.


Also, based off a few rough tests I did (they might not be completely correct/accurate, but they give a good indication),
V.45 spends something like 0.0007s on average, when it enters the if statement after the while loop when thinking_time is 0
(where is does a SELECT from the DB and clears the TT).
   - So it seems that doing this stuff on the think where thinking_time is 0 is clearly not a performance issue.




    ******IDEAS FOR FUTURE VERSIONS******:

    - In the think_ function, for the early return block where thinking_time < 0.000001,
      consider resetting the TT (IF !surpassedDB - if surpassedDB, then DON'T reset the TT).
      Since without doing this, a few "bad" elements in the TT remain, since these were put
      there in the era when the DB was still used over the engine. Of course, the number of
      these elements added to the TT is miniscule, since the search right before this if statement
      was depth_limit = 1.
         - In addition, let this if statement run for both depth_limit and time_limit modes.
           So just eliminate the "max_depth_limit != UNDEFINED" sub-condition.
         - The reason to let it run for the time_limit mode as well is that without doing so,
           control goes to the while loop, doesn't run an iteration, then goes past the end
           of the loop, resets the TT (if the DB has never been surpassed), and then does
           some SELECTS. These SELECTS are unnecessary on a thinking_time = 0 think, and
           take up a finite amount of time (although it should be very miniscule).
        - Besides, resetting the TT and returning right in that if statement makes control easier.
          Now depth_limit and time_limit modes terminate at the same spot, with the same behaviour,
          if thinking_time = 0. Don't have to trace the control from time_limit throughout the function.
        - Note that in the if statement, for the embedded if statement testing if
          !surpassedDB, before resetting the TT, this essentially also checks that the DB
          wasn't surpassed on the depth_limit = 1 think just done before the if statement.
          Since if the engine was able to surpass the DB with a depth_limit = 1 think, it would
          have surpassed it before on a depth_limit = 8, 9, 10, etc think. Therefore,
          !surpassedDB --> the engine didn't just surapss the DB with the depth_limit = 1 think.

        - When testing the new version with these changes against V.44, look for the following:
            - In the depth_limit match, V.45 should be equal, or possibly better (doubt it though).
              Since the change here is that the TT is reset after a thinking_time = 0 think,
              so that should only help or keep things equal (shouldn't make things worse...).
            - In the time_limit match, only aiming for equality. The behaviour is essentially
              the same, just without the additional stuff like SELECTS (that aren't used when
              the think is for thinking_time = 0). Also, V.45 may have a very slightly smaller
              average time for thinks where the thinking_time = 0.


    - In future versions, the engine may be stronger in its evaluation function than
      V.41 was (i.e., the engine used to generate Database C). If this is the case,
      then if a think_ reaches 18 ply, it would be slightly preferable to use the
      engine's think rather than the DB.
         - So in the think_ function, you would change the <= 18 at the end of the loop to < 18,
           the <= 16 in the loop (for testing whether to break) to < 16, and in the condition
           for the while loop (for the sub-condition on depth_limit mode), change > 18 to >= 18.
         - This makes it so that the goal for "surpassing" the DB (i.e., prefering the engine)
           is at least 8 ply, not at least 19 ply.
         - Note that if a future version becomes faster/more efficient at thinking,
           but still evaluates positions the same, then should be completely equally as good
           to still use the DB at 18 ply. No reason to prefer the engine at 18 ply here.

    - You could try always resetting the TT at the end of the think_ function ALWAYS
      (regardless of whether the DB was surpassed or not), just in case this somehow
      improves the engine. Test it in a depth_limit match against whatever the current
      version is at the time of reading this.

    - Try to find out why modifying the TT, on runs in the think_ function where the
      DB ends up being used, appears to be detrimental. It seems like this modified TT
      is actually worse when used on a subsequent run in think_, where the result is
      important as the DB isn't used.
         - If you can solve this mystery, fix the TT and make a new version (V.45), and
           ensure it is at least around equal with this version (V.44) in a depth_limit match.
                (since V.44 just did a hacky fix to the underlying problem, for depth_limit mode)
         - Also ensure it is as superior to previous versions in a depth_limit match as V.44 is.
           Then, test V.45 in a time_limit match against its predecessors. Should be clearly
           superior.


    -  Instead of picking a position from the playable positions textfile, just select a position from the DB,
       within a certain acceptable evaluation range (up to you). Make sure the LIMIT is 1 for the select,
       and on each game you can randomly pick the number of pieces (maybe 0 to 6). Then the WHERE is
       the number of pieces, and an evaluation within the desired range.

    -  In addition to passing a board to child nodes, pass a string to the constructors as well. Pass it like
       the board is passed - i.e., as a shared_ptr for efficiency. Then remove char in string representing
       last_move at the end of the constructor.
       The point of also passing this string is to instead add the string to a "position_info_for_TT" object
       when inserting into the TT, instead of adding the vector<vector<char>> board. The idea
       is to make it faster in the add_position_to_transposition_table() function.
       Then when using the TT, just check if the strings match (and whether is_comp_turn matches too of course).

    -  In analyze_last_move, in the first for loop through the bucket in the TT, keep track
       of the index of the duplicate (if it exists), and a bool saying whether a duplicate
       was even found. Then in the second for loop near the end of analyze_last_move, you
       know that index in the bucket (if the bool = true) is the duplicate. Just check
       if its !possible_moves_sorted.empty(), and then set possible_moves of the calling object
       equal to it.
            - For this, just see the difference in analyze_last_move between the Version 43
              and Version 41 files, in the folder "Testing there are never more than 2 copies..."
              folder. Note that the folder name no longer has anything to do with testing
              for >= 2 copies.
            - Could just copy the difference there verbatim, for analyze_last_move.
            - In a few trials I ran there, there was about a 1.5% speed gain.



    -       A   B   C   D   E   F   G

        6 |   |   |   |   |   |   |   |
          |---|---|---|---|---|---|---|
        5 | C | C | C |   |   |   |   |
          |---|---|---|---|---|---|---|
        4 |   |   |   |   |   |   |   |
          |---|---|---|---|---|---|---|
        3 |   |   |   |   |   |   |   |
          |---|---|---|---|---|---|---|
        2 |   |   |   | U |   |   |   |
          |---|---|---|---|---|---|---|
        1 |   |   |   | U |   |   |   |
          |---|---|---|---|---|---|---|

            - Assume the D5 square was considered winning for the comp with the D algorithm.
            - However, if it's the user's move here, they could go to D3, forcing D4, and then they could go to D5.
            - So in such a situation where the user has a vertical two in a row three below the D square and it's their move,
              don't evaluate as winning for the comp (or winning for the user is the situation is reversed).
                - Unless the D4 square is also winning for the comp, then obviously evaluate as winning.
                - Think of any other exceptions where this situation would still be winning.

    - For this kind of situation:

            A   B   C   D   E   F   G

        6 |   |   |   |   |   |   |   |
          |---|---|---|---|---|---|---|
        5 |   | C | C |   |   | C | C |
          |---|---|---|---|---|---|---|
        4 |   |   |   |   |   |   |   |
          |---|---|---|---|---|---|---|
        3 |   |   |   |   |   |   |   |
          |---|---|---|---|---|---|---|
        2 |   |   |   |   |   |   |   |
          |---|---|---|---|---|---|---|
        1 |   |   |   |   |   |   |   |
          |---|---|---|---|---|---|---|

          Only one square amplifying a 2-in-a-row should be counted here, since either pair of C's could
          be taken away with no detriment.


    - Consider doing the D algorithm anytime you've reached a quiescent state in the search tree? Doesn't have to be only at
      depth >= depth_limit.
        - Point of this is to save time by not going down branches which you can already evaluate as winning/losing immediately.
        - Shouldn't improve the engine in a depth_limit match, but could in a time_limit one?

    - Try to export the D algorithm to other columns as well? Might be tricky.

    - When user is thinking, clear out obsolete positions from the TT? I.e., positions with less pieces than the current position.
      Maybe don't do this every time the user thinks, but around every 5-10 turns?
	  - If the comp is in the middle of cleaning the TT and the user moves, it's fine. At least some of the TT's memory has been released.
	  - Doing this periodic cleaning could make it safer for the TT to muse longer than 10 seconds, since less chance of a memory overflow.

    - If the user decides the thinking_time to be, say, 1.0 seconds, get the computer to think no longer than 1.5-2.0 seconds.
  	  - Do this by getting it to return from minimax if a static flag in position is raised saying the time limit has passed 1.5.
	  - Use another thread in the think_ function to keep track of when the time passes 1.5, and then sets a static bool flag to true in position.
          - So if the comp is currently in depth 11 calculations, it stops and only uses its depth 10 calculations.
	  - In each loop iteration in think_, do something like unique_ptr<position> temp = think...**call constructor**
	  - Then, on the next line check if the thread has raised the flag saying the time limit has been exceeded.
		- If it hasn't, do pt = move(temp);
		- If it has, don't do pt = move(temp) since that search iteration was stopped. pt is then returned with only its depth 10 calculations.

    - Only value an odd/even square amplifying a 2-in-a-row if one of its next square(s) (either/both?) are also the correct odd/even.
      I'm not sure whether this would be beneficial or bad.

    - If a square amplifying a 2-in-a-row can be played immediately, as well as its next square(s), then it shouldn't be worth anything?
      (unless there are squares amplifying a 3-in-a-row for the player or opponent directly above).
	   - The logic behind not valuing these squares is that when the player plays in either the current_square or next_square, the opponent immediately
             fills the other, preventing the player from ever winning with a 4-in-a-row from that group.

    - Play around with the coefficient used to multiply a square's value if it's odd/even. So far 1.75 works very nicely,
      but it's possible an even value is better.

    - If a row barrier is in a column (C or E) then don't look at moving in columns in
      the smaller adjacent group unless...
          - Opponent is threatening a 4-in-a-row (or comp is).
          - Moving in some empty squares in the columns could give comp or opponent a 4-in-a-row involving some square underneath the row
            barrier in column C/E. Note that this square underneath the row barrier could have a piece in it. If it's empty,
            assume a piece is in it.

      If a node is stored in the TT, its possible_moves vector should be stored with it. Then a node that accesses the TT will know
      not to bother with some columns in its OWN search.

      If the opponent actually plays in one of these columns in the game, then the resulting position won't be in the TT and the comp will have
      all columns in its possible moves vector by starting "from scratch" in constructor 2.

      Have array of bools for where each column has been ignored? Array passed on to child nodes, stored in TT.


        A   B   C   D   E   F   G

    6 |   |   |   |   |   |   |   |
      |---|---|---|---|---|---|---|
    5 |   |   | X |   |   |   |   |
      |---|---|---|---|---|---|---|
    4 |   |   |   | X |   |   |   |
      |---|---|---|---|---|---|---|
    3 |   |   |   |   |   |   |   |
      |---|---|---|---|---|---|---|
    2 |   | X | X | X |   |   |   |
      |---|---|---|---|---|---|---|
    1 |   |   |   |   | O |   |   |
      |---|---|---|---|---|---|---|

      Say a row barrier was at E4:
	- For E3, E2, E1, assume piece X is already there IF square if empty. If enemy piece there, ignore it.
        - For E3, F2 could help but column G is useless.
        - For E2, both column F and G are useless.
        - For E1, cannot do anything. Note that if E1 were empty, F and G may be useful for it.
        - So ignore column G, since it's useless on ALL the squares below row barrier E4. Column F is useful on at least one square (E3) so can't ignore it.

        - Basically, column F is NOT useful for a square if any of the following:
		- square holds enemy piece.
                - The square on the other side (so for E2 it would be D2, horizontally speaking) is an enemy piece.
                - There are three of his own pieces in a row on the other side (D2, C2, B2, for E2).

        - Column G is NOT useful for a square if any of the following:
                - square holds enemy piece.
                - square in between on F holds enemy piece.
                - The square on the other side of the E square is an enemy piece.
                - There are two of his own pieces in a row on the other side (D2, C2).

        - So either column F/G not useful if E square holds enemy piece or if square on other side of E holds enemy piece.
        - In addition, F specifically not useful if three pieces in a row on other side.
        - In addition, G specifically not useful is two pieces in a row on other side, or if the F square holds enemy piece.
        - Run this algorithm for all E squares under row barrier to see whether F/G is not useful for all of them. If so, ignore F/G.

        - Note that all this can be done recursively. See whether columns potentially useful for E3 and call recurisvely on E2.
          Note that a column only has to be potentially useful for one square (under RB) to NOT be ignored.

      If possible_moves vector's size = like 2 or 1 but the game is not over, manually rebuild the possible_moves vector to reincorporate any abaonded columns.
      Then set a boolean flag to never try to do this algorithm again in child nodes (this flag should also be stored in TT?). The flag also tells you that there's
      no need to check if the possible_move vector's size = 2 or 1 in child nodes anymore, since all possible_moves have been reincorporated.

      Only bother doing all this if the row barrier in column C/E is on second/third/fourth lowest square? Idk

      Point of all this is to obviously lower the branching factor.



    -------------------------------- After July above




     - Improve the hash function. Right now you have it where top-right squares are given the highest base value, before being
       multiplied by a constant for 'C', 'U', or ' '. But many of those squares are all left empty until the game progresses enough.
	  - Update your algorithm to "value" squares that could be played in within a few moves. This depends on the row value of
            the highest pieces currently in each column.
          - Doing this should allow more different boards to get different hash values, lowering the avg. size of each bucket in the TT.
          - As an aside, run a test seeing what the avg. size of each bucket is in the TT.

     - If a square amplifying a 2-in-a-row has no pieces underneath it, then give it many or few points, depending on if it's the player's or
       opponent's turn.
          - If it's the opponent's turn but the player has an 'A' right above this square, then give the player lots of points.

     - For the if and else if group (0.25 and 0.75), only allow their conditions to be tested if (meaning, put them both inside the following
        big if block):
        - The square is NOT involved in a stacked threat situation (either as the top or bottom square).

      - Take out the else if for *0.75 in smart_evaluation(). Fine if both happen, since it's indicative of a square being
        one of those tit-for-tat columns.

      - Experiment with adjusting values for a 3-in-a-row with next_square under a fellow 4-in-a-row square. Also experiment
        with not completely discarding squares amplifying a 3-in-a-row directly below the opponent's 4-in-a-row square.

      - Make a third version of the "Generating File of Playable Positions" project. The current version of it right now uses
        a Version you made in October to select positions.
            - In the new version, replace the engine with this V.23 Engine. Also allow for positions with up to 8 pieces to be generated,
              and not just an even number of pieces! (although maybe you already allow for odd numbers, I don't know).
            - Then, after running overnight, copy the file of positions generated and replace the playable positions text file in this project.

      - For something like:    | | |X|X| | |

        - Count ALL FOUR of the empty squares as squares amplifying a 2-in-a-row....
            - The inner 2 squares you obviously already count, and they each have two next_squares.
            - The 2 outer squares you do not count yet. They only have ONE next_square (the respective inner square), and "other_next_square"
              is UNDEFINED.

      - For something like:  | |X| |X| |

        - Count ALL THREE of the empty squares as squares amplifying a 2-in-a-row.
            - You already count the inner square, which has two next_squares.
            - The two other squares you do not count yet. They each only have one next_square (the inner square).

      -  X
          |  |
            |  |
               X

            In the diagram above, count each of the squares as their own "amplifying 2-in-a-row" squares, since if it and its
            neighbor get filled, a 4-in-a-row is created.
                - If its neighbour (i.e., its "next_square") gets filled with an enemy piece do not count square.
                - If its neighbour gets filled with another X, obviously count the square as amplifying a 3-in-a-row, as you already do.
                - Not sure what "other_next_square" would be for each of these amplifying squares. Probably UNDEFINED.

      - squares amplifying a 2-in-a-row that can be filled in ONE MOVE should be treated differently (either positively or negatively).
        Change quiescent search requirements to also allow no squares amplifying a 2-in-a-row to be able to be filled immediately?

      - squares amplfying a 2-in-a-row that cannot ever make a 4-in-a-row (since outer 2 squares are filled with enemy piece
        or OUT-OF-BOUNDS), should NOT be counted.

      - Go through main.cpp and make it better? Esp. in play_game() -- make it think for 0 seconds where appropriate.

      - Play with adjusting the 0.5 and 0.75 parameters in smart_evaluation().
            - For the 0.75, I've confirmed it's most likely beneficial to have (as opposed to no parameter for it), but play with increasing
              it to something like 0.9.
                - With the parameter at 0.75, Version 17 did well against Version 16. Without the parameter (so effectively 1.0),
                  Version 17 still did well against Version 16 but not quite as well.
                - I'm assuming there may be a parabola here, with the absolute max between 0.75 and 1.0.
            - For the 0.5, not sure if it would be better raised or lowered.
            - If possible, try to fit data points to some function.

      - For evaluating a position, give a small amount to the player whose turn it is to move in that position. Since the Engines now think
        according to a time limit and not a depth limit, some positions will have comp to move and some will have user to move.
            - Actually, even when depth limit was used for thinking, quiescence search sometimes/often caused the search to go further.
            - So even then, giving the side to move a small amount (like +/- 5 or something) would have been useful.

      -	For a stacked threat of two 3-in-a-rows above the opponent�s own amplifying square for a 3-in-a-row,
            ensure the two groups have equal value, if you can!

            -	The group of two above should NOT be worth more.

      //////////////////////////////////////////////////////////////////

            EDIT: For below idea, it actually isn't that good. Sure, the square amplifying the 2-in-a-row, can't be filled, but one
            of its next square(s) should be, resulting in a square amplifying a 3-in-a-row RIGHT BELOW the oppponent's 3-in-a-row square.
            Such a scenario gives the player a square amplifying a 3-in-a-row, and mostly nullifies the opponent's square as well.
      - If a player has a square amplifying a 2-in-a-row, do not count it if the opponent has a square the makes a 4-in-a-row directly above it.
            - Reason: if the player actually uses this square, the opponent wins immediately.
            - Of course, if the player's square amplifies a 2-in-a-row and connects with at least a single piece, then obviously count it
              since it creates a 4-in-a-row.
            - Note that it's fine if a next_square is directly below one of the opponent's squares making a 4-in-a-row. Since here,
              next_square isn't the one adding to evaluation. It's only there to see if the player's square amplifying a 2-in-a-row
              could POTENTIALLY win by filling in next_square (or already would make a 4-in-a-row if next_square is filled).
                - In this potential scenario, filling in next_square wins immediately, so the opponent's amplifying square above is moot.
            - You'll do this probably near the end of smart_evaluation():
                - Go through each player's amplifying_2_vectors. If the square does not have an 'A' in the player's copy_board
                  (or any other char representing a square amplifying a 3-in-a-row?), then it is a normal square amplifying a 2-in-a-row.
                - If so, then check the square directly above in the OPPONENT'S copy_board. If this square is in-bounds and has
                  an 'A', then that square would give the opponent a 4-in-a-row.
                - So don't give points to the user.

      - If the player has an amplifying 2-in-a-row where one of the NEXT_SQUARES is the square right below/above one of the player's
        square amplifying a 3-in-a-row, give the player points. This is because if the 2-in-a-row is amplified into a 3-in-a-row, the player will
        have a stacked threat set up.
            - You will probably implement this near the end of smart_evaluation, as you're running through the player's
              info_for_amplifying_squares.
            - IMPORTANT: Create a new struct called "treasure_spot_and_value". Should be used instead of coordinate_and_value in smart_evaluation(),
              in order to store the next_squares of a spot.
            - For each spot amplifying the player's 2-in-a-row, check if the player has an 'A' in its copy_board below/above one of
              the 2-in-a-row's next_squares.
                - If both next_squares have an 'A' below/above them,
                  give points for both 'A' (should not be penalized for having 2 separate instances).
                - Multiply the square amplifying the 2-in-a-row by some constant (maybe 1.5 its current value?).
                    - If this square has two next_squares that are both above/below an 'A', multiply the square amplifying 2-in-a-row's value
                      by the constant twice. So (square's current value)*(1.5)*(1.5).


      /////////////////////////////////////////////// ONE BIG IDEA:

      - In initialize_row_barriers, create two copy_boards for comp and user.
        - Send these copy_boards to find_winning_squares() by reference (one appropriate copy_board sent in each function call).
        - In find_winning_squares, update the copy_board with any winning squares found, by making the square in copy_board = 'A'.
        - Do NOT remove duplicates in find_winning_squares, not necessary.
        - Back in initialize_row_barriers, run through each squares_winning_for_player vector, doing the following:
            - If the same square in the opponent's copy_board also stores 'A', make the square a row barrier (if there's not a lower barrier yet).
            - If the square above in the player's copy_board also stores 'A', make the square a row barrier (if there's not a lower barrier yet).
                - Note that this is the idea behind the bottom paragraph.

        - Four additional ideas I came up with:
            - Make copy_board_for_comp and copy_board_for_user early on in smart_evaluation(). Then pass it by reference to initialize_row_barriers()
              and find_winning_squares(), if needed. Add any 'A' to it, but then before passing back to smart_evaluation(), make sure
              all 'A' are removed! copy_board should be as it was before it left smart_evaluation().
                - This is all just to save time on having to not make two more copies of 2-D vectors.

            - In initialize_row_barriers(),
              when checking if a square is a row barrier, see if it's an amplifying square for a 3-in-a-row for both sides before seeing
              if it's the bottom square of a stacked threat for one player.

            - In initialize_row_barriers(), run through the first player's squares_winning vector. Check if it qualifies for one of the two
              requirements to be a row barrier. If it's the bottom square of a stacked threat, add the top square to the special vector.
              Then run through the second payer's squares_winning vector. Only check if these squares are the bottom squares of a stacked threat,
              since if it was a square that both players point to, I would have already caught it when running through the first player's vector.

            - At the end of initialize_row_barriers(), run through the special vector of amplifying squares that I decided to keep since they
              were the top square of a stacked threat. Now, check if they still hold that status. If either of these are true:
                - The row barrier of that column is NOT directly under the amplifying square anymore, or
                - The row barrier of that column is an amplifying square for both players

                Then do not keep this amplifying square in the special vector. It should not be counted due to being inaccessible in the game.

      - The bottom square of the two squares in a stacked threat coefficient IS A ROW BARRIER!
         - It is essentially impossible for play to continue above the bottom square, unless the player decides not to win on the spot.
         - Use this fact to update the initialize_row_barriers() function.
         - An amplifying square for both sides is obviously still a row barrier, but the bottom square in a stacked threat for one player also
           qualifies.
         - Note that this also solves the problem of not counting things like stacked threats with 3+ squares, since the squares
           above first two (specifically, above the bottom of the first two) are not counted due to row_barriers.

         - However, note that so far, this algorithm prevents the top square in the stacked threat from being counted. So,
           in the initialize_row_barrier function, record this top square in some vector. Then:
            - Give it a normal value for 3-in-a-rows in find_individual_player_evaluation (don't give stacked_threat bonus points value).
            - Make sure you run through these special squares FIRST in find_individual_player_evaluation, in order to make sure
              the copy_board holds an 'A'. Allows the bottom square in the stacked_threat to qualify for the bonus points.

            - Note now that in find_individual_player_evaluation(), you only have to check if a square amplifying a 3-in-a-row has an
              'A' above it, not below it. This also removes inconsistencies / "randomness" with which square in a stacked_threat gets
              assigned the bonus points.
                - However, I'm still checking above and below just to be safe, even though there should never be an 'A' below.

      /////////////////////////////////////////////// ONE BIG IDEA

      - If a player has a square amplifying a 2-in-a-row, where one of its next_squares is directly below one of the opponent's 'A', then
        multiply the player's square's value by some constant, like 1.2. If the player's square has both of its next_squares directly under
        two of the opponent's 'A', multiply by the constant twice (*1.2*1.2).
            - Reason for this whole idea: If the player turns the 2-in-a-row into 3-in-a-row, then the opponent's square amplifying a
              3-in-a-row will have its value severly lowered (it will be *0.25, currently!).

      - If comp is winning find shortest path to win. If for some reason this is way too hard to do, at least make the computer
        choose a move if it wins on the spot!

      - If comp is losing, play the move that takes the longest to lose. Even though the comp doesn't go for any lose-in-1 if it can
        help it, it should be putting up the tougest resistance (not just the "not easiest" resistance).

      - Used anything you learn in CMPT 225 (open-indexing hash table, quadratic hashing, etc).

      - Add a bool attribute storing whose turn it is for the "position_info_for_TT" struct. Then when adding a position to the TT
        (in the add_position_to_transposition_table() function) and when checking if a position is in the TT, make sure to include
        this bool variable.
            - The reason is to avoid treating a position the same if its the comp's turn in one and the user's turn in the other.
              This should never happen since in the 2nd constructor (which includes whenever a new game starts), the TT is reset,
              apparently including positions even with INT_MAX or INT_MIN evaluations, although double check this.

            - So while there should never be a problem, it wouldn't hurt to include the bool variable, just in case you've missed something.


      - Apparently the order of squares in each of the amplifying vectors affects the evaluation calculated
        in smart_evaluation(). See where this could be the case, and fix it if possible.
            - It should be noted though that with transposition tables, the same position (arriving via a different
              move, so different order of amplifying squares) will never have to be evaluated twice.


      - Only randomize the possible_moves vector in constructors 1 and 2 (since the position object created in them
        directly decides what move the comp plays in the actual game).
            - In constructor 3, optimize the order of moves in possible_moves. The thousands of positions created
              in constructor 3 are just for the minimax process (i.e., finding evaluations). It doesn't matter how these
              evaluations/numbers are found, so efficiency is key here. The order of possible_moves for position objects
              created in Constructor 3 don't affect the move ultimately chosen by the computer, just the speed by which
              it happens.

            - A FEW OPTIMIZATION TECHNIQUES (Technique 2 is an idea for Version 10, and may allow depth_limit to
                                             be increased):
                1) Each time in analyze_last_move(), re-arrange the possible_moves vector to store moves that can
                   make a 4-in-a-row immediately.
                   DONE

                2) In the 3rd constructor (NOT THE 2ND AND 1ST!!) re-arrange the possible_moves vector to store moves
                   that, on average, tend to end up being the chosen move in the minimax calculations. To figure this out:

			  STRATEGY #1:
			  -------------
			  - Favour moves that are closer to the middle column (D) and have more pieces in a 3x3 square around them (where the move
                            is in the centre of the square).
				- Not sure whether to include all pieces ('C' and 'U') when counting in the 3x3 square, or just similar pieces.

			  - Maybe favour moves that are lower on the board (closer to the bottom row)? Not so sure about this one.....

			  - To discover more traits of good moves, see Strategy #2.



			  STRATEGY #2 (Investigating via Simulation):
			  --------------------------------------------
                        - Run a Versus Simulation, and for one (or both) of the Engines,
                          each time a move is "chosen" in its minimax process, record its
                          column & row in some static variables. You could even record the data to a file that keeps growing
                          in size over multiple simulations, giving you more and more accurate data.
                        - Note that each possible_move should be given equal fair chances to be examined sooner in the
                          minimax process. So, make sure to randomize the possible_moves vector in the 3rd constructor!
                          This won't allow randomness to affect what move an Engine actually chooses in the game, but it
                          will allow each move in possible_moves to have a fair chance of being looked at first.
                            - Then, if, for example, a particular column is "chosen" more in the minimax process, this
                              tells you it's because that column tends to be better on average, rather than because of
                              the order of moves in possible_moves.
                        - Then at the end, print all this data out (or print some averages from the data in the file).
                          This should give you an idea of which columns & rows tend
                          to be more successful (and thus should be tried first by being at the front of the possible_moves
                          vector).
                            - Note that the row value is relative compared to other moves available. For example, if a chosen
                              move has row value of 4, record how much higher/lower it is than the other moves that
                              could have been chosen. Do not just record "4".

                NOTE: Optimization Technique 1) and 2) are constantly overwriting each other...
                    - Technique 2) in Constructor 3 re-arranges possible_moves with moves that work on average. Then it
                      calls analyze_last_move() and...
                    - Technique 1) in analyze_last_move() puts moves that concretely work in the current position at the front.
                    - THIS IS GOOD. The result is a possible_moves vector with any moves that concretely work in the position
                      at the front of the vector, followed by moves that have a good chance of working on average, followed
                      by the scrappy moves at the back. Alpha-beta and minimax pruning should greatly work here!


        **NOTE**: The suggestion below is kind of useless if you do iterative deepening, since the depth searched is just how much
        time is available in an arbitrary time constraint you give to the Engine.

      - For the position class, make a static constant variable called X that equals breadth^depth, or 7^depth_limit. This is theoretically how many
        moves the Engine looks at when it thinks, if it weren't for any minimax/alpha beta pruning.
		- Then, each time the opponent moves and the Engine thinks of the new position (i.e., Constructor 2), check the size of possible_moves vector.
                - If possible_moves vector's size is less, keep incrementing depth_limit as long as possible_moves.size()^depth_limit is close to X.
		- This essentially lets the Engine increase its search depth, if one or more column(s) are full (decreasing breadth).
                - But, the Engine can never much slower than it was before, which is where the check for possible_moves.size()^depth_limit being close to X comes in.
                - By "close to X" I mean less than 2X ish. As long as the runtime isn't double as long, it's fine, since the engine is usually pretty quick.
                - Note that this means depth_limit should still be static, but cannot be const anymore since it sometimes gets updated in Constructor 2.

		- IMPORTANT UPDATE: Alpha-beta pruning is more effective when the depth limit is higher. So, let's say there were 7 possible moves, and depth limit
                  was 9. Theoretical max value of #moves = 7^9. Then, let's say there's only 3 possible moves, so we make depth limit = 20...
                  New theoretical max value of #moves = 3^20, which is 86 times greater than 7^9.

		  HOWEVER: if we assume Alpha-Beta pruning cuts down search depth by half (on average), then we get 7^4.5 and 3^10 for average number of moves
                  looked at. 3^10 is only 9.3 times greater than 7^4.5. So still not ideal whatsoever, but not nearly as drastic as 86 times greater.

                  So, incorporate this in. Maybe make a new search depth limit acceptable if, after alpha-beta prunes around half the search depth, the new average
                  #moves is <= 2x #moves at the start of the game with alpha-beta pruning. This will require trial and error, since I don't know
                  how effective alpha-beta pruning is on the search depth for my engine.

      - Let's say one square amplifies 3 'U', allowing user to win the game. Then assume the square directly above it amplifies 3 'C', allowing comp to
        win the game. The square for the comp is MUCH less valuable than the square for the user, and to explain consider the 2 possible scenarios:
		- SCENARIO 1) Comp is forced via zugzwang to move under the user square, allowing the user to win the game.
                - SCENARIO 2) User is forced via zugzwang to move under the user square, allowing comp to fill the user square, and then user fills the
                              comp square. No one wins from the interaction (okay, maybe there is ANOTHER comp square above, but two winning squares
                              directly stacked is handled by the evaluation function).

                You'll have to do some experimentation to see how much less to make the upper square worth than the lower square. Note that...
			- You should probably evaluate the lower square the same, since its value isn't affected whatsoever by the upper square.
                        - However, the upper square should be given a lesser value, since it is IMPOSSIBLE for it to ever be used to win the game (for whichever
                          player is being considered to use it at the moment in the smart evaluation function).
				- Okay, it's not IMPOSSIBLE if the player using the lower square misses they can win in one move, allowing the other player
                                  to fill the square and then fill the upper square, winning the game (so this requires TWO BLUNDERS in a row!).

      - Keep the amplifying vectors constantly sorted. Should be sorted in some fashion so that the lower squares for each
        column are looked at first.
            - When you create a new position object (going 1 depth deeper into calculations), then new amplifying squares you add
              to the amplifying vectors should be placed in sorted order.
            - Insertion sort will be great for this! Since the amplifying vectors are going to be always kept sorted in the aforementioned
              fashion.

            - Note that in the think_ functions (which are called from main()), it is necessarily guaranteed the passed
              in amplifying vector params will be sorted in the desired fashion. So in these think_ functions,
              make sure the comp sorts the passed in amplifying vectors (stl sort works best here, since it should be assumed
              the amplifying vectors aren't at all sorted).

      - A "when push comes to shove" idea for squares making a column a "finished column".
            - For this kind of square, both and user and comp are threatening a 4-in-a-row.
            - But if this square is ever utilized to seize a win, only the comp or user will get to take advantage of it.
            - So, it makes sense to figure out which player will actually be able to use the square, should one of the players
              eventually end up in zugzwang.

            - This can be easily figured out by counting the number of empty squares (not including the winning square and directly above it)
              and factoring in whose turn it is currently.
                 - Example: If it's the comp's turn and there are an odd number of squares, then if the winning square gets used
                   it will happen when the comp has to move in the square under the winning square (due to zugzwang) and the user
                   places on the winning square. So only count the winning square as an advantage for the user!
                        - Still make a column remain a finished column, of course.

            - **IMPORTANT EXCEPTION**: If the square directly under the winning square is an amplifying square into a 4-in-a-row
              for one of the players, then DISCARD THE ABOVE ALGORITHM. Things have completely changed now.

                - If the square directly under COULD become an amplifying square for a 4-in-a-row, then things are a bit tricky:
                    - You could run the above algorithm. Then, for whoever is the "victor" of that algorithm, check if
                      the opponent could possibly make the square directly under a winning square in the future.
                        - If the opponent can, then discard the algorithm.

      - INTERESTING IDEA: I'm guessing that in smart_evaluation(), a temp_board is used to store 'A' chars in squares
              that win for one of the players. Instead, make this temp_board store ENUMS in each square, for the following options:
                - An enum if the square has a 'C'
                - An enum if the square has a 'U'
                - An enum representing what the square does for the comp and user. Possibilites for each of them:
                    - Square could amplifying a 4-in-a-row for a player, or POSSIBLY amplify a 4-in-a-row in the future,
                      or NEVER be able to amplify a 4-in-a-row in the future.
                    - Makes 9 enums, since these 3 possibilities are for each player. Example enum value:
                      "AmplifiesCompAndNothingUser".  Means square wins for comp, and can never do anything for the user.

                - With these enums, it will now be straightforward to check if a square wins for both user and comp (causing finished column
                  and meaning the whole above algorithm runs), as well as straightforward to check the status of whether
                  a square could possibly be a 4-in-a-row for one of the players (useful for the square UNDERNEATH a finished column square,
                  in the above algorithm).

                - NOTE: If making temp_board store enums is too inefficient (either time or space wise), you could continue using chars,
                  as long as you remember what char means what for the 9 possibilities for each empty square.

      - Maybe only clean up the amplifying vectors at something like depth 6 instead of at depth 0 in 2nd constructor?
        Reason is that there are about 7 times more depth 7 positions than depth 6 ones, and for every depth 6 position
        that you clean up, its 7 depth 7 child positions will be evaluated faster.

            - Note that this logic can be continually applied backwards. Maybe it's best to clean up at every depth level?
              Try to mathemtically prove something to help here.


        **NOTE**: The bottom suggestion is probably useless when using iterative deepening, since iterative deepening
        sorts moves from likely best to likely worst, based off hash table giving move orders of positions that were looked at
        at one less depth.

       - Get the possible_moves vector in position.h to store moves in this order: D,E,C,F,B,G,A
         It goes from the middle out, since in general moves in the middle tend to be better. This will allow the computer
         to find better moves quicker, allowing more alpha-beta pruning.
            - Also, STOP randomizing possible_moves' order on each instantiation in constructors 2 & 3! Not efficient!

*/
