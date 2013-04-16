///	\file WordBuffer.h
///		C++ header file declaring the WordBuffer class.

#if !defined(AFX_WORDBUFFER_H__41C2529D_D49F_4935_913F_9EAA1BA7BF45__INCLUDED_)
#define AFX_WORDBUFFER_H__41C2529D_D49F_4935_913F_9EAA1BA7BF45__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stdint.h>
#include <stdio.h>
#include <string.h>

///		This class encapsulates the behavior of uint16_t-data buffer.
class WordBuffer
{
// Construction/destruction.
public:
  /**
   * Creates the buffer of the correct size.
   * \param[in]   size    The size of the buffer.
   */
	WordBuffer(int size);

	/**
	 * Deletes the buffer.
	 */
	virtual ~WordBuffer();

// Protected data members.
protected:
	int		    size;				///< The size of the data storage buffer.
	uint16_t	*buffer;		///< The data storage buffer.

// Public member functions.
public:
	/**
	 * Returns the size of the buffer.
	 * \return The size of the buffer, in # of 16-bit words.
	 */
	int getSize() {return size;};

	/**
	 * Returns a pointer to the data buffer.
	 */
	uint16_t*	getBuffer() {return buffer;};

	/**
	 * Returns the value of the i-th element of the buffer.
	 * \param[in]    i     Index of the element to fetch.
	 * \param[out]   value The value of the element.
	 * \return   0 if i is within the bounds of the buffer array.
	 * \par
	 *    WBUFERR_INDEXINVALID if the element is outside the bounds of the
	 *    buffer array.
	 */
	int getElement(int i, uint16_t &value);

  /**
   * Sets the value of the i-th element of the buffer.
   * \param[in]    i     Index of the element to fetch.
   * \param[in]    value The value to set the element to.
   * \return   0 if i is within the bounds of the buffer array.
   * \par
   *    WBUFERR_INDEXINVALID if the element is outside the bounds of the
   *    buffer array.
   */
	int setElement(int i, uint16_t value);

	/**
	 * Sets all the elements to zero.
	 */
	void zero() {memset(buffer, 0, size*sizeof(uint16_t));};

	/**
	 * Access an element.
	 * \param[in]    i   Index of element, will be forced to valid range.
	 * \return   Reference to a valid element.
	 */
	uint16_t& operator[](int i);
};

///   This class encapsulates the behavior of int16_t-data buffer.
class ShortBuffer
{
// Construction/destruction.
public:
  /**
   * Creates the buffer of the correct size.
   * \param[in]   size    The size of the buffer.
   */
  ShortBuffer(int size);

  /**
   * Deletes the buffer.
   */
  virtual ~ShortBuffer();

// Protected data members.
protected:
  int       size;       ///< The size of the data storage buffer.
  int16_t   *buffer;    ///< The data storage buffer.

// Public member functions.
public:
  /**
   * Returns the size of the buffer.
   * \return The size of the buffer, in # of 16-bit words.
   */
  int getSize() {return size;};

  /**
   * Returns a pointer to the data buffer.
   */
  int16_t* getBuffer() {return buffer;};

  /**
   * Returns the value of the i-th element of the buffer.
   * \param[in]    i     Index of the element to fetch.
   * \param[out]   value The value of the element.
   * \return   0 if i is within the bounds of the buffer array.
   * \par
   *    WBUFERR_INDEXINVALID if the element is outside the bounds of the
   *    buffer array.
   */
  int getElement(int i, int16_t &value);

  /**
   * Sets the value of the i-th element of the buffer.
   * \param[in]    i     Index of the element to fetch.
   * \param[in]    value The value to set the element to.
   * \return   0 if i is within the bounds of the buffer array.
   * \par
   *    WBUFERR_INDEXINVALID if the element is outside the bounds of the
   *    buffer array.
   */
  int setElement(int i, int16_t value);

  /**
   * Sets all the elements to zero.
   */
  void zero() {memset(buffer, 0, size*sizeof(int16_t));};

  /**
   * Access an element.
   * \param[in]    i   Index of element, will be forced to valid range.
   * \return   Reference to a valid element.
   */
  int16_t& operator[](int i);

  /**
   * Copy as much of the contents of another ShortBuffer object as possible.
   * \param[in]   buff    ShortBuffer object to copy data from.
   */
  int copy( ShortBuffer &buff );
};

#endif // !defined(AFX_WORDBUFFER_H__41C2529D_D49F_4935_913F_9EAA1BA7BF45__INCLUDED_)
