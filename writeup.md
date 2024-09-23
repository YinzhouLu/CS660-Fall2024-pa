Leaving pages associated with a removed file in the buffer pool can lead to several issues:

Data Inconsistency:

Stale Pages: The buffer pool may contain stale pages that reference the old version of the file. If the file is re-added to the database (possibly with different content), the buffer pool might serve outdated pages to transactions, causing them to operate on incorrect data.
Incorrect Reads/Writes: Subsequent read or write operations might access or modify these outdated pages, leading to data corruption or unexpected behavior.
Buffer Pool Pollution:

Wasted Resources: Retaining pages of a non-existent file consumes buffer pool space unnecessarily, reducing the available memory for other active files and potentially degrading performance.
Possible Bug – Flush Inconsistency:

Flush Operations: If the buffer pool retains dirty pages of a removed file and later attempts to flush them, it might result in writing data to a file that no longer exists or has been re-created differently. This can cause crashes or corruption of the new file.
Key Collision: Since the buffer pool uses the file name as a key for retrieving pages, re-adding a file with the same name but different content could lead to conflicts between old dirty pages and new pages.