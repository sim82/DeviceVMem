/*
 * DeviceHeapModel.cpp
 *
 *  Created on: Feb 10, 2010
 *      Author: sim
 */

#include <stdio.h>

#include "DeviceHeapModel.h"

using namespace vmem;

DeviceHeapModel::DeviceHeapModel( int numPages ) :
 m_pageFree(numPages),
 m_pageDirty(numPages),
 m_lru(numPages),
 m_serial(0)


{
	// TODO Auto-generated constructor stub
	m_pageFree.set();
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
				printf( "ERROR: uninitialized read: %d\n", int(mop.m_page));
			} else {
				if( m_pageDirty[mop.m_page] ) {
					printf( "device dirty read: %d\n", int( mop.m_page ));
 				}

				m_lru[mop.m_page] = m_serial;
			}


		} else if( mop.m_flags & MemOp::FL_W ) {
			m_pageFree.set(mop.m_page, false);
			m_pageDirty.set(mop.m_page);
			m_lru[mop.m_page] = m_serial;
		}

		m_serial++;
	}
}


size_t vmem::DeviceHeapModel::getCandidate()
{
	size_t ff = m_pageFree.find_first();
	if( ff < m_pageFree.npos ) {
		return ff;
	}

	size_t low_idx = 0;
	int	low_cnt = INT_MAX;
	for( std::vector<int>::iterator i = m_lru.begin(); i != m_lru.end(); ++i ) {
		if( *i < low_cnt ) {
			low_idx = i - m_lru.begin();
			low_cnt = *i;
		}
	}
	assert( low_cnt >= 0 );
	return low_idx;


}


int main() {
	std::vector<MemOp> mops;
	mops.push_back(MemOp(true, 0));
	mops.push_back(MemOp(true, 1));

	mops.push_back(MemOp(false, 0));
	mops.push_back(MemOp(false, 1));
	mops.push_back(MemOp(true, 2));

	DeviceHeapModel dhm(10 * 1024);
	dhm.apply(mops);

	for( int i = 0; i < 100000; i++ ) {
		size_t c = dhm.getCandidate();

		mops.clear();
		mops.push_back(MemOp( true, c ));
		dhm.apply(mops);
	}
	printf( "done\n" );
}





