/*
 * Optics.cpp
 *
 *  Created on: Mar 22, 2010
 *      Author: sim
 */
#if 1
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include <vector>
#include <ext/pb_ds/priority_queue.hpp>


struct Point {
	float m_x;

	Point( float x ) : m_x(x) {

	}


	float dist(const Point &other) const {
		return fabs(m_x - other.m_x);
	}

};

//struct Point {
//	float m_x;
//	float m_y;
//	Point( float x, float y ) : m_x(x), m_y(y) {
//
//	}
//
//
//	float dist(const Point &other) const {
//		float dx = m_x - other.m_x;
//		float dy = m_y - other.m_y;
//		return sqrt(dx*dx + dy*dy);
//	}
//
//};

struct OsMember {
	int m_obj;
	float m_rDist;

	OsMember( int obj, float rDist ) : m_obj(obj), m_rDist(rDist) {

	}

	bool operator>( const OsMember &other ) const {
		return m_rDist > other.m_rDist;
	}

//	void swap( OsMember &other ) {
////		assert( m_obj == other.m_obj );
//		printf( "swap: %d %d\n", m_obj, other.m_obj);
//		int ot = other.m_obj;
//		float dt = other.m_rDist;
//		other.m_obj = m_obj;
//		other.m_rDist = m_rDist;
//		m_obj = ot;
//		m_rDist = dt;
//
//	}

};

typedef __gnu_pbds::priority_queue<OsMember,std::greater<OsMember>,__gnu_pbds::binomial_heap_tag> my_pq;

class Optics {
public:
	std::vector<Point *> m_objSet;
	//bool *m_processed;
	float *m_reachDist;
	float *m_coreDist;

#define FL_PROC (0x1)
#define FL_RD (0x2)
#define FL_CD (0x4)

	char *m_flags;

	int *m_orderedSet;
	int m_osNext;

	my_pq m_orderSeeds;
	std::vector<my_pq::point_iterator> m_mosPtr;

	void initNaN( float *v, int n ) {
		for( int i = 0; i < n; i++ ) {

			v[i] = 1000000;

		}
	}

	inline float randf() {
		return rand() / float(RAND_MAX);
	}
	Optics() : m_osNext(0) {
		const int N = 1024 * 8;
//		objSet = new Obj[N];

		m_reachDist = new float[N];
		m_coreDist = new float[N];

		m_flags = new char[N];

		m_mosPtr.resize(N);
		memset( m_flags, 0, sizeof( char ) * N );


		m_orderedSet = new int[N];
//		Random rnd = new Random();
//		for( int i = 0; i < N; i++ ) {
//			objSet[i] = new Point( rnd.nextFloat(), rnd.nextFloat() );
//		}

		float p[] = {1,4,6,2,5,7,1.5,3,4.5,6.5,1.7};
		for( int i = 0; i < N; i++ ) {
//			m_objSet.push_back(new Point( p[i] ));
			m_objSet.push_back( new Point(randf()/*, randf()*/) );
		}



		for( int i = 0; i < N; i++ ) {
			printf( "expand: %d %d\n", i, N );
			if( !hasProcessed(i) ) {

				expandClusterOrder( i, 1.1, 1 );
			}
		}
	}


	void getNeighbors( int obj, float eps, std::vector<int> &ret ) {
		ret.clear();
		Point *o = m_objSet[obj];
		for( size_t i = 0; i < m_objSet.size(); i++ ) {
			if( i != size_t(obj) ) {
				if( o->dist(*m_objSet[i]) <= eps ) {
					ret.push_back(i);
				}
			}
		}
	}


	void setCoreDistance(int obj, std::vector<int> &neighbors, float eps,
			int minPts) {

		if( int(neighbors.size()) < minPts ) {
			unsetCoreDist(obj);
		} else {
			Point *o = m_objSet[obj];
			std::vector<float> ndists(neighbors.size());
			for( int i = 0; i < neighbors.size(); i++ ) {
				ndists[i] = o->dist(*m_objSet[neighbors[i]]);
			}

			std::sort(ndists.begin(), ndists.end());
			//Arrays.sort(ndists);

			m_coreDist[obj] = ndists[minPts-1];
			setCoreDist(obj);

		}

		//System.out.printf( "coreDist %d: %f\n", obj, coreDist[obj] );
	}

	inline bool hasCoreDist( int i ) {
		return (m_flags[i] & FL_CD) != 0;
	}

	inline bool hasReachDist( int i ) {
		return (m_flags[i] & FL_RD) != 0;
	}
	inline bool hasProcessed( int i ) {
		return (m_flags[i] & FL_PROC) != 0;
	}

	inline void setCoreDist( int i ) {
		m_flags[i] |= FL_CD;
	}
	inline void unsetCoreDist( int i ) {
		m_flags[i] &= ~FL_CD;
	}

	inline void setReachDist( int i ) {
		m_flags[i] |= FL_RD;
	}
	inline void unsetReachDist( int i ) {
		m_flags[i] &= ~FL_RD;
	}

	inline void setProcessed( int i ) {
		m_flags[i] |= FL_PROC;
	}
	void orderSeedsUpdate( std::vector<int> &neighbors, int centerObj) {
		float cDist = m_coreDist[centerObj];
		assert( hasCoreDist(centerObj));

		Point *co = m_objSet[centerObj];

		for( std::vector<int>::iterator it = neighbors.begin(); it < neighbors.end(); ++it ) {
			int obj = *it;
			//printf( "obj: %d\n", obj );
			if( !hasProcessed(obj) ) {
				Point *o = m_objSet[obj];

				float newRDist = std::max( cDist, co->dist(*o));

				if( !hasReachDist(obj) ) {
					m_reachDist[obj] = newRDist;
					setReachDist(obj);
					//OsMember osm = new OsMember(obj, newRDist);
					m_mosPtr[obj] = m_orderSeeds.push(OsMember(obj, newRDist));
				} else {
					//System.out.printf( "new r dist: %f %f\n", newRDist, reachDist[obj] );


					if( newRDist < m_reachDist[obj] ) {
						m_reachDist[obj] = newRDist;
						m_orderSeeds.modify(m_mosPtr[obj], OsMember(obj, newRDist));
					}
				}
			}
		}
	}


	void expandClusterOrder(int obj, float eps, int minPts ) {
		std::vector<int> neighbors(minPts);
		getNeighbors( obj, eps, neighbors );
		setProcessed(obj);

		unsetReachDist(obj);

		setCoreDistance( obj, neighbors, eps, minPts );
		m_orderedSet[m_osNext] = obj;
		m_osNext++;

		if( hasCoreDist(obj) ) {
			orderSeedsUpdate( neighbors, obj );

			while( !m_orderSeeds.empty() ) {
				OsMember cosm = m_orderSeeds.top();
				m_orderSeeds.pop();

				int currentObj = cosm.m_obj;

				getNeighbors(currentObj, eps, neighbors);
				setProcessed(currentObj);
				setCoreDistance(currentObj, neighbors, eps, minPts);

				m_orderedSet[m_osNext] = currentObj;
				m_osNext++;

				if( hasCoreDist(currentObj)) {
					orderSeedsUpdate(neighbors, currentObj);
				}

			}
		}
	}





};


int main( int argc, char *argv[] ) {
	printf( "optics\n");
	Optics optics;

	for( int i = 0; i < optics.m_osNext; i++ ) {
		printf( "os: %d %f\n", optics.m_orderedSet[i], optics.m_reachDist[i] );
	}
	return 0;

}

#endif
