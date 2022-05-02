#define O_RDONLY  0x000
#define O_WRONLY  0x001
#define O_RDWR    0x002
#define O_CREATE  0x200
#define O_TRUNC   0x400

/* 
 * VALUES FOUND IN mman.h definitions
*/
#define PROT_READ        0x2                /* Page can be read.  */
#define PROT_WRITE        0x4                /* Page can be written.  */

# define MAP_ANONYMOUS        0x20                /* Don't use a file.  */

#define MAP_SHARED        0x01                /* Share changes.  */
#define MAP_PRIVATE        0x02                /* Changes are private.  */
