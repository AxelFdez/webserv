/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   getConfigFile.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chris <chris@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/03 15:10:41 by chris             #+#    #+#             */
/*   Updated: 2023/11/29 15:00:52 by chris            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/getConfigFile.hpp"

getConfigFile::getConfigFile() {}
getConfigFile::~getConfigFile() {}

getConfigFile::getConfigFile( char* file, std::string cgi_path ) {
    _cgi_path = cgi_path;
    _serverToken = false;
    _locationToken = false;
    _vectorIndice = -1;

    std::ifstream   argFile( file );
    std::string     line;
    size_t          lineNumber = 1;

    if ( argFile.is_open() ) {

        while( std::getline( argFile, line ) ) {

            trimSpaces( line );
            if ( !line.empty() ) {

                if ( line.at(0) == '#' ) {
                    lineNumber++;
                    continue;
                }
                checkSemicolon( line, lineNumber );
                checkSyntax_server_location( line, lineNumber );
                checkServerScope( line, lineNumber );
                checkLocationScope( line, lineNumber );
                cleanLine( line );
                checkValue( line, lineNumber );
                if ( line == "server[") {
                    _vectorIndice++;
                    config.push_back( std::vector<std::string>() );
                }
                config[_vectorIndice].push_back( line ) ;
                lineNumber++;
            }
            else {
                lineNumber++;
            }
        }
        argFile.close();
        if ( _serverToken == true ) {
            throw std::invalid_argument( RED "Error:\nconfig file: syntax error: ']' missing! line: " + to_string( lineNumber )+ " getFile 3" RESET);
        }
    }
    else { throw std::invalid_argument( "argument file can't be read" ); }
}

void getConfigFile::checkLocationScope( std::string line, size_t lineNumber ) {

    if ( line.find( "location", 0, 8 ) == 0 && line.at(line.size() -1) == '{' &&  _locationToken == true ) {
        throw std::invalid_argument( RED "Error:\nconfig file: syntax error: '" + line + "' line: " + to_string( lineNumber )+ " checkLocationScope 1" RESET);
    }
    if ( line.find( "location", 0, 8 ) == 0 && line.at(line.size() -1) == '{') {
        _locationToken = true;
    }
    if ( line == "}" &&  _locationToken == true ) {
        _locationToken = false;
    }
    else if ( line == "}" &&  _locationToken == false ) {
        throw std::invalid_argument( RED "Error:\nconfig file: syntax error: '" + line + "' line: " + to_string( lineNumber -1 )+ " checkLocationScope 2" RESET);
    }
}

void getConfigFile::checkServerScope( std::string line, size_t lineNumber ) {

    if ( (_vectorIndice == -1 && line != "server[")
        || (line == "]" && _serverToken == false)
        || (line == "server[" && _serverToken == true )) {
        throw std::invalid_argument( RED "Error:\nconfig file: syntax error: '" + line + "' line: " + std::to_string( lineNumber )+ " getFile 1" ); RESET;
    }
    if ( line == "server[" ) {
        _serverToken = true;
    }
    if ( line == "]" ) {
        _serverToken = false;
    }
}

void getConfigFile::checkSyntax_server_location( std::string & line, size_t lineNumber ) {

    if ( line.find( "server", 0, 5 ) == 0 && line.at(line.size() -1) != ';' ) {

        if ( line.at(line.size()-1) != '[' ) {
            throw std::invalid_argument( RED "Error:\nconfig file: syntax error: '[' missing! line: " + to_string( lineNumber )+ " check check Syntax 1" RESET);
        }
        for ( size_t i = 6; i != line.size() -1; i++ ) {
            if ( !isspace(line[i]) ) {
                throw std::invalid_argument( RED "Error:\nconfig file: syntax error: " + line + " line: " + to_string( lineNumber )+ " check check Syntax 2" RESET);
            }
        }
        line.erase(std::remove_if(line.begin(), line.end(), isspace ), line.end());
    }
    if ( line.find( "location", 0, 8 ) == 0 && line.at(line.size() -1) != ';' ) {
        if ( line.at(line.size()-1) != '{' ) {
            throw std::invalid_argument( RED "Error:\nconfig file: syntax error: '{' missing! line: " + to_string( lineNumber )+ " check check Syntax 3" RESET);
        }
    }
}

void getConfigFile::checkSemicolon( std::string const& line, size_t lineNumber ) const{

    if (  (line.find("]") != std::string::npos || line.find("}") != std::string::npos) ) {

        if ( (line.size() != 1) ){
            throw std::invalid_argument( RED "Error:\nconfig file: syntax error: line: " + to_string( lineNumber ) + " check Semicolon 1" RESET);
        }
        return;
    }
    if (( line.find( "server", 0, 5 ) == 0 && line.find("[")  != std::string::npos )
        || ( line.find( "location", 0, 8 ) == 0 && line.find("{")  != std::string::npos )) {

    if ( line.find(";") != std::string::npos ) {
        throw std::invalid_argument( RED "Error:\nconfig file: syntax error: line: " + to_string( lineNumber ) + " check Semicolon 2" RESET);
    }
        return;
    }
    else if ( line.at(line.size()-1) == ';' && count( line.begin(), line.end(), ';' ) == 1 ) {
        return;
    }
    else { throw std::invalid_argument( RED "Error:\nconfig file: syntax error: line: " + to_string( lineNumber ) + " check Semicolon 3" RESET); }
}

void getConfigFile::checkValue( std::string line, size_t lineNumber ) {

    if (( line.find("index", 0, 5) == 0 && !isspace(line[5])) || (line.find("methods", 0, 7) == 0 && !isspace(line[7]) )) {
        throw std::invalid_argument( RED "Error:\nconfig file: syntax error: line: chValue1 " + to_string( lineNumber ) + RESET);
    }
    else if ( line != "server[" && line != "]" && line != "}" ) {

        if ( !(find_if( line.begin(), line.end(), ::isspace ) != line.end()) )   {
            throw std::invalid_argument( RED "Error:\nconfig file: syntax error: line: chValue2 " + to_string( lineNumber ) + RESET);
        }
    }
    if ( line.find("download ", 0, 9) == 0 || line.find("directory_listing", 0, 17) == 0 ) {
        line = getValue( line );
        if (line != "on" && line != "off" ) {
            throw std::invalid_argument( RED "Error:\nconfig file: wrong value: must be on/off line: " + to_string( lineNumber ) + RESET);
        }
    }
    if ( line.find("listen", 0, 6 ) == 0 ) {
        checkListen( line, lineNumber );
    }
    if ( line.find("body_size_max", 0, 13 ) == 0 ) {
        checkBodySize( line, lineNumber );
    }
    if ( line.find("host", 0, 4 ) == 0 ) {
        checkHost( line, lineNumber );
    }
    if ( line.find("methods", 0, 7 ) == 0 ) {
        checkMethods( line, lineNumber );
    }
}

void getConfigFile::trimSpaces( std::string& line ) {

    int i = 0;
    while( isspace(line[i]) ) { i++; }
    if ( i > 0 ){
        line = line.substr(i);
    }

    std::string::reverse_iterator rit = line.rbegin();
    while (rit != line.rend() && std::isspace(*rit)) { rit++; }
    line.erase(rit.base(), line.end());
}

void getConfigFile::cleanLine( std::string & line ) {

    for (size_t i = 0; i < line.size(); i++) {
        if (isspace(line[i])) {
            while (i + 1 < line.size() && isspace(line[i + 1])) {
                line.erase(i + 1, 1);
            }
            if (line[i + 1] == ';' || line[i + 1] == '{' ) {
                line.erase(i, 1);
            }
        }
    }
}

std::string getConfigFile::getValue( std::string str ) {

    if ( !str.empty() ) {

        if ( str.find(' ') != std::string::npos ) {
            str = str.substr(str.find(' ') +1);
        }
        else {
            return "";
        }
        if ( str.find('{') != std::string::npos || str.find(';') != std::string::npos ) {
            str.resize( str.size() -1 );
        }
    }
    return str;
}

std::string getConfigFile::getCGI_PATH() const{
    return _cgi_path;
}

void getConfigFile::checkBodySize( std::string line, size_t lineNumber ) {

    std::string value = getValue( line );
    if ( value.empty() ) {
        throw std::invalid_argument( RED "Error:\nconfig file: no value: 'body_size_max directive' line: " + to_string( lineNumber ) + " check listen" RESET);
    }
    else {
        std::istringstream iss( value );
        int number;
        if ( value.find_first_not_of("0123456789") == std::string::npos
            && iss >> number ) {
                return;
        }
        else {
            throw std::invalid_argument( RED "Error:\nconfig file: wrong value: 'body_size_max directive' line: " + to_string( lineNumber ) + " check listen" RESET);
        }
    }
}

void getConfigFile::checkMethods( std::string line, size_t lineNumber ) {

    line = getValue( line );
    std::string tmp;
    int methNumber = 0;

    for ( size_t i = 0; i < line.length(); i++ ) {

        if ( isspace(line[i]) || i == line.length() -1) {

            tmp = line;
            if ( isspace(line[i]) )
                tmp.resize(i);
            methNumber++;
            if (( tmp != "GET" && tmp != "POST" && tmp != "DELETE" ) || methNumber > 3) {

                throw std::invalid_argument( RED "Error:\nconfig file: 'methods directive' line: " + to_string( lineNumber ) + " check Methods" RESET );
            }
            line = line.substr(i+1);
            i = 0;
        }
    }
}

void getConfigFile::checkListen( std::string line, size_t lineNumber ) {

    std::string value = getValue( line );
    if ( value.empty() ) {
        throw std::invalid_argument( RED "Error:\nconfig file: no value: 'listen directive' line: " + to_string( lineNumber ) + " check listen" RESET);
    }
    else {
        std::istringstream iss( value );
        int number;
        if ( value.find_first_not_of("0123456789") == std::string::npos
            && iss >> number && number < 65535 ) {
                return;
        }
        else {
            throw std::invalid_argument( RED "Error:\nconfig file: wrong value: 'listen directive' line: " + to_string( lineNumber ) + " check listen" RESET);
        }
    }
}

void getConfigFile::checkHost( std::string line, size_t lineNumber ) {

    std::string value = getValue( line );

    if ( std::count( value.begin(), value.end(), '.' ) != 3
        || value.find_first_of(" \t\n\r") != std::string::npos
        || value.find_first_not_of("0123456789.") != std::string::npos ) {
            throw std::invalid_argument( RED "Error:\nconfig file: wrong value: 'host directive' line: " + to_string( lineNumber ) + " check host1" RESET);
    }
    for ( int i = 0; i < 4; i++ ) {

        std::stringstream iss( value );
        int number;

        if( iss >> number && number >= 0 && number <= 255 ) {

            if ( value.find('.') ) {
                value = value.substr( value.find('.') +1 );
            }
        }
        else { throw std::invalid_argument( RED "Error:\nconfig file: wrong value: 'host directive' line: " + to_string( lineNumber ) + " check host3" RESET); }
    }
}