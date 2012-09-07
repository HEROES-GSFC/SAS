///	\file WordBuffer.cpp
///		C++ source file defining the behavior of the CWordBuffer class.

#include "StdAfx.h"
#include "ShortBuffer.h"
#include "ErrorCodes.h"

//	Constructor, initializes size and buffer.
WordBuffer::WordBuffer( int size )
{
  this->buffer = new uint16_t[size];
  this->size = size;
}

//	Destructor.  Deletes buffer.
WordBuffer::~WordBuffer()
{
	// Delete the buffer, if any.
	delete []buffer;
}

// Return element, if within bounds.
int WordBuffer::getElement(int i, uint16_t &value)
{
  // Get the element if it is within bounds.
  if ( i >= 0 && i < size )
  {
    value = buffer[i];
    return 0;
  }

  // Otherwise, return an error.
  return WBUFERR_INDEXINVALID;
}

// Set element, if within bounds.
int WordBuffer::setElement(int i, uint16_t value)
{
  // Get the element if it is within bounds.
  if ( i >= 0 && i < size )
  {
    buffer[i] = value;
    return 0;
  }

  // Otherwise, return an error.
  return WBUFERR_INDEXINVALID;
}

// Access an element within the valid bounds.
uint16_t& WordBuffer::operator[](int i)
{
  // Force the index to be within bounds.
  i = i < 0 ? 0 : i;
  i = i >= size ? size-1 : i;

  // Return the element.
  return buffer[i];
}

///////////////////////////////////////////////////////////////////////////////
// ShortBuffer

//  Constructor.  Sets size and creates buffer.
ShortBuffer::ShortBuffer( int size )
{
  this->buffer = new int16_t[size];
  this->size = size;
}

//   Destructor.  Deletes any buffer and closes the mutex.
ShortBuffer::~ShortBuffer()
{
  // Delete the buffer, if any.
  delete []buffer;
}

// Return element, if within bounds.
int ShortBuffer::getElement(int i, int16_t &value)
{
  // Get the element if it is within bounds.
  if ( i >= 0 && i < size )
  {
    value = buffer[i];
    return 0;
  }

  // Otherwise, return an error.
  return WBUFERR_INDEXINVALID;
}

// Set element, if within bounds.
int ShortBuffer::setElement(int i, int16_t value)
{
  // Get the element if it is within bounds.
  if ( i >= 0 && i < size )
  {
    buffer[i] = value;
    return 0;
  }

  // Otherwise, return an error.
  return WBUFERR_INDEXINVALID;
}

// Access an element within the valid bounds.
int16_t& ShortBuffer::operator[](int i)
{
  // Force the index to be within bounds.
  i = i < 0 ? 0 : i;
  i = i >= size ? size-1 : i;

  // Return the element.
  return buffer[i];
}

int ShortBuffer::copy( ShortBuffer &buff )
{
  int length = size < buff.getSize() ? size : buff.getSize();
  memcpy( buffer, buff.buffer, length * sizeof(int16_t) );
  return 0;
}
