/*
 * DeviceHeapModel.h
 *
 *  Created on: Feb 10, 2010
 *      Author: sim
 */

#ifndef DEVICEHEAPMODEL_H_
#define DEVICEHEAPMODEL_H_

#include <vector>
#include <boost/dynamic_bitset.hpp>
#include <boost/array.hpp>
#include <stdint.h>
#include <list>

namespace vmem {

struct MemOp {
	const static uint8_t	FL_R = 0x1;
	const static uint8_t	FL_W = 0x2;

	uint8_t	m_flags;
	size_t	m_page;

	MemOp() {
		m_flags = 0;
		m_page = 0;
	}

	MemOp( bool wr, size_t page ) {
		if( wr ) {
			m_flags = FL_W;
		} else {
			m_flags = FL_R;
		}

		m_page = page;
	}
};

class DeviceHeapModel {
	boost::dynamic_bitset<>	m_pageFree;
	boost::dynamic_bitset<>	m_pageDirty;
//	std::vector<int> m_lru;

//	int *m_lru2;

//	uint64_t	m_serial;

	std::list<int> m_lrul;
	std::vector<std::list<int>::iterator> m_lrux;

public:
	DeviceHeapModel( int numPages );
	virtual ~DeviceHeapModel();

//	template <int N>
//	void apply( boost::array<MemOp,N> mops ) {
//
//	}

	void apply( std::vector<MemOp> &mops );

	size_t getCandidate();
};
}

#endif /* DEVICEHEAPMODEL_H_ */
