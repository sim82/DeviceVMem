/*
 * DeviceHeapModel.cpp
 *
 *  Created on: Feb 10, 2010
 *      Author: sim
 */

#include <stdio.h>
#include <ext/pb_ds/priority_queue.hpp>
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

struct OsMember {
	int m_obj;
	float m_rDist;

	OsMember( int obj, float rDist ) : m_obj(obj), m_rDist(rDist) {

	}

	bool operator>( const OsMember &other ) const {
		return m_rDist > other.m_rDist;
	}

	void swap( OsMember &other ) {
//		assert( m_obj == other.m_obj );
		printf( "swap: %d %d\n", m_obj, other.m_obj);
		int ot = other.m_obj;
		float dt = other.m_rDist;
		other.m_obj = m_obj;
		other.m_rDist = m_rDist;
		m_obj = ot;
		m_rDist = dt;

	}

};

typedef __gnu_pbds::priority_queue<OsMember,std::greater<OsMember>,__gnu_pbds::binomial_heap_tag> my_pq;
typedef __gnu_pbds::priority_queue<int,std::greater<int>,__gnu_pbds::binomial_heap_tag> my_int_pq;
int maindhm() {
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
	printf( "done. pq2\n" );


#if 1
	my_pq pq;

	pq.push(OsMember(1, 2.0));
	pq.push(OsMember(2, 3.0));
	my_pq::point_iterator it = pq.push(OsMember(3, 4.0));
	pq.push(OsMember(4, 5.0));

	pq.modify(it, OsMember(3,1.0));
	printf( "sizeof: %d\n", int(sizeof( my_pq::point_iterator)));
	while( !pq.empty() ) {
		const OsMember &c = pq.top();

		printf( "%d %f\n", c.m_obj, c.m_rDist );
		pq.pop();
	}

	std::vector<int> v1;

	v1.push_back(4);
	v1.push_back(2);
	v1.push_back(1);
	v1.push_back(5);
	v1.push_back(3);
	std::sort( v1.begin(), v1.end());
	for( int i = 0; i < v1.size(); i++ ) {
		printf( "s: %d\n", v1[i]);
	}



#else
	my_int_pq pq;

	pq.push(2);
	pq.push(1);
	my_int_pq::point_iterator it = pq.push(3);
	pq.push(5);

	pq.modify(it, 7);

	while( !pq.empty() ) {
		const int &c = pq.top();

		printf( "%d %f\n", c );
		pq.pop();
	}
#endif

}





