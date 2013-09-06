#pragma once
#ifndef _CACHEMANAGER_H_
#define _CACHEMANAGER_H_

// stl
#include <string>
#include <map>
// boost
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/bind.hpp>

namespace LAE {

template<typename T>
class CacheManager {
public:
	class Initializer {
	public:
		Initializer( const std::string& prefix, const std::string& pathPrefix ) {
			CacheManager<T>::init();
		}
	};

	static void init() {
		if( instance == 0 ) {
			instance = new CacheManager();
		}
	}

	static CacheManager* i() {
		return instance;
	}

	boost::shared_ptr<const T> createObject( const std::string& name ) {
		auto it = cacheMap_.find(name);
		if( it == cacheMap_.end() ) {
			T* t = new T();
			boost::shared_ptr< const T > p(t, &CacheManager<T>::removeObject );
			t->load( name );
			cacheMap_.insert( std::make_pair(name, p) );
			return p;
		} else {
			return boost::shared_ptr< const T >( it->second );
		}
	}

	void clearAll() {
		cacheMap_.clear();
	}

private:
	CacheManager() {}
	~CacheManager() {}

	CacheManager(const CacheManager&);
	CacheManager& operator=(const CacheManager&);

	typedef std::map<std::string, boost::weak_ptr< const T > > HMap_t;
	
	static CacheManager<T>* instance;

	HMap_t cacheMap_;
	static void removeObject( const T* object ) {
		instance->cacheMap_.erase( object->getFileName() );
		delete object;
	}
};

}

#endif // _CACHEMANAGER_H_
