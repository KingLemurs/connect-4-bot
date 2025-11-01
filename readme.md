Connect4 Functions:
- getNextOpenRow(): Helper function for finding the lowest possible move in a col.
- actionForEmptyHolder(): Main function for placing pieces

AI Functions:
- UpdateAI(): This is the entry point for my connect4 AI, which plays the best move returned by the negamax() function.
- negamax(): This is my main recursive algorithm for scoring and simulating AI moves within a certain depth.
- evalBoard(): This is the function that scores each 4 bit window that the negamax function uses to determine how good the move is.
- scoreMove(): Helper function for evalBoard()
- getValidAIMoveForCol(): Helper function for finding the lowest possible move in a col.
- scoreWindow(): Helper function for adding up scores in a window.