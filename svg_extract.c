#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <libgen.h>

#define TSP_HEADER_LEN 91
#define TSP_HEADER "NAME: %s\nCOMMENT: TSP art\nTYPE: TSP\nDIMENSION: %d\nEDGE_WEIGHT_TYPE: EUC_2D\nNODE_COORD_SECTION\n"

bool get_svg_dims(int * width, int * height, char * svg_buffer) {
  char* tmp;
  char* svg_width_ptr;
  char* svg_height_ptr;
  char* svg_width;
  char* svg_height;
  
  // Width
  tmp = strstr(svg_buffer, "<svg width=");
  if(tmp == NULL) {
    fprintf(stderr, "[%s:%d] Could not find svg width\n", __FILE__, __LINE__);
    return false;
  }
  
  svg_width_ptr = tmp + 12;
  tmp = strstr(svg_width_ptr, "\" ");
  if(tmp == NULL) {
    fprintf(stderr, "[%s:%d] Failed to parse svg width\n", __FILE__, __LINE__);
    return false;    
  }
  
  if((svg_width = malloc(sizeof(char) * (tmp-svg_width_ptr+1))) == NULL) {
    fprintf(stderr, "[%s:%d] ", __FILE__, __LINE__); perror("malloc");
    return false;    
  }
  strncpy(svg_width, svg_width_ptr, tmp-svg_width_ptr);
  *width = atoi(svg_width);
  
  // Height
  tmp = strstr(svg_width_ptr, "height=");
  if(tmp == NULL) {
    fprintf(stderr, "[%s:%d] Could not find svg height\n", __FILE__, __LINE__);
    return false;
  }
  
  svg_height_ptr = tmp + 8;
  tmp = strstr(svg_height_ptr, "\" ");
  if(tmp == NULL) {
    fprintf(stderr, "[%s:%d] Failed to parse svg height\n", __FILE__, __LINE__);
    return false;    
  }
  
  if((svg_height = malloc(sizeof(char) * (tmp-svg_height_ptr+1))) == NULL) {
    fprintf(stderr, "[%s:%d] ", __FILE__, __LINE__); perror("malloc");
    return false;    
  }
  strncpy(svg_height, svg_height_ptr, tmp-svg_height_ptr);
  *height = atoi(svg_height);
  
  return true;  
}

int parse_tsp(char* svg_buffer, char* tsp_body) {
  char * aux = svg_buffer;
  int bytes_to_write = 0;
  int i = 1;  
  int written;
  
  float cx;
  float cy;
  char* tmp;
  char* coord_ptr;
  char coord[20];
  
  while((aux = strstr(aux, "<circle cx=")) != NULL) {
    // Parse cx and cy
    coord_ptr = aux + 12;
    if((tmp = strstr(aux, "\" ")) == NULL) {
      fprintf(stderr, "[%s:%d] Failed to parse circle cx\n", __FILE__, __LINE__); 
      return -1;    
    }
    strncpy(coord, coord_ptr, tmp-coord_ptr);
    coord[tmp-coord_ptr] = 0;
    cx = atof(coord);
    
    if((aux = strstr(aux, "cy=")) == NULL) {
      fprintf(stderr, "[%s:%d] Failed to parse circle cy\n", __FILE__, __LINE__); 
      return -1;     
    }
    coord_ptr = aux + 4;
    if((tmp = strstr(aux, "\" ")) == NULL) {
      fprintf(stderr, "[%s:%d] Failed to parse circle cy\n", __FILE__, __LINE__); 
      return -1;    
    }
    strncpy(coord, coord_ptr, tmp-coord_ptr);
    coord[tmp-coord_ptr] = 0;
    cy = atof(coord);     
    
    written = sprintf(tsp_body, "%d %.3f %.3f\n", i, cx, -cy);
    
    tsp_body += written;
    bytes_to_write += written;    
    i++;
  }
  
  return bytes_to_write;    
}

int int_length(int num) { return floor(log10(abs(num))) + 1; }

int main(int argc, char* argv[]) {
  if (argc != 2) {
    printf("Usage: %s input_file.svg\n", argv[0]);
    return -1;
  }
  
  FILE * svg_file;
  FILE * tsp_file;
  long bufsize;
  struct stat stat;  
  char * input_path = argv[1];
  char * output_path;
  char * svg_buffer;
  char * tsp_header;
  char * tsp_body;
  char * tsp_name = basename(input_path);
  int tsp_cities;
  int tsp_cities_length;
  int ret;
  int input_lines = 0;
  
  // Build the output path  
  if((output_path = malloc(sizeof(char) * (strlen(input_path)+1))) == NULL) {
    fprintf(stderr, "[%s:%d] ", __FILE__, __LINE__); perror("malloc");
    return -1;
  }
  strcpy(output_path, input_path);
  char * tmp = strrchr(output_path, '.');
  if(tmp == NULL || strncmp(tmp, ".svg", 4) != 0) {
    printf("%s: invalid extension (expected svg)\n", output_path);
    return -1;    
  }
  tmp[1] = 't'; tmp[2] = 's'; tmp[3] = 'p';
  
  // Open and read the input file
  if((svg_file = fopen(input_path, "r")) == NULL) {
    fprintf(stderr, "[%s:%d] ", __FILE__, __LINE__); perror("fopen");
    return -1;    
  }
  
  if(fseek(svg_file, 0L, SEEK_END) == -1) {
    fprintf(stderr, "[%s:%d] ", __FILE__, __LINE__); perror("fseek");
    return -1;    
  }  
  
  bufsize = ftell(svg_file);
  
  if(fseek(svg_file, 0L, SEEK_SET) == -1) {
    fprintf(stderr, "[%s:%d] ", __FILE__, __LINE__); perror("fseek");
    return -1;    
  }
  
  if((svg_buffer = malloc(sizeof(char) * (bufsize+1))) == NULL) {
    fprintf(stderr, "[%s:%d] ", __FILE__, __LINE__); perror("malloc");
    return -1;    
  }
  
  if((ret = fread(svg_buffer, sizeof(char), bufsize, svg_file)) < bufsize) {
    if(ferror(svg_file) != 0)
      printf("fread: An error ocurred while reading\n");    
    else 
      printf("fread: Unexpected end of file (read %d expected %ld)\n", ret, bufsize); 
    
    return -1;
  }
  
  // Parse width and height of the svg
  int svg_width;
  int svg_height;
  if(!get_svg_dims(&svg_width, &svg_height, svg_buffer)) {
    return -1;    
  }
  
  for(char* c = svg_buffer; *c != '\0'; c++) {
    if(*c == '\n') input_lines++; 
  }
  tsp_cities = input_lines-4;
  tsp_cities_length = int_length(tsp_cities);    
    
  // Open the output file
  if((tsp_file = fopen(output_path, "w")) == NULL) {
    fprintf(stderr, "[%s:%d] ", __FILE__, __LINE__); perror("fopen");
    return -1;    
  }
  
  // Write TSP in buffer
  // Worst case size (CITIES X Y) where X, Y may be negative and have 3 decimals
  int total_bytes_body = tsp_cities * (tsp_cities_length + 2 + int_length(svg_width) + 6 + int_length(svg_height) + 4);
  int total_bytes_to_write_body;
  if((tsp_body = malloc(sizeof(char) * total_bytes_body)) == NULL) {
    fprintf(stderr, "[%s:%d] ", __FILE__, __LINE__); perror("malloc");
    return -1;   
  }
  if((total_bytes_to_write_body = parse_tsp(svg_buffer, tsp_body)) <= 0) {
    return -1;    
  }
  
  // Write TSP header and body to file
  int total_bytes = TSP_HEADER_LEN + strlen(tsp_name) + tsp_cities_length;  
  if((tsp_header = malloc(sizeof(char) * total_bytes)) == NULL) {
    fprintf(stderr, "[%s:%d] ", __FILE__, __LINE__); perror("malloc");
    return -1;   
  }
  
  if((ret = snprintf(tsp_header, total_bytes, TSP_HEADER, tsp_name, tsp_cities)) != total_bytes-1) {
    fprintf(stderr, "[%s:%d] snprintf: Wrote %d instead of %d bytes\n", __FILE__, __LINE__, ret, total_bytes-1);
    return -1;
  }
  
  // Minus 1 because we do not want to write the NULL byte yet (we will do in the next fwrite)    
  if((ret = fwrite(tsp_header,  sizeof(char), total_bytes-1, tsp_file)) != total_bytes-1) {
    fprintf(stderr, "[%s:%d] fwrite: Wrote %d instead of %d bytes\n", __FILE__, __LINE__, ret, total_bytes-1);
    return -1;    
  } 
  
  if((ret = fwrite(tsp_body,  sizeof(char), total_bytes_to_write_body, tsp_file)) != total_bytes_to_write_body) {
    fprintf(stderr, "[%s:%d] fwrite: Wrote %d instead of %d bytes\n", __FILE__, __LINE__, ret, total_bytes_to_write_body);
    return -1;    
  }
  
  // Clean everything, exit
  free(svg_buffer);
  
  if(fclose(svg_file) != 0) {
    fprintf(stderr, "[%s:%d] ", __FILE__, __LINE__); perror("fclose");
    return -1;    
  } 
  
  if(fclose(tsp_file) != 0) {
    fprintf(stderr, "[%s:%d] ", __FILE__, __LINE__); perror("fclose");
    return -1;    
  } 
      
}
