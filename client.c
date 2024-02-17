#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"
#include "parson.h"

int check_input(char *input) {
    if(strcmp(input,"\n")==0){
        fprintf(stderr,"Invalid input.Please enter another one.\n");
        return 1;
    }
    return 0;
}

int check_space(char *input) {
    if(strstr(input," ")!=NULL){
        fprintf(stderr,"Input should not contain space character.\n");
        return 1;
    }
    return 0;
}

int isNumber(char *text)
{
    int j;
    j = strlen(text);
    while(j--)
    {
        if(text[j] >= '0' && text[j] <= '9')
            continue;

        return 1;
    }

    return 0;
}

void removeChar(char* s, char c)
{
 
    int j, n = strlen(s);
    for (int i = j = 0; i < n; i++)
        if (s[i] != c)
            s[j++] = s[i];
 
    s[j] = '\0';
}

int main(int argc, char *argv[])
{
    char *message;
    char *response;
    int sockfd;
	int login=0;
	char*cookie=NULL;
    char *token=NULL;

    while(1) {

        char *buffer;
    	buffer = (char*) malloc (80);
    	fgets(buffer, 80, stdin);
		if (buffer[strlen(buffer) - 1] == '\n')//escapare caracter final
		    buffer[strlen(buffer) - 1] = '\0';

        if(strcmp(buffer,"register")==0) {
            
            char *name = (char*) malloc (80);

            do{

                printf("username=");
                fgets(name,80,stdin);

            } while(check_input(name)==1 || check_space(name)==1);
            
            char *pass = (char*) malloc (80);
            
            do{

                printf("password=");
                fgets(pass,80,stdin);

            }while(check_input(pass)==1 || check_space(pass)==1);

            JSON_Value *value = json_value_init_object();                             
	   		JSON_Object *object = json_value_get_object(value);
	   		char *str = NULL;
			
			if (name[strlen(name) - 1] == '\n')//escapare caracter final
				name[strlen(name) - 1] = '\0';
			
			if (pass[strlen(pass) - 1] == '\n')//escapare caracter final
				pass[strlen(pass) - 1] = '\0';
	   		
			json_object_set_string(object, "username", name);
	   		json_object_set_string(object, "password", pass);
	    	str = json_serialize_to_string_pretty(value);
   			//formare mesaj
    		message = compute_post_request("localhost", "/api/v1/tema/auth/register", "application/json", &str, 1, NULL, 1, token);
            //puts(message);
	    	sockfd = open_connection("34.241.4.235", 8080, AF_INET, SOCK_STREAM, 0);
	   		send_to_server(sockfd, message);
	   		response = receive_from_server(sockfd);
	   		//puts(response);
            
            if(strstr(response,"is taken")!=NULL) {
                
                fprintf(stdout,"Username taken! Register again and with a new one.\n");
                close_connection(sockfd);
                continue;
            
            } else {

                fprintf(stdout,"New client has registered.\n");
            }
	   		
            close_connection(sockfd);

        } else if (strcmp(buffer,"login")==0){
            
            if(login == 1) {

				fprintf(stderr,"Someone is already logged in. Please select another operation.\n");
				continue;
			
            }

            char *name = (char*) malloc (80);
            do {

                printf("username=");
                fgets(name,80,stdin);

            } while(check_input(name) == 1 || check_space(name) == 1);

            char *pass = (char*) malloc (80);
            do {

                printf("password=");
                fgets(pass,80,stdin);

            } while(check_input(pass)==1 || check_space(name) == 1); 

			if (name[strlen(name) - 1] == '\n')//escapare caracter final
				name[strlen(name) - 1] = '\0';
			
			if (pass[strlen(pass) - 1] == '\n')//escapare caracter final
				pass[strlen(pass) - 1] = '\0';
			
			JSON_Value *value = json_value_init_object();
	   		JSON_Object *object = json_value_get_object(value);
	   		char *str = NULL;
			
			if (name[strlen(name) - 1] == '\n')//escapare caracter final
				name[strlen(name) - 1] = '\0';
			
			if (pass[strlen(pass) - 1] == '\n')//escapare caracter final
				pass[strlen(pass) - 1] = '\0';
	   		
			json_object_set_string(object, "username", name);
	   		json_object_set_string(object, "password", pass);
	    	str = json_serialize_to_string_pretty(value);
   			//formare mesaj
    		message = compute_post_request("localhost", "/api/v1/tema/auth/login", "application/json", &str, 1, NULL, 1, token);
            //puts(message);
	    	sockfd = open_connection("34.241.4.235", 8080, AF_INET, SOCK_STREAM, 0);
	   		send_to_server(sockfd, message);
	   		response = receive_from_server(sockfd);
	   		//puts(response);
            
            if (strstr(response,"No account")!=NULL){

                fprintf(stdout,"User not register! Please register the user first.\n");
                close_connection(sockfd);
                continue;

            } else if(strstr(response,"Credentials")!=NULL) {
                
                fprintf(stdout,"Wrong credentials! Please enter name and password again.\n");
                close_connection(sockfd);
                continue;
            
            } else {

                char* cookies; 
                char* aux = response; 
 
                while ((cookies = strtok_r(aux, "\n", &aux))) {
                    if (strstr(cookies, "Set-Cookie:") != NULL) {
                        cookie = strtok(cookies, ";");
                        cookie = strtok(cookie, ":");
                        cookie = strtok(0, " ");
                        break;
                    }
                }

                fprintf(stdout,"Welcome user %s!\n",name);
            
            }

	   		close_connection(sockfd);
	   		login = 1;

        } else if (strcmp(buffer,"enter_library")==0) {
			            
			message = compute_get_request("localhost", "/api/v1/tema/library/access", NULL, &cookie, 1, token);
            //puts(message);
            
	    	sockfd = open_connection("34.241.4.235", 8080, AF_INET, SOCK_STREAM, 0);
            send_to_server(sockfd, message);
	   		response = receive_from_server(sockfd);
	   		//puts(response);
            
            if(strstr(response,"logged in")!=NULL) {

                fprintf(stderr,"The user isn't logged in!\n");

            } else {

                char* tokens; 
                char* aux = response; 
    
                while ((tokens = strtok_r(aux, "\n", &aux))) {
                    if (strstr(tokens, "token") != NULL) {
                        token = strtok(tokens, ":");
                        token = strtok(0, "\"");
                        break;
                    }
                }

                fprintf(stdout,"Welcome to the library!\n");
            }
	   		close_connection(sockfd);


        } else if (strcmp(buffer,"get_books")==0) {

            if(login == 0) {
                fprintf(stderr,"User not logged in! Access denied!\n");
                continue;
            }

            message = compute_get_request("localhost", "/api/v1/tema/library/books", NULL, &cookie, 1, token);
            //puts(message);
            sockfd = open_connection("34.241.4.235", 8080, AF_INET, SOCK_STREAM, 0);
	   		send_to_server(sockfd, message);
	   		response = receive_from_server(sockfd);
	   		//puts(response);

            if(strstr(response,"missing")!=NULL) {

                fprintf(stderr,"You are not authorized! Please enter the library first!\n");
            
            } else {
                               
                if(strlen(response)==460) {
                    fprintf(stderr,"Books not found! Please add some books first.\n");
                    close_connection(sockfd);
                    continue;
                }
                
                char *res = strtok(basic_extract_json_response(response),"]");
                char *tok = strtok(res,"{");
                tok[strlen(tok)-2]='\0';
                puts("Book found! Here are the details: ");
                char * info = strdup(tok);
                removeChar(info,'\"');
                puts(info);
                puts("");
                while(tok!=NULL){
                    tok = strtok(NULL,"{");
                    if(tok!=NULL){

                        if(tok[strlen(tok)-2]=='}') 
                            tok[strlen(tok)-2]='\0';

                        if (tok[strlen(tok)-1]=='}')
                            tok[strlen(tok)-1]='\0';
                        
                        puts("Book found! Here are the details: ");
                        info = strdup(tok);
                        removeChar(info,'\"');
                        puts(info);
                        puts("");

                    }
                }
            }
            close_connection(sockfd);

        } else if (strcmp(buffer,"get_book")==0) { 
            
            if (login == 0) {
                fprintf(stderr,"User not logged in! Access denied.\n");
                continue;
            }

            char *id = (char*) malloc (80);
            
            do {

                printf("id=");
                fgets(id,80,stdin);
                if (id[strlen(id) - 1] == '\n')//escapare caracter final
				    id[strlen(id) - 1] = '\0';

            } while(check_input(id)==1 || isNumber(id)==1);
            


            char buf[100] = "/api/v1/tema/library/books/";
            strcat(buf,id);
            message = compute_get_request("localhost", buf, NULL, &cookie, 1, token);
            //puts(message);
            sockfd = open_connection("34.241.4.235", 8080, AF_INET, SOCK_STREAM, 0);
	   		send_to_server(sockfd, message);
	   		response = receive_from_server(sockfd);
            //puts(response);
            
            if(strstr(response,"missing")!=NULL) {
                
                fprintf(stderr,"You are not authorized! Please enter the library first!\n");

            } else if(strstr(response,"was found")!=NULL) {
                
                fprintf(stdout,"Invalid book with ID = %s! Search for a good one!\n",id);
                close_connection(sockfd);
                continue;
            
            } else {

                char *res = strtok(basic_extract_json_response(response),"]");
                res++;
                res[strlen(res)-1]='\0';
                char* tok = strtok(res,",");
                puts("Book found!\n");
                removeChar(tok,'\"');
                tok[0]=tok[0]-32;
                puts(tok);
                while(tok!=NULL){
                    tok=strtok(NULL,",");
                    if(tok!=NULL){
                        removeChar(tok,'\"');
                        tok[0]=tok[0]-32;
                        puts(tok);
                    }
                }
                puts("");
            }

            close_connection(sockfd);
        
        } else if (strcmp(buffer,"add_book")==0) {
            
            if (login == 0) {
                fprintf(stderr,"User not logged in! Access denied.\n");
                continue;
            }

            char *title = (char*) malloc (80);
            char *author = (char*) malloc (80);
			char *genre = (char*) malloc (80);
			char *publisher = (char*) malloc (80);
			char *page_count = (char*) malloc (80);

            do {

                printf("title=");
                fgets(title,80,stdin);
                if (title[strlen(title) - 1] == '\n')//escapare caracter final
				    title[strlen(title) - 1] = '\0';

            } while (check_input(title) == 1);
			
            do {

                printf("author=");
                fgets(author,80,stdin);
                if (author[strlen(author) - 1] == '\n')//escapare caracter final
				    author[strlen(author) - 1] = '\0';

            } while (check_input(author) == 1);


            do {
                printf("genre=");
                fgets(genre,80,stdin);
            } while (check_input(genre)==1);

			if (genre[strlen(genre) - 1] == '\n')//escapare caracter final
				genre[strlen(genre) - 1] = '\0';

            do {
			    
                printf("page_count=");
                fgets(page_count,80,stdin);
                if (page_count[strlen(page_count) - 1] == '\n')//escapare caracter final
				    page_count[strlen(page_count) - 1] = '\0';
                
            } while ((check_input(page_count)==1) || (isNumber(page_count)==1));


            do {

                printf("publisher=");
                fgets(publisher,80,stdin);

            } while (check_input(publisher)==1);

			if (publisher[strlen(publisher) - 1] == '\n')//escapare caracter final
				publisher[strlen(publisher) - 1] = '\0';

			
			JSON_Value *value = json_value_init_object();
	   		JSON_Object *object = json_value_get_object(value);

			json_object_set_string(object, "title", title);
			json_object_set_string(object, "author", author);
			json_object_set_string(object, "genre", genre);
			json_object_set_number(object, "page_count", atoi(page_count));
			json_object_set_string(object, "publisher", publisher);
			char *str = NULL;

			str = json_serialize_to_string_pretty(value);
			
			message = compute_post_request("localhost", "/api/v1/tema/library/books", "application/json", &str, 1, NULL, 1, token);
            //puts(message);
	    	sockfd = open_connection("34.241.4.235", 8080, AF_INET, SOCK_STREAM, 0);
	   		send_to_server(sockfd, message);
	   		response = receive_from_server(sockfd);
	   		//puts(response);
            
            if(strstr(response,"missing")!=NULL) {
                
                fprintf(stderr,"You are not authorized! Please enter the library first!\n");

            } else {

                fprintf(stdout,"Book %s added to library!\n",title);
            }
	   		
            close_connection(sockfd);

        } else if (strcmp(buffer,"delete_book")==0) {
            
            if (login == 0) {
                fprintf(stderr,"User not logged in! Access denied.\n");
                continue;
            }

            char *id = (char*) malloc (80);
            
            do {
                
                printf("id=");
                fgets(id,80,stdin);
                if (id[strlen(id) - 1] == '\n')//escapare caracter final
				    id[strlen(id) - 1] = '\0';

            } while (check_input(id)==1 || isNumber(id)==1);


            char buf[100] = "/api/v1/tema/library/books/";
            strcat(buf,id);
            message = compute_delete_request("localhost", buf, NULL, token);
            //puts(message);
            sockfd = open_connection("34.241.4.235", 8080, AF_INET, SOCK_STREAM, 0);
	   		send_to_server(sockfd, message);
	   		response = receive_from_server(sockfd);
	   		//puts(response);

            if(strstr(response,"missing")!=NULL) {
                
                fprintf(stderr,"You are not authorized! Please enter the library first!\n");

            } else if(strstr(response,"was deleted")!=NULL) {
                
                fprintf(stderr,"Invalid book ID! Try again with a good one!\n");
                close_connection(sockfd);
                continue;
            
            } else {

                fprintf(stdout,"Book succesfully deleted!\n");
            }

            close_connection(sockfd);

        } else if (strcmp(buffer,"logout")==0) {
            
            message = compute_get_request("localhost", "/api/v1/tema/auth/logout", NULL, &cookie, 1, token);
            //puts(message);
            sockfd = open_connection("34.241.4.235", 8080, AF_INET, SOCK_STREAM, 0);
	   		send_to_server(sockfd, message);
	   		response = receive_from_server(sockfd);
	   		//puts(response);

            if(strstr(response,"logged in")!=NULL) {
                
                fprintf(stderr,"No one is logged in!\n");

            } else {

                fprintf(stdout,"Succesfully logged out! See you next time ;)\n");
                login=0;
                token = NULL;
                cookie = NULL;

            }

            close_connection(sockfd);


        } else if (strcmp(buffer,"exit")==0) {

            fprintf(stdout,"Goodbye user! Hope you enjoyed the experience!\n");
            break;

        } else {

            fprintf(stderr,"Wrong input operation.\n");
        
        }
    }

    return 0;
}
