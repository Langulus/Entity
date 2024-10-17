///                                                                           
/// Langulus::Entity                                                          
/// Copyright (c) 2013 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include <Anyness/TSet.hpp>
#include <Anyness/TMap.hpp>
#include <Anyness/Many.hpp>
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

#if 0
   #define ENTITY_VERBOSE_ENABLED()             1
   #define ENTITY_VERBOSE_SELF(...)             Logger::Info(this, ": ", __VA_ARGS__)
   #define ENTITY_VERBOSE_SELF_TAB(...)         const auto scoped = Logger::InfoTab(this, ": ", __VA_ARGS__)
   #define ENTITY_VERBOSE(...)                  Logger::Append(__VA_ARGS__)
   #define ENTITY_CREATION_VERBOSE_SELF(...)    Logger::Verbose(Self(), __VA_ARGS__)
   #define ENTITY_SELECTION_VERBOSE_SELF(...)   Logger::Verbose(Self(), __VA_ARGS__)
#else
   #define ENTITY_VERBOSE_ENABLED()             0
   #define ENTITY_VERBOSE_SELF(...)             LANGULUS(NOOP)
   #define ENTITY_VERBOSE_SELF_TAB(...)         LANGULUS(NOOP)
   #define ENTITY_VERBOSE(...)                  LANGULUS(NOOP)
   #define ENTITY_CREATION_VERBOSE_SELF(...)    LANGULUS(NOOP)
   #define ENTITY_SELECTION_VERBOSE_SELF(...)   LANGULUS(NOOP)
#endif
