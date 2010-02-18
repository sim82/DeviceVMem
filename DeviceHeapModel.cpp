/*
 * DeviceHeapModel.cpp
 *
 *  Created on: Feb 10, 2010
 *      Author: sim
 */

#include <stdio.h>

#include "DeviceHeapModel.h"

using namespace vmem;
//#define LRU_VERBOSE

DeviceHeapModel::DeviceHeapModel( int numPages ) :
 m_pageFree(numPages),
 m_pageDirty(numPages),
 m_lrux(numPages, m_lrul.end())
{
	// TODO Auto-generated constructor stub
	m_pageFree.set();
//	m_lru2 = new int[numPages];
}

DeviceHeapModel::~DeviceHeapModel() {
	// TODO Auto-generated destructor stub
}

void vmem::DeviceHeapModel::apply(std::vector<MemOp>  &mops)
{

	for( std::vector<MemOp>::iterator mopi = mops.begin(); mopi != mops.end(); mopi++ ) {
		MemOp mop = (*mopi);
		//printf( "mop: %d %d\n", mop.m_flags, int(mop.m_page));

		assert( bool(mop.m_flags & MemOp::FL_R) != bool(mop.m_flags & MemOp::FL_W));

		if( mop.m_flags & MemOp::FL_R ) {
			if( m_pageFree[mop.m_page] ) {
				// check for read on uninitialized page -> this should be
				// considered an error
				printf( "ERROR: uninitialized read: %d\n", int(mop.m_page));
			} else {
				if( m_pageDirty[mop.m_page] ) {
					printf( "device dirty read: %d\n", int( mop.m_page ));
 				}


#ifdef LRU_VERBOSE
				printf( "push back in lru list (r): %d\n", int(mop.m_page));
#endif
				// push back accessed page in lru list
				m_lrul.erase(m_lrux[mop.m_page]);
				m_lrul.push_back(mop.m_page);
				m_lrux[mop.m_page] = --m_lrul.end();

			}


		} else if( mop.m_flags & MemOp::FL_W ) {

			// set page to non-free state
			m_pageFree.set(mop.m_page, false);

			// set page dirty state
			m_pageDirty.set(mop.m_page);

			if( m_lrux[mop.m_page] != m_lrul.end() ) {
				m_lrul.erase(m_lrux[mop.m_page]);
#ifdef LRU_VERBOSE
				printf( "push back in lru list (w): %d\n", int(mop.m_page));
#endif
			} else {
#ifdef LRU_VERBOSE
				printf( "introduce to lru list: %d\n", int(mop.m_page));
#endif
			}
			m_lrul.push_back(mop.m_page);
			m_lrux[mop.m_page] = --m_lrul.end();

		}
	}
}


size_t vmem::DeviceHeapModel::getCandidate()
{
	size_t ff = m_pageFree.find_first();
	if( ff < m_pageFree.npos ) {
#ifdef LRU_VERBOSE
		printf( "get candidate: return free page: %d\n", int(ff) );
#endif
		return ff;
	}

	ff = m_lrul.front();
#ifdef LRU_VERBOSE
	printf( "get candidate: return lru page: %d\n", int(ff) );
#endif

	return ff;
}


int main() {
	std::vector<MemOp> mops;

	mops.push_back(MemOp(true, 1));
	mops.push_back(MemOp(true, 0));


	mops.push_back(MemOp(false, 0));
	mops.push_back(MemOp(false, 1));
	mops.push_back(MemOp(true, 2));

	DeviceHeapModel dhm(10 * 1024);
	dhm.apply(mops);
//	size_t c = dhm.getCandidate();

//	printf( "get candidate: %d\n", c);

	for( int i = 0; i < 1000 * 1000; i++ ) {
		size_t c = dhm.getCandidate();
//		printf( "get candidate2: %d\n", c);
		mops.clear();
		mops.push_back(MemOp( true, c ));
		dhm.apply(mops);
	}
	printf( "done\n" );
}





