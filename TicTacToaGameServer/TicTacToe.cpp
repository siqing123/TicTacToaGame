
#include <iostream>
#include <stdlib.h>
#include <string>
#include "TicTacToe.h"

using namespace std;
TicTacToe::TicTacToe() 
{
}
TicTacToe::~TicTacToe()
{
}

void TicTacToe::Draw() const
{
	cout << "Tic Tac Toe: " << endl;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			cout << matrix[i][j] << " ";
		}
		cout << endl;
	}
	cout << endl;

}

bool TicTacToe::Input(char name,char a)
{
	if (a < '1' || a > '@') // invalid input
	{
		return false;
	}

	int index;

	if ('1' < a && a < '9' || a == '1' || a == '9')
	{
		 index = std::stoi(&a) - 1 ;
		 cout << index << endl;
	}
	else
	{
		 index =  int(a) - 48 - 1 ;
	}

	if (matrix[index/4][index%4] == a)
		matrix[index / 4][index % 4] = name;
	else
	{
		cout << "Field is already in use try again!" << endl;
		return false;
	}

	return true;
}

void TicTacToe::TogglePlayer()
{
	if (player == 'X')
		player = 'O';
	else
		player = 'X';
}
bool TicTacToe::CheckPlayerWin(char who)
{
	// row match wins
	for (int i = 0; i < 4; ++i) 
	{
		if (matrix[i][0] == who && matrix[i][1] == who && matrix[i][2] == who )
		{
			return true;
		}

		if (matrix[i][1] == who && matrix[i][2] == who && matrix[i][3] == who)
		{
			return true;
		}
	}

	// column match wins
	for (int i = 0; i < 4; ++i)
	{
		if (matrix[0][i] == who && matrix[1][i] == who && matrix[2][i] == who)
		{
			return true;
		}

		if (matrix[1][i] == who && matrix[2][i] == who && matrix[3][i] == who)
		{
			return true;
		}
	}

	// diagonal match wins
	if (matrix[0][0] == who && matrix[1][1] == who && matrix[2][2] == who )
	{
		return true;
	}
	if ( matrix[1][1] == who && matrix[2][2] == who && matrix[3][3] == who)
	{
		return true;
	}

	if (matrix[3][0] == who && matrix[2][1] == who && matrix[1][2] == who)
	{
		return true;
	}
	if ( matrix[2][1] == who && matrix[1][2] == who && matrix[0][3] == who)
	{
		return true;
	}
	return false;
}

void TicTacToe::ResetBoard()
{
	matrix[0][0] = '1';
	matrix[0][1] = '2';
	matrix[0][2] = '3';
	matrix[0][3] = '4';
	matrix[1][0] = '5';
	matrix[1][1] = '6';
	matrix[1][2] = '7';
	matrix[1][3] = '8';
	matrix[2][0] = '9';
	matrix[2][1] = ':';
	matrix[2][2] = ';';
	matrix[2][3] = '<';
	matrix[3][0] = '=';
	matrix[3][1] = '>';
	matrix[3][2] = '?';
	matrix[3][3] = '@';
}

int TicTacToe::IncTurn()
{
	return mTurn++;
}

void TicTacToe::SetTurn(int num)
{
	mTurn = num;
}
