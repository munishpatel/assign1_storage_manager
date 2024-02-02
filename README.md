
**Storage Manager CS525 Advanced Database Organization Assignment No. 1**

----------------------------------------------------

***Authors***
Assignment 1
Group 27. Spring 2024 

- Munish Patel 
- Rutvik Savaliya
- Krima Patel


----------------------------------------------------

***Steps to test the storage manager***

----------------------------------------------------

We have added all the tests for assignment 1 in the same test file `test_assign1_1.c`. Following make command compiles, links and runs the code seamlessly. Output of the code is an executable `Group27_output ` file.
```
$ make
```

To execute the code, and get the output use below command. 
```
$ ./test_assign1
```

Before using above command to retest the storage manager, Use below command to cleanup any object files and output files
```
$ make clean
```
Output after you execute the Group27_output:

```
[test_assign1_1.c-test single page content-L94-03:27:10] OK: expected true: character in page read from disk is the one we expected.
[test_assign1_1.c-test single page content-L94-03:27:10] OK: expected true: character in page read from disk is the one we expected.
[test_assign1_1.c-test single page content-L94-03:27:10] OK: expected true: character in page read from disk is the one we expected.
[test_assign1_1.c-test single page content-L94-03:27:10] OK: expected true: character in page read from disk is the one we expected.
reading first block
[test_assign1_1.c-test single page content-L104-03:27:10] OK: finished test
```

----------------------------------------------------

***Functionalities***

----------------------------------------------------

****Manipulate page file****

`createPageFile()`: Make a new page file that is PAGE_SIZE .The file is opened in write mode and has '0' bytes inside it.

`openPageFile()` :If the input file is present, it opens it; otherwise, it returns the error RC_FILE_NOT_FOUND. specifying the total number of pages throughout. We set the file name, overall page count, and current page position in the file handler to continue usage after the file is opened.
      
`closePageFile()` : Closes a file with open pages. If the file is successfully closed, it returns RC_OK.

`destroyPageFile()` : Remove or destroy a page file. RC_OK is returned if the file is successfully deleted.

****Read functions****

`readBlock()` : Read nth block from the file.

`getBlockPos()` : In the page file, return the current page position.

`readFirstBlock()` : Reads the 1st block in the page file.

`readPreviousBlock()` : Reads the page file's preceding block and sets the current page position to that block.

`readCurrentBlock()` : Reads the most recent block and sets the page position to that block.

`readNextBlock()` : Reads next block and sets the current page position to the next block.

`readLastBlock()` : Reads the last block of the file and sets the current page position to the last block.

****Write Functions****

`writeBlock()` : Changes the current page position to the page number after writing the block at position (pageNum). Updates the overall amount of pages as well.

`writeCurrentBlock()` : Writes the current block using current page position

`appendEmptyBlock()` : Appends a new empty block with null bytes in the file
    
`ensureCapacity()` : Checks to see if the file's capacity is equal to the specified number of pages. If not, add n-new blank blocks to the page to fill the available space. Please take note that "w" represents the current page capacity and "n" represents the necessary page capacity.
