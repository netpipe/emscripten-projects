# fat12-fat16-manager

Tiny command line tool which lets you do some trivial operations (receive, send, delete, recover files) within a FAT12/FAT16 disk image.

## How-to-use:

### 1. Build and run:

  * Method 1
  ```
  $ make default
  $ ./fat <./test-img/image_name.img>
  >_ ...
  >_ ...doing stuff... (see help for info)
  >_ ...
  ```
      
  * Method 2
  ```
  $ make default
  $ ln -s fat <func_name>
  $ ./<func_name> <./test-img/image_name.img> [file_name]
  ```
  
  Where `func_name` can be one of: `recv`, `send`, `del`, `undel` or `showdir`.
      
### 2. Clean project:

  ```
  $ make clean
  ```
  
### 3. Get help:

  ```
  $ ./fat help
  ```
