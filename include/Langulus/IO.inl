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


namespace Langulus::Entity
{
   
   /// Get a file interface, relying on external modules to find it           
   ///   @param path - the path for the file                                  
   ///   @return the file interface, or nullptr if file doesn't exist         
   Ref<A::File> Runtime::GetFile(const Path& path) {
      auto& fileSystems = GetModules<A::FileSystem>();
      LANGULUS_ASSERT(fileSystems, Module,
         "Can't retrieve file `", path, "` - no file system module available");
      return fileSystems.template As<A::FileSystem*>()->GetFile(path);
   }
   
   /// Get a folder interface, relying on external modules to find it         
   ///   @param path - the path for the folder                                
   ///   @return the folder interface, or nullptr if folder doesn't exist     
   Ref<A::Folder> Runtime::GetFolder(const Path& path) {
      auto& fileSystems = GetModules<A::FileSystem>();
      LANGULUS_ASSERT(fileSystems, Module,
         "Can't retrieve folder `", path, "` - no file system module available");
      return fileSystems.template As<A::FileSystem*>()->GetFolder(path);
   }

   /// Get the current working path (where the main exe was executed)         
   ///   @return the path                                                     
   const Path& Runtime::GetWorkingPath() const {
      auto& fileSystems = GetModules<A::FileSystem>();
      LANGULUS_ASSERT(fileSystems, Module,
         "Can't retrieve working path", " - no file system module available");
      return fileSystems.template As<A::FileSystem*>()->GetWorkingPath();
   }

   /// Get the current data path, like GetWorkingPath() / "data"              
   ///   @return the path                                                     
   const Path& Runtime::GetDataPath() const {
      auto& fileSystems = GetModules<A::FileSystem>();
      LANGULUS_ASSERT(fileSystems, Module,
         "Can't retrieve data path", " - no file system module available");
      return fileSystems.template As<A::FileSystem*>()->GetDataPath();
   }

} // namespace Langulus::Entity
