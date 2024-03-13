///                                                                           
/// Langulus::Entity                                                          
/// Copyright (c) 2013 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include "IO.hpp"


namespace Langulus::A
{

   /// Get the current working path (where the main exe was executed)         
   ///   @return the path                                                     
   LANGULUS(INLINED)
   const Path& FileSystem::GetWorkingPath() const noexcept {
      return mWorkingPath;
   }

   /// Get the current data path, like GetWorkingPath() / "data"              
   ///   @return the path                                                     
   LANGULUS(INLINED)
   const Path& FileSystem::GetDataPath() const noexcept {
      return mMainDataPath;
   }
   
   /// Check if file physically exists                                        
   ///   @return true if file exists                                          
   LANGULUS(INLINED)
   bool File::Exists() const noexcept {
      return mExists;
   }

   /// Check if file is read-only                                             
   ///   @return true if file exists and is read-only                         
   LANGULUS(INLINED)
   bool File::IsReadOnly() const noexcept {
      return mIsReadOnly;
   }

   /// Check file format, that is deduced from file extension, and compared   
   /// against all reflected file extensions                                  
   ///   @return the file format                                              
   LANGULUS(INLINED)
   DMeta File::GetFormat() const noexcept {
      return mFormat;
   }

   /// Get the size of the file in bytes (if file exists)                     
   ///   @return the size of the file in bytes, or 0 if it doesn't exist      
   LANGULUS(INLINED)
   Size File::GetBytesize() const noexcept {
      return mByteCount;
   }

   /// Get the full path of the file                                          
   ///   @return a reference to the path                                      
   LANGULUS(INLINED)
   const Path& File::GetFilePath() const noexcept {
      return mFilePath;
   }

   /// Read a file and attempt to deserialize it as provided type T           
   /// Throws if deserialization isn't possible                               
   ///   @tparam T - the type to deserialize as, if possible                  
   ///   @return the deserialized instance of T                               
   template<class T> LANGULUS(INLINED)
   T File::ReadAs() const {
      return ReadAs(MetaDataOf<T>()).template As<T>();
   }
  
   /// Check if folder physically exists                                      
   ///   @return true if folder exists                                        
   LANGULUS(INLINED)
   bool Folder::Exists() const noexcept {
      return mExists;
   }

   /// Check if file is read-only                                             
   ///   @return true if file exists and is read-only                         
   LANGULUS(INLINED)
   bool Folder::IsReadOnly() const noexcept {
      return mIsReadOnly;
   }

   /// Get the full path of the folder                                        
   ///   @return a reference to the path                                      
   LANGULUS(INLINED)
   const Path& Folder::GetFolderPath() const noexcept {
      return mFolderPath;
   }

} // namespace Langulus::A