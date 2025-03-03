///                                                                           
/// Langulus::Entity                                                          
/// Copyright (c) 2013 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Entity/Thing.hpp"
#include "Entity/Module.hpp"
#include <Langulus/Math/Vector.hpp>


namespace Langulus::A
{

   ///                                                                        
   ///   Abstract economy module                                              
   ///                                                                        
   struct Economy : virtual Module {
      LANGULUS_BASES(Module);
      Economy() : Resolvable {this}, Module {nullptr} {}
   };

   ///                                                                        
   ///   Abstract economy unit                                                
   ///                                                                        
   struct EconomyUnit : virtual Unit {
      LANGULUS(PRODUCER) Economy;
      LANGULUS_BASES(Unit);
      EconomyUnit() : Resolvable {this} {}
   };

   ///                                                                        
   ///   Abstract resource                                                    
   ///                                                                        
   struct Resource : virtual EconomyUnit {
      LANGULUS(PRODUCER) Economy;
      LANGULUS_BASES(EconomyUnit);
      Resource() : Resolvable {this} {}

      using Real = Langulus::Real;
      using Place = Math::Vec3;

      virtual Count GetQuantity() const noexcept = 0;
      virtual Count GetQuantityLocal(const Place&, Real radius) const noexcept = 0;
      virtual Count GetInstanceCount() const noexcept = 0;
      virtual Count GetInstanceCountLocal(const Place&, Real radius) const noexcept = 0;

      virtual Real GetSupply() const noexcept = 0;
      virtual Real GetSupplyLocal(const Place&, Real radius) const noexcept = 0;

      virtual Real GetDemand() const noexcept = 0;
      virtual Real GetDemandLocal(const Place&, Real radius) const noexcept = 0;
   };
   
   ///                                                                        
   ///   Abstract resource instance                                           
   ///                                                                        
   struct ResourceInstance : virtual EconomyUnit {
      LANGULUS(PRODUCER) Economy;
      LANGULUS_BASES(EconomyUnit);
      ResourceInstance() : Resolvable {this} {}

      virtual Count GetQuantity() const noexcept = 0;
      virtual Count GetCapacity() const noexcept = 0;
   };
   
   ///                                                                        
   ///   Abstract converter                                                   
   ///                                                                        
   struct Converter : virtual EconomyUnit {
      LANGULUS(PRODUCER) Economy;
      LANGULUS_BASES(EconomyUnit);
      Converter() : Resolvable {this} {}

      using Real = Langulus::Real;
      using Place = Math::Vec3;

      virtual Count GetInstanceCount() const noexcept = 0;
      virtual Count GetInstanceCountLocal(const Place&, Real radius) const noexcept = 0;
   };

   ///                                                                        
   ///   Abstract converter instance                                          
   ///                                                                        
   struct ConverterInstance : virtual EconomyUnit {
      LANGULUS(PRODUCER) Economy;
      LANGULUS_BASES(EconomyUnit);
      ConverterInstance() : Resolvable {this} {}
   };

   ///                                                                        
   ///   Abstract trader                                                      
   ///                                                                        
   struct Trader : virtual EconomyUnit {
      LANGULUS(PRODUCER) Economy;
      LANGULUS_BASES(EconomyUnit);
      Trader() : Resolvable {this} {}
   };

} // namespace Langulus::A