#pragma once
class TicTacToe
{
public:
	TicTacToe();
	~TicTacToe();

	void Draw() const;
	bool Input(char name,char a);
	void TogglePlayer();
	bool CheckPlayerWin(char who);
	void ResetBoard();
	int IncTurn();
	void SetTurn(int num);
	int GetTurn() const { return mTurn; }
private:
	char matrix[4][4] = { '1', '2', '3', '4', '5', '6', '7', '8', '9',':',';','<','=','>', '?' ,'@' };
	char player{ 'X' };												  
	int mTurn{ 1 };													 
};																	 
