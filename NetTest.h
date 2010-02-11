/*
 * NetTest.h
 *
 *  Created on: Feb 9, 2010
 *      Author: sim
 */

#ifndef NETTEST_H_
#define NETTEST_H_
namespace vm {


class NetTest {
public:
	NetTest();
	virtual ~NetTest();
	void tx();
	void rx();
};
}

#endif /* NETTEST_H_ */
