  /// \file PointerMapper.h
/*
 *
 * PointerMapper.h header template automatically generated by a class generator
 * Creation date : lun. janv. 18 2016
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


#ifndef POINTERMAPPER_H
#define POINTERMAPPER_H

#include "xdrstream/XdrDefinitions.h"

namespace xdrstream
{

class IODevice;
class Record;

/** 
 * @brief PointerMapper class
 */ 
class PointerMapper 
{
private:
	/**
	 * @brief Constructor
	 */
	PointerMapper();

	/**
	 * @brief Destructor
	 */
	~PointerMapper();

	/**
	 *  @brief  Add a pointer tag to the mapping
	 *
	 *  @param  pOldAddress the old address of the object
	 *  @param  pNewAddress the new address of the object
	 */
	Status addPointerTag(void *pOldAddress, void *pNewAddress);

	/**
	 *  @brief  Add a pointer reference to the mapping
	 *
	 *  @param  pOldAddress the old address of the object
	 *  @param  pDestinationAddress the destination address where the pointer is referenced
	 */
	Status addPointerReference(void *pOldAddress, void **pTargetAddress);

	/**
	 *  @brief  Perform the pointer mapping.
	 *          Replace the destination addresses (references) by the pointer tags
	 *          Note : The maps are cleared after this operation
	 */
	Status performPointerMapping();

	/**
	 *  @brief  Clear the pointer mapping
	 */
	void clearMapping();

private:
	typedef std::map<void *, void *>         PointerTagMap;
	typedef std::multimap<void *, void **>   PointerReferenceMap;

	PointerTagMap                  m_pointerTagMap;
	PointerReferenceMap            m_pointerReferenceMap;

	friend class IODevice;   // to add pointers and pointer tags
}; 

} 

#endif  //  POINTERMAPPER_H
