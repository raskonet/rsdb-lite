#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#define size_of_attribute(Struct,Attribute) sizeof(((Struct*)0)->Attribute)

#define COLUMN_USERNAME_SIZE 32
#define COLUMN_EMAIL_SIZE 255


typedef struct{
  char* buffer;
  size_t buffer_length;
  ssize_t input_length;
}InputBuffer;

typedef enum{
 META_COMMAND_SUCCESS,
 META_COMMAND_UNRECOGNOISED_COMMAND
}MetaCommandResult;

typedef enum{
  PREPARE_SUCCESS,
  PREPARE_SYNTAX_ERROR,
  PREPARE_UNRECOGNISED_STATEMENT
}PrepareResult;

typedef enum{
  STATEMENT_INSERT,
  STATEMENT_SELECT
}StatementType;

typedef struct{
  uint32_t id;
  char* username[COLUMN_USERNAME_SIZE];
  char* email[COLUMN_EMAIL_SIZE];
}Row;

typedef struct{
  StatementType type;
  Row row_to_insert;
}Statement;


const uint32_t ID_SIZE=size_of_attribute(Row,id);
const uint32_t USERNAME_SIZE=size_of_attribute(Row,username);
const uint32_t EMAIL_SIZE=size_of_attribute(Row,email);
const uint32_t ID_OFFSET=0;
const uint32_t USERNAME_OFFSET=ID_SIZE+ID_SIZE;
const uint32_t EMAIL_OFFSET = USERNAME_OFFSET+USERNAME_SIZE;
const uint32_t ROW_SIZE = EMAIL_SIZE+ID_SIZE+USERNAME_SIZE;

void serialize_row(Row* source, void* destination) {
    memcpy(destination + ID_OFFSET, &(source->id), ID_SIZE);
    memcpy(destination + USERNAME_OFFSET, &(source->username), USERNAME_SIZE);
    memcpy(destination + EMAIL_OFFSET, &(source->email), EMAIL_SIZE);
}

void deserialise_row(void* source,Row* destination){
    memcpy(&(destination->id),source+ID_OFFSET,ID_SIZE);
    memcpy(&(destination->username),source+USERNAME_OFFSET,USERNAME_SIZE);
    memcpy(&(destination->email),source+EMAIL_OFFSET,EMAIL_SIZE);
}

//TABLE STRUCTURE FROM HERE >>>
#define TABLE_MAX_PAGES 100
const uint32_t PAGE_SIZE=4096;
const uint32_t ROWS_PER_PAGE=PAGE_SIZE/ROW_SIZE;
const uint32_t ROWS_PER_TABLE=TABLE_MAX_PAGES*ROWS_PER_PAGE;

typedef struct{
  uint32_t num_rows;
  void* pages[TABLE_MAX_PAGES];
}Table;
//TABLE DEFINNITION ENDS HERE

void* row_slot(Table* table,uint32_t row_num){
    uint32_t cur_page_num_here=row_num/ROWS_PER_PAGE;
    void* cur_page_here=table->pages[cur_page_num_here];
    if(cur_page_here==NULL){
    cur_page_here=table->pages[cur_page_num_here]=malloc(PAGE_SIZE);
  }
  uint32_t row_offset_here=row_num/ROWS_PER_PAGE;
  uint32_t byte_offset_here=ROW_SIZE*row_offset_here;
  return cur_page_here+byte_offset_here;
}
void print_prompt(){
  printf("db > ");
}
InputBuffer* new_input_buffer(){
  InputBuffer* input_buffer=malloc(sizeof(InputBuffer));
  input_buffer->buffer=NULL;
  input_buffer->buffer_length=0;
  input_buffer->input_length=0;
  return input_buffer;
}

void read_input(InputBuffer* input_buffer){
  ssize_t bytes_read=getline(&(input_buffer->buffer),&(input_buffer->buffer_length),stdin);
  if(bytes_read<=0){
    printf("Failure to read input,ERROR 40..something\n");
    exit(EXIT_FAILURE);
  }
  input_buffer->input_length=bytes_read-1;
  input_buffer->buffer[bytes_read-1]=0;
}

void close_input_buffer(InputBuffer* input_buffer){
  free(input_buffer->buffer);
  free(input_buffer);
}

MetaCommandResult do_meta_command(InputBuffer* input_buffer){
  if(strcmp(input_buffer->buffer,".exit")==0){
    printf("exiting server..\n");
    exit(EXIT_SUCCESS);
  }else{
    return META_COMMAND_UNRECOGNOISED_COMMAND;
  }
}

PrepareResult prepare_statement(InputBuffer* input_buffer,Statement* statement){
  if(strncmp(input_buffer->buffer,"insert",6)==0){
    statement->type=STATEMENT_INSERT;
    int args_assigned=sscanf(input_buffer->buffer,"insert %d %s %s"
    ,&statement->row_to_insert.id,statement->row_to_insert.username,
    statement->row_to_insert.email);
    if(args_assigned<3){
      return PREPARE_SYNTAX_ERROR;
    }
    return PREPARE_SUCCESS;
  }
  if(strcmp(input_buffer->buffer,"select")==0){
    statement->type=STATEMENT_SELECT;
    return PREPARE_SUCCESS;
  }
  return PREPARE_UNRECOGNISED_STATEMENT;
}

void execute_statement(Statement* statement){
  switch(statement->type){
    case(STATEMENT_INSERT):
      printf("something something inserting...\n");
      break;
    
    case(STATEMENT_SELECT):
      printf("something something selccting..\n");
    break;
    
  }


}

int main(int argc,char* argv[]){
 InputBuffer* input_buffer=new_input_buffer();
  while(true){
    print_prompt();
    read_input(input_buffer);
   if(input_buffer->buffer[0]=='.'){
      switch(do_meta_command(input_buffer)){
        case(META_COMMAND_SUCCESS):
          printf("something something meta command");
          break;

        
        case(META_COMMAND_UNRECOGNOISED_COMMAND):
          printf("coudnt recognise command ,try again and better wrote a recogniseable command this time...\n");
          break;
        
      }
    }
    Statement statement;
        switch(prepare_statement(input_buffer,&statement)){
    case(PREPARE_SUCCESS):
        break;
      
      case(PREPARE_UNRECOGNISED_STATEMENT):
        printf("Type of statement coudld not be detrmined :- \n");
      printf("look at teh documentation to see the types of statements available..\n");
      continue;
      
    }    
  execute_statement(&statement);
  printf("executed..\n");
  }

}
