/**
 * \file DiamondBoard.cpp
 * \date Dec 7, 2012
 * \author Kurt Dietz (some code by Bill Cleveland originally).
 */
#include "stdafx.h"
#include "DiamondBoard.h"

int DiamondBoard::boardCount = 0;

void DiamondBoard::initLibrary()
{
  int result;

  // If no boards are initialized, open the DSCUD library.
  if(boardCount == 0)
  {
    if ((result = dscInit(DSC_VERSION)) != DE_NONE)
      throw result;
  }
}

void DiamondBoard::incrementBoardCount()
{
  boardCount++;
}

void DiamondBoard::decrementBoardCount()
{
  // Decrement the board count.
  boardCount--;

  // Close the library if there are no more boards left.
  int result;
  if(boardCount == 0)
  {
    if ((result = dscFree()) != DE_NONE)
      throw result;
  }
}

DiamondBoard::DiamondBoard(unsigned char type, DSCCB *dsccb)
{
  int result;

  initLibrary();

  // Open the board with the given type and configuration settings.
  if ((result = dscInitBoard(type, dsccb, &handle)) != DE_NONE)
    throw result;

  incrementBoardCount();
}

DiamondBoard::~DiamondBoard()
{
  int result;

  // Close the board.
  if ((result = dscFreeBoard(handle)) != DE_NONE)
    throw result;

  // Decrement the board count.
  decrementBoardCount();
}

int DiamondBoard::getLastError()
{
  ERRPARAMS errparams;
  dscGetLastError(&errparams);

  return (int)errparams.ErrCode;
}

int DiamondBoard::getLastError(std::string& errStr)
{
  ERRPARAMS errparams;
  dscGetLastError(&errparams);

  errStr = errparams.errstring;
  return (int)errparams.ErrCode;
}

std::string DiamondBoard::getErrorString(int err)
{
  std::string retVal;
  retVal = ::dscGetErrorString(err);
  return retVal;
}
