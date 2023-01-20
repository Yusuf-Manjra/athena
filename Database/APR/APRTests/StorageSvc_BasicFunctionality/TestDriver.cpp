/*
  Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration
*/

#include "TestDriver.h"
#include "SimpleTestClass.h"

#include "libname.h"

#include "PersistentDataModel/Guid.h"
#include "PersistentDataModel/Token.h"

#include "StorageSvc/Shape.h"
#include "StorageSvc/IStorageSvc.h"
#include "StorageSvc/IStorageExplorer.h"
#include "StorageSvc/DbSelect.h"
#include "StorageSvc/DbReflex.h"
#include "StorageSvc/DatabaseConnection.h"
#include "StorageSvc/FileDescriptor.h"
#include "StorageSvc/DbType.h"
#include "StorageSvc/DbString.h"

#include <stdexcept>
#include <iostream>
#include <sstream>
#include <memory>

using namespace std;


TestDriver::TestDriver()
{ }


TestDriver::~TestDriver()
{
   std::cout << "[OVAL] Number of floating tokens : " << Token::numInstances() << std::endl;
}

void
TestDriver::testWriting()
{
   cout << "createStorageSvc" << endl;
   pool::IStorageSvc* storSvc = pool::createStorageSvc("StorageSvc");
   if ( ! storSvc ) {
      throw std::runtime_error( "Could not create a StorageSvc object" );
   }
   storSvc->addRef();
   cout << "startSession" << endl;
   pool::Session* sessionHandle = 0;
   if ( ! ( storSvc->startSession( pool::RECREATE, m_storageType.type(), sessionHandle ).isSuccess() ) ) {
      throw std::runtime_error( "Could not start a session." );
   }

   cout << "Session connect" << endl;
   pool::FileDescriptor fd( m_filename, m_filename );
   if ( ! ( storSvc->connect( sessionHandle, pool::RECREATE, fd ).isSuccess() ) ) {
      throw std::runtime_error( "Could not start a connection." );
   }
   pool::DatabaseConnection* connection = fd.dbc();

  // Retrieve the dictionary
  RootType class_SimpleTestClass ( "SimpleTestClass" );
  if ( ! class_SimpleTestClass ) {
    throw std::runtime_error( "Could not retrieve the dictionary for class SimpleTestClass" );
  }
  Guid objGuid = pool::DbReflex::guid(class_SimpleTestClass);
  // Creating class shape
  const pool::Shape* objShape = 0;
  storSvc->createShape( fd, m_objContainerName, objGuid, objShape );
  if( !objShape ) throw std::runtime_error( "Could not create a persistent shape for SimpleTestClass" );
  // object cache
  std::vector< SimpleTestClass* > myObjects;
  myObjects.reserve(m_nObjects);

  RootType class_DbString ( "pool::DbString" );
  if( ! class_DbString ) {
    throw std::runtime_error( "Could not retrieve the dictionary for class DbString" );
  }
  Guid strGuid = pool::DbReflex::guid(class_DbString);
  const pool::Shape* strShape = 0;
  storSvc->createShape( fd, m_strContainerName, strGuid, strShape );
  if( !strShape ) throw std::runtime_error( "Could not create a persistent shape for DbString" );
  std::vector< pool::DbString > myStrings;
  myStrings.reserve(m_nObjects);
  
  if( !class_SimpleTestClass.Properties().HasProperty( "ClassID" ) ) {
     std::ostringstream error;
     error << "There is no ClassID property for class \"SimpleTestClass\"" << std::ends;
     throw std::runtime_error( error.str() );
  }

  for ( int i = 0; i < m_nObjects; ++i ) {
    myObjects.push_back( new SimpleTestClass() );
    SimpleTestClass* myObject = myObjects.back();
    myObject->data = i;

    // Writing the object.
    Token* token = nullptr;
    if ( ! ( storSvc->allocate( fd, m_objContainerName, m_storageType.type(),
				myObject, objShape, token ).isSuccess() ) ) {
      throw std::runtime_error( "Could not write a SimpleTestClass object" );
    }
    delete token; token = nullptr;

    myStrings.push_back( "This is my test DbString #" + std::to_string(i) );
    const pool::DbString* sp = &myStrings.back();    
    if( !storSvc->allocate( fd, m_strContainerName, m_storageType.type(),
                            sp, strShape, token ).isSuccess() ) {
       throw std::runtime_error( "Could not write a dbString" );
    }
    delete token; token = nullptr;
    
    if( m_commitEveryRow ) {
       if ( ! ( storSvc->endTransaction( connection, pool::Transaction::TRANSACT_COMMIT ).isSuccess() ) ) {
          throw std::runtime_error( "Could not end a transaction." );
       }
    }
  }
  if( !m_commitEveryRow ) {
     // Closing the transaction.
     if ( ! ( storSvc->endTransaction( connection, pool::Transaction::TRANSACT_COMMIT ).isSuccess() ) ) {
        throw std::runtime_error( "Could not end a transaction." );
     }
  }
  // Clearing the cache
  for ( std::vector< SimpleTestClass* >::iterator iObject = myObjects.begin();
	iObject != myObjects.end(); ++iObject ) delete *iObject;
  myObjects.clear();

  if ( ! ( storSvc->disconnect( fd ).isSuccess() ) ) {
    throw std::runtime_error( "Could not disconnect." );
  }
  if ( ! ( storSvc->endSession( sessionHandle ).isSuccess() ) ) {
    throw std::runtime_error( "Could not end correctly the session." );
  }
  storSvc->release();
}


void
TestDriver::testReading()
{
  pool::IStorageSvc* storSvc = pool::createStorageSvc("StorageSvc");
  if ( ! storSvc ) {
    throw std::runtime_error( "Could not create a StorageSvc object" );
  }
  storSvc->addRef();
  void* pVoid = 0;
  pool::DbStatus sc = storSvc->queryInterface( pool::IStorageExplorer::interfaceID(), &pVoid );
  pool::IStorageExplorer* storageExplorer = (pool::IStorageExplorer*)pVoid;
  if ( !( sc == pool::DbStatus::Success && storageExplorer ) ) {
    storSvc->release();
    throw std::runtime_error( "Could not retrieve a IStorageExplorer interface" );
  }

  pool::Session* sessionHandle = 0;
  if ( ! ( storSvc->startSession( pool::READ, m_storageType.type(), sessionHandle ).isSuccess() ) ) {
    throw std::runtime_error( "Could not start a session." );
  }

  pool::FileDescriptor* fd = new pool::FileDescriptor( m_filename, m_filename );
  sc = storSvc->connect( sessionHandle, pool::READ, *fd );
  if ( sc != pool::DbStatus::Success ) {
    throw std::runtime_error( "Could not start a connection." );
  }

  // Fetch the containers
  std::vector<const Token*> containerTokens;
  storageExplorer->containers( *fd, containerTokens );
  if ( containerTokens.size() != 2 ) {
    throw std::runtime_error( "Unexpected number of containers" );
  }

  const Token* objContToken = nullptr;
  for( const Token* containerToken : containerTokens ) {
     if( containerToken->contID() == m_objContainerName ) {
        objContToken = containerToken;
        break;
     }
  }
  if( !objContToken )  throw std::runtime_error( "Could not find the main Container in the DB");

  const Token* strContToken = nullptr;
  for( const Token* containerToken : containerTokens ) {
     if( containerToken->contID() == m_strContainerName ) {
        strContToken = containerToken;
        break;
     }
  }
  if( !strContToken )  throw std::runtime_error( "Could not find the String container in the DB");


  // Fetch the objects in the container.
  pool::DbSelect selectionObject("");
  sc = storageExplorer->select( *fd, m_objContainerName, selectionObject );
  int iObject = 0;
  if ( sc.isSuccess() ) {
    Token* objectToken = 0;
    while ( storageExplorer->next( selectionObject, objectToken ).isSuccess() ) {
      const Guid& guid = objectToken->classID();
      const pool::Shape* shape = 0;
      if ( storSvc->getShape( *fd, guid, shape ) != pool::IStorageSvc::IS_PERSISTENT_SHAPE ) {
	throw std::runtime_error( "Could not fetch the persistent shape" );
      }
      RootType classType = pool::DbReflex::forGuid(guid);
      if(!classType){
	throw std::runtime_error( "Could not resolve the class by guid" );
      }
      void* ptr = 0;
      if ( ! ( storSvc->read( *fd, *objectToken, shape, &ptr ) ).isSuccess() ) {
	throw std::runtime_error( "failed to read an object back from the persistency" );
      }

      if ( shape->shapeID().toString() != "4E1F4DBB-1973-1974-1999-204F37331A01" ) {
	throw std::runtime_error( std::string("read wrong class type: ") + shape->shapeID().toString());
      }

      SimpleTestClass* object = reinterpret_cast< SimpleTestClass* >( ptr );
      if ( object->data != iObject ) {
         throw std::runtime_error( "Object read different from object written!   Obj data=" + std::to_string(object->data) );
      }
      delete object;
      ++iObject;
      objectToken->release();
    }
  }
  if ( iObject != m_nObjects ) {
     throw std::runtime_error( std::string("Read ") + std::to_string(iObject) + " instead of " + std::to_string(m_nObjects));
  }

  // Read the Strings
  sc = storageExplorer->select( *fd, m_strContainerName, selectionObject );
  if( sc.isSuccess() ) {
     Token* stringToken = nullptr;
     while( storageExplorer->next( selectionObject, stringToken ).isSuccess() ) {
        const Guid& guid = stringToken->classID();
        const pool::Shape* shape = 0;
        if ( storSvc->getShape( *fd, guid, shape ) != pool::IStorageSvc::IS_PERSISTENT_SHAPE ) {
           throw std::runtime_error( "Could not retrieve the String shape" );
        }
        RootType classType = pool::DbReflex::forGuid(guid);
        if(!classType){
           throw std::runtime_error( "Could not resolve the class by guid" );
        }
        DbString str;
        void *ptr = &str;
        if( !storSvc->read( *fd, *stringToken, shape, &ptr ).isSuccess() ) {
           throw std::runtime_error( "failed to read an object back from the persistency" );
        }

        if ( shape->shapeID().toString() != "DA8F479C-09BC-49D4-94BC-99D025A23A3B" ) {
           throw std::runtime_error( std::string("read wrong class type: ") + shape->shapeID().toString());
        }

        cout << "Read back string: " << str << endl;
        stringToken->release();
     }
  }

  
  if ( ! ( storSvc->disconnect( *fd ).isSuccess() ) ) {
    throw std::runtime_error( "Could not disconnect." );
  }
  delete fd;

  std::cout << "Closing the session" << std::endl;
  if ( ! ( storSvc->endSession( sessionHandle ).isSuccess() ) ) {
    throw std::runtime_error( "Could not end correctly the session." );
  }
  storageExplorer->release();
  storSvc->release();
}
