///                                                                           
/// Langulus::Entity                                                          
/// Copyright(C) 2013 Dimo Markov <langulusteam@gmail.com>                    
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include "Common.hpp"

namespace Langulus::Entity
{
   /// Some predeclarations                                                   

   class Entity;
   class Runtime;


   ///                                                                        
   ///   An abstract unit                                                     
   ///                                                                        
   ///   Unit is a shorter name for a component, or extension. It's used for  
   /// composing Entity behavior. Units are usually produced from factories   
   /// inside external, dynamically loaded modules. There are units for       
   /// graphics, input, AI, content, and whatever extensions you make.        
   ///                                                                        
   class Unit : public Resolvable {
   protected:
      friend class Entity;

      // Entities that are coupled with this unit                       
      TAny<Entity*> mOwners;

   public:
      /// A unit can not be default-created, nor be shallowly copied          
      Unit() = delete;
      Unit(const Unit&) = delete;
      Unit& operator = (const Unit&) = delete;

      /// A unit can only be moved or created with a resolvable type          
      Unit(DMeta) noexcept;
      Unit(Unit&&) noexcept;
      Unit& operator = (Unit&&) noexcept;
      virtual ~Unit();
      
      void Select(Verb&);

   public:
      virtual void Refresh() {}

      NOD() Runtime* GetRuntime() const noexcept;
      NOD() const TAny<Entity*>& GetOwners() const noexcept;

      template<CT::Trait T, SeekStyle = SeekStyle::UpToHere>
      Trait SeekTrait() const;

      template<SeekStyle = SeekStyle::UpToHere, CT::Data T>
      bool SeekValue(TMeta, T&) const;
      template<SeekStyle = SeekStyle::UpToHere, CT::Trait T, CT::Data D>
      bool SeekValue(D&) const;

      template<SeekStyle SEEK = SeekStyle::UpToHere>
      bool DoInHierarchy(Verb&);

   protected:
      void Couple(Entity*);
      void Decouple(Entity*);
      void ReplaceOwner(Entity*, Entity*);
   };

} // namespace Langulus::Entity


namespace Langulus::CT
{

   /// Any type that inherits A::Unit is considered a unit                    
   template<class T>
   concept Unit = DerivedFrom<T, ::Langulus::Entity::Unit>;

} // namespace Langulus::CT

#include "Unit.inl"
