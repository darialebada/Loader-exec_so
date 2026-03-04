## Executable Loader

### File Hierarchy
- **skel-lin** – contains the provided code skeleton
- **checker-lin** – contains the checker used for testing on Linux

### Implementation
The implementation can be found in: **skel-lin/loader/loader.c**

---

## Implementation Description

The goal of this project is to implement a **loader for ELF executable files on Linux**, provided as a **dynamic library**.

A **custom handler function** is implemented to intercept and handle **page faults**. The first step is identifying the **segment and page** containing the address that triggered the page fault.

Three possible cases are handled:

1. **The page is already mapped**, or the page does not belong to any segment  
   → the **default handler** is executed.

2. **The page belongs to a segment but has not yet been mapped**  
   → the page must be mapped manually.

In this case, the address of the page that caused the page fault is determined and the page is mapped with **read and write permissions** in order to allow modification of its contents.

Next, the loader determines how the data should be copied into memory:

- **Page entirely inside the file-backed memory region**  
  → the data is copied from the executable file.

- **Page completely outside the file-backed memory region**  
  → the page is fully **zero-initialized**.

- **Page partially inside the file-backed memory region**  
  → the data from the file-backed region is copied, and the remaining part of the page is **zero-initialized**.

Finally, the permissions of the mapped page are updated to match the **permissions defined in the executable file**.
