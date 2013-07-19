/**
 * \file DiamondBoard.h
 * \date Dec 7, 2012
 * \author Kurt Dietz
 */

#ifndef DIAMONDBOARD_H_
#define DIAMONDBOARD_H_

#include <string>

#ifdef _WIN32
#include "dscud.h"
#else
#include <dmm/dscud.h>
#endif

/**
 * Base class for all Diamond Systems board classes.
 */
class DiamondBoard
{
public:
  /**
   * Opens board.  If first board, initializes the DSCUD library, too.
   * \param[in]     type    DSCUD board type designator.
   * \param[in]     dsccb   DSCUD board configuration structure.
   */
  DiamondBoard(unsigned char type, DSCCB *dsccb);

  /**
   * Closes board.  If no more are opened, closes the DSCUD library, too.
   */
  virtual ~DiamondBoard();

  /**
   * Returns the last board error ID encountered.
   */
  int getLastError();

  /**
   * Returns the last board error ID encountered and a string describing it.
   * \param[out]    errStr    String describing last error ID encountered.
   */
  int getLastError(std::string& errStr);

  /**
   * Returns a string describing a given error ID.
   * \param[in]   err   Error (from getLastError()) to get description of.
   */
  std::string getErrorString(int err);

protected:
  DSCB        handle;       ///< Handle to board for operations.

private:
  static int  boardCount;
  static void initLibrary();
  static void incrementBoardCount();
  static void decrementBoardCount();
  bool        init;
};

#endif /* DIAMONDBOARD_H_ */
