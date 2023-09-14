//>=------------------------------------------------------------------------=<//
// file:    OAHashTable.cpp
// author:  Tristan Baskerville
// course:  CS280
// brief:   
//   This file contains the implementation for the OAHashTable class.
//
//   Hours spent on this assignment: ~10
//   Specific portions that gave you the most trouble: 
//     index_of, was also fun seeing how vastly different each iteration was.
//
// Copyright © 2020 DigiPen, All rights reserved.
//>=------------------------------------------------------------------------=<//
#include <cmath> // std::ciel

//>=------------------------------------------------------------------------=<//
/*!
    \brief
      Constructs an OAHashTable with the given configuration passed in.
      This config dictates things like the starting table size, hash
      functions, load factor, growth rate, etc. The constructor
      sets up some of values needed for stats, then allocates our
      first internal table array.
    \param Config
      configuration settings for an OAHashTable instance.
*/
//>=------------------------------------------------------------------------=<//
template<typename T>
OAHashTable<T>::OAHashTable(const OAHTConfig& Config) 
  : config_(Config), stats_(), table_(nullptr)
{
  //  give some values over to stats
  stats_.TableSize_ = config_.InitialTableSize_;
  stats_.PrimaryHashFunc_ = config_.PrimaryHashFunc_;
  stats_.SecondaryHashFunc_ = config_.SecondaryHashFunc_;
  //  allocate the table array for use
  table_ = allocate_table(stats_.TableSize_);
}

//>=------------------------------------------------------------------------=<//
/*!
    \brief
      Destructs an OAHashTable instance. Uses default destructor for
      items such as stats and config. Calls the clear operator in the
      event the client needs to free up additional memory,
      then simply calles the delete operator on the internal table memory.
*/
//>=------------------------------------------------------------------------=<//
template<typename T>
OAHashTable<T>::~OAHashTable()
{
  //  in case elements are still remaining and they need to be freed
  clear();
  //  delete the internal memory
  delete[] table_;
}

//>=------------------------------------------------------------------------=<//
/*!
    \brief
      Given a key and some data, store the data in the hash table using
      the hashed key to search for appropriate storage locations.
      When called, checks if the table needs to be grown (and grows it
      accordingly). Searches the hash table using the key to find a slot
      to place the given data in. Uses linear probing during the times a more
      optimal slot is taken. If the key is already present in the hash
      table, throws an exception to inform the user there is duplicate
      data. Increments total count being tracked in statistics.
    \param Key
      String we are hashing to find an appropriate location to store Data.
    \param Data
      Data we wish to store in the hash table.
*/
//>=------------------------------------------------------------------------=<//
template<typename T>
void OAHashTable<T>::insert(const char* Key, const T& Data)
{
  //  check the load factor of the next insert. if this will surpass it, grow
  if (need_growing()) grow_table();

  OAHTSlot* slot = nullptr;
  //  make sure the desired slot is not already in the table, store
  //  desired slot in the 'slot' variable
  if (index_of(Key, slot) != DNE)
    //  if the item is a dulpicate, inform the client
    throw OAHTException(OAHTException::E_DUPLICATE, "Key exists in table.");

  //  init basic data in the slot after it is found.
  init_slot(*slot, Key, Data);
  //  increment total object count in stats
  ++stats_.Count_;
}

//>=------------------------------------------------------------------------=<//
/*!
    \brief
      Given a key, remove the element from the hash table by searching
      the hash table with the hashed key to find where the data is stored.
      Uses linear probing when needed. If the item was not found, throws
      an exception so the client knows the element was not found inside
      the hash table. When a slot is found, marks it as deleted (if that
      is what we are told to do), or packs the elements together
      that were pushed during linear probing.
    \param Key
      String we are hashing to find the slot we stored data in.
*/
//>=------------------------------------------------------------------------=<//
template<typename T>
void OAHashTable<T>::remove(const char* Key)
{
  OAHTSlot* slot = nullptr;
  //  get the slot we stored the key and data in
  int index = index_of(Key, slot);
  //  if the method did not return a valid index, inform the client
  //  that the search failed (throws an exception).
  if (index == DNE) item_not_found("Key not in table.");
  //  calls client defined free (if exists) and marks slot based on policy
  delete_slot(*slot, (config_.DeletionPolicy_ == OAHTDeletionPolicy::PACK) ? 
                     OAHTSlot::UNOCCUPIED : OAHTSlot::DELETED);

  //  pack together the remaining slots that were shifted during linear probing
  pack(index);
}

//>=------------------------------------------------------------------------=<//
/*!
    \brief
      Given a key, find and return the associated data that was originally
      passed along with the key. Hashes the key and uses linear probing
      to find the slot we stored data into. If the key cannot be used
      to find the slot, it does not exist in the hash table, so we throw
      an exception to inform the client. Returns a reference to the data
      once it is found.
    \param Key
      The string we are hashing to find the slot we stored data in.
    \return
      A reference to the data stored in the slot assicated with the Key.
*/
//>=------------------------------------------------------------------------=<//
template<typename T>
const T& OAHashTable<T>::find(const char* Key) const
{
  OAHTSlot* slot = nullptr;
  //  if the method did not return a valid index, inform the client
  //  that the search failed (throws an exception).
  if (index_of(Key, slot) == DNE) item_not_found("Item not found in table.");
  //  associated data client requested
  return slot->Data;
}

//>=------------------------------------------------------------------------=<//
/*!
    \brief
      Empties the entire hash table of any and all elements. Calls the
      client defined free function on all remaining elements in the hash table. 
      Marks every slot with the UNOCCUPIED flag for future re-use.
*/
//>=------------------------------------------------------------------------=<//
template<typename T>
void OAHashTable<T>::clear()
{
  //  iterate over table and delete any occupied elements
  for (unsigned i = 0; i < stats_.TableSize_; ++i)
  {
    OAHTSlot& slot = table_[i]; // current slot
    //  if there is an element here
    if (slot.State == OAHTSlot::OCCUPIED) 
      //  delete the occupied slot and set its state to unoccupied
      delete_slot(slot, OAHTSlot::UNOCCUPIED);

    //  if any slot has the old deleted flag, simply set it to unoccupied
    //  (client memory would already be freed at this point)
    if (slot.State == OAHTSlot::DELETED) slot.State = OAHTSlot::UNOCCUPIED;
  }
}

//>=------------------------------------------------------------------------=<//
/*!
    \brief
      Returns the internal stats being tracked within the hash table
      and give a copy to the client.
    \return
      The internally tracked stats stored within the hash table.
*/
//>=------------------------------------------------------------------------=<//
template<typename T>
OAHTStats OAHashTable<T>::GetStats() const
{
  return stats_;
}

//>=------------------------------------------------------------------------=<//
/*!
    \brief
      Returns the internal array used in the hash table for all the
      slots, which hold the key and data pairs being used by the client.
    \return
      The internal slot array holding the pairs of keys and associated data.
*/
//>=------------------------------------------------------------------------=<//
template<typename T>
typename OAHashTable<T>::OAHTSlot const* OAHashTable<T>::GetTable() const
{
  return table_; // internal slot array
}

//>=------------------------------------------------------------------------=<//
/*!
    \brief
      Given a size, allocate a slot array large enough to be used for 
      the hash table internal memory. Calls the new[] operator, sets all
      State flags to UNOCCUPIED, and sets the probes flag to 0. Catches
      a bad alloc exception from the STL and in turn throws a "no memory"
      exception that the client is expecting. Returns allocated memory.
    \param size
      The size we are growing the internal array to be.
    \return
      The pointer to the newly allocated internal array.
*/
//>=------------------------------------------------------------------------=<//
template<typename T>
typename OAHashTable<T>::OAHTSlot*
OAHashTable<T>::allocate_table(unsigned size)
{
  OAHTSlot* new_table; // pointer to new internal array

  try
  {
    //  allocate our new table
    new_table = new OAHTSlot[size];
    //  initialize the slot with an unoccupied state and 0 probes
    for (unsigned i = 0; i < size; ++i)
    {
      new_table[i].State = OAHTSlot::UNOCCUPIED;
      new_table[i].probes = 0;
    }
  }
  //  out of memory exception
  catch (std::bad_alloc& e)
  {
    //  throw a more client friendly exception
    throw OAHTException(OAHTException::E_NO_MEMORY, e.what());
  }

  return new_table; // return allocated memory
}

//>=------------------------------------------------------------------------=<//
/*!
    \brief
      Given a reference to a slot, set its key and its data fields with the
      the provided method parameters. Uses set_key(...) to set the internal Key.
    \param slot
      The reference to the slot we wish to store the key and data into.
    \param key
      The string we are storing in the desired slot.
    \param data
      The data we are storing inside the slot as well.
*/
//>=------------------------------------------------------------------------=<//
template<typename T>
void OAHashTable<T>::init_slot(OAHTSlot& slot, const char* key, const T& data)
{
  //  set the key and fill in the initial slot data. do not set probes
  //  as it is updated before slot is filled
  set_key(slot.Key, key);
  slot.Data = data;
  //  set the state to show this slot is being used
  slot.State = OAHTSlot::OCCUPIED;
}

//>=------------------------------------------------------------------------=<//
/*!
    \brief
      Given a pointer to a char array and a string, store the string
      into the char array using strncpy. Specific function used as
      the char array is limited to MAX_KEYLEN and not a null-terminator.
      If a null terminator is not found before the max limit is reached 
      one will be inserted at the end of the array.
    \param slot_key
      The character array we are storing the string INTO.
    \param string_key
      The string we are copying into the char array.
*/
//>=------------------------------------------------------------------------=<//
template<typename T>
void OAHashTable<T>::set_key(char* slot_key, const char* string_key)
{
  //  copy contents of string_key into slot_key up until a NUL
  //  character is encountered, or MAX_KEYLEN is hit.
  strncpy(slot_key, string_key, MAX_KEYLEN);
  //  if string_key is larger than MAX_KEYLEN, manually set the last
  //  character of slot_key to a null terminator.
  if (strlen(string_key) >= MAX_KEYLEN) slot_key[MAX_KEYLEN - 1] = 0;
}

//>=------------------------------------------------------------------------=<//
/*!
    \brief
      When called, recalculate the internal table array size using the growth
      factor and finding the closest prime. Allocates a table using this
      new size, then swaps the old internal pointer and size. 
      Calls insert() on all the elements in the old table array.
      Afterwards, deallocates the old table array using the delete[] operator.
*/
//>=------------------------------------------------------------------------=<//
template<typename T>
void OAHashTable<T>::grow_table()
{
  //  calculate the new size
  double factor = std::ceil(stats_.TableSize_ * config_.GrowthFactor_);
  unsigned new_limit = GetClosestPrime(static_cast<unsigned>(factor));
  //  store old values
  OAHTSlot* old_table = table_;
  unsigned old_limit = stats_.TableSize_;
  //  update new values
  table_ = allocate_table(new_limit);
  stats_.TableSize_ = new_limit;
  ++stats_.Expansions_; // indicate we resized
  stats_.Count_ = 0; // reset since we are calling insert

  //  insert data from old table into new table
  for (unsigned i = 0; i < old_limit; ++i)
  {
    OAHTSlot& slot = old_table[i];
    //  we swapped out the tables BECAUSE we are calling insert,
    //  meaning we are reentering the function that called
    //  this function. weird solution though...
    if (slot.State == OAHTSlot::OCCUPIED) insert(slot.Key, slot.Data);
  }

  //  delete the old table now that we have reused all its data.
  delete[] old_table;
}

//>=------------------------------------------------------------------------=<//
/*!
    \brief
      Decides if the current internal array needs to be grown or not.
      Calculates the current load factor and compares against the max allowed.
      If the limit is exceeded, the table can then be grown.
    \return
      Whether the table needs to be grown or not
*/
//>=------------------------------------------------------------------------=<//
template<typename T>
bool OAHashTable<T>::need_growing() const
{
  //  if MaxLF is set to 1.0, we only grow when full
  if (config_.MaxLoadFactor_ == 1.0) return stats_.Count_ == stats_.TableSize_;

  //  LF can be calculated using count / size
  return (static_cast<double>(stats_.Count_ + 1) / stats_.TableSize_) 
         > config_.MaxLoadFactor_; // return if it is exceeded
}

//>=------------------------------------------------------------------------=<//
/*!
    \brief
      Given an index, and assuming the deletion policy is set to "pack",
      pack the elements that are following the previously deleted element.
      Traverse the following elements until we have made a full cycle or until
      an empty slot is encountered, reinserting everything encountered.
    \param index
      index the previously removed element was located
*/
//>=------------------------------------------------------------------------=<//
template<typename T>
void OAHashTable<T>::pack(int index)
{
  //  if we aren't supposed to pack, bail
  if (config_.DeletionPolicy_ != OAHTDeletionPolicy::PACK) return;
  
  int i = index + 1; // starting right after the deleted element
  while (i != index) // stop when a full cycle is completed
  {
    //  break early if we hit an unoccupied slot
    if (table_[i].State == OAHTSlot::UNOCCUPIED) break;

    OAHTSlot& slot = table_[i]; // current slot
    slot.State = OAHTSlot::UNOCCUPIED; // update state
    --stats_.Count_; // decrement count to offset insert
    insert(slot.Key, slot.Data); // reinsert the data

    //  increment i and loop to front of table if needed
    (++i) %= stats_.TableSize_;
  }
}

//>=------------------------------------------------------------------------=<//
/*!
    \brief
      Searches the internal table array for a matching key hash, returning
      an index. Can be used when inserting an element or removing one, 
      as the slot param is used to hold a reference for whichever one is needed.
      Uses linear probing when a collision is found so to find the best 
      appropriate slot to be used.
    \param Key
      The string used to hash the value to find the appropriate slot for
      inserting and removing.
    \param slot
      The pointer-reference to a slot that can be used for insering or removing
    \return
      The index to an element that matches the key parameter. Returns
      DNE (-1) if the element is not in the internal table array.
*/
//>=------------------------------------------------------------------------=<//
template<typename T>
int OAHashTable<T>::index_of(const char* Key, OAHTSlot*& slot) const
{
  unsigned stride = 1; // assume linear probing
  // if (and only if) we are doing double hashing, get the stride/increment
  if (config_.SecondaryHashFunc_)
    //  in the event secondary hash func returns 0, we use size - 1 and add 1
    stride = config_.SecondaryHashFunc_(Key, stats_.TableSize_ - 1) + 1;

  //  store the first index we started at
  const int start = config_.PrimaryHashFunc_(Key, stats_.TableSize_);
  int i = start; // set i to begin at start
  int loc = DNE; // index we are returning, default to -1
  
  //  since the conditional is set to be the starting value,
  //  do-while pleasantly fixes this issue!
  do
  {
    //  increment current number of probes performed
    ++stats_.Probes_;

    //  if the current slot is not occupied
    if (table_[i].State != OAHTSlot::OCCUPIED)
    {
      //  keep track of the first deleted or unoccupied slot found
      if (slot == nullptr) slot = &table_[i];
      //  if we are at an unoccupied slot, stop
      if (table_[i].State == OAHTSlot::UNOCCUPIED) break;
    }
    //  if the current element IS occupied
    else
    {
      //  if the key matches the key stored at this slot
      if (strncmp(table_[i].Key, Key, MAX_KEYLEN) == 0)
      {
        //  we have a match!
        loc = i; // update index we are returning
        slot = &table_[i]; // update the slot we are pointing at
        break; // bail early
      }
    }

    // increment current index, wrap to beginning if needed
    (i += stride) %= stats_.TableSize_;
  } while (i != start); // i == start on first iter, do while helps

  //  DNE is returned if we stopped at an unoccupied slot
  //  otherwise it is the value of i where the key was found
  return loc;
}

//>=------------------------------------------------------------------------=<//
/*!
    \brief
      Calls the client-defined free function if one is provided, and
      sets the state to whatever is appropriate. Updates the internal
      count as well.
    \param slot
      The slot who's internal data we are deleting.
    \param state
      The current state we are updating the flag to. can either be
      DELETED or UNOCCUPIED.
*/
//>=------------------------------------------------------------------------=<//
template<typename T>
void OAHashTable<T>::delete_slot(OAHTSlot& slot, 
  typename OAHTSlot::OAHTSlot_State state)
{
  //  call the free proc if it exists
  if (config_.FreeProc_) config_.FreeProc_(slot.Data);
  //  set the state to deleted or unoccupied
  slot.State = state;
  //  update stats to reflect the deletion
  --stats_.Count_;
}

//>=------------------------------------------------------------------------=<//
/*!
    \brief
      Helper function to call when we cannot find an element the client 
      requested to find. Passes the string parameter for the 
      exceptions description.
    \param what
      The message indicating what went wrong for the client.
*/
//>=------------------------------------------------------------------------=<//
template<typename T>
void OAHashTable<T>::item_not_found(const char* what) const
{
  throw OAHTException(OAHTException::E_ITEM_NOT_FOUND, what);
}