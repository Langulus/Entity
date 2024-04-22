///                                                                           
/// Langulus::Entity                                                          
/// Copyright (c) 2013 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include "../Entity/Thing.hpp"


namespace Langulus::A 
{

   struct File;
   struct Folder;


   ///                                                                        
   ///   Abstract file system module                                          
   ///                                                                        
   struct FileSystem : virtual Module {
   protected:
      // Working directory path                                         
      // This is the only full path that is exposed to the system,      
      // unless LANGULUS(PARANOID) is enabled                           
      Path mWorkingPath;
      // Main data directory, for both reading and writing              
      // Relative to mWorkingPath                                       
      Path mMainDataPath;

   public:
      LANGULUS_BASES(Module);

      NOD() virtual Ref<File>   GetFile  (const Path&) = 0;
      NOD() virtual Ref<Folder> GetFolder(const Path&) = 0;

      NOD() const Path& GetWorkingPath() const noexcept;
      NOD() const Path& GetDataPath() const noexcept;
   };

   ///                                                                        
   ///   Abstract file interface                                              
   ///                                                                        
   struct File : virtual Unit {
   protected:
      Path mFilePath;
      bool mExists {};
      DMeta mFormat {};
      Size mByteCount {};
      bool mIsReadOnly {};

   public:
      LANGULUS(PRODUCER) FileSystem;
      LANGULUS_BASES(Unit);

      NOD() bool Exists() const noexcept;
      NOD() bool IsReadOnly() const noexcept;
      NOD() const Path& GetFilePath() const noexcept;
      NOD() DMeta GetFormat() const noexcept;
      NOD() Size GetBytesize() const noexcept;

      NOD() virtual Many ReadAs(DMeta) const = 0;

      NOD() virtual Ref<File>    RelativeFile  (const Path&) const = 0;
      NOD() virtual Ref<Folder>  RelativeFolder(const Path&) const = 0;
      
      template<class T>
      NOD() T ReadAs() const;

      /// Abstract file reader stream                                         
      struct Reader {
      protected:
         File* mFile;
         Offset mProgress {};

      public:
         Reader() = delete;
         Reader(File* f)
            : mFile {f} {}

         File* GetFile() const noexcept { return mFile; }

         virtual Offset Read(Many&) = 0;
      };

      /// Abstract file writer stream                                         
      struct Writer {
      protected:
         File* mFile;
         Offset mProgress {};
         bool mAppend = false;

      public:
         Writer() = delete;
         Writer(File* f, bool append)
            : mFile {f}, mAppend {append} {}

         File* GetFile() const noexcept { return mFile; }

         virtual Offset Write(const Many&) = 0;
      };

      NOD() virtual Ref<Reader> NewReader() const = 0;
      NOD() virtual Ref<Writer> NewWriter(bool append) const = 0;
   };

   ///                                                                        
   ///   Abstract folder interface                                            
   ///                                                                        
   struct Folder : virtual Unit {
   protected:
      Path mFolderPath;
      bool mExists = false;
      bool mIsReadOnly = false;

   public:
      LANGULUS(PRODUCER) FileSystem;
      LANGULUS_BASES(Unit);

      NOD() bool Exists() const noexcept;
      NOD() bool IsReadOnly() const noexcept;
      NOD() const Path& GetFolderPath() const noexcept;

      NOD() virtual Ref<File>   RelativeFile  (const Path&) const = 0;
      NOD() virtual Ref<Folder> RelativeFolder(const Path&) const = 0;
   };

} // namespace Langulus::A

#include "IO.inl"