//#pragma once
//
//#include <string>
//#include <map>
//
//#include "../Logger.h"
//
//namespace LAE {
//template<typename T1>
//class Cache {
//public:
//	static void clear( bool clearPersist = false ) {
//		typename HMap_t::iterator tmp;
//		for( typename HMap_t::iterator i = cacheMap.begin(); i != cacheMap.end(); ) {
//			if( ( *i ).second->isPersist() && !clearPersist ) {
//				++i;
//				continue;
//			}
//			//delete (*i).second;
//			tmp = i;
//			++tmp;
//			cacheMap.erase( i );
//			i = tmp;
//		}
//	}
//
//	static const T1* getObject( const std::string& name, bool persist = false ) {
//		auto it = cacheMap.find( name );
//		if( it == cacheMap.end() ) {
//			std::string fname = pathPrefix;
//			fname.append( name );
//
//			//LOG_DEBUG(prefix << " '" << fname << "' not in cache, loading..." << std::endl;
//
//			//T1 *obj = new T1(persist);
//			TSHPointer obj = TSHPointer( new T1( persist ) );
//			obj->load( fname );
//			cacheMap[name] = obj;
//			return obj.get();
//		} else {
//			return it->second.get();
//		}
//	}
//
//	static void freeObject( const std::string& name ) {
//		typename HMap_t::iterator i = cacheMap.find( name );
//		if( i != cacheMap.end() ) {
//			if( ( *i ).second->isPersist() ) {
//				return;
//			}
//
//			//LOG_DEBUG(prefix << " '" << name << "' removed from cache" << std::endl;
//
//			//delete cacheMap[name];
//			cacheMap.erase( i );
//		}
//	}
//
//private:
//	typedef boost::shared_ptr<T1>	TSHPointer;
//	typedef std::map<std::string, TSHPointer>	HMap_t;
//
//	static HMap_t cacheMap;
//
//	static std::string prefix;
//	static std::string pathPrefix;
//};
//}
