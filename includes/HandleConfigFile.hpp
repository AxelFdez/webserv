/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HandleConfigFile.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chris <chris@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/30 14:21:37 by chris             #+#    #+#             */
/*   Updated: 2023/10/31 08:47:40 by chris            ###   ########.fr       */
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
#include "getConfigFile.hpp"

class getConfigFile;

class HandleConfigFile : public getConfigFile {

    public:
        HandleConfigFile();
        HandleConfigFile( char* file );
        virtual ~HandleConfigFile();

        std::string              getServerValues(size_t serverNb, std::string const& toFind); 
        std::vector<std::string> getLocationValues(size_t serverNb, std::string const& request, std::string const& toFind) const; 


    private:
        // Copy & Affectation
        HandleConfigFile( HandleConfigFile const& src );
        HandleConfigFile & operator=( HandleConfigFile const& rhs );

        // Attributes
        std::vector<std::vector<int> >                   _ports;
        std::vector<int>                                 _body_size_max;
        std::vector<std::vector<std::map<std::string, std::vector<std::string> > > > _locations;

        void        get_ports_bodySize();
        void        getLocations();


        // Tools
        std::string                 getKey( std::string str );
        std::vector<std::string>    splitStringToVector( std::string str );
        // print
        void      print_config() const;
        void      print_ports_bodySize() const;
        void      print_locations() const;


};
