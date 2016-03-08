  /// \file XdrDefinitions.h
/*
 *
 * XdrDefinitions.h header template automatically generated by a class generator
 * Creation date : lun. f�vr. 8 2016
 *
 * This file is part of xdrstream libraries.
 * 
 * xdrstream is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * based upon these libraries are permitted. Any copy of these libraries
 * must include this copyright notice.
 * 
 * xdrstream is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with xdrstream.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * @author rete
 * @copyright CNRS , IPNL
 */


#ifndef XDRDEFINITIONS_H
#define XDRDEFINITIONS_H

// apple stuff for stdint.h
#ifdef __APPLE__
#include <_types.h>
#include <_types/_uint8_t.h>
#include <_types/_uint16_t.h>
#include <_types/_uint32_t.h>
#include <_types/_uint64_t.h>
#include <sys/_types/_int16_t.h>
#include <sys/_types/_int32_t.h>
#include <sys/_types/_int64_t.h>
#else
#include <stdint.h>
#endif

// -- std headers
#include <set>
#include <iostream>
#include <string>
#include <sstream>
#include <utility>
#include <map>
#include <vector>
#include <list>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <exception>
#include <stdexcept>
#include <limits>

namespace xdrstream
{

class Block;
class Record;
class IODevice;
class PointerMapper;
class XdrStream;

#define XDR_BIT(n)       (1ULL<< (n))
#define XDR_SETBIT(n,i)  ((n) |= XDR_BIT(i))
#define XDR_CLRBIT(n,i)  ((n) &= ~XDR_BIT(i))
#define XDR_TESTBIT(n,i) ((bool)(((n) & i) != 0))

// macro to simplify api calls

#ifdef XDRLCIO_DEBUG_MODE


#define XDR_STREAM( Operation ) \
{ \
	xdrstream::Status status = Operation; \
	if( ! XDR_TESTBIT( status, xdrstream::XDR_SUCCESS ) ) \
	{ \
		std::cerr << "XDR_STREAM returned on line " << __LINE__ << " of file " << __FILE__ << std::endl; \
		return status; \
	} \
}

#define DEBUG_PRINT( message ) std::cout << message << std::endl;
#define DEBUG_DO( Operation ) Operation

#else


#define XDR_STREAM( Operation ) \
{ \
	xdrstream::Status status = Operation; \
	if( ! XDR_TESTBIT( status, xdrstream::XDR_SUCCESS ) ) \
		return status; \
}

#define DEBUG_PRINT( message )
#define DEBUG_DO( Operation )

#endif


// various typedefs
typedef uint32_t                     xdr_size_t;
typedef uint32_t                     xdr_word_t;
typedef uint32_t                     xdr_version_t;
typedef int64_t                      xdr_addr_t;
typedef int32_t                      xdr_marker_t;

typedef std::map<std::string, Block *>    BlockMap;
typedef std::map<std::string, Record *>   RecordMap;
typedef unsigned long long                Status;

typedef void *(* xdr_allocator)(xdr_size_t size);

template <typename T>
struct xdr_allocator_helper
{
	static void *alloc(xdr_size_t size) { return new T [ size ]; }
};

// marker definitions
static const xdr_marker_t undefinedMarker = 1000;
static const xdr_marker_t blockMarker     = 1001;
static const xdr_marker_t recordMarker    = 1002;

/**
 *  @brief  OpenMode enum
 */
enum OpenMode
{
	XDR_NOT_OPENED     = XDR_BIT(0),
	XDR_READ           = XDR_BIT(1),
	XDR_WRITE          = XDR_BIT(2),
	XDR_APPEND         = XDR_READ | XDR_WRITE
};

/**
 *  @brief  StreamingMode enum
 */
enum StreamingMode
{
	XDR_READ_STREAM    = XDR_BIT(1),
	XDR_WRITE_STREAM   = XDR_BIT(2)
};

/**
 *  @brief  DeviceState enum
 */
enum DeviceState
{
	XDR_OPENED     = XDR_BIT(0),
	XDR_CLOSED     = XDR_BIT(1),
	XDR_ERROR      = XDR_BIT(2)
};

/**
 *  @brief  StatusCode enum
 */
enum StatusCode
{
	XDR_SUCCESS            = XDR_BIT(0),
	XDR_FAILURE            = XDR_BIT(1),
	XDR_NOT_FOUND          = XDR_BIT(2),
	XDR_UNEXPECTED_MARKER  = XDR_BIT(3),
	XDR_RECORD_NOT_FOUND   = XDR_BIT(4),
	XDR_BLOCK_NOT_FOUND    = XDR_BIT(5),
	XDR_ALREADY_OPENED     = XDR_BIT(6),
	XDR_OPEN_FAILED        = XDR_BIT(7),
	XDR_IO_ERROR           = XDR_BIT(8),
	XDR_INVALID_PARAMETER  = XDR_BIT(9),
	XDR_EOF                = XDR_BIT(10),
	XDR_EOR                = XDR_BIT(11),
	XDR_ALREADY_PRESENT    = XDR_BIT(12),
	XDR_DEVICE_NOT_OPENED  = XDR_BIT(13)
};

//----------------------------------------------------------------------------------------------------

inline void printStatus( Status status )
{
	std::cout << "Status : ";

	for( int i=0 ; i<14 ; i++ )
	{
		if( XDR_TESTBIT( status, XDR_BIT(i) ) )
		{
			std::cout << i << " ";
		}
	}

	std::cout << std::endl;
}

//----------------------------------------------------------------------------------------------------

inline void printRawBuffer(char *pBuffer, xdr_size_t bufferSize , uint32_t returnLine = 10)
{
	std::cout << "buffer address : " << (void *) pBuffer << " size : " << bufferSize << std::endl;

	for(int i=0 ; i<bufferSize ; i++)
	{
		printf("%2x", pBuffer[i]);

		if(i%returnLine == 0 && i != 0)
			std::cout << std::endl;
	}

	std::cout << std::endl;
}

//----------------------------------------------------------------------------------------------------

inline void printSubRawBuffer(char *pBuffer, xdr_size_t from, xdr_size_t to, uint32_t returnLine = 10)
{
	std::cout << "(from-to) buffer address : " << (void *) pBuffer << std::endl;

	for(xdr_size_t i=from ; i<to ; i++)
	{
		printf("%2x", pBuffer[i]);

		if(i%returnLine == 0 && i != 0)
			std::cout << std::endl;
	}

	std::cout << std::endl;
}

//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

/**
 *  @brief  Header structure.
 *          Defines a block of data describing a data header
 */
struct Header
{
public:
	/**
	 *  @brief  Constructor
	 */
	Header();

public:
	xdr_word_t            m_optionWord;       ///< An option word used to read/write data structure
	xdr_size_t            m_headerLength;     ///< The header length
	xdr_marker_t          m_marker;           ///< The header marker, describing the type of data after the header
	xdr_size_t            m_length;           ///< The incoming data block length
	xdr_version_t         m_version;          ///< The version of the block of data
	std::string           m_name;             ///< The name of the block of data
};

//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

inline Header::Header() :
		m_optionWord(0),
		m_headerLength(0),
		m_marker(undefinedMarker),
		m_length(0),
		m_version(0)
{
	/* nop */
}

} 

#endif  //  XDRDEFINITIONS_H
