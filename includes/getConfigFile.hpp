/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   getConfigFile.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chris <chris@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/03 15:10:28 by chris             #+#    #+#             */
/*   Updated: 2023/11/15 16:44:29 by chris            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <vector>
#include <stdexcept>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <string>
#include "policeColor.hpp"


class getConfigFile {

    public:
        getConfigFile( char* file );
        virtual ~getConfigFile();
        virtual std::string getValue( std::string str );

    protected:
        std::vector<std::vector<std::string> > config;

    private:
        getConfigFile();
        getConfigFile( getConfigFile const& src );
        getConfigFile & operator=( getConfigFile const& rhs );

        bool                        _serverToken;
        bool                        _locationToken;
        size_t                      _vectorIndice;

        void checkServerScope( std::string line, size_t lineNumber );
        void checkLocationScope( std::string line, size_t lineNumber );
        void checkSemicolon( std::string const& line, size_t lineNumber ) const;
        void checkSyntax_server_location( std::string & lin, size_t lineNumber );
        void checkValue( std::string line, size_t lineNumber );
        void checkListen( std::string line, size_t lineNumber );
        void checkBodySize( std::string line, size_t lineNumber );
        void checkHost( std::string line, size_t lineNumber );
        void checkMethods( std::string line, size_t lineNumber );

        void trimSpaces( std::string& line );
        void cleanLine( std::string & line, size_t lineNumber );
};
