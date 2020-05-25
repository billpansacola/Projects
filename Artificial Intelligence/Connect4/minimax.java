import java.util.*;

public class minimax extends AIModule
{
	int player;
	int opponent;
	int maxDepth = 5;
	int bestMoveSeen;

	public void getNextMove(final GameStateModule game)
	{
        player = game.getActivePlayer();
        opponent = (game.getActivePlayer() == 1?2:1);
		//begin recursion
		while(!terminate){
			minimax(game, 0, player);
            if(!terminate)
				chosenMove = bestMoveSeen;
		}
		if(game.canMakeMove(chosenMove))
			game.makeMove(chosenMove);
	}

	private int minimax(final GameStateModule state, int depth, int playerID) {
        if (terminate)
            return 0;
        if (depth == maxDepth) {
            return eval(state);
        }
        depth++;
        int value = 0;
        //max's turn
        int bestVal = Integer.MIN_VALUE;
        if(playerID == player){
            value = Integer.MIN_VALUE + 1;
            for(int i = 0; i < state.getWidth(); i++){
                if(state.canMakeMove(i)) {
                    state.makeMove(i);
                    value = Math.max(value, minimax(state, depth, opponent));
                    state.unMakeMove();
                    if (value > bestVal){
                        bestVal = value;
                        if (depth == 1) { //top of recursion, make our move choice
                            bestMoveSeen = i;
                        }
                    }
                }
            }
            return value;
        }

        else { //min's turn
            value = Integer.MAX_VALUE;
            for(int i = 0; i < state.getWidth(); i++) {
                if (state.canMakeMove(i)) {
                    state.makeMove(i);
                    value = Math.min(value, minimax(state, depth, player));
                    state.unMakeMove();
                }
            }
            return value;
        }
    }

    // randomly assigns a value to a state
	private int eval(final GameStateModule state){
	    Random rand = new Random();
		return rand.nextInt(100);
	}


}