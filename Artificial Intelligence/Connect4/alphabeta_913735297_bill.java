import java.util.*;

public class alphabeta_913735297_bill extends AIModule
{
    int player;
    int opponent;
    int bestMoveSeen;

    public void getNextMove(final GameStateModule game)
    {
        player = game.getActivePlayer();
        opponent = (game.getActivePlayer() == 1?2:1);
        while(!terminate)
        {
            alphabeta(game, 11, Integer.MIN_VALUE, Integer.MAX_VALUE, player);
            if(!terminate) {
                chosenMove = bestMoveSeen;
            }
                   
		}
		if(game.canMakeMove(chosenMove))
			game.makeMove(chosenMove);
    }

    private int eval(final GameStateModule state, int playerID)
    {
        int firstPlayer = -1;
        int secondPlayer = -1;
        int seq_3 = 100;
        int seq_2 = 25;
        int threat_weight = 200;

        int O = playerID;

        int[][] combo_two = new int [][]{{O,O, 0 ,0},{O,0,O,0},{O,0,0,O},{0,O,O,0},{0,O,0,O},{0,0,O,O}};
        int[][] combo_three = new int [][]{{0,O,O,O},{O,0,O,O},{O,O,0,O},{O,O,O,0}};

        int c_score = 0;
        int r_score = 0;
        int d_score = 0;
        int num_cthreats = 0;
        int num_rthreats = 0;
        int num_dthreats = 0;
        int oddThreats = 0;
        int evenThreats = 0;

        if (playerID == 1)
        {
            firstPlayer = 1;
            secondPlayer = 0;
        }
        else
        {
            secondPlayer = 1;
            firstPlayer = 0;
        }

        //col
        for(int x = 0; x < state.getWidth(); x++)
        {
            int seq_count = 0;
            boolean col_threat = false;
            for(int y = 0; y < state.getHeight(); y++)
            {
                if(state.getAt(x, y) == O)
                {
                    seq_count++;
                }
                else
                {
                    if(seq_count == 3 && (state.getAt(x,y) == 0))
                    {
                        col_threat = true;
                        if(state.getHeightAt(x) % 2 == 1)
                        {
                            evenThreats++;
                        }
                        else
                        {
                            oddThreats++;
                        }
                    }
                    break;
                }
            }
            switch(seq_count)
            {
                case 2:
                    c_score += seq_2;
                case 3:
                    c_score += seq_3;
                    if(col_threat)
                        num_cthreats++;
                default:
                    break; 
            }
        }
        //row
        for(int y = 0; y < state.getHeight(); y++)
        {
            for(int x = 0; x < state.getWidth()-3; x++)
            {
                boolean two_combo = false;
                for(int[] combo : combo_two)
                {
                    if(state.getAt(x,y) == combo[0] && state.getAt(x+1,y) == combo[1] && state.getAt(x+2,y) == combo[2] && state.getAt(x+3,y) == combo[3])
                    {
                        if(combo == combo_two[3] || combo == combo_two[0] || combo == combo_two[5])
                        {
                            r_score += 5*seq_2;
                        }
                        r_score += seq_2;
                        two_combo = true;
                        break;
                    }
                }

                // if there exists a combo of 2, no need to search for a combo of 3, thus saving some time
                if(two_combo)
                    continue;

                for(int[] combo: combo_three)
                {
                    if(state.getAt(x,y) == combo[0] && state.getAt(x+1,y) == combo[1] && state.getAt(x+2,y) == combo[2] && state.getAt(x+3,y) == combo[3])
                    {
                        if(combo == combo_three[0] && x < state.getWidth()-4)
                        {
                            if(state.getAt(x+1,y) == combo_three[3][0] && state.getAt(x+2,y) == combo_three[3][1] && state.getAt(x+3,y) == combo_three[3][2] && state.getAt(x+4,y) == combo_three[3][3])
                            {
                                r_score += 5*seq_3;
                                num_rthreats++;
                            }
                        }
                        r_score += seq_3;
                        num_rthreats++;
                        if((x+1)%2 == 0)
                        {
                            evenThreats++;
                        }
                        else
                        {
                            oddThreats++;
                        }
                        break;
                    }
                }
            }
        }
        //diagonals L-R
        for(int x = 0; x < state.getWidth() - 3; x++)
        {
            for(int y = 0; y < state.getHeight() - 3; y++)
            {
                boolean two_combo = false;
                for(int[] combo : combo_two)
                {
                    if(state.getAt(x,y) == combo[0] && state.getAt(x+1,y+1) == combo[1] && state.getAt(x+2,y+2) == combo[2] && state.getAt(x+3,y+3) == combo[3])
                    {
                        if(combo == combo_two[3] || combo == combo_two[0] || combo == combo_two[5])
                        {
                            d_score += 5*seq_2;
                        }
                        d_score += seq_2;
                        two_combo = true;
                        break;
                    }
                }

                if(two_combo)
                    continue;

                for(int[] combo: combo_three)
                {
                    if(state.getAt(x,y) == combo[0] && state.getAt(x+1,y+1) == combo[1] && state.getAt(x+2,y+2) == combo[2] && state.getAt(x+3,y+3) == combo[3])
                    {
                        if(combo == combo_three[0] && x < state.getWidth()-4 && y < state.getHeight()-4)
                        {
                            if(state.getAt(x+1,y+1) == combo_three[3][0] && state.getAt(x+2,y+2) == combo_three[3][1] && state.getAt(x+3,y+3) == combo_three[3][2] && state.getAt(x+4,y+4) == combo_three[3][3])
                            {
                                d_score += 5*seq_3;
                                num_dthreats++;
                            }
                        }
                        d_score += seq_3;
                        num_dthreats++;
                        if((x+1) % 2 == 0)
                        {
                            if(state.getAt(x,y) == 0 || state.getAt(x+2,y+2) == 0)
                            {
                                evenThreats++;
                            }
                        }
                        else
                        {
                            if(state.getAt(x,y) == 0 || state.getAt(x+2,y+2) == 0)
                            {
                                oddThreats++;
                            }
                        }
                        break;
                    }
                }
            }
        }

        //diagonals R-L
        for(int x = state.getWidth()-1; x >= state.getWidth() - 4; x--)
        {
            for(int y = 0; y < state.getHeight() - 3; y++)
            {
                boolean two_combo = false;
                for(int[] combo : combo_two)
                {
                    if(state.getAt(x,y) == combo[0] && state.getAt(x-1,y+1) == combo[1] && state.getAt(x-2,y+2) == combo[2] && state.getAt(x-3,y+3) == combo[3])
                    {
                        if(combo == combo_two[3] || combo == combo_two[0] || combo == combo_two[5])
                        {
                            d_score += 5*seq_2;
                        }
                        d_score += seq_2;
                        two_combo = true;
                        break;
                    }
                }

                if(two_combo)
                    continue;

                for(int[] combo: combo_three)
                {
                    if(state.getAt(x,y) == combo[0] && state.getAt(x-1,y+1) == combo[1] && state.getAt(x-2,y+2) == combo[2] && state.getAt(x-3,y+3) == combo[3])
                    {
                        if(combo == combo_three[0] && x > state.getWidth()-4 && y < state.getHeight()-4)
                        {
                            if(state.getAt(x-1,y+1) == combo_three[3][0] && state.getAt(x-2,y+2) == combo_three[3][1] && state.getAt(x-3,y+3) == combo_three[3][2] && state.getAt(x-4,y+4) == combo_three[3][3])
                            {
                                d_score += 5*seq_3;
                                num_dthreats++;
                            }
                        }
                        d_score += seq_3;
                        num_dthreats++;
                        if((x+1) % 2 == 0)
                        {
                            if(state.getAt(x,y) == 0 || state.getAt(x-2,y+2) == 0)
                            {
                                evenThreats++;
                            }
                        }
                        else
                        {
                            if(state.getAt(x,y) == 0 || state.getAt(x-2,y+2) == 0)
                            {
                                oddThreats++;
                            }
                        }
                        break;
                    }
                }
            }
        }

        int val = threat_weight*((firstPlayer * oddThreats)+(secondPlayer * evenThreats))+(c_score+(num_cthreats*threat_weight))+ 2*(r_score+(num_rthreats*threat_weight))+(3*d_score+(num_dthreats*threat_weight));
        return val;
    }

    private int alphabeta(final GameStateModule state, int depth, int alpha, int beta, int playerID)
    {
        if(terminate)
            return 0;
        if(depth == 0 || state.isGameOver())
        {
            if(state.isGameOver())
            {
                if(state.getWinner() == player)
                {
                    return Integer.MAX_VALUE;
                }
                else
                {
                    return Integer.MIN_VALUE;
                }
            }
            return eval(state,player) - (int)((double)(eval(state, opponent)*1.1));
        }
        int val = 0;
        if(playerID == player)
        {
            val = Integer.MIN_VALUE+1;
            for(int i : ab_successor(state))
            {
                if(state.canMakeMove(i))
                {
                    state.makeMove(i);
                    val = Math.max(val, alphabeta(state, depth-1, alpha, beta, opponent));
                    state.unMakeMove();
                    if(val > alpha)
                    {
                        alpha = val;
                        bestMoveSeen = i;
                    }
                    if(alpha >= beta)
                    {
                        break;
                    }

                }
            }
            return val;
        }
        else
        {
            val = Integer.MAX_VALUE-1;
            for(int i : ab_successor(state))
            {
                if(state.canMakeMove(i))
                {
                    state.makeMove(i);
                    val = Math.min(val, alphabeta(state, depth-1, alpha, beta, player));
                    state.unMakeMove();
                    if(val < beta)
                    {
                        beta = val;
                        bestMoveSeen = i;
                    }
                    if(alpha >= beta)
                    {
                        break;
                    }

                }
            }
            return val;
        }
    }

    public List<Integer> ab_successor(final GameStateModule state)
    {
        ArrayList<Integer> potential_moves = new ArrayList<Integer>();
        ArrayList<Integer> moves = new ArrayList<Integer>();
        potential_moves.add(3);
        potential_moves.add(4);
        potential_moves.add(2);
        potential_moves.add(1);
        potential_moves.add(5);
        potential_moves.add(0);
        potential_moves.add(6);

        for(int i = 0; i < potential_moves.size(); i++)
        {
            int index = potential_moves.get(i);
            if(player % 2 == 1)
            {
                if(state.canMakeMove(index))
                {
                    state.makeMove(index);
                    if(state.getHeightAt(index) % 2 == 1)
                    {
                        moves.add(0, index);
                        state.unMakeMove();
                        continue;
                    }
                }
            }
            else
            {
                if(state.canMakeMove(index))
                {
                    state.makeMove(index);
                    if(state.getHeightAt(index) % 2 == 0)
                    {
                        moves.add(0, index);
                        state.unMakeMove();
                        continue;
                    }
                }
            }
            moves.add(index);
        }

        return moves;
    }
}