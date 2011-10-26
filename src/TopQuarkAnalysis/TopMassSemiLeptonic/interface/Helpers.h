#ifndef TopQuarkAnalysis_TopMassSemiLeptonic_Helpers_h
#define TopQuarkAnalysis_TopMassSemiLeptonic_Helpers_h


// -*- C++ -*-
//
// Package:    TopMassSemiLeptonic
// Namespace:  my::helpers
//
// $Id: Helpers.h,v 1.11 2010/11/27 15:16:20 vadler Exp $
//


#include <string>
#include <sstream>


namespace my
{

  namespace helpers
  {

    template < typename T >
    std::string to_string( T const & value )
    {
        std::stringstream sstr;
        sstr << value;
        return sstr.str();
    }

  }

}


#endif
