#include <db/BufferPool.hpp>
#include <db/Database.hpp>
#include <numeric>

using namespace db;

BufferPool::BufferPool()
// TODO pa1: add initializations if needed
{
  // TODO pa1: additional initialization if needed
}

BufferPool::~BufferPool() {
  // TODO pa1: flush any remaining dirty pages
  for(int i=0; i<page_ids.size(); i++) {
    flushPage(page_ids[i]);
  }

  //clear the pages, page ids and the is_dirty vectors
  pages.clear();
  page_ids.clear();
  is_dirty.clear();

}

Page &BufferPool::getPage(const PageId &pid) {
  // TODO pa1: If already in buffer pool, make it the most recent page and return it

  for(int i=0; i<pages.size(); i++) {

    //finds the page correspondinf to the given page id
    if(page_ids[i] == pid) {

      Page p = std::move(pages[i]);
      PageId p_id = std::move(page_ids[i]);
      bool dirty_page = std::move(is_dirty[i]);

      pages.erase(pages.begin()+i);
      page_ids.erase(page_ids.begin()+i);
      is_dirty.erase(is_dirty.begin()+i);

      //makes it the most recent page in the list and returns it
      pages.insert(pages.begin(), p);
      page_ids.insert(page_ids.begin(), p_id);
      is_dirty.insert(is_dirty.begin(), dirty_page);

      return p;
    }
  }

  // TODO pa1: If there are no available pages, evict the least recently used page. If it is dirty, flush it to disk

  if(pages.size() == DEFAULT_NUM_PAGES) {

    //flushes the page if it is dirty
    if(is_dirty[pages.size()-1]) {
      flushPage(page_ids[pages.size()-1]);
    }

    pages.pop_back();
    page_ids.pop_back();
    is_dirty.pop_back();
  }

  // TODO pa1: Read the page from disk to one of the available slots, make it the most recent page

  const Database &db = getDatabase();
  const DbFile &file = db.get(pid.file);
  Page page;
  file.readPage(page, pid.page);

  pages.insert(pages.begin(), page);
  page_ids.insert(page_ids.begin(), pid);
  is_dirty.insert(is_dirty.begin(), 0);

  return page;

}

void BufferPool::markDirty(const PageId &pid) {
  // TODO pa1: Mark the page as dirty. Note that the page must already be in the buffer pool
  bool flag=true;

  for(int i=0; i<pages.size(); i++) {

    //marks the page with page id as dirty
    if(page_ids[i] == pid) {
      is_dirty[i] = true;
      flag = false;
      break;
    }
  }

  if(flag) {
    throw std::logic_error("page doesn't exist in bufferpool");
  }

}

bool BufferPool::isDirty(const PageId &pid) const {
  // TODO pa1: Return whether the page is dirty. Note that the page must already be in the buffer pool

  //checks if given page is dirty
  for(int i=0; i<pages.size(); i++) {

    if(page_ids[i] == pid) {
      return is_dirty[i];
    }
  }

  throw std::logic_error("page doesn't exist in bufferpool");

}

bool BufferPool::contains(const PageId &pid) const {
  // TODO pa1: Return whether the page is in the buffer pool

  //checks if the bufferpool contains the page
  for(int i=0; i<pages.size(); i++) {
    if(page_ids[i] == pid) {
      return true;
    }
  }
  return false;
}

void BufferPool::discardPage(const PageId &pid) {
  // TODO pa1: Discard the page from the buffer pool. Note that the page must already be in the buffer pool

  bool flag=true;

  for(int i=0; i<pages.size(); i++) {

    //discards the page with the given page id
    if(page_ids[i] == pid) {

      pages.erase(pages.begin()+i);
      page_ids.erase(page_ids.begin()+i);
      is_dirty.erase(is_dirty.begin()+i);
      flag=false;

      break;
    }
  }

  if(flag) {
    throw std::logic_error("page doesn't exist in bufferpool");
  }
}

void BufferPool::flushPage(const PageId &pid) {
  // TODO pa1: Flush the page to disk. Note that the page must already be in the buffer pool

  bool flag=true;

  for(int i=0; i<pages.size(); i++) {

    if(page_ids[i] == pid) {

      if(is_dirty[i]) {
        const Database &db = getDatabase();
        const DbFile &file = db.get(pid.file);

        Page page = std::move(pages[i]);
        file.writePage(page, pid.page);

        pages[i] = std::move(page);
        is_dirty[i] = false;
      }

      flag = false;
    }
  }

  if(flag) {
    throw std::logic_error("page doesn't exist in bufferpool");
  }
}

void BufferPool::flushFile(const std::string &file) {
  // TODO pa1: Flush all pages of the file to disk
  for (int i=0; i<pages.size(); i++) {

    if(page_ids[i].file == file) {
      flushPage(page_ids[i]);
    }
  }

}
