#include "Board.h"
#include <algorithm>
using namespace std;

//#define USE_SET_NUMBER_OF_PIECES

namespace stacker
{

struct LineDetails
{
	int nPieces;
	int nStartPosition;
	Direction eDirection;
} g_Lines[] = 
#if BOARD_HEIGHT < 15
{{	3,	2,	Left	},
{	3,	0,	Left	},
{	3,	4,	Right	},
{	3,	1,	Left	},
{	2,	5,	Left	},
{	2,	2,	Right	},
{	2,	3,	Left	},
{	2,	1,	Left	},
{	1,	6,	Right	},
{	1,	1,	Left	},
#else
{{	4,	2,	Left	},
{	4,	0,	Left	},
{	4,	4,	Right	},
{	3,	1,	Left	},
{	3,	5,	Left	},
{	3,	2,	Right	},
{	3,	3,	Left	},
{	3,	1,	Left	},
{	2,	6,	Right	},
{	2,	1,	Left	},
#endif
#if BOARD_HEIGHT > 10 && BOARD_HEIGHT < 15
{	1,	3,	Left	},
{	1,	6,	Right	},
{	1,	2,	Left	},
{	1,	3,	Right	},
{	1,	0,	Left	},
#elif BOARD_HEIGHT > 15
{	2,	3,	Left	},
{	2,	6,	Right	},
{	2,	2,	Left	},
{	2,	3,	Right	},
{	2,	0,	Left	},
#endif
#if BOARD_HEIGHT > 15
{	1,	2,	Left	},
{	1,	1,	Right	},
{	1,	6,	Left	},
{	1,	4,	Right	},
{	1,	2,	Left	},
#endif
};

Board::Board() : m_pCurrentPiece(NULL)
{
	Reset();
}

void Board::Reset()
{
	if( m_pCurrentPiece ) delete m_pCurrentPiece;
	m_nCurrentPieceRow = 0;
	m_bGameOver = false;

	//Initialize board to all off
	for(int x=0; x<BOARD_WIDTH; x++)
		for(int y=0; y<BOARD_HEIGHT; y++)
			m_data[y][x] = false;

	m_pCurrentPiece = new CurrentPiece(this, m_nCurrentPieceRow);
}

bool Board::PieceAt(int nX, int nY)
{
	if( nX < 0 || nX >= GetWidth() || nY < 0 || nY >= GetHeight() ) return false;

	return m_data[nY][nX];
}

void Board::SetPieceAt(int nX, int nY, bool bEnable)
{
	if( nX < 0 || nX >= GetWidth() || nY < 0 || nY >= GetHeight() ) return;

	m_data[nY][nX] = bEnable;
}

void Board::Step()
{
	if( IsGameOver() ) return;

	if (!m_pCurrentPiece)
	{
		m_nCurrentPieceRow++;
		if( m_nCurrentPieceRow >= BOARD_HEIGHT ) return;
		m_pCurrentPiece = new CurrentPiece(this, m_nCurrentPieceRow);
	}

	if( m_pCurrentPiece->Stopped() )
	{
		if( !m_pCurrentPiece->Fall() ) { CheckGameOver(); delete m_pCurrentPiece; m_pCurrentPiece = 0; }
	}
	else 
		m_pCurrentPiece->Move();
}

bool Board::Stop()
{
	if (!m_pCurrentPiece) return false;

	return m_pCurrentPiece->Stop();
}

void Board::CheckGameOver()
{
	if( IsGameOver() || !m_pCurrentPiece ) return;

	//Check m_pCurrentPieces's row for any pieces
	for( int x=m_pCurrentPiece->GetPosition(); x<GetWidth(); ++x )
		if( PieceAt(x, m_pCurrentPiece->GetRow() ) ) return;

	m_bGameOver = true;
}

bool Board::WonGame()
{
	return GetPiecesOnRow(GetHeight()-1) > 0;
}

int Board::GetPiecesOnRow(int nRow)
{
	int nCount = 0;
	for( int x = 0; x<GetWidth(); ++x )
		if( PieceAt(x, nRow ) ) nCount++;

	return nCount;
}

Board::CurrentPiece::CurrentPiece(Board *pBoard, int nRow) : m_pBoard(pBoard), m_nRow(nRow), m_bStopped(false), m_nPosition(g_Lines[nRow].nStartPosition), m_eDirection(g_Lines[nRow].eDirection), m_pPoints(NULL)
{
#if defined(USE_SET_NUMBER_OF_PIECES)
	m_nWidth = g_Lines[m_nRow].nPieces;
#else
	m_nWidth = min(g_Lines[m_nRow].nPieces, nRow <=0 ? g_Lines[m_nRow].nPieces : pBoard->GetPiecesOnRow(m_nRow - 1));
#endif
	Apply(true);
}

bool Board::CurrentPiece::Move()
{
	if( m_nPosition <= 0 && m_eDirection == Left ) { m_eDirection = Right; return false; }
	if( m_nPosition >= (m_pBoard->GetWidth() - m_nWidth) && m_eDirection == Right ) { m_eDirection = Left; return false; }

	Apply(false);

	if( m_eDirection == Left ) m_nPosition--;
	if( m_eDirection == Right ) m_nPosition++;

	Apply(true);

	return true;
}

void Board::CurrentPiece::Apply(bool bOn)
{
	for(int i=0; i<m_nWidth; i++)
		m_pBoard->SetPieceAt(m_nPosition + i, m_nRow, bOn);
}

bool Board::CurrentPiece::Stop()
{
	if( Stopped() ) return false;
	m_bStopped = true;
	m_pPoints = new Point[m_nWidth];
	for( int i=0; i<m_nWidth; i++ )
		(m_pPoints+i)->x = m_nPosition + i, (m_pPoints+i)->y = m_nRow;

	return true;
}

bool Board::CurrentPiece::Fall()
{
	for(int i=0; i < m_nWidth; i++)
	{
		if( (m_pPoints + i)->y > 0 && !m_pBoard->PieceAt((m_pPoints + i)->x, (m_pPoints + i)->y - 1) ) { m_pBoard->SetPieceAt((m_pPoints + i)->x, (m_pPoints + i)->y, false); m_pBoard->SetPieceAt((m_pPoints + i)->x, (m_pPoints + i)->y - 1, true); (m_pPoints + i)->y = (m_pPoints + i)->y - 1; return true; }
		else if( (m_pPoints + i)->y >= 0 && (m_pPoints + i)->y != m_nRow ) {m_pBoard->SetPieceAt((m_pPoints + i)->x, (m_pPoints + i)->y, false); (m_pPoints + i)->y = -1; return true; }
	}

	return false;
}

}
