/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HandleConfigFile.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: axfernan <axfernan@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/30 14:21:31 by chris             #+#    #+#             */
/*   Updated: 2023/11/14 10:29:58 by axfernan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/HandleConfigFile.hpp"

// HandleConfigFile::HandleConfigFile() {}



HandleConfigFile::HandleConfigFile( char* file ) : getConfigFile( file ) {

    get_ports_bodySize();
    getLocations();



    // print_config();
    // print_ports_bodySize();
    print_locations();


}

void HandleConfigFile::getLocations() {

    for ( int i = 0; i != config.size(); i++ ) {

        std::vector<std::string>::const_iterator it = config[i].begin();
        _locations.push_back(std::vector<std::map<std::string, std::vector<std::string> > >());
        size_t iLocation = 0;
        for ( ; it != config[i].end(); it++ ) {

            if ( (*it).find("location", 0, 8 ) == 0 ) {

                _locations[i].push_back( std::map<std::string, std::vector<std::string> >() );
                while ( *it != "}" ) {
                    _locations[i][iLocation].insert( std::pair<std::string, std::vector<std::string> >( getKey( *it ), splitStringToVector( getValue(*it) )) );
                    it++;
                }
                iLocation++;
            }
        }
    }
}



HandleConfigFile::~HandleConfigFile() {}


void HandleConfigFile::get_ports_bodySize() {
    /* _ports is a vector of vector<int>, each vector contains the ports of the server...
        ex: _ports[0] contains all the ports of server[] 1
            _ports[1] contains all the ports of server[] 2

        _body_size_max is a vector that contain de body_size_max of each server...
        ex: _body_size_max[0] contain the _body_size_max value of server[] 1
            _body_size_max[1] contain the _body_size_max value of server[] 2
    */

    std::vector<std::vector<std::string> >::iterator it = config.begin();
    for ( int i = 0; i != config.size(); i++ ) {

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
            if ( (*it).find("body_size_max ", 0, 14 ) == 0 ) {

                std::string value = getValue( *it );
                std::istringstream iss( value );
                int number;
                if (iss >> number ) {
                    _body_size_max.push_back( number );
                }
            }
        }
    }
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
std::vector<std::string> HandleConfigFile::getLocationValues(size_t serverNb, std::string const& request, std::string const& toFind) const{

    std::vector<std::string> result;

    if ( serverNb < _locations.size() ) {

        for ( int i = 0; i < _locations[serverNb].size(); i++ ) {

            std::map<std::string, std::vector<std::string> >::const_iterator it = _locations[serverNb][i].find("location");
            if ( it != _locations[serverNb][i].end() ) {

                if ( std::find( it->second.begin(), it->second.end(), request ) != it->second.end() ) {

                    it = _locations[serverNb][i].find(toFind);
                    if ( it != _locations[serverNb][i].end() ) {
                        std::cout << serverNb << " " << i << std::endl;
                        result = it->second;
                        break;
                    }
                }
            }
        }
    }
    return result;
}

// ******** TOOLS **************************************************************************

std::string HandleConfigFile::getKey( std::string str ) {

    if ( !str.empty() ) {
        str.resize(str.find(' ') );
    }
    return str;
}

// ******** PRINT **************************************************************************
void HandleConfigFile::print_config() const{

    // std::vector<std::vector<std::string> >::const_iterator it = config.begin();
    for ( int i = 0; i != config.size(); i++ ) {

        std::vector<std::string>::const_iterator it = config[i].begin();
        for ( ; it != config[i].end(); it++ ) {
            std::cout << "config [" << i << "] -> " << *it << std::endl;
        }
        std::cout << std::endl;
    }
}

void      HandleConfigFile::print_locations() const{

    for ( int i = 0; i != _locations.size(); i++ ) {

        for ( int j = 0; j != _locations[i].size(); j++ ) {

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

    for ( int i = 0; i != _ports.size(); i++ ) {
        for ( int j = 0; j != _ports[i].size(); j++ ) {
            std::cout << "server " << i+1 << " ports = " << _ports[i][j] << std::endl;
        }
        std::cout << std::endl;
    }
    for ( int i = 0; i != _body_size_max.size(); i++ ) {

        std::cout << "server " << i+1<< " bodySize = " << _body_size_max[i] << std::endl;
    }
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