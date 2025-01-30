/*
    This is a parser header file which help to parse the filename and path
*/
#ifndef PATHPARSER_H
#define PATHPARSER_H

struct path_root{
    int driver_no;
    struct path_part* first;
};

struct path_root* pathparser_parse(const char* path, const char* current_directory_path);
struct path_part* pathparser_parse_path_part(struct path_part* last_part, const char** path);

/*
    Here we are using linked list like struct for moving from 
    one path to another, as this is easy way to move from 
    one path to another
*/
struct path_part{
    /* store the name of the path */
    const char* part;
    /* store the next path just like linked list */
    struct path_part* next;
};


void pathparser_free(struct path_root* root);

#endif