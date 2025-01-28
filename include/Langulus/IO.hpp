///                                                                           
/// Langulus::Entity                                                          
/// Copyright (c) 2013 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Entity/Thing.hpp"


namespace Langulus::A 
{

   struct File;
   struct Folder;


   ///                                                                        
   ///   Abstract file system module                                          
   ///                                                                        
   struct FileSystem : virtual Module {
      LANGULUS_BASES(Module);

   protected:
      // Working directory path                                         
      // This is the only full path that is exposed to the system,      
      // unless LANGULUS(PARANOID) is enabled                           
      Path mWorkingPath;
      // Main data directory, for both reading and writing              
      // Relative to mWorkingPath                                       
      Path mMainDataPath;

   public:
      virtual auto GetFile  (const Path&) -> Ref<File>   = 0;
      virtual auto GetFolder(const Path&) -> Ref<Folder> = 0;

      auto GetWorkingPath() const noexcept -> const Path&;
      auto GetDataPath()    const noexcept -> const Path&;
   };


   ///                                                                        
   ///   Abstract file interface                                              
   ///                                                                        
   struct File : virtual Unit {
      LANGULUS(PRODUCER) FileSystem;
      LANGULUS_BASES(Unit);

   protected:
      Path mFilePath;
      bool mExists {};
      DMeta mFormat {};
      Size mByteCount {};
      bool mIsReadOnly {};

      // Parent directory substring, mapped onto mFilePath              
      Token mParentDirectory;
      // File name after all directories, mapped onto mFilePath         
      Token mFileName;
      // The extension of the filename, mapped onto mFilePath           
      Token mFileExtension;

   public:
      virtual auto ReadAs        (DMeta)       const -> Many = 0;
      virtual auto RelativeFile  (const Path&) const -> Ref<File> = 0;
      virtual auto RelativeFolder(const Path&) const -> Ref<Folder> = 0;
      
      auto Exists()      const noexcept -> bool;
      auto IsReadOnly()  const noexcept -> bool;
      auto GetFilePath() const noexcept -> const Path&;
      auto GetFormat()   const noexcept -> DMeta;
      auto GetBytesize() const noexcept -> Size;

      template<class T>
      T ReadAs() const;


      ///                                                                     
      /// Abstract file reader stream                                         
      struct Reader {
      protected:
         Ref<File> mFile;
         Offset    mProgress = 0;

      public:
         Reader() = delete;
         virtual ~Reader() {}

         Reader(File* f)
            : mFile {f} {}

         virtual auto Read(Many&) -> Offset = 0;

         auto GetFile() const noexcept -> const Ref<File>& {
            return mFile;
         }
      };


      ///                                                                     
      /// Abstract file writer stream                                         
      struct Writer {
      protected:
         Ref<File> mFile;
         Offset    mProgress = 0;
         bool      mAppend = false;

      public:
         Writer() = delete;
         virtual ~Writer() {}

         Writer(File* f, bool append)
            : mFile  {f}
            , mAppend{append} {}

         virtual auto Write(const Many&) -> Offset = 0;

         auto GetFile() const noexcept -> const Ref<File>& {
            return mFile;
         }
      };

      virtual auto NewReader() const -> Ref<Reader> = 0;
      virtual auto NewWriter(bool append) const -> Ref<Writer> = 0;
   };


   ///                                                                        
   ///   Abstract folder interface                                            
   ///                                                                        
   struct Folder : virtual Unit {
      LANGULUS(PRODUCER) FileSystem;
      LANGULUS_BASES(Unit);

   protected:
      Path mFolderPath;
      bool mExists = false;
      bool mIsReadOnly = false;

   public:
      virtual auto RelativeFile  (const Path&) const -> Ref<File> = 0;
      virtual auto RelativeFolder(const Path&) const -> Ref<Folder> = 0;

      auto Exists()        const noexcept -> bool;
      auto IsReadOnly()    const noexcept -> bool;
      auto GetFolderPath() const noexcept -> const Path&;
   };

} // namespace Langulus::A

#include "IO.inl"