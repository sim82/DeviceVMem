/*
 * Heap.cpp
 *
 *  Created on: Feb 9, 2010
 *      Author: sim
 */

#include <stdio.h>

#include "Heap.h"

using namespace vmem;

// FIXME: it's a bit stupid to use a 32bit constant here. replace this with the actual
// max-value of size_t
#define PAGE_TERM (0xFFFFFFFF)

Heap::Heap(size_t pagesize, size_t n_pages) :
	m_pagesize(pagesize),
	m_numpages(n_pages),
	m_bufsize(m_pagesize * m_numpages),
	m_nextfree(0)
{
	assert( m_pagesize >= sizeof(size_t));

	m_buf = new u1_t[m_bufsize];

	initFreelist();
}

void vmem::Heap::initFreelist()
{
	for( size_t i = 0; i < m_numpages; i++ ) {
		u1_t *p = getAddrByPage(i);
		size_t *ps = (size_t*)p;

		if( i < m_numpages - 1 ) {
			*ps = i+1;
		} else {
			*ps = PAGE_TERM;
		}
	}
}

Heap::~Heap() {
}



void vmem::Heap::freePage(size_t p)
{
	size_t *ps = (size_t*)getAddrByPage(p);
	*ps = m_nextfree;
	m_nextfree = p;
}



size_t vmem::Heap::allocPage()
{
	assert( m_nextfree != PAGE_TERM ); // we ran out of pages...
//	if( m_nextfree == PAGE_TERM ) {
//		printf( "out of pages\n" );
//	}

	size_t p = m_nextfree;
	size_t *ps = (size_t*)getAddrByPage(m_nextfree);
	m_nextfree = *ps;

	return p;
}

int main( int argc, char *argv[] ) {
	Heap h(1024, 100);

	for( int i = 0; i < 100; i++ ) {
		printf( "alloc %d:\n", i );

		size_t p = h.allocPage();
		u1_t *a = h.getAddrByPage(p);
		printf( "mem: %d %p\n", int(p), a );
	}

	for( int i = 20; i < 70; i++ ) {
		printf( "free %d:\n", i );

		h.freePage(h.getPageByAddr(h.getAddrByPage(i) + 10 ));
	}
	for( int i = 0; i < 50; i++ ) {
		printf( "alloc %d:\n", i );

		size_t p = h.allocPage();
		u1_t *a = h.getAddrByPage(p);
		printf( "mem: %d %p\n", int(p), a );
	}

}
