/* decompress a jpeg , but format it like a block ( raw jpg ) line by line
 * Gleicon - 05/10/99
 * cc -o jdec jdec.c -ljpeg
 */
 
#include <stdio.h>
#include <jpeglib.h>
#include <setjmp.h>
#include <stdarg.h>


/* Error handling */

struct error_mgr {
	struct jpeg_error_mgr pub;    
	jmp_buf setjmp_buffer;        
	};

typedef struct error_mgr * my_error_ptr;

    
METHODDEF(void) my_error_exit (j_common_ptr cinfo) {
	my_error_ptr myerr = (my_error_ptr) cinfo->err;
        (*cinfo->err->output_message) (cinfo);
        longjmp(myerr->setjmp_buffer, 1);
        }
   
GLOBAL(int) read_JPEG_file (char * filename, char *dest) {  
	  struct jpeg_decompress_struct cinfo;
	  struct error_mgr jerr;
	  FILE *infile;
	  FILE *outfile;   
	  JSAMPARRAY buffer;      
	  int row_stride, z;   
	  char *q;
	  
	  
	  if ((infile = fopen(filename, "rb")) == NULL) {
	  	fprintf(stderr, "Can't open: %s\n", filename);
	        return 0;
	        }
	  
	  if ((outfile = fopen(dest, "wb+")) == NULL) {
	  	fprintf(stderr, "Can't open: %s\n", filename);
	        return 0;
	        }
	  
	  cinfo.err = jpeg_std_error(&jerr.pub);
	  jerr.pub.error_exit = my_error_exit;  
	  
	  if (setjmp(jerr.setjmp_buffer)) {
	 	jpeg_destroy_decompress(&cinfo);
	  	jpeg_destroy_decompress(&cinfo);
	  	fclose(infile);
	  	fclose (outfile);
	  	return 0;
	  	}
	  
	  jpeg_create_decompress(&cinfo);
	  jpeg_stdio_src(&cinfo, infile);
	  (void) jpeg_read_header(&cinfo, TRUE);
	  (void) jpeg_start_decompress(&cinfo);
	  row_stride = cinfo.output_width * cinfo.output_components;

	  fprintf (stderr, "Information about: %s:\n", filename);
	  fprintf (stderr, "Width : %d\n", cinfo.output_width);
	  fprintf (stderr, "Height: %d\n", cinfo.output_height);
	  fprintf (stderr, "Components: %d\n", cinfo.output_components);
	  fprintf (stderr, "Size: %d\n", row_stride*cinfo.output_height);
	  
     buffer = (*cinfo.mem->alloc_sarray)
	                  ((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);
	  
	  /* pnm header  */
	  
	  
//	  fprintf (outfile,"P6\n%d %d\n255\n", cinfo.output_width, cinfo.output_height);
	  
	  
	  /* lines */
	  
	  while (cinfo.output_scanline < cinfo.output_height) {
	      (void) jpeg_read_scanlines(&cinfo, buffer, 1);
	      
	      fwrite (buffer[0], row_stride, 1, outfile);
	      
	      q=buffer[0];
	      
	      for (z=0;z!=cinfo.output_width; z++) {
	      fprintf (outfile, "%02x", *q++);
	      fprintf (outfile, "%02x", *q++);
	      fprintf (outfile, "%02x", *q++);
	      }
	 
	         
		   	       
       	}

	  (void) jpeg_finish_decompress(&cinfo);
	  jpeg_destroy_decompress(&cinfo);
	    
	  fclose(infile);
	  fclose (outfile);
	  
	
	}	           
	            
void main ( int argc, char **argv) { 
	
	if (argc < 3) {
		fprintf(stderr,"Error !\n");
		return ;
		}
	fprintf (stderr," Infile:%s\nOutfile:%s\n", argv[1], argv[2]);
	
	read_JPEG_file (argv[1], argv[2]); 
	
	}	  
	  	                  
	              
                
               
 