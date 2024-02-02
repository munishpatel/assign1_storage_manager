#include<stdio.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<unistd.h>
#include<string.h>
#include<math.h>

#include "storage_mgr.h"

FILE *current_page;

extern void initStorageManager (void) {
	// Initialized file pointer in storage manager
	current_page = NULL;
} 


extern RC createPageFile (char *fileName) {
	// Opened a file usinf fopen functions with file name and w+ functionality to get access of reading and writing together.
	current_page = fopen(fileName, "w+");

	// Condition to check the file is opened or not
	if(current_page == NULL) {
		return RC_FILE_NOT_FOUND;
	} else {
		// Creating an empty page in memory with the size of page.
		SM_PageHandle blank_page = (SM_PageHandle)calloc(PAGE_SIZE, sizeof(char));
		
		// Writing empty page to file.
		if(fwrite(blank_page, sizeof(char), PAGE_SIZE,current_page) < PAGE_SIZE)
			printf("writting fails \n");
		else
			printf("writting succeeded \n");
		
		// Closing file stream to delete all the buffers. 
		fclose(current_page);
		
		// De-allocating the memory using free function for previously allocated to 'blank_page'.
		//It is a good practise to do memory managment.
		free(blank_page);
		
		return RC_OK;
	}
}

extern RC openPageFile (char *fileName, SM_FileHandle *file_handler) {
	// Opened a file for reading mode only using r.
	current_page = fopen(fileName, "r");

	// Condition to check that file successfully opend or not.
	if(current_page == NULL) {
		return RC_FILE_NOT_FOUND;
	} else { 
		// Update the file_handler's filename variable to target the reading file with set the current position 0.
		file_handler->fileName = fileName;
		file_handler->curPagePos = 0;

		/* In order to calculate the total size, we perform following steps -
		   1. Move the position of the file stream to the end of file
		   2. Check the file end position
		   3. Move the position of the file stream to the beginning of file  
		
		fseek(current_page, 0L, SEEK_END);
		int totalSize = ftell(current_page);
		fseek(current_page, 0L, SEEK_SET);
		file_handler->totalNumPages = totalSize/ PAGE_SIZE;  */
		
		/* Using fstat() to get the file total size.
		   fstat() is a system call that is used to determine information about a file based on its file descriptor.
		   'st_size' member variable of the 'stat' structure gives the total size of the file in bytes.
		*/

		struct stat file_info;
		if(fstat(fileno(current_page), &file_info) < 0)    
			return RC_ERROR;
		file_handler->totalNumPages = file_info.st_size/ PAGE_SIZE;

		// Closing file stream so that all the buffers are flushed. 
		fclose(current_page);
		return RC_OK;
	}
}

extern RC closePageFile (SM_FileHandle *file_handler) {
	// Condition to chekc the storage manager pointer is initialized if it is, then close.
	if(current_page != NULL)
		current_page = NULL;	
	return RC_OK; 
}


extern RC destroyPageFile (char *fileName) {
	// Opened a file using fopen function to read only.
	current_page = fopen(fileName, "r");
	
	// condition to check the file is opened or not.
	if(current_page == NULL)
		return RC_FILE_NOT_FOUND; 
	
	// Deleting the given filename so that it is no longer accessible.	
	remove(fileName);
	return RC_OK;
}

extern RC readBlock (int page_number, SM_FileHandle *file_handler, SM_PageHandle memory_page) {
	// Condition to check the page_number parameter is less than Total number of pages and less than 0, then return respective error code
	if (page_number > file_handler->totalNumPages || page_number < 0)
        	return RC_READ_NON_EXISTING_PAGE;

	// Opening file stream in read mode. 'r' mode creates an empty file for reading only.	
	current_page = fopen(file_handler->fileName, "r");

	// conditioned to check if file was successfully opened.
	if(current_page == NULL)
		return RC_FILE_NOT_FOUND;
	
	// Set the cursor(pointer) position to the file stream. Position is calculated by Page Number x Page Size
	// And if fseek() return 0 then it will not give any error.
	int islooksucess = fseek(current_page, (page_number * PAGE_SIZE), SEEK_SET);
	if(islooksucess == 0) {
		// reading the content and storing it in the location pointed out by memory_page.
		fread(memory_page, sizeof(char), PAGE_SIZE, current_page);
	} else {
		return RC_READ_NON_EXISTING_PAGE; 
	}
    	
	// Set the current page position to the cursor(pointer) position of the file stream
	file_handler->curPagePos = ftell(current_page); 
	
	// Closed the file stream to delete the all buffers     	
	fclose(current_page);
	
    	return RC_OK;
}

extern int getBlockPos (SM_FileHandle *file_handler) {
	// Return the current page position retrieved from the file handle	
	return file_handler->curPagePos;
}

extern RC readFirstBlock (SM_FileHandle *file_handler, SM_PageHandle memory_page) {
	// Re-directed (passing) to readBlock(...) function with pageNumber = 0 in the first block	
	return readBlock(0, file_handler, memory_page);
}

extern RC readPreviousBlock (SM_FileHandle *file_handler, SM_PageHandle memory_page) {
	//printf("CURRENT PAGE POSITION = %d \n", file_handler->curPagePos);
	//printf("TOTAL PAGES = %d \n", file_handler->totalNumPages);

	// Calculate current page number by dividing page size by current page position	
	int current_page_position = file_handler->curPagePos / PAGE_SIZE;

	// Re-directed (passing) to readBlock(...) function with pageNumber = currentPagePosition - 1 i.e. previous block
	return readBlock(current_page_position - 1, file_handler, memory_page);
}

extern RC readCurrentBlock (SM_FileHandle *file_handler, SM_PageHandle memory_page) {
	// Calculate current page number by dividing page size by current page position	
	int current_page_position = file_handler->curPagePos / PAGE_SIZE;
	
	// Re-directed (passing) to readBlock(...) function with pageNumber = currentPagePosition i.e. current block
	return readBlock(current_page_position, file_handler, memory_page);
}

extern RC readNextBlock (SM_FileHandle *file_handler, SM_PageHandle memory_page){
	// Calculate current page number by dividing page size by current page position	
	int current_page_position = file_handler->curPagePos / PAGE_SIZE;
	
	printf("CURRENT PAGE POSITION = %d \n", file_handler->curPagePos);
	// Re-directed (passing) to readBlock(...) function with pageNumber = currentPagePosition + 1 i.e. last block
	return readBlock(current_page_position + 1, file_handler, memory_page);
}

extern RC readLastBlock (SM_FileHandle *file_handler, SM_PageHandle memory_page){
	// Re-directed (passing) to readBlock(...) function with pageNumber = totalNumPages i.e. last block
	// printf("TOTAL PAGES = %d \n", file_handler->totalNumPages);	
	return readBlock(file_handler->totalNumPages - 1, file_handler, memory_page);
}

extern RC writeBlock (int page_number, SM_FileHandle *file_handler, SM_PageHandle memory_page) {
	// Condition to check if the pageNumber parameter is less than Total number of pages and less than 0, then return respective error code
	if (page_number > file_handler->totalNumPages || page_number < 0)
        	return RC_WRITE_FAILED;
	
	// Opened file stream in read & write mode. 'r+' mode opens the file for both reading and writing.	
	current_page = fopen(file_handler->fileName, "r+");
	
	// condition to check if file was successfully opened.
	if(current_page == NULL)
		return RC_FILE_NOT_FOUND;

	// Setting the cursor(pointer) position of the file stream. The seek is successfull if fseek() return 0
	int isSeekSuccess = fseek(current_page, (page_number * PAGE_SIZE), SEEK_SET);
	if(isSeekSuccess == 0) {

		// Writing content from memory_page to current_page stream
		fwrite(memory_page, sizeof(char), strlen(memory_page), current_page);

		// Setting the current page position to the cursor(pointer) position of the file stream
		file_handler->curPagePos = ftell(current_page);
		

		// Closing file stream so that all the buffers are flushed.     	
		fclose(current_page);
	} else {
		return RC_WRITE_FAILED;
	}	
	
	return RC_OK;
}

extern RC writeCurrentBlock (SM_FileHandle *file_handler, SM_PageHandle memory_page) {
	// Calculating current page number by dividing page size by current page position	
	int current_page_position = file_handler->curPagePos / PAGE_SIZE;
	
	// Incrementing total number of pages since we are adding this content to a new location as in current empty block.
	file_handler->totalNumPages++;
	return writeBlock(current_page_position, file_handler, memory_page);
}


extern RC appendEmptyBlock (SM_FileHandle *file_handler) {
	// Creating an empty page of size PAGE_SIZE bytes
	SM_PageHandle empty_block = (SM_PageHandle)calloc(PAGE_SIZE, sizeof(char));
	
	// Moving the cursor (pointer) position to the begining of the file stream.
	// And the seek is success if fseek() return 0
	int isSeekSuccess = fseek(current_page, 0, SEEK_END);
	
	if( isSeekSuccess == 0 ) {
		// Writing an empty page to the file
		fwrite(empty_block, sizeof(char), PAGE_SIZE, current_page);
	} else {
		free(empty_block);
		return RC_WRITE_FAILED;
	}
	
	// De-allocating the memory previously allocated to 'emptyPage'.
	// This is optional but always better to do for proper memory management.
	free(empty_block);
	
	// Incrementing the total number of pages since we added an empty black.
	file_handler->totalNumPages++;
	return RC_OK;
}

extern RC ensureCapacity (int number_total_pages, SM_FileHandle *file_handler) {
	// Opening file stream in append mode. 'a' mode opens the file to append the data at the end of file.
	current_page = fopen(file_handler->fileName, "a");
	
	if(current_page == NULL)
		return RC_FILE_NOT_FOUND;
	
	// Checking if numberOfPages is greater than totalNumPages.
	// If that is the case, then add empty pages till numberofPages = totalNumPages
	while(number_total_pages > file_handler->totalNumPages)
		appendEmptyBlock(file_handler);
	
	// Closing file stream so that all the buffers are flushed. 
	fclose(current_page);
	return RC_OK;
}
