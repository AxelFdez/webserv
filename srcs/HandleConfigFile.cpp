/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HandleConfigFile.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: axfernan <axfernan@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/30 14:21:31 by chris             #+#    #+#             */
/*   Updated: 2023/11/29 11:02:35 by axfernan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/HandleConfigFile.hpp"

HandleConfigFile::HandleConfigFile( char* file, std::string cgi_path ) : getConfigFile( file, cgi_path ) {

    get_ports_bodySize();
    getLocations();


    // print_config();
    // print_ports_bodySize();
    // print_locations();
}

HandleConfigFile::~HandleConfigFile() {}

// ******** TOOLS **************************************************************************

void HandleConfigFile::getLocations() {

    std::vector<std::string> locationsTmp;

    for (size_t i = 0; i != config.size(); i++ ) {

        std::vector<std::string>::const_iterator it = config[i].begin();
        _locations.push_back(std::vector<std::map<std::string, std::vector<std::string> > >());
        size_t iLocation = 0;
        for ( ; it != config[i].end(); it++ ) {

            if ( (*it).find("location", 0, 8 ) == 0 ) {

                _locations[i].push_back( std::map<std::string, std::vector<std::string> >() );
                while ( *it != "}" ) {

                    std::string key = getKey(*it);
                    std::vector<std::string> values = splitStringToVector( getValue(*it) );

                    if ( key == "location" ) {

                        locationsTmp.push_back( values[0] );
                        if ( values[0].size() > 1 && values[0].at(values[0].size() -1) == '/' ) {
                            values[0].resize(values[0].size() -1);
                        }
                    }
                    _locations[i][iLocation].insert( std::pair<std::string, std::vector<std::string> >( key, values ));
                    it++;
                }
                iLocation++;
            }
        }
        checkDuplicateLocation( locationsTmp );
    }
}

void HandleConfigFile::checkDuplicateLocation( std::vector<std::string> & vecLoc ) {

    if ( !vecLoc.empty() ) {

        for (size_t i = 0; i < vecLoc.size(); ++i) {

            for (size_t j = i + 1; j < vecLoc.size(); ++j) {

                if (vecLoc[i] == vecLoc[j]) {
                    throw std::invalid_argument( RED "Error: config file: duplicate location" RESET );
                }
            }
        }
        vecLoc.clear();
    }
}

void HandleConfigFile::get_ports_bodySize() {
    /* _ports is a vector of vector<int>, each vector contains the ports of the server...
        ex: _ports[0] contains all the ports of server[] 1
            _ports[1] contains all the ports of server[] 2

        _body_size_max is a vector that contain de body_size_max of each server...
        ex: _body_size_max[0] contain the _body_size_max value of server[] 1
            _body_size_max[1] contain the _body_size_max value of server[] 2
    */

    //std::vector<std::vector<std::string> >::iterator it = config.begin();
    for ( size_t i = 0; i != config.size(); i++ ) {

        _ports.push_back( std::vector<int>() );
        std::vector<std::string>::iterator it = config[i].begin();
        for ( ; it != config[i].end(); it++ ) {

            if ( (*it).find("listen ", 0, 6 ) == 0 ) {

                std::string value = getValue( *it );
                std::istringstream iss( value );
                int number;
                if (iss >> number ) {
                    _ports[i].push_back( number );
                }
            }
            if ( (*it).find( "body_size_max ", 0, 14 ) == 0 ) {

                std::string value = getValue( *it );
                std::istringstream iss( value );
                int number;
                if (iss >> number ) {
                    _body_size_max.push_back( number +1 );
                }
            }
        }
    }
}

std::string HandleConfigFile::getKey( std::string str ) {

    if ( !str.empty() ) {
        if ( str.find(' ') != std::string::npos ) {
            str.resize(str.find(' ') );
        }
    }
    return str;
}

std::vector<std::string> HandleConfigFile::splitStringToVector( std::string str ) {

    std::vector<std::string> vec;

    for ( size_t i = 0; i < str.size(); i++ ) {

        if ( isspace(str[i]) ) {
            std::string tmp = str.substr(0, i);
            vec.push_back( tmp );
            while ( isspace(str[i]) ) { i++; }
            str = str.substr(i);
            i = 0;
        }
    }
    vec.push_back( str );
    return vec;
}

// ******** PRINT **************************************************************************

void HandleConfigFile::print_config() const{

    // std::vector<std::vector<std::string> >::const_iterator it = config.begin();
    for ( size_t i = 0; i != config.size(); i++ ) {

        std::vector<std::string>::const_iterator it = config[i].begin();
        for ( ; it != config[i].end(); it++ ) {
            std::cout << "config [" << i << "] -> " << *it << std::endl;
        }
        std::cout << std::endl;
    }
}

void      HandleConfigFile::print_locations() const{

    for ( size_t i = 0; i != _locations.size(); i++ ) {

        for ( size_t j = 0; j != _locations[i].size(); j++ ) {

            std::cout << B_GREEN <<"server[" << i << "]" << "location[" << j << "]" << RESET << std::endl;

            std::map<std::string, std::vector<std::string> >::const_iterator it = _locations[i][j].begin();
            for ( ; it != _locations[i][j].end(); it++ ) {
                std::vector<std::string>::const_iterator itVec = it->second.begin();
                std::cout << it->first << std::endl;

                for( ; itVec != it->second.end(); itVec++ ) {
                    std::cout << B_GRAY << *itVec << RESET << std::endl;
                }
            }
        }
    }
}

void      HandleConfigFile::print_ports_bodySize() const{

    for ( size_t i = 0; i != _ports.size(); i++ ) {
        for ( size_t j = 0; j != _ports[i].size(); j++ ) {
            std::cout << "server " << i+1 << " ports = " << _ports[i][j] << std::endl;
        }
        std::cout << std::endl;
    }
    for ( size_t i = 0; i != _body_size_max.size(); i++ ) {

        std::cout << "server " << i+1<< " bodySize = " << _body_size_max[i] << std::endl;
    }
}

// ******** GETTERS ************************************************************************

std::vector<std::vector<int> >   HandleConfigFile::getPorts() const{

    return _ports;
}

long long  HandleConfigFile::getBodySizeMax(size_t serverNb) const{

    if ( serverNb < _body_size_max.size() ) {
        return _body_size_max[serverNb];
    }
    return -1;
}

std::string HandleConfigFile::getServerValues(size_t serverNb, std::string const& toFind) {

    std::vector<std::string>::iterator it = config[serverNb].begin();
    for ( ; it != config[serverNb].end(); it++ ) {

        if ( (*it).find("location", 0, 8 ) == 0 ) {
            while ( *it != "}" ) {
                it++;
            }
        }
        if ( (*it).find(toFind, 0) == 0 ) {
            return getValue(*it);
        }
    }
    return "";
}

std::map<std::string, std::vector<std::string> > *HandleConfigFile::findLocation(size_t serverNb, std::string const& findLoc) {

    // if location 'findLoc' is found -> return map's address
    // else -> return the default location '/'
    // if there is no default location -> return NULL

    std::map<std::string, std::vector<std::string> > *mapReturn = NULL;
    if ( serverNb < _locations.size() ) {

        for ( size_t i = 0; i < _locations[serverNb].size(); i++ ) {

            std::map<std::string, std::vector<std::string> >::const_iterator it = _locations[serverNb][i].find("location");
            if ( it != _locations[serverNb][i].end() ) {

                if ( it->second[0] == "/" ) { // default location
                    mapReturn = &_locations[serverNb][i];
                    continue;
                }
                if ( it->second[0].find( findLoc.c_str(), 0, it->second[0].length() ) == 0 ) {

                    mapReturn = &_locations[serverNb][i];
                    break;
                }
            }
        }
    }
    return mapReturn;
}

std::vector<std::string> & HandleConfigFile::getLocationValues(size_t serverNb, std::string const& path, std::string const& toFind) {

    static std::vector<std::string> emptyVector;
    std::map<std::string, std::vector<std::string> > *location = findLocation(serverNb, path);
    if ( location ) {

        std::map<std::string, std::vector<std::string> >::iterator it = (*location).find( toFind );
        if ( it != (*location).end() ) {

            return it->second;
        }
    }
    return emptyVector;
}

std::string const HandleConfigFile::getErrorPage( size_t serverNb, int errorCode ) {

    std::string errorPath;
    if ( serverNb < config.size() ) {
        std::string errorNb = to_string( errorCode );

        for ( size_t i = 0; i < config[serverNb].size(); i++ ) {

            if ( getKey(config[serverNb][i]).find( errorNb ) != std::string::npos ) {
                errorPath = getValue(config[serverNb][i]);
            }
        }
    }
    return errorPath;
}