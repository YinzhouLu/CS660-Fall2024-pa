The key methods implemented in the BufferPool class include:

getPage(const PageId &pid): Retrieves a page from the buffer pool or loads it from disk if not present.
markDirty(const PageId &pid): Marks a page in the buffer pool as dirty, indicating it has been modified.
isDirty(const PageId &pid) const: Checks if a page in the buffer pool is dirty.
contains(const PageId &pid) const: Checks if a page is present in the buffer pool.
discardPage(const PageId &pid): Removes a page from the buffer pool.
flushPage(const PageId &pid): Writes a dirty page back to disk and marks it as clean.
flushFile(const std::string &file): Flushes all dirty pages of a specific file to disk.

Key Design Decisions

1. Page Storage and Management
   Vectors for Storage: Pages, their corresponding IDs, and dirty flags are stored in parallel std::vectors (pages, page_ids, is_dirty). This approach simplifies the association between pages and their metadata.
2. Error Handling
   Consistent Exception Messages: Clear and consistent exception messages are provided when a page is not found in the buffer pool.
   Ensuring Page Existence: Methods that require a page to be present in the buffer pool check for its existence and handle errors appropriately.
   Detailed Explanation of Methods
   Destructor: ~BufferPool()
   Flushes all dirty pages to disk upon destruction.
   Iterates over page_ids using a range-based for loop and calls flushPage on each.
   getPage(const PageId &pid)
   Page Retrieval:
   Checks if the page is already in the buffer pool.
   If found, moves it to the front of the vectors to mark it as most recently used and returns it.
   If not found, proceeds to load the page from disk.
   Page Loading and Eviction:
   If the buffer pool is full, evicts the least recently used page.
   Calls flushPage if the page is dirty.
   Removes the page and its metadata from the vectors.
   Reads the page from disk using file.readPage and inserts it at the front of the vectors.

Performance Optimization: Alternative data structures (e.g., std::unordered_map for faster lookups) could be considered.

Time Spent and Challenges Faced

Time Investment：around 35hr

Learning C++: The first 10 hours were dedicated to learning C++, as this was the first time working with the language.
Familiarized with syntax, pointers, references, and class structures.

Challenges：The Database::remove function flushes any dirty pages associated with the file before it is removed.

Leaving pages associated with a removed file in the buffer pool can lead to several issues:

Data Inconsistency:

Stale Pages: The buffer pool may contain stale pages that reference the old version of the file. If the file is re-added to the database (possibly with different content), the buffer pool might serve outdated pages to transactions, causing them to operate on incorrect data.
Incorrect Reads/Writes: Subsequent read or write operations might access or modify these outdated pages, leading to data corruption or unexpected behavior.
Buffer Pool Pollution:

Wasted Resources: Retaining pages of a non-existent file consumes buffer pool space unnecessarily, reducing the available memory for other active files and potentially degrading performance.
Possible Bug – Flush Inconsistency:

Flush Operations: If the buffer pool retains dirty pages of a removed file and later attempts to flush them, it might result in writing data to a file that no longer exists or has been re-created differently. This can cause crashes or corruption of the new file.
Key Collision: Since the buffer pool uses the file name as a key for retrieving pages, re-adding a file with the same name but different content could lead to conflicts between old dirty pages and new pages.


