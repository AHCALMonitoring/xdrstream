  /// \file BufferDevice.cc
/*
 *
 * BufferDevice.cc source template automatically generated by a class generator
 * Creation date : dim. janv. 17 2016
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
 * @author Remi Ete
 * @copyright CNRS , IPNL
 */


#include "xdrstream/BufferDevice.h"

namespace xdrstream
{

BufferDevice::BufferDevice(xdr_size_t size) :
		IODevice(),
		m_expandSize(1024),
		m_isOwner(true)
{
	if( 0 == size )
		throw std::invalid_argument("BufferDevice::BufferDevice(size) : Couldn't have a null buffer size !");

	m_pBuffer = new char[size];
	m_bufferSize = size;
	m_pBufferPosition = m_pBuffer;
	m_openMode = XDR_WRITE;
}

//----------------------------------------------------------------------------------------------------

BufferDevice::BufferDevice(char *pBuffer, xdr_size_t size, bool copy) :
		IODevice(),
		m_expandSize(1024),
		m_isOwner(true)
{
	if( NULL == pBuffer || 0 == size )
		throw std::invalid_argument("BufferDevice::BufferDevice(b, s, owner) : Couldn't have a null buffer or null buffer size !");

	if( ! copy )
	{
		m_pBuffer = pBuffer;
	}
	else
	{
		m_pBuffer = new char[size];
		memcpy(m_pBuffer, pBuffer, size);
	}

	m_bufferSize = size;
	m_pBufferPosition = m_pBuffer;
	m_openMode = XDR_READ;
}

//----------------------------------------------------------------------------------------------------

BufferDevice::BufferDevice(OpenMode openMode, char *pBuffer, xdr_size_t size, bool copy) :
		IODevice(),
		m_expandSize(1024),
		m_isOwner(true)
{
	if( NULL == pBuffer || 0 == size )
		throw std::invalid_argument("BufferDevice::BufferDevice(m, b, s, owner) : Couldn't have a null buffer or null buffer size !");

	if( openMode == XDR_NOT_OPENED )
		throw std::invalid_argument("BufferDevice::BufferDevice(m, b, s, owner) : Open mode can't be XDR_NOT_OPENED !");

	if( ! copy )
	{
		m_pBuffer = pBuffer;
	}
	else
	{
		m_pBuffer = new char[size];
		memcpy(m_pBuffer, pBuffer, size);
	}

	m_bufferSize = size;
	m_pBufferPosition = m_pBuffer;
	m_openMode = openMode;
}

//----------------------------------------------------------------------------------------------------

BufferDevice::~BufferDevice() 
{
	if( m_isOwner && NULL != m_pBuffer )
		delete [] m_pBuffer;

	m_bufferSize = 0;
	m_pBuffer = NULL;
	m_pBufferPosition = NULL;
}

//----------------------------------------------------------------------------------------------------

Status BufferDevice::setBuffer(char *pBuffer, xdr_size_t size, bool copy)
{
	if( NULL == pBuffer || 0 == size )
		return XDR_INVALID_PARAMETER;

	if( m_isOwner )
		delete [] m_pBuffer;

	if( ! copy )
	{
		m_pBuffer = pBuffer;
	}
	else
	{
		m_pBuffer = new char[ size ];
		memcpy( m_pBuffer, pBuffer, size );
	}

	m_bufferSize = size;

	xdr_size_t offset = this->isWritable() ? size : 0 ;
	m_pBufferPosition = m_pBuffer + offset;

	return XDR_SUCCESS;
}

//----------------------------------------------------------------------------------------------------

void BufferDevice::setOwner(bool owner)
{
	m_isOwner = owner;
}

//----------------------------------------------------------------------------------------------------

char *BufferDevice::getBuffer()
{
	return m_pBuffer;
}

//----------------------------------------------------------------------------------------------------

const char *BufferDevice::getBuffer() const
{
	return m_pBuffer;
}

//----------------------------------------------------------------------------------------------------

xdr_size_t BufferDevice::getBufferSize() const
{
	return m_bufferSize;
}

//----------------------------------------------------------------------------------------------------

void BufferDevice::setExpandSize(xdr_size_t size)
{
	if( 1024 > size )
		return;

	m_expandSize = size;
}

//----------------------------------------------------------------------------------------------------

xdr_size_t BufferDevice::getExpandSize() const
{
	return m_expandSize;
}

//----------------------------------------------------------------------------------------------------

Status BufferDevice::expandBuffer(xdr_size_t size)
{
	// can't expand buffer is not in write mode !
	if( ! this->isWritable() )
		return XDR_IO_ERROR;

	xdr_size_t expandSize = ( size == 0 ) ? m_expandSize : size ;

	char *pTmpBuffer = m_pBuffer;
	m_pBuffer = new char [ m_bufferSize + expandSize ];

	// copy old buffer
	memcpy( m_pBuffer , pTmpBuffer , m_bufferSize );

	// set additional bytes to 0
	memset( m_pBuffer + m_bufferSize , 0 , expandSize );

	// delete the old buffer
	delete [] pTmpBuffer;
	m_bufferSize = m_bufferSize + expandSize;

	return XDR_SUCCESS;
}

//----------------------------------------------------------------------------------------------------

Status BufferDevice::reset()
{
	if( ! this->isWritable() )
		return XDR_IO_ERROR;

	memset( m_pBuffer , 0 , m_bufferSize );
	m_pBufferPosition = m_pBuffer;

	return XDR_SUCCESS;
}

//----------------------------------------------------------------------------------------------------

bool BufferDevice::isReadable() const
{
	return XDR_TESTBIT( m_openMode , XDR_READ );
}

//----------------------------------------------------------------------------------------------------

bool BufferDevice::isWritable() const
{
	return XDR_TESTBIT( m_openMode , XDR_WRITE );
}

//----------------------------------------------------------------------------------------------------

xdr_size_t BufferDevice::size() const
{
	return this->getBufferSize();
}

//----------------------------------------------------------------------------------------------------

xdr_size_t BufferDevice::getPosition() const
{
	return ( m_pBufferPosition - m_pBuffer );
}

//----------------------------------------------------------------------------------------------------

Status BufferDevice::seek(xdr_size_t position)
{
	if( position >= m_bufferSize )
		return XDR_IO_ERROR;

	m_pBufferPosition = m_pBuffer + position;

	return XDR_SUCCESS;
}

//----------------------------------------------------------------------------------------------------

Status BufferDevice::readData(void *pAddress, xdr_size_t dataSize)
{
	// must be in read mode (or append)
	if( ! this->isReadable() )
		return XDR_IO_ERROR;

	// reached end of buffer ?
	if( this->getBufferSize() - this->getPosition() < dataSize + sizeof(xdr_size_t) )
		return XDR_EOF;

	// read size of next value to read and check consistency with user input
	xdr_size_t size = ( (xdr_size_t) m_pBufferPosition[0] ) & 0xFFFFFF;

	// size 0 means no more data
	if( size != dataSize )
		return XDR_EOF;

	// increment the buffer position
	m_pBufferPosition += sizeof( xdr_size_t );

	// copy data !
	memcpy(pAddress, m_pBufferPosition, size);

	// increment the buffer position
	m_pBufferPosition += size;

	return XDR_SUCCESS;
}

//----------------------------------------------------------------------------------------------------

Status BufferDevice::readCString(char *&pCString, xdr_size_t &strLen)
{
	// must be in read mode (or append)
	if( ! this->isReadable() )
		return XDR_IO_ERROR;

	// reached end of buffer ?
	if( this->getBufferSize() - this->getPosition() < sizeof(xdr_size_t) )
		return XDR_EOF;

	xdr_size_t size = ( (xdr_size_t) m_pBufferPosition[0] ) & 0xFFFFFF;
	strLen = size;

	// check again for EOF
	if( this->getBufferSize() - this->getPosition() < size )
		return XDR_EOF;

	// increment the buffer position
	m_pBufferPosition += sizeof( xdr_size_t );

	// insert NULL character at end of c-string
	pCString = new char [ strLen + 1 ];
	pCString[ strLen ] = '\0';

	if( 0 != strLen )
	{
		// copy data !
		memcpy(pCString, m_pBufferPosition, size);

		// increment the buffer position
		m_pBufferPosition += size;
	}

	return XDR_SUCCESS;
}

//----------------------------------------------------------------------------------------------------

Status BufferDevice::readStaticArray(void *pAddress, xdr_size_t arraySize, xdr_size_t elementSize)
{
	// must be in read mode (or append)
	if( ! this->isReadable() )
		return XDR_IO_ERROR;

	// reached end of buffer ?
	if( this->getBufferSize() - this->getPosition() < 2*sizeof(xdr_size_t) )
		return XDR_EOF;

	// get initial cursor position in case of io error
	xdr_size_t initialPosition = this->getPosition();

	xdr_size_t readArraySize = ( (xdr_size_t) m_pBufferPosition[0] ) & 0xFFFFFF;
	m_pBufferPosition += sizeof( xdr_size_t );

	xdr_size_t readElementSize = ( (xdr_size_t) m_pBufferPosition[0] ) & 0xFFFFFF;
	m_pBufferPosition += sizeof( xdr_size_t );

	if( arraySize != readArraySize || readElementSize != elementSize )
	{
		XDR_STREAM( this->seek( initialPosition ) )
		return XDR_INVALID_PARAMETER;
	}

	xdr_size_t totalSize = readArraySize*readElementSize;

	// check again for EOF
	if( this->getBufferSize() - this->getPosition() < totalSize )
	{
		XDR_STREAM( this->seek( initialPosition ) )
		return XDR_EOF;
	}

	if( 0 != totalSize )
	{
		// ready to read !
		// copy data to user variable
		memcpy(pAddress, m_pBufferPosition, totalSize);

		// increment the buffer position
		m_pBufferPosition += totalSize;
	}

	return XDR_SUCCESS;

}

//----------------------------------------------------------------------------------------------------

Status BufferDevice::readDynamicArray(void *&pAddress, xdr_size_t &arraySize, xdr_size_t elementSize, xdr_allocator allocator)
{
	// must be in read mode (or append)
	if( ! this->isReadable() )
		return XDR_IO_ERROR;

	// reached end of buffer ?
	if( this->getBufferSize() - this->getPosition() < 2*sizeof(xdr_size_t) )
		return XDR_EOF;

	// get initial cursor position in case of io error
	xdr_size_t initialPosition = this->getPosition();

	xdr_size_t readArraySize = ( (xdr_size_t) m_pBufferPosition[0] ) & 0xFFFFFF;
	m_pBufferPosition += sizeof( xdr_size_t );

	xdr_size_t readElementSize = ( (xdr_size_t) m_pBufferPosition[0] ) & 0xFFFFFF;
	m_pBufferPosition += sizeof( xdr_size_t );

	if( readElementSize != elementSize )
	{
		XDR_STREAM( this->seek( initialPosition ) )
		return XDR_INVALID_PARAMETER;
	}

	xdr_size_t totalSize = readArraySize*readElementSize;

	// check again for EOF
	if( this->getBufferSize() - this->getPosition() < totalSize )
	{
		XDR_STREAM( this->seek( initialPosition ) )
		return XDR_EOF;
	}

	// return if nothing more to read
	if( 0 == totalSize )
		return XDR_SUCCESS;

	pAddress = allocator( readArraySize );

	if( ! pAddress )
	{
		XDR_STREAM( this->seek( initialPosition ) )
		return XDR_IO_ERROR;
	}

	// ready to read !
	// copy data to user variable
	memcpy(pAddress, m_pBufferPosition, totalSize);
	arraySize = readArraySize;

	// increment the buffer position
	m_pBufferPosition += totalSize;

	return XDR_SUCCESS;
}

//----------------------------------------------------------------------------------------------------

Status BufferDevice::writeData(const void *pAddress, xdr_size_t dataSize)
{
	// must be in write mode (or append)
	if( ! this->isWritable() )
		return XDR_IO_ERROR;

	// reached end of buffer ?
	if( this->getBufferSize() - this->getPosition() < dataSize + sizeof(xdr_size_t) )
	{
		xdr_size_t reallocationSize = std::max( dataSize + sizeof(xdr_size_t), this->getExpandSize() );
		XDR_STREAM( this->expandBuffer(reallocationSize) )
	}

	// write size and move cursor
	( (xdr_size_t *) m_pBufferPosition )[0] = dataSize;
	m_pBufferPosition += sizeof(xdr_size_t);

	// write data if any and move cursor
	if( 0 != dataSize )
	{
		memcpy( m_pBufferPosition , pAddress , dataSize );
		m_pBufferPosition += dataSize;
	}

	return XDR_SUCCESS;
}

//----------------------------------------------------------------------------------------------------

Status BufferDevice::writeArray(const void *pAddress, xdr_size_t arraySize, xdr_size_t elementSize)
{
	// must be in write mode (or append)
	if( ! this->isWritable() )
		return XDR_IO_ERROR;

	// reached end of buffer ?
	if( this->getBufferSize() - this->getPosition() < arraySize*elementSize + sizeof(xdr_size_t) )
	{
		xdr_size_t reallocationSize = std::max( arraySize*elementSize + sizeof(xdr_size_t), this->getExpandSize() );
		XDR_STREAM( this->expandBuffer(reallocationSize) )
	}

	// write array size and move cursor
	( (xdr_size_t *) m_pBufferPosition )[0] = arraySize;
	m_pBufferPosition += sizeof(xdr_size_t);

	// write element size and move cursor
	( (xdr_size_t *) m_pBufferPosition )[0] = elementSize;
	m_pBufferPosition += sizeof(xdr_size_t);

	// write data if any and move cursor
	if( 0 != arraySize )
	{
		memcpy( m_pBufferPosition , pAddress , arraySize*elementSize );
		m_pBufferPosition += arraySize*elementSize;
	}

	return XDR_SUCCESS;
}

//----------------------------------------------------------------------------------------------------

Status BufferDevice::writeEmptyBytes(xdr_size_t nBytes)
{
	if( 0 == nBytes )
		return XDR_INVALID_PARAMETER;

	// must be in write mode (or append)
	if( ! this->isWritable() )
		return XDR_IO_ERROR;

	// reached end of buffer ?
	if( this->getBufferSize() - this->getPosition() < nBytes )
	{
		xdr_size_t reallocationSize = std::max( nBytes , this->getExpandSize() );
		XDR_STREAM( this->expandBuffer(reallocationSize) )
	}

	memset( m_pBufferPosition , 0 , nBytes );
	m_pBufferPosition += nBytes;

	return XDR_SUCCESS;
}

//----------------------------------------------------------------------------------------------------

Status BufferDevice::recordHeader(StreamingMode mode, Header &recordHeader)
{
	if( XDR_WRITE_STREAM == mode )
	{
		XDR_STREAM( this->write( & recordHeader.m_optionWord ) )
		XDR_STREAM( this->write( & recordHeader.m_headerLength ) )
		XDR_STREAM( this->write( & recordHeader.m_marker ) )
		XDR_STREAM( this->write( & recordHeader.m_length ) )
		XDR_STREAM( this->write( & recordHeader.m_version ) )
		XDR_STREAM( this->write( & recordHeader.m_name ) )
	}
	else if( XDR_READ_STREAM == mode )
	{
		XDR_STREAM( this->read( & recordHeader.m_optionWord ) )
		XDR_STREAM( this->read( & recordHeader.m_headerLength ) )
		XDR_STREAM( this->read( & recordHeader.m_marker ) )
		XDR_STREAM( this->read( & recordHeader.m_length ) )
		XDR_STREAM( this->read( & recordHeader.m_version ) )
		XDR_STREAM( this->read( & recordHeader.m_name ) )
	}

	return XDR_SUCCESS;
}

//----------------------------------------------------------------------------------------------------

Status BufferDevice::blockHeader(StreamingMode mode, Header &blockHeader)
{
	if( XDR_WRITE_STREAM == mode )
	{
		XDR_STREAM( this->write( & blockHeader.m_optionWord ) )
		XDR_STREAM( this->write( & blockHeader.m_headerLength ) )
		XDR_STREAM( this->write( & blockHeader.m_marker ) )
		XDR_STREAM( this->write( & blockHeader.m_length ) )
		XDR_STREAM( this->write( & blockHeader.m_version ) )
		XDR_STREAM( this->write( & blockHeader.m_name ) )
	}
	else if( XDR_READ_STREAM == mode )
	{
		XDR_STREAM( this->read( & blockHeader.m_optionWord ) )
		XDR_STREAM( this->read( & blockHeader.m_headerLength ) )
		XDR_STREAM( this->read( & blockHeader.m_marker ) )
		XDR_STREAM( this->read( & blockHeader.m_length ) )
		XDR_STREAM( this->read( & blockHeader.m_version ) )
		XDR_STREAM( this->read( & blockHeader.m_name ) )
	}

	return XDR_SUCCESS;
}

} 

