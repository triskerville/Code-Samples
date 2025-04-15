//>=------------------------------------------------------------------------=<//
// file:    ObjectAllocator.cpp
// author:  Tristan Baskerville
// course:  CS280
// brief:  
//   This file contains the implementation for the Object Allocator class.
//
//   Hours spent on this assignment: ~35
//   Specific portions that gave you the most trouble: External Headers
//
// Copyright © 2020 DigiPen, All rights reserved.
//>=------------------------------------------------------------------------=<//

#include "ObjectAllocator.h"
#include <cstring>

namespace
{
  // const value for sizeof(void*);
  constexpr size_t ptrSize = sizeof(void*);
}

//>=------------------------------------------------------------------------=<//
/*!
    \brief
      ctor for an ObjectAllocator instance
    \param _objectSize
      size of the objects to allocate
    \param _config
      configuration to use with the OA
*/
//>=------------------------------------------------------------------------=<//
ObjectAllocator::ObjectAllocator(size_t _objectSize, const OAConfig& _config) :
  pagelist_(nullptr),
  freelist_(nullptr),
  config_(_config),
  stats_(),
  headerOffset_(config_.HBlockInfo_.size_ + config_.PadBytes_),
  blockOffset_(headerOffset_ + _objectSize + config_.PadBytes_)
{
  stats_.ObjectSize_ = _objectSize;

  stats_.PageSize_ = ptrSize + config_.ObjectsPerPage_ *
    (config_.HBlockInfo_.size_ + 2 * config_.PadBytes_ + _objectSize);

  //  only allocate page if not using new/delete
  if (config_.UseCPPMemManager_ != true)
    allocatePage();

}

//>=------------------------------------------------------------------------=<//
/*!
    \brief
      destructor for an ObjectAllocator instance
*/
//>=------------------------------------------------------------------------=<//
ObjectAllocator::~ObjectAllocator()
{
  //  empty the entire pagelist.
  //  there are no pages if the C++ Memory Manager is in use.
  while (pagelist_)
  {
    //  memory to be deleted
    BYTE* page = reinterpret_cast<BYTE*>(pagelist_);

    //  (DEBUG) make sure there are no remaining headers left by the client
    if (config_.DebugOn_ && config_.HBlockInfo_.type_ == config_.hbExternal)
    {
      for (unsigned i = 0; i < config_.ObjectsPerPage_; ++i)
      {
        BYTE* block = page + ptrSize + i * blockOffset_ + headerOffset_;

        //  if this wasn't freed by the client, free it
        if (not onFreelist(block))
          headerSetterExternal(block - headerOffset_, false);
      }
    }

    pagelist_ = pagelist_->Next;
    delete[] page;
  }
}

//>=------------------------------------------------------------------------=<//
/*!
    \brief
      Allocates an object and returns to client
    \param _label
      Optional _label to use with external headers
    \return
      allocated memory for client
*/
//>=------------------------------------------------------------------------=<//
void* ObjectAllocator::Allocate(const char* _label)
{
  GenericObject* obj;
  //  normal functionality
  if (not config_.UseCPPMemManager_)
  {
    obj = popFreelist(_label, &ObjectAllocator::setPatternAlloc);
  }
  //  only used when set to
  else
  {
    try
    {
      obj = reinterpret_cast<GenericObject*>(new BYTE[stats_.ObjectSize_]);
    }
    catch (std::bad_alloc&)
    {
      throw OAException(OAException::E_NO_MEMORY, "No system memory free");
    }
  }

  //  update stats
  stats_.Allocations_ += 1;
  stats_.FreeObjects_ -= 1;
  if ((stats_.ObjectsInUse_ += 1) > stats_.MostObjects_)
    stats_.MostObjects_ = stats_.ObjectsInUse_;

  return obj;
}

//>=------------------------------------------------------------------------=<//
/*!
    \brief
      Takes a pointer from client and "releases" it
    \param _object
      Pointer to memory to release
*/
//>=------------------------------------------------------------------------=<//
void ObjectAllocator::Free(void* _object)
{
  if (not config_.UseCPPMemManager_)
  {
    if (config_.DebugOn_) 
      verifyPointer(_object);

    if (config_.HBlockInfo_.type_ != config_.hbNone)
      removeHeader(_object);
    
    popFreelist(_object, &ObjectAllocator::setPatternFreed);
  }
  else // UseCPPMemManager_ == true
  {
    delete [] reinterpret_cast<BYTE*>(_object);
  }

  //  update stats
  stats_.Deallocations_ += 1;
  stats_.ObjectsInUse_  -= 1;
  stats_.FreeObjects_   += 1;
}

//>=------------------------------------------------------------------------=<//
/*!
    \brief
      Dumps memory currently in use by client
    \param _fn
      Pointer to client defined callback function 
    \return
      Total number of objects in use by client
*/
//>=------------------------------------------------------------------------=<//
unsigned ObjectAllocator::DumpMemoryInUse(DUMPCALLBACK _fn) const
{
  GenericObject* page = pagelist_;
  
  //  call _fn on all allocated obj's
  while (page)
  {
    BYTE* byte_page = reinterpret_cast<BYTE*>(page);
    for (unsigned i = 0; i < config_.ObjectsPerPage_; ++i)
    {
      //  find next object
      const BYTE* block = byte_page + ptrSize + 
                          i * blockOffset_ + headerOffset_;

      //  check if the object is in use
      if (not onFreelist(block))
        _fn(block, stats_.ObjectSize_);
    }

    page = page->Next;
  }

  return stats_.ObjectsInUse_;
}

//>=------------------------------------------------------------------------=<//
/*!
    \brief
      Verifies the pad bytes for corrupted memory
    \param _fn
      Pointer to client defined callback function
    \return
      Total count of potentially corrupt memory
*/
//>=------------------------------------------------------------------------=<//
unsigned ObjectAllocator::ValidatePages(VALIDATECALLBACK _fn) const
{
  const GenericObject* page = pagelist_;
  unsigned count = 0;

  //  call _fn on all potentially corrupted memory
  while (page)
  {
    const BYTE* page_block = reinterpret_cast<const BYTE*>(page);
    for (unsigned i = 0; i < config_.ObjectsPerPage_; ++i)
    {
      const BYTE* block = page_block + ptrSize + 
                          i * blockOffset_ + headerOffset_;

      if (badPadBytes(block))
      {
        _fn(block, stats_.ObjectSize_);
        ++count;
      }
    }

    page = page->Next;
  }

  //  total count of bad memory found
  return count;
}

//>=------------------------------------------------------------------------=<//
/*!
    \brief
      Does nothing. Not Implemented
    \return
      N/A
*/
//>=------------------------------------------------------------------------=<//
unsigned ObjectAllocator::FreeEmptyPages()
{
  //  not implemented
  return 0;
}

//>=------------------------------------------------------------------------=<//
/*!
    \brief
      Sets the debug state for the Object Allocator
    \param state
      True to enable debug functionality. False to disable debug features.
*/
//>=------------------------------------------------------------------------=<//
void ObjectAllocator::SetDebugState(bool _debugState)
{
  config_.DebugOn_ = _debugState;
}

//>=------------------------------------------------------------------------=<//
/*!
    \brief
      Returns a pointer to the internal freelist
    \return
      freelist
*/
//>=------------------------------------------------------------------------=<//
const void* ObjectAllocator::GetFreeList() const
{
  return freelist_;
}

//>=------------------------------------------------------------------------=<//
/*!
    \brief
      Returns a pointer to the internal pagelist
    \return
      pagelist
*/
//>=------------------------------------------------------------------------=<//
const void* ObjectAllocator::GetPageList() const
{
  return pagelist_;
}

//>=------------------------------------------------------------------------=<//
/*!
    \brief
      Returns the configuration settings used
    \return
      Configuration settings
*/
//>=------------------------------------------------------------------------=<//
OAConfig ObjectAllocator::GetConfig() const
{
  return config_;
}

//>=------------------------------------------------------------------------=<//
/*!
    \brief
      Returns the currently tracked statistic
    \return
      Current statistics
*/
//>=------------------------------------------------------------------------=<//
OAStats ObjectAllocator::GetStats() const
{
  return stats_;
}

//>=------------------------------------------------------------------------=<//
/*!
    \brief
      Allocates a new page and places it onto the pagelist
*/
//>=------------------------------------------------------------------------=<//
void ObjectAllocator::allocatePage()
{
  if (stats_.PagesInUse_ >= config_.MaxPages_)
    throw OAException(OAException::E_NO_PAGES, "No extra pages available");

  BYTE* page;

  try
  {
    //  initilize a page to zeros
    page = new BYTE[stats_.PageSize_]();
  }
  catch (std::bad_alloc&)
  {
    throw OAException(OAException::E_NO_MEMORY, "No system memory free");
  }

  pushPagelist(page);
}

//>=------------------------------------------------------------------------=<//
/*!
    \brief
      Takes a page pointer and places on front of pagelist
    \param _pageBlock
      pointer to the page
*/
//>=------------------------------------------------------------------------=<//
void ObjectAllocator::pushPagelist(BYTE* _pageBlock)
{
  GenericObject* page = reinterpret_cast<GenericObject*>(_pageBlock);
  page->Next = pagelist_;
  pagelist_ = page;

  for (size_t i = 0; i < config_.ObjectsPerPage_; ++i)
  {
    BYTE* block = _pageBlock + ptrSize + i * blockOffset_ + headerOffset_;

    popFreelist(block, &ObjectAllocator::setPatternUnalloc);
  }

  //  update stats
  stats_.PagesInUse_ += 1;
  stats_.FreeObjects_ += config_.ObjectsPerPage_;
}

//>=------------------------------------------------------------------------=<//
/*!
    \brief
      Removes an object off of the front of the freelist
    \param _label
      Optional label to use with external headers
    \param _fn
      Pattern callback to set debug memory
    \return
      allocated memory for client
*/
//>=------------------------------------------------------------------------=<//
GenericObject* ObjectAllocator::popFreelist(const char* _label,
  PATTERNCALLBACK _fn)
{
  //  allocate a new page if no more space
  if (freelist_ == nullptr)
    allocatePage();

  GenericObject* obj = freelist_;
  freelist_ = freelist_->Next;
  
  //  call pattern setting function
  (this->*_fn)(obj);
  
  if (config_.HBlockInfo_.type_ != config_.hbNone)
    generateHeader(obj, _label);

  return obj;
}

//>=------------------------------------------------------------------------=<//
/*!
    \brief
      Surrenders a pointer from the client
    \param _object
      Pointer to place onto freelist
    \param _fn
      Pattern callback to set debug memory
*/
//>=------------------------------------------------------------------------=<//
void ObjectAllocator::popFreelist(void* _object, PATTERNCALLBACK _fn)
{
  GenericObject* obj = reinterpret_cast<GenericObject*>(_object);
  
  //  call pattern callback
  (this->*_fn)(obj);

  obj->Next = freelist_;
  freelist_ = obj;
}

//>=------------------------------------------------------------------------=<//
/*!
    \brief
      Sets up the memory for the desired header
    \param _object
      Location of object to find spot for header
    \param _label
      Optional label to use with external headers
*/
//>=------------------------------------------------------------------------=<//
void ObjectAllocator::generateHeader(void* _object, const char* _label)
{
  BYTE* block = reinterpret_cast<BYTE*>(_object);
  BYTE* header = block - headerOffset_;

  //  create desired header
  if (config_.hbBasic == config_.HBlockInfo_.type_)
    headerSetterBasic(header, stats_.Allocations_ + 1, 1);
  else if ( config_.hbExtended == config_.HBlockInfo_.type_)
    headerSetterExtended(header, 1, stats_.Allocations_ + 1, 1);
  else if (config_.hbExternal == config_.HBlockInfo_.type_)
    headerSetterExternal(header, true, _label, stats_.Allocations_ + 1);
}

//>=------------------------------------------------------------------------=<//
/*!
    \brief
      Removes the memory from previous header
    \param _object
      Location of object to find spot for header
*/
//>=------------------------------------------------------------------------=<//
void ObjectAllocator::removeHeader(void* _object)
{
  // Assert(config_.HBlockInfo_.type_ != config_.hbNone);
  
  BYTE* block = reinterpret_cast<BYTE*>(_object);
  BYTE* header = block - headerOffset_;
  
  //  set header to 0
  if (config_.HBlockInfo_.type_ == config_.hbBasic)
    headerSetterBasic(header);
  else if (config_.HBlockInfo_.type_ == config_.hbExtended)
    headerSetterExtended(header);
  else if (config_.HBlockInfo_.type_ == config_.hbExternal)
    headerSetterExternal(header, false);
}

//>=------------------------------------------------------------------------=<//
/*!
    \brief
      Set up for the Basic header
    \param _header
      Pointer to the location to write to
    \param _allocs
      allocation number
    \param _flag
      in-use flag
*/
//>=------------------------------------------------------------------------=<//
void ObjectAllocator::headerSetterBasic(BYTE* _header, unsigned _allocs, char _flag)
{
  //  set the allocation # field
  *(reinterpret_cast<unsigned*>(_header)) = _allocs;
  //  set in use flag to true
  *(_header + sizeof(unsigned)) = _flag;
}

//>=------------------------------------------------------------------------=<//
/*!
    \brief
      Set up for the Extended header
    \param _header
      Pointer to the location to write to
    \param _use
      total number of uses for this memory
    \param _allocs
      allocation number
    \param _flag
      in-use flag
*/
//>=------------------------------------------------------------------------=<//
void ObjectAllocator::headerSetterExtended(BYTE* _header, short unsigned _use, 
  unsigned _allocs, char _flag)
{
  //  skip the user defined bytes
  _header += config_.HBlockInfo_.additional_;
  //  increment the _use counter. not sure how else to do this tbh
  if (_use == 1)
    (*(reinterpret_cast<short unsigned*>(_header))) += 1;

  //  jump _use counter
  _header += sizeof(unsigned short);
  //  set allocation # field
  *(reinterpret_cast<unsigned*>(_header)) = _allocs;

  //  jump allocation # field
  _header += sizeof(unsigned);
  //  set in use flag to true;
  *_header = _flag;
}

//>=------------------------------------------------------------------------=<//
/*!
    \brief
      Set up for the External header
    \param _header
      Pointer to the location to write to
    \param _build
      true=construct, false=destroy
    \param _label
      Optional label to use with header
    \param _allocs
      allocation number
*/
//>=------------------------------------------------------------------------=<//
void ObjectAllocator::headerSetterExternal(BYTE* _header, bool _build, 
  const char* _label, unsigned _allocs)
{
  //  if build is true, construct header, else, destroy it
  if (_build)
  {
    try
    {
      *(reinterpret_cast<MemBlockInfo**>(_header)) = new MemBlockInfo{ true,
                            const_cast<char*>(_label), _allocs };
    }
    catch (std::bad_alloc&)
    {
      throw OAException(OAException::E_NO_MEMORY, "No system memory free");
    }
  }
  else
  {
    delete *(reinterpret_cast<MemBlockInfo**>(_header));
    std::memset(_header, 0, config_.HBlockInfo_.size_);
  }
}

//>=------------------------------------------------------------------------=<//
/*!
    \brief
      Verifies there are no issues with a freed pointer
    \param _object
      Pointer to the memory that was freed
    \exception
      Can throw if: _object not on block boundary
                    _object previosuly freed
                    _object has corrupted pad bytes
*/
//>=------------------------------------------------------------------------=<//
void ObjectAllocator::verifyPointer(const void* _object) const
{
  // Assert (config_.DebugOn_ == true); // Debug only function

  if (badBoundary(_object))
    throw OAException(OAException::E_BAD_BOUNDARY, 
                      "Pointer not on block boundary");

  if (badPadBytes(_object))
    throw OAException(OAException::E_CORRUPTED_BLOCK, 
                      "Pointer has corrupted pad bytes");
    
  if (onFreelist(_object))
    throw OAException(OAException::E_MULTIPLE_FREE, "Pointer already freed");

}

//>=------------------------------------------------------------------------=<//
/*!
    \brief
      Checks if an object is on the freelist
    \param _object
      Object to check
    \return
      Wether or not an object was on the freelist
*/
//>=------------------------------------------------------------------------=<//
bool ObjectAllocator::onFreelist(const void* _object) const
{
  //  check const-time operation if available
  if (config_.HBlockInfo_.type_ != config_.hbNone)
    return !isHeaderInUse(_object);
  
  const GenericObject* obj = reinterpret_cast<const GenericObject*>(_object);
  
  const GenericObject* freelistIter = freelist_;
  while (freelistIter)
  {
    if (obj == freelistIter)
      return true;

    freelistIter = freelistIter->Next;
  }

  return false;
}

//>=------------------------------------------------------------------------=<//
/*!
    \brief
      Checks the in-use bit of a header, if one exists
    \param _object
      Location of object to find header
    \return the in-use bit
*/
//>=------------------------------------------------------------------------=<//
bool ObjectAllocator::isHeaderInUse(const void* _object) const
{
  // Assert(config_.HBlockInfo_.type_ != config_.hbNone);

  const BYTE* obj_block = reinterpret_cast<const BYTE*>(_object);

  if (config_.HBlockInfo_.type_ == config_.hbBasic ||
      config_.HBlockInfo_.type_ == config_.hbExtended)
  {
    //  return the bit saved at the tail of the header
    return static_cast<bool>(*(obj_block - config_.PadBytes_ - 1));
  }
  else if (config_.HBlockInfo_.type_ == config_.hbExternal)
  {
    //  returns whether or not a header exists
    return (*reinterpret_cast<MemBlockInfo* const*>
      (obj_block - headerOffset_)) != nullptr;
  }

  //  not sure how you got here
  return false;
}

//>=------------------------------------------------------------------------=<//
/*!
    \brief
      Checks if the object is on a block boundary
    \param _object
      Object to check
    \return
      Wether or not the object is on a block boundary
*/
//>=------------------------------------------------------------------------=<//
bool ObjectAllocator::badBoundary(const void* _object) const
{
  const GenericObject* obj = reinterpret_cast<const GenericObject*>(_object);
  const BYTE* obj_block = reinterpret_cast<const BYTE*>(_object);
  const GenericObject* page = pagelist_;

  size_t header_off = config_.HBlockInfo_.size_ + config_.PadBytes_;
  size_t block_off = header_off + stats_.ObjectSize_ + config_.PadBytes_;

  while (page)
  {
    //  find what page the pointer is on
    if (obj > page && obj < page + stats_.PageSize_)
    {
      const BYTE* page_block = reinterpret_cast<const BYTE*>(page);
      size_t page_mem = (obj_block - header_off) - (page_block + ptrSize);
      //  returns if the object is properly aligned. == 0 means aligned
      return static_cast<bool>(page_mem % block_off != 0);
    }

    page = page->Next;
  }

  //  getting here means object wasn't on any page
  return true;
}

//>=------------------------------------------------------------------------=<//
/*!
    \brief
      Verifies the pad bytes next to objects
    \param _object
      Pointer to object to check pad bytes
    \return
      Health of pad bytes
*/
//>=------------------------------------------------------------------------=<//
bool ObjectAllocator::badPadBytes(const void* _object) const
{
  const BYTE* block = reinterpret_cast<const BYTE*>(_object);
  const BYTE* left_pad = block - config_.PadBytes_;
  const BYTE* right_pad = block + stats_.ObjectSize_;

  for (unsigned i = 0; i < config_.PadBytes_; ++i)
    if (left_pad[i] != PAD_PATTERN || right_pad[i] != PAD_PATTERN)
      return true;

  return false;
}

//>=------------------------------------------------------------------------=<//
/*!
    \brief
      Set the pattern used for newly allocated memory
    \param _object
      Pointer to the location to write to
*/
//>=------------------------------------------------------------------------=<//
void ObjectAllocator::setPatternAlloc(GenericObject* _object)
{
  //  do nothing when debug mode off
  if (not config_.DebugOn_) return;
  BYTE* obj_block = reinterpret_cast<BYTE*>(_object);

  std::memset(obj_block, ALLOCATED_PATTERN, stats_.ObjectSize_);
}

//>=------------------------------------------------------------------------=<//
/*!
    \brief
      Set the pattern used for unallocated memory
    \param _object
      Pointer to the location to write to/near
*/
//>=------------------------------------------------------------------------=<//
void ObjectAllocator::setPatternUnalloc(GenericObject* _object)
{
  //  do nothing when debug mode off
  if (not config_.DebugOn_) return;
  BYTE* obj_block = reinterpret_cast<BYTE*>(_object);

  //  set block memory pattern
  std::memset(obj_block, UNALLOCATED_PATTERN, stats_.ObjectSize_);

  //  set padding before & after block
  std::memset(obj_block - config_.PadBytes_, PAD_PATTERN, config_.PadBytes_);
  std::memset(obj_block + stats_.ObjectSize_, PAD_PATTERN, config_.PadBytes_);
}

//>=------------------------------------------------------------------------=<//
/*!
    \brief
      Set the pattern used for freed memory
    \param _object
      Pointer to the location to write to
*/
//>=------------------------------------------------------------------------=<//
void ObjectAllocator::setPatternFreed(GenericObject* _object)
{
  //  do nothing when debug mode off
  if (not config_.DebugOn_) return;
  BYTE* obj_block = reinterpret_cast<BYTE*>(_object);

  //  set freed memory pattern
  std::memset(obj_block, FREED_PATTERN, stats_.ObjectSize_);
}
