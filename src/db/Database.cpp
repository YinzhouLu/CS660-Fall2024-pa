#include <db/Database.hpp>
#include <iostream>

using namespace db;

BufferPool &Database::getBufferPool() { return bufferPool; }

Database &db::getDatabase() {
  static Database instance;
  return instance;
}


void Database::add(std::unique_ptr<DbFile> file) {
  // TODO pa1: add the file to the catalog. Note that the file must not exist.

  //Check if file exists in the catalog of files.
  std::string fname = file->getName();

  if(files.contains(fname)) {
    throw std::runtime_error("Database file already exists");
  }

  //if file doesn't exist in the catalog, add it to the catalog
  files[fname] = std::move(file);
}

std::unique_ptr<DbFile> Database::remove(const std::string &name) {
  // TODO pa1: remove the file from the catalog. Note that the file must exist.

  //check if file exists in catalog
  if(files.contains(name)) {

    Database &db = getDatabase();
    std::unique_ptr<DbFile> file = std::move(files[name]);

    //flush the file to disk
    db.getBufferPool().flushFile(name);

    //remove file from catalog
    files.erase(name);

    return file;
  }

  throw std::logic_error("File doesn't exist");


}

DbFile &Database::get(const std::string &name) const {
  // TODO pa1: get the file from the catalog. Note that the file must exist.


  for (const auto & [ key, value ] : files) {

    //check if file exists in catalog
    if(key == name) {
      //return the database file associated with it
      return *value;
    }
  }

  throw std::logic_error("File doesn't exist");
}