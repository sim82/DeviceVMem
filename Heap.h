/*
 * Heap.h
 *
 *  Created on: Feb 9, 2010
 *      Author: sim
 */

#ifndef HEAP_H_
#define HEAP_H_

#include <stdint.h>
#include <cstring>
#include <sys/types.h>
#include <assert.h>

typedef uint8_t u1_t;

namespace vmem {

class Heap {
	size_t	m_pagesize;
	size_t 	m_numpages;
	size_t	m_bufsize;

	u1_t *m_buf;

	size_t	m_nextfree;

	void initFreelist();
public:
	Heap( size_t pagesize, size_t n_pages );
	virtual ~Heap();

	inline u1_t* getAddrByPage( size_t p ) const {
		return &m_buf[p * m_pagesize];
	}

	inline size_t getPageByAddr( u1_t *paddr ) {
		off_t off = paddr - m_buf;

		assert( off >= 0 );

		size_t p = off / m_pagesize;

		assert( p < m_numpages );
		return p;
	}

	size_t allocPage();
	void freePage( size_t p );




};
}


#endif /* HEAP_H_ */
