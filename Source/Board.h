#pragma once

//#define STACKER_TALL
//#define STACKER_TALLER

#define BOARD_WIDTH		7
#if !defined(STACKER_TALL) && !defined(STACKER_TALLER)
#define BOARD_HEIGHT	10
#elif defined(STACKER_TALL) && !defined(STACKER_TALLER)
#define BOARD_HEIGHT	15
#else
#define BOARD_HEIGHT	20
#endif

namespace stacker
{

typedef enum tagDirection
{
	Left,
	Right
} Direction;

class Board
{
public:
	Board();

	void Reset();

	int GetWidth() { return BOARD_WIDTH; }
	int GetHeight() { return BOARD_HEIGHT; }

	bool PieceAt(int nX, int nY);
	void SetPieceAt(int nX, int nY, bool bEnable);

	int GetLineRow()	{ return m_nCurrentPieceRow; }

	void Step();
	bool Stop();

	int GetPiecesOnRow(int nRow);
	void CheckGameOver();
	bool IsGameOver()	{ return m_bGameOver; }
	bool WonGame();
protected:
	bool m_data[BOARD_HEIGHT][BOARD_WIDTH];

protected:
	class CurrentPiece
	{
	public:
		CurrentPiece(Board *pBoard, int nRow);
		~CurrentPiece() { delete [] m_pPoints; }
		bool Move();
		bool Stop();
		bool Fall();

		bool Stopped()		{ return m_bStopped;	}
		int GetRow()		{ return m_nRow;		}
		int GetPosition()	{ return m_nPosition;	}
		int GetWidth()		{ return m_nWidth;		}

	protected:
		int m_nPosition;
		Direction m_eDirection;
		bool m_bStopped;
		int m_nWidth; int m_nRow;
		Board * m_pBoard;
		class Point		{		public: int x, y;		};
		Point	* m_pPoints;

	protected:
		void Apply(bool bOn);
	} * m_pCurrentPiece;

	int m_nCurrentPieceRow;
	bool m_bGameOver;
};

}
