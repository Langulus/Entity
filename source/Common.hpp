///                                                                           
/// Langulus::Entity                                                          
/// Copyright(C) 2013 Dimo Markov <langulusteam@gmail.com>                    
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include <LangulusFlow.hpp>
#include <LangulusMath.hpp>

#if defined(LANGULUS_EXPORT_ALL) || defined(LANGULUS_EXPORT_ENTITY)
   #define LANGULUS_API_ENTITY() LANGULUS_EXPORT()
#else
   #define LANGULUS_API_ENTITY() LANGULUS_IMPORT()
#endif

namespace Langulus::Entity
{

   using namespace ::Langulus::Anyness;
   using ::Langulus::Flow::Verb;
   using ::Langulus::Flow::Construct;
   using ::Langulus::Flow::Charge;
   using ::Langulus::Flow::SeekStyle;
   using ::Langulus::Flow::Resolvable;
   using ::Langulus::Flow::Temporal;
   using ::Langulus::Flow::Time;
   using ::Langulus::Flow::Scope;

} // namespace Langulus::Entity