/*
 * NetTest.cpp
 *
 *  Created on: Feb 9, 2010
 *      Author: sim
 */

#include "NetTest.h"
#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::udp;
using namespace boost::asio::ip;

using namespace vm;

NetTest::NetTest() {
	// TODO Auto-generated constructor stub

}

NetTest::~NetTest() {
	// TODO Auto-generated destructor stub
}

void NetTest::tx()
{
	boost::asio::io_service io_service;
	udp::endpoint udp_ep;

	boost::array<uint8_t,4> addr = { {127,0,0,1} };
	//udp_ep.address( address_v4( boost::array<unsigned char,u4_t>(addr) );
	udp_ep.address( address_v4( addr ));
	udp_ep.port(21845);

	udp::socket sock(io_service);

	sock.open(udp::v4());
	boost::array<char,1472> send_buf= {{1,2,3,4,5,6,7,8}};
	while( true ) {
		sock.send_to( boost::asio::buffer(send_buf), udp_ep );
	}
}

void NetTest::rx() {
	boost::asio::io_service io_service;
	udp::socket socket(io_service, udp::endpoint(udp::v4(),21845));



	boost::array<uint8_t,1500> rx_buf;

	int i = 0;
	while( true ) {
		size_t len = socket.receive(boost::asio::buffer(rx_buf));
		i++;
		if( i % 100000 == 0 ) {
			printf( "rx: %d %d\n", i, int(len) );
		}
	}


}
//int main(int argc, char *argv[] ) {
//	NetTest nt;
//
//	if( argc == 1 ) {
//		nt.rx();
//	} else {
//		nt.tx();
//	}
//
//}
