///                                                                           
/// Langulus::Entity                                                          
/// Copyright (c) 2013 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include <Anyness/TSet.hpp>
#include <Anyness/TMap.hpp>
#include <Anyness/TMany.hpp>
#include <Anyness/Ref.hpp>
#include <Anyness/Trait.hpp>
#include <Anyness/Path.hpp>
#include <Flow/Verb.hpp>
#include <Flow/Resolvable.hpp>

#if defined(LANGULUS_EXPORT_ALL) or defined(LANGULUS_EXPORT_ENTITY)
   #define LANGULUS_API_ENTITY() LANGULUS_EXPORT()
#else
   #define LANGULUS_API_ENTITY() LANGULUS_IMPORT()
#endif


namespace Langulus
{

   using namespace Anyness;
   using namespace Flow;

} // namespace Langulus::Entity


/// Make the rest of the code aware, that Langulus::Entity has been included  
#define LANGULUS_LIBRARY_ENTITY() 1
