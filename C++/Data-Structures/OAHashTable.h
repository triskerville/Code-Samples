//>=------------------------------------------------------------------------=<//
// file:    OAHashTable.h
// author:  Tristan Baskerville
// course:  CS280
// brief:   
//   This file contains the declaration for the OAHashTable class, along with
//   the OAHashTableException class and the OAHTStats class.
//
//   Public operations for an OAHashTable instance include:
//     + Default Constructor
//     + Destructor
//     + Method to insert an element
//     + Method to remove an element
//     + Method to find an element
//     + Method to clear all elements
//     + Getter for the internal statistics
//     + Getter for the internal table array
//
//   Hours spent on this assignment: ~10
//   Specific portions that gave you the most trouble: 
//     index_of, was also fun seeing how vastly different each iteration was.
//
// Copyright © 2020 DigiPen, All rights reserved.
//>=------------------------------------------------------------------------=<//
#ifndef OAHASHTABLEH
#define OAHASHTABLEH

#include <string>
#include "Support.h"

/*
client-provided hash function: takes a key and table size,
returns an index in the table.
*/
typedef unsigned (*HASHFUNC)(const char *, unsigned);

//! Max length of our "string" keys
const unsigned MAX_KEYLEN = 32;

//! The exception class for the hash table
class OAHashTableException
{
  private:  
    int error_code_;      //!< Code for the exception
    std::string message_; //!< Readable string describing the exception

  public:
    /*
      Non-Default Constructor

      \param ErrCode
        what type of exception this is

      \param Message
        the associated message to the exception
    */
    OAHashTableException(int ErrCode, const std::string& Message) :
        error_code_(ErrCode), message_(Message) {};

    //! Destructor
    virtual ~OAHashTableException() {
    }

    /*
      Retrieves exception code

      \return
        One of: E_ITEM_NOT_FOUND, E_DUPLICATE, E_NO_MEMORY
    */
    virtual int code() const { 
      return error_code_; 
    }

    /*
      Retrieve human-readable string describing the exception

      \return
        The description of the exception
    */
    virtual const char *what() const {
      return message_.c_str();
    }
    //! Possible exception conditions
    enum OAHASHTABLE_EXCEPTION {E_ITEM_NOT_FOUND, E_DUPLICATE, E_NO_MEMORY};
};

//! The policy used during a deletion
enum OAHTDeletionPolicy {MARK, PACK};

//! OAHashTable statistical info
struct OAHTStats
{
  //! Default constructor
  OAHTStats() : Count_(0), TableSize_(0), Probes_(0), Expansions_(0),
                    PrimaryHashFunc_(0), SecondaryHashFunc_(0) {};
  unsigned Count_;             //!< Number of elements in the table
  unsigned TableSize_;         //!< Size of the table (total slots)
  unsigned Probes_;            //!< Number of probes performed
  unsigned Expansions_;        //!< Number of times the table grew
  HASHFUNC PrimaryHashFunc_;   //!< Pointer to primary hash function
  HASHFUNC SecondaryHashFunc_; //!< Pointer to secondary hash function
};

//! Hash table definition (open-addressing)
template <typename T>
class OAHashTable
{
  public:

    typedef void (*FREEPROC)(T); //!< client-provided free proc (we own the data)

    //! Configuration for the hash table
    struct OAHTConfig
    {
      /*
        Constructor

        \param InitialTableSize
          starting size of table array

        \param PrimaryHashFunc
          client defined callback for primary hashing

        \param SecondaryHashFunc
          client defined callback for extra hashes

        \param MaxLoadFactor
          largest ratio of in use to size allowed

        \param GrowthFactor
          size at which to grow the internal array

        \param Policy
          how are elements deleted

        \param FreeProc
          client defined callback for deleting extra memory
      */
      OAHTConfig(unsigned InitialTableSize, 
                 HASHFUNC PrimaryHashFunc, 
                 HASHFUNC SecondaryHashFunc = 0,
                 double MaxLoadFactor = 0.5,
                 double GrowthFactor = 2.0, 
                 OAHTDeletionPolicy Policy = PACK,
                 FREEPROC FreeProc = 0) :

        InitialTableSize_(InitialTableSize), PrimaryHashFunc_(PrimaryHashFunc), 
        SecondaryHashFunc_(SecondaryHashFunc), MaxLoadFactor_(MaxLoadFactor), 
        GrowthFactor_(GrowthFactor), DeletionPolicy_(Policy),
        FreeProc_(FreeProc) {}

      unsigned InitialTableSize_;         //!< The starting table size
      HASHFUNC PrimaryHashFunc_;          //!< First hash function
      HASHFUNC SecondaryHashFunc_;        //!< Hash function to resolve collisions
      double MaxLoadFactor_;              //!< Maximum LF before growing
      double GrowthFactor_;               //!< The amount to grow the table
      OAHTDeletionPolicy DeletionPolicy_; //!< MARK or PACK
      FREEPROC FreeProc_;                 //!< Client-provided free function
    };
      
      //! Slots that will hold the key/data pairs
    struct OAHTSlot
    {
      //! The 3 possible states the slot can be in
      enum OAHTSlot_State {OCCUPIED, UNOCCUPIED, DELETED};

      char Key[MAX_KEYLEN]; //!< Key is a string
      T Data;               //!< Client data
      OAHTSlot_State State; //!< The state of the slot
      int probes;           //!< For testing
    };

    OAHashTable(const OAHTConfig& Config); // Constructor
    ~OAHashTable();                        // Destructor

      // Insert a key/data pair into table. Throws an exception if the
      // insertion is unsuccessful.
    void insert(const char *Key, const T& Data);

      // Delete an item by key. Throws an exception if the key doesn't exist.
      // Compacts the cluster by reinserting key/data pairs, if necessary (PACK)
    void remove(const char *Key);

      // Find and return data by key. Throws an exception (E_ITEM_NOT_FOUND)
      // if not found.
    const T& find(const char *Key) const;

      // Removes all items from the table (Doesn't deallocate table)
    void clear();

      // Allow the client to peer into the data
    OAHTStats GetStats() const;
    const OAHTSlot *GetTable() const;

  private:
    typedef OAHashTableException OAHTException; //!< shorthand for my use
    static const int DNE = -1; //!< signifies an element does not exist

    OAHTSlot* allocate_table(unsigned size);
    void init_slot(OAHTSlot& slot, const char* key, const T& data);
    void set_key(char* slot_key, const char* string_key);

    //  Expands the table when the load factor reaches a certain point
    //  (greater than MaxLoadFactor) Grows the table by GrowthFactor,
    //  making sure the new size is prime by calling GetClosestPrime
    void grow_table();
    bool need_growing() const;
    void pack(int index);

    //  Workhorse method to locate an item (if it exists)
    //  Returns the index of the item in the table
    //  Sets Slot to point to the slot in the table where it belongs 
    //  Returns -1 if it's not in the table
    int index_of(const char *Key, OAHTSlot* &Slot) const;
    void delete_slot(OAHTSlot& slot, typename OAHTSlot::OAHTSlot_State state);

    void item_not_found(const char*) const;

    const OAHTConfig config_; //!< configuration setup for the hash table
    mutable OAHTStats stats_; //!< tracks statistics related to the hash table
    OAHTSlot* table_; //!< internal table array holding key and data pairs
};

//  We are using templates and the function definitions must be in this file.
#include "OAHashTable.cpp"

#endif
