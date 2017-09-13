#include <gtk/gtk.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <sstream>

GtkListStore * list_rooms;
GtkListStore * list_users;

using namespace std;

int room = 0; 
char * username = (char*)" ";
std:: string user_name = string(username);
char * password = (char*)" ";
std:: string pass_word = string(password);
char * roomname = (char*)" ";
char * message; 
char * command = (char*)" ";
std:: string commanD = string(command);
char * host = (char*)"localhost";
int port;

char * tArgs = (char*)" ";

char* uname=NULL;
char* pword=NULL;

vector<string> roomVector;
vector<string> userVector;

GtkWidget *passWord;
GtkWidget *userName;
GtkWidget *roomName;
GtkWidget *enterMessage;
GtkWidget *messageField;
//GtkWidget *tree_view_users;
//GtkWidget *tree_view_rooms;




#define MAX_RESPONSE (10 * 1024)
char * response = (char*)malloc(sizeof(char)*MAX_RESPONSE);
char * messagesToPut = (char*)malloc(sizeof(char)*MAX_RESPONSE);

int open_client_socket(char * host, int port) {
    // Initialize socket address structure
    struct  sockaddr_in socketAddress;

    // Clear sockaddr structure
    memset((char *)&socketAddress,0,sizeof(socketAddress));

    // Set family to Internet
    socketAddress.sin_family = AF_INET;
    // Set port
    socketAddress.sin_port = htons((u_short)port);

    // Get host table entry for this host
    struct  hostent  *ptrh = gethostbyname(host);
    if ( ptrh == NULL ) {
	perror("gethostbyname");
	exit(1);
    }

    // Copy the host ip address to socket address structure
    memcpy(&socketAddress.sin_addr, ptrh->h_addr, ptrh->h_length);

    // Get TCP transport protocol entry
    struct  protoent *ptrp = getprotobyname("tcp");
    if ( ptrp == NULL ) {
	perror("getprotobyname");
	exit(1);
    }

    // Create a tcp socket
    int sock = socket(PF_INET, SOCK_STREAM, ptrp->p_proto);
    if (sock < 0) {
	perror("socket");
	exit(1);
    }

    // Connect the socket to the specified server
    if (connect(sock, (struct sockaddr *)&socketAddress,
		sizeof(socketAddress)) < 0) {
	perror("connect");
	exit(1);
    }

    return sock;
}


int sendCommand(char *  host, int port, char * command, char * response) {

    int sock = open_client_socket( host, port);

    if (sock<0) {
	return 0;
    }

    // Send command
    write(sock, command, strlen(command));
    write(sock, "\r\n",2);

    //Print copy to stdout
    write(1, command, strlen(command));
    write(1, "\r\n",2);

    // Keep reading until connection is closed or MAX_REPONSE
    int n = 0;
    int len = 0;
    while ((n=read(sock, response+len, MAX_RESPONSE - len))>0) {
	len += n;
    }
    response[len]=0;

    printf("response:\n%s\n", response);

    close(sock);

    return 1;
}


/* Create the list of "Rooms" */
static GtkWidget *create_listRoom( const char * titleColumn, GtkListStore *model ){
    GtkWidget *scrolled_window;
    GtkWidget *tree_view;
//    GtkListStore *model;
    GtkTreeIter iter;
    GtkCellRenderer *cell;
    GtkTreeViewColumn *column;

    int i;

    /* Create a new scrolled window, with scrollbars only if needed */
    scrolled_window = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
	    GTK_POLICY_AUTOMATIC, 
	    GTK_POLICY_AUTOMATIC);

    //model = gtk_list_store_new (1, G_TYPE_STRING);
    tree_view = gtk_tree_view_new ();
    gtk_container_add (GTK_CONTAINER (scrolled_window), tree_view);
    gtk_tree_view_set_model (GTK_TREE_VIEW (tree_view), GTK_TREE_MODEL (list_rooms));
    gtk_widget_show (tree_view);

    for (i = 0; i < roomVector.size(); i++) {
	gchar *msg = g_strdup_printf("%s",((char*)roomVector[i].c_str())); 
	gtk_list_store_append (GTK_LIST_STORE (list_rooms), &iter);
	gtk_list_store_set (GTK_LIST_STORE (list_rooms), &iter,0, msg,-1);
	g_free (msg);
    }   


    cell = gtk_cell_renderer_text_new ();

    column = gtk_tree_view_column_new_with_attributes (titleColumn,
	    cell,
	    "text", 0,
	    NULL);

    gtk_tree_view_append_column (GTK_TREE_VIEW (tree_view),
	    GTK_TREE_VIEW_COLUMN (column));

    return scrolled_window;
}
//WHY IS THIS ONE GIVING ERRROR
static GtkWidget *create_listUser( const char * titleColumn, GtkListStore *model ){
    GtkWidget *scrolled_window;
    GtkWidget *tree_view;
//    GtkListStore *model;
    GtkTreeIter iter;
    GtkCellRenderer *cell;
    GtkTreeViewColumn *column;

    int i;

    // Create a new scrolled window, with scrollbars only if needed 
    scrolled_window = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
	    GTK_POLICY_AUTOMATIC, 
	    GTK_POLICY_AUTOMATIC);

    //model = gtk_list_store_new (1, G_TYPE_STRING);
    tree_view = gtk_tree_view_new ();
    gtk_container_add (GTK_CONTAINER (scrolled_window), tree_view);
    gtk_tree_view_set_model (GTK_TREE_VIEW (tree_view), GTK_TREE_MODEL (list_users));
    gtk_widget_show (tree_view);

    for (i = 0; i < userVector.size(); i++) {
	gchar *msg = g_strdup_printf("%s",((char*)userVector[i].c_str()));
	gtk_list_store_append (GTK_LIST_STORE (list_users), &iter);
	gtk_list_store_set (GTK_LIST_STORE (list_users), &iter,0, msg,-1);
	g_free (msg);
    }   


    cell = gtk_cell_renderer_text_new ();

    column = gtk_tree_view_column_new_with_attributes (titleColumn,
	    cell,
	    "text", 0,
	    NULL);

    gtk_tree_view_append_column (GTK_TREE_VIEW (tree_view),
	    GTK_TREE_VIEW_COLUMN (column));

    return scrolled_window;
}

void update_tree_view(int roomOrUser){
    printf("in update tree view\n");
    if(roomOrUser == 1){
	gtk_list_store_clear(list_users);
	create_listUser("Users",list_users);
	//gtk_tree_view_set_model (GTK_TREE_VIEW (tree_view_users), GTK_TREE_MODEL (list_users));
    }
    else{
	gtk_list_store_clear(list_rooms);
	create_listRoom("Rooms",list_rooms);
	//gtk_tree_view_set_model (GTK_TREE_VIEW (tree_view_rooms), GTK_TREE_MODEL (list_rooms));
    }
}
static void insert_text( GtkTextBuffer *buffer, const char * initialText ){
    GtkTextIter iter;

    gtk_text_buffer_get_iter_at_offset (buffer, &iter, 0);
    gtk_text_buffer_insert (buffer, &iter, initialText,-1);
}

/* Create a scrolled text area that displays a "message" */
static GtkWidget *create_text( const char * initialText ){
    GtkWidget *scrolled_window;
    GtkWidget *view;
    GtkTextBuffer *buffer;

    view = gtk_text_view_new ();
    buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (view));

    scrolled_window = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
	    GTK_POLICY_AUTOMATIC,
	    GTK_POLICY_AUTOMATIC);

    gtk_container_add (GTK_CONTAINER (scrolled_window), view);
    insert_text (buffer, initialText);

    gtk_widget_show_all (scrolled_window);

    return scrolled_window;
}

static gboolean update_messages(GtkWidget * data){
    printf("in update messages\n");
    char response2[MAX_RESPONSE];
    GtkWidget *view;
    view = gtk_bin_get_child(GTK_BIN(data));
    GtkTextBuffer *buffer;
    buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (view));
    
    if(uname!= NULL){
	std::string username1 = string(uname);
	std::string password1 = string(pword);
	string room1 = string(roomname);

	std::string com = string("GET-MESSAGES2");
	commanD = com + string(" ") + username1 + string(" ") + password1 + string(" ") + to_string(-1) + string(" ") + room1 + string("\r\n"); 
	sendCommand(host,port,(char*)commanD.c_str(),messagesToPut);	

	//create_listUser("User",list_users);
	gtk_text_buffer_set_text(buffer," ",1);	
	insert_text(buffer,messagesToPut);	
    }
    return TRUE;
}


//WHY is this one giving error
static gboolean update_list_users(){
    printf("in update user list\n");
    char response2[MAX_RESPONSE];
    if(uname!= NULL){

	std::string username1 = string(uname);
	std::string password1 = string(pword);
	string room1 = string(roomname);

	std:: string comRoom = string("CHECK-USER-EXIST-IN-ROOM");
	commanD = comRoom + string(" ") + username1 + string(" ") + password1 + string(" ") + room1 + string("\r\n");
	sendCommand(host,port,(char*)commanD.c_str(),response2);
	if(strstr(response2, "true\r\n") != NULL){
	    std::string comUsersInRoom = string("GET-USERS-IN-ROOM");
	    commanD = comUsersInRoom + string(" ") + username1 + string(" ") + password1 + string(" ") + room1 + string("\r\n");
	    sendCommand(host,port,(char*)commanD.c_str(),response);
	}
	else{
	    std::string com = string("GET-ALL-USERS");
	    commanD = com + string(" ") + username1 + string(" ") + password1 + string("\r\n"); 
	    sendCommand(host,port,(char*)commanD.c_str(),response);
	}

	std::stringstream stream(response);
	std::string temp;
	userVector.clear();
	while(getline(stream,temp,'\n')){
	    temp.pop_back();
	    if(temp!=""){
		printf("usersName %s\n",temp.c_str());
		userVector.push_back(temp);
	    }
	}

	//create_listUser("User",list_users);
	update_tree_view(1);
    }
    return TRUE;
}


static gboolean update_list_rooms(){
    printf("In update room list\n");
    if(uname!=NULL){
	std::string username1 = string(uname);
	std::string password1 = string(pword);
	
	std::string com = string("LIST-ROOMS");
	commanD = com + string(" ") + username1 + string(" ") + password1 + string("\r\n"); 
	sendCommand(host,port,(char*)commanD.c_str(),response);

	std::stringstream stream(response);
	std::string temp;
	
	roomVector.clear();
	while(getline(stream,temp,'\n')){
	    temp.pop_back();
	    if(temp!=""){
		printf("roomName %s/n",temp.c_str());
		roomVector.push_back(temp);
	    }
	}
	update_tree_view(0);
    }
    return TRUE;
}
/* Add some text to our text widget - this is a callback that is invoked
   when our window is realized. We could also force our window to be
   realized with gtk_widget_realize, but it would have to be part of
   a hierarchy first */


/*static gboolean
time_handler(GtkWidget *widget){
    if (widget->window == NULL) return FALSE;

    //gtk_widget_queue_draw(widget);

    //update_list_rooms();

    return TRUE;
}*/

void createInputLRoom(GtkWidget *widget, gpointer data){
    char * name = (char*) gtk_entry_get_text(GTK_ENTRY(roomName));
    roomname = strdup(name);
    std::string room1 = string(name);
    std::string username1 = string(uname);
    std::string password1 = string(pword);

    if(strcmp("LEAVE-ROOM",command)==0){
	commanD = command + string(" ") + username1 + string(" ") + password1 + string(" ") + room1 + string("\r\n");
	sendCommand(host,port,(char*)commanD.c_str(),response);
	return;
    }
    if(strcmp(response, "OK\r\n") == 0) {
	update_list_rooms();
	std::string com = string("SEND-MESSAGE");
	string x = username1 + " left the room";
	commanD = com + string(" ") + username1 + string(" ") + password1 + string(" ") + room1 + string(" ") + x + string("\r\n"); 
	sendCommand(host,port,(char*)commanD.c_str(),messagesToPut);	
    }
}

void createInputERoom(GtkWidget *widget, gpointer data){
    char * responseR = (char*)malloc(sizeof(char)*MAX_RESPONSE);
    char * name = (char*) gtk_entry_get_text(GTK_ENTRY(roomName));
    roomname = strdup(name);
    // printf("room is%s\n",roomname);

   std::string room1 = string(name);
    std::string username1 = string(uname);
    std::string password1 = string(pword);

    if(strcmp("ENTER-ROOM",command)==0){
	commanD = command + string(" ") + username1 + string(" ") + password1 + string(" ") + room1 + string("\r\n");
	sendCommand(host,port,(char*)commanD.c_str(),responseR);
	return;
    }
    if(responseR == "OK\r\n") {
	printf("in here now!!!!!!!!!!!!!!!!!!!!!!!");
	update_list_rooms();
	std::string com = string("SEND-MESSAGE");
	string x = username1 + " entered the room";
	commanD = com + string(" ") + username1 + string(" ") + password1 + string(" ") + room1 + string(" ") + x + string("\r\n"); 
	sendCommand(host,port,(char*)commanD.c_str(),messagesToPut);	
    }
}



void createMessage(GtkWidget* widget, GtkWidget* data){
    char * enteredMessage = (char*) gtk_entry_get_text(GTK_ENTRY(enterMessage));
    std::string gotMessage = string(enteredMessage);
    string room1 = string(roomname);
    /*const char * room = (char*)gtk_entry_get_text(GTK_ENTRY(roomName));
      std:: string room1 = string(room);*/
    std::string username1 = string(uname);
    std::string password1 = string(pword);

    /*GtkTextIter iter;
    GtkTextIter iter2;
    const GtkTextIter iter11 = iter;
    const GtkTextIter iter22 = iter2;
    GtkWidget *view;
    GtkTextBuffer *buffer;
    buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (view));
    gtk_text_buffer_get_iter_at_offset (buffer, &iter, 0);
    gtk_text_buffer_get_iter_at_offset (buffer, &iter2, -1);
    view = gtk_bin_get_child(GTK_BIN(data));

    char* message = (char*)gtk_text_buffer_get_text(buffer,&((const GtkTextIter)iter11),&((const GtkTextIter)iter22),FALSE);*/  

    std::string com = string("SEND-MESSAGE");
    commanD = com + string(" ") + username1 + string(" ") + password1 + string(" ") + room1 + string(" ") + string(gotMessage) + string("\r\n"); 
    sendCommand(host,port,(char*)commanD.c_str(),response);

    /*if(strstr(response, "OK\r\n") != NULL) {
	update_messages();
    }*/
}

void createInputRoom(GtkWidget* widget, gpointer data){
    char* name = (char*) gtk_entry_get_text(GTK_ENTRY(roomName));
    std::string room1 = string(name);
    /*const char * room = (char*)gtk_entry_get_text(GTK_ENTRY(roomName));
    std:: string room1 = string(room);*/
    std::string username1 = string(uname);
    std::string password1 = string(pword);

    if(strcmp("CREATE-ROOM",command) == 0){
	commanD = command + string(" ") + username1 + string(" ") + password1 + string(" ") + room1 + string("\r\n");
	sendCommand(host,port,(char*)commanD.c_str(),response);
	return;
    }
    if(strstr(response, "OK\r\n") != NULL) {
	update_list_rooms();
    }
}
void createInput(GtkWidget *widget, gpointer data){
    const char * user = (char*)gtk_entry_get_text(GTK_ENTRY(userName));
    std:: string username1 = string(user);
    const char * pass = (char*)gtk_entry_get_text(GTK_ENTRY(passWord));
    std:: string password1 = string(pass);
   /* const char * room = (char*)gtk_entry_get_text(GTK_ENTRY(roomName));
    std:: string room1 = string(room);*/

    string space = string(" ");

    if (strcmp("ADD-USER", command) == 0){
	uname=strdup(user);
	pword=strdup(pass);
	commanD = command + space + username1 + space + password1 + string("\r\n");
	sendCommand(host, port, (char*)commanD.c_str(), response);
	return;
    }
    else if (strcmp("LOG-IN", command) == 0){
	uname=strdup(user);
	pword=strdup(pass);
	commanD = command + space + username1 + space + password1 + string("\r\n");
	sendCommand(host,port,(char*)commanD.c_str(),response);
//	update_list_users();
	return;
    }
    /*else if(strcmp("CREATE-ROOM",command) == 0){
	commanD = command + space + username1 + space + password1 + space + room1 + string("\r\n");
	sendCommand(host,port,(char*)commanD.c_str(),response);
	return;
    }*/

}


void enterRoom(GtkWidget * widget, gpointer data){
    GtkWidget *enterWindow;
    enterWindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW (enterWindow), "Enter A Room");
    //g_signal_connect (enterWindow, "destroy", G_CALLBACK (gtk_main_quit), NULL);
    gtk_container_set_border_width (GTK_CONTAINER (enterWindow), 10);
    gtk_widget_set_size_request (GTK_WIDGET (enterWindow), 250, 200);
    gtk_widget_show(enterWindow);

    GtkWidget *table = gtk_table_new (8, 4, TRUE);
    gtk_container_add (GTK_CONTAINER (enterWindow), table);
    gtk_table_set_row_spacings(GTK_TABLE (table), 4);
    gtk_table_set_col_spacings(GTK_TABLE (table), 4);
    gtk_widget_show (table);

    GtkWidget *labelName = gtk_label_new("Which Room Do You Want to Enter");
    gtk_table_attach_defaults (GTK_TABLE (table), labelName, 0, 4, 0, 1);
    gtk_widget_show (labelName);

    /*list_rooms = gtk_list_store_new (1, G_TYPE_STRING);
    //update_list_rooms();
    GtkWidget *listRooms = create_listRoom ("Rooms", list_rooms);
    gtk_table_attach_defaults (GTK_TABLE (table), listRooms, 0, 4, 1, 5);
    gtk_widget_show (listRooms);*/

    GtkWidget * labelName1 = gtk_label_new("Enter room name to Enter");
    gtk_table_attach_defaults (GTK_TABLE (table), labelName1, 0, 4, 5, 6);
    gtk_widget_show(labelName1);

    roomName = gtk_entry_new();
    gtk_table_attach_defaults (GTK_TABLE (table), roomName, 0, 4, 6, 7);
    gtk_widget_show (roomName);

    command = (char*) "ENTER-ROOM";

    GtkWidget *done_button = gtk_button_new_with_label ("Enter Room");
    gtk_table_attach_defaults(GTK_TABLE (table), done_button, 0, 4, 7, 8);
    gtk_signal_connect(GTK_OBJECT (done_button), "clicked", GTK_SIGNAL_FUNC (createInputERoom), NULL);
    g_signal_connect_swapped(GTK_OBJECT (done_button), "clicked", G_CALLBACK (gtk_widget_destroy), enterWindow);
    gtk_widget_show (done_button);

    gtk_widget_show(enterWindow);
    gtk_widget_show(table);
}

void leaveRoom(GtkWidget * widget, gpointer data){
    GtkWidget *leaveWindow;
    leaveWindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW (leaveWindow), "Leave A Room");
    //g_signal_connect (leaveWindow, "destroy", G_CALLBACK (gtk_main_quit), NULL);
    gtk_container_set_border_width (GTK_CONTAINER (leaveWindow), 10);
    gtk_widget_set_size_request (GTK_WIDGET (leaveWindow), 250, 200);
    gtk_widget_show(leaveWindow);

    GtkWidget *table = gtk_table_new (8, 4, TRUE);
    gtk_container_add (GTK_CONTAINER (leaveWindow), table);
    gtk_table_set_row_spacings(GTK_TABLE (table), 5);
    gtk_table_set_col_spacings(GTK_TABLE (table), 5);
    gtk_widget_show (table);

    GtkWidget *labelName = gtk_label_new("Which Room Do You Want to Leave");
    gtk_table_attach_defaults (GTK_TABLE (table), labelName, 0, 4, 0, 1);
    gtk_widget_show (labelName);

    /*list_rooms = gtk_list_store_new (1, G_TYPE_STRING);
    //update_list_rooms();
    GtkWidget *listRooms = create_listRoom ("Rooms", list_rooms);
    gtk_table_attach_defaults (GTK_TABLE (table), listRooms, 0, 4, 1, 5);
    gtk_widget_show (listRooms);*/

    GtkWidget * labelName1 = gtk_label_new("Enter room name to leave");
    gtk_table_attach_defaults (GTK_TABLE (table), labelName1, 0, 4, 5, 6);
    gtk_widget_show(labelName1);

    roomName = gtk_entry_new();
    gtk_table_attach_defaults (GTK_TABLE (table), roomName, 0, 4, 6, 7);
    gtk_widget_show (roomName);

    command = (char*) "LEAVE-ROOM";

    GtkWidget *done_button = gtk_button_new_with_label ("Leave Room");
    gtk_table_attach_defaults(GTK_TABLE (table), done_button, 0, 4, 7, 8);
    gtk_signal_connect(GTK_OBJECT (done_button), "clicked", GTK_SIGNAL_FUNC (createInputLRoom), NULL);
    g_signal_connect_swapped(GTK_OBJECT (done_button), "clicked", G_CALLBACK (gtk_widget_destroy), leaveWindow);
    gtk_widget_show (done_button);

    gtk_widget_show(leaveWindow);
    gtk_widget_show(table);
}

void createRoom(GtkWidget * widget, gpointer data){
    GtkWidget *roomWindow;
    roomWindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW (roomWindow), "Create Your Room");
    gtk_container_set_border_width (GTK_CONTAINER (roomWindow), 10);
    gtk_widget_set_size_request (GTK_WIDGET (roomWindow), 250, 200);
    gtk_widget_show(roomWindow);

    GtkWidget *table = gtk_table_new (2, 2, TRUE);
    gtk_container_add (GTK_CONTAINER (roomWindow), table);
    gtk_table_set_row_spacings(GTK_TABLE (table), 5);
    gtk_table_set_col_spacings(GTK_TABLE (table), 5);
    gtk_widget_show (table);

    GtkWidget *labelName = gtk_label_new("Enter A Room Name");
    gtk_table_attach_defaults (GTK_TABLE (table), labelName, 0, 3, 0, 2);
    gtk_widget_show (labelName);

    roomName = gtk_entry_new();
    gtk_table_attach_defaults (GTK_TABLE (table), roomName, 0, 3, 1, 2);
    gtk_widget_show (roomName);

    command = (char*) "CREATE-ROOM";

    GtkWidget *done_button = gtk_button_new_with_label ("Create Room");
    gtk_table_attach_defaults(GTK_TABLE (table), done_button, 0, 3, 2, 3);
    gtk_signal_connect(GTK_OBJECT (done_button), "clicked", GTK_SIGNAL_FUNC (createInputRoom), NULL);
    g_signal_connect_swapped(GTK_OBJECT (done_button), "clicked", G_CALLBACK (gtk_widget_destroy), roomWindow);
    gtk_widget_show (done_button);

    gtk_widget_show(roomWindow);
    gtk_widget_show(table);
}

void loginAccount(GtkWidget * widget, gpointer data){
    GtkWidget *loginWindow;
    loginWindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW (loginWindow), "Login to Your Account");
    //g_signal_connect (loginWindow, "destroy", G_CALLBACK (gtk_main_quit), NULL);
    gtk_container_set_border_width (GTK_CONTAINER (loginWindow), 10);
    gtk_widget_set_size_request (GTK_WIDGET (loginWindow), 250, 200);
    gtk_widget_show(loginWindow);

    GtkWidget *table = gtk_table_new (4, 4, TRUE);
    gtk_container_add (GTK_CONTAINER (loginWindow), table);
    gtk_table_set_row_spacings(GTK_TABLE (table), 5);
    gtk_table_set_col_spacings(GTK_TABLE (table), 5);
    gtk_widget_show (table);

    GtkWidget *labelUser = gtk_label_new("Username");
    gtk_table_attach_defaults (GTK_TABLE (table), labelUser, 0, 4, 0, 1);
    gtk_widget_show (labelUser);

    userName = gtk_entry_new();
    gtk_table_attach_defaults (GTK_TABLE (table), userName, 0, 4, 1, 2);
    gtk_widget_show (userName);

    GtkWidget *labelPassword = gtk_label_new("Password");
    gtk_table_attach_defaults (GTK_TABLE (table), labelPassword, 0, 4, 2, 3);
    gtk_widget_show (labelPassword);

    passWord = gtk_entry_new();
    gtk_table_attach_defaults (GTK_TABLE (table), passWord, 0, 4, 3, 4);
    gtk_widget_show (passWord);

    command = (char*)"LOG-IN";

    GtkWidget *login_button = gtk_button_new_with_label ("Login");
    gtk_table_attach_defaults(GTK_TABLE (table), login_button, 0, 4, 4, 5);
    gtk_signal_connect(GTK_OBJECT (login_button), "clicked", GTK_SIGNAL_FUNC (createInput), NULL);
   // gtk_signal_connect(GTK_OBJECT (login_button), "clicked", GTK_SIGNAL_FUNC (createInput), password);
    g_signal_connect_swapped(GTK_OBJECT (login_button), "clicked", G_CALLBACK (gtk_widget_destroy), loginWindow);
    gtk_widget_show (login_button);

    gtk_widget_show(loginWindow);
    gtk_widget_show(table);
}
void createAccount(GtkWidget * widget, gpointer data){
    GtkWidget *accountWindow;
    accountWindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW (accountWindow), "Create Your Account");
    //g_signal_connect (accountWindow, "destroy", G_CALLBACK (gtk_main_quit), NULL);
    gtk_container_set_border_width (GTK_CONTAINER (accountWindow), 10);
    gtk_widget_set_size_request (GTK_WIDGET (accountWindow), 250, 200);
    gtk_widget_show(accountWindow);

    GtkWidget *table = gtk_table_new (4, 4, TRUE);
    gtk_container_add (GTK_CONTAINER (accountWindow), table);
    gtk_table_set_row_spacings(GTK_TABLE (table), 5);
    gtk_table_set_col_spacings(GTK_TABLE (table), 5);
    gtk_widget_show (table);

    GtkWidget *labelUser = gtk_label_new("Username");
    gtk_table_attach_defaults (GTK_TABLE (table), labelUser, 0, 4, 0, 1);
    gtk_widget_show (labelUser);

    userName = gtk_entry_new();
    gtk_table_attach_defaults (GTK_TABLE (table), userName, 0, 4, 1, 2);
    gtk_widget_show (userName);

    GtkWidget *labelPassword = gtk_label_new("Password");
    gtk_table_attach_defaults (GTK_TABLE (table), labelPassword, 0, 4, 2, 3);
    gtk_widget_show (labelPassword);

    passWord = gtk_entry_new();
    gtk_table_attach_defaults (GTK_TABLE (table), passWord, 0, 4, 3, 4);
    gtk_widget_show (passWord);

    command = (char*)"ADD-USER";

    GtkWidget *done_button = gtk_button_new_with_label ("Create");
    gtk_table_attach_defaults(GTK_TABLE (table), done_button, 0, 4, 4, 5);
    gtk_signal_connect(GTK_OBJECT (done_button), "clicked", GTK_SIGNAL_FUNC (createInput), NULL);
    // gtk_signal_connect(GTK_OBJECT (done_button), "clicked", GTK_SIGNAL_FUNC (createInput), password);
    g_signal_connect_swapped(GTK_OBJECT (done_button), "clicked", G_CALLBACK (gtk_widget_destroy), accountWindow);
    gtk_widget_show (done_button);

    gtk_widget_show(accountWindow);
    gtk_widget_show(table);
}

void
printUsage(){
    printf("Usage: talk-client-server port\n");
    exit(1);
}
int main( int   argc, char *argv[] ){
    if (argc < 2) {
	printUsage();
    }
    port = atoi(argv[1]);

    GtkWidget *window;
    GtkWidget *listRooms;
    //tree_view_rooms = gtk_bin_get_child(GTK_BIN(listRooms));   
    GtkWidget *listUsers;
    GtkWidget *messages;
    GtkWidget *myMessage;

    gtk_init (&argc, &argv);

    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    GdkColor color;
    color.red = 0xffff;
    gtk_window_set_title (GTK_WINDOW (window), "Paned Windows");
    gtk_widget_modify_bg(window, GTK_STATE_NORMAL, &color);
    g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);
    gtk_container_set_border_width (GTK_CONTAINER (window), 10);
    gtk_widget_set_size_request (GTK_WIDGET (window), 450, 400);

    // Create a table to place the widgets. Use a 7x4 Grid (7 rows x 4 columns)
    GtkWidget *table = gtk_table_new (8, 4, TRUE);
    gtk_container_add (GTK_CONTAINER (window), table);
    gtk_table_set_row_spacings(GTK_TABLE (table), 5);
    gtk_table_set_col_spacings(GTK_TABLE (table), 5);
    gtk_widget_show (table);

    // Add list of rooms. Use columns 0 to 4 (exclusive) and rows 0 to 4 (exclusive)
    list_rooms = gtk_list_store_new (1, G_TYPE_STRING);
    //update_list_rooms();
    listRooms = create_listRoom ("Rooms", list_rooms);
    //tree_view_rooms = gtk_bin_get_child(GTK_BIN(listRooms));
    g_timeout_add(5000,(GSourceFunc) update_list_rooms, (gpointer) list_rooms);
    gtk_table_attach_defaults (GTK_TABLE (table), listRooms, 0, 2, 0, 2);
    gtk_widget_show (listRooms);

    // Add list of USERS. 
    list_users = gtk_list_store_new (1, G_TYPE_STRING);
    //update_list_rooms();
    listUsers = create_listUser ("Users", list_users);
    //tree_view_users = gtk_bin_get_child(GTK_BIN(listUsers));
    g_timeout_add(5000, (GSourceFunc) update_list_users, (gpointer) list_users);
    gtk_table_attach_defaults (GTK_TABLE (table), listUsers, 2, 4, 0, 2);
    gtk_widget_show (listUsers);

    // Add messages text. Use columns 0 to 4 (exclusive) and rows 4 to 7 (exclusive) 
    messages = create_text ("");
    g_timeout_add(2000, (GSourceFunc) update_messages, messages);
    gtk_table_attach_defaults (GTK_TABLE (table), messages, 0, 4, 2, 5);
    gtk_widget_show (messages);

    // Add messages text. Use columns 0 to 4 (exclusive) and rows 4 to 7 (exclusive) 
    /*myMessage = create_text ("");
    gtk_table_attach_defaults (GTK_TABLE (table), myMessage, 0, 4, 5, 7);
    gtk_widget_show (myMessage);*/
    enterMessage = gtk_entry_new();
    gtk_table_attach_defaults (GTK_TABLE (table), enterMessage, 0, 4, 5, 7);
    gtk_widget_show (enterMessage);

    // Add send button. Use columns 0 to 1 (exclusive) and rows 4 to 7 (exclusive)
    GtkWidget *send_button = gtk_button_new_with_label ("Send");
    gtk_table_attach_defaults(GTK_TABLE (table), send_button, 0, 1, 7, 9);
    gtk_signal_connect(GTK_OBJECT (send_button), "clicked", GTK_SIGNAL_FUNC (createMessage),(gpointer) enterMessage);
    gtk_widget_show (send_button);

    //Add Create Account Button
    GtkWidget *create_account_button = gtk_button_new_with_label ("Create Account");
    gtk_table_attach_defaults(GTK_TABLE (table), create_account_button, 1, 2, 7, 8);
    gtk_signal_connect(GTK_OBJECT (create_account_button), "clicked", GTK_SIGNAL_FUNC (createAccount),(gpointer) "");
    gtk_widget_show (create_account_button);

    //Add Login Button
    GtkWidget *login_button = gtk_button_new_with_label ("Login");
    gtk_table_attach_defaults(GTK_TABLE (table), login_button, 1, 2, 8, 9);
    gtk_signal_connect(GTK_OBJECT (login_button), "clicked", GTK_SIGNAL_FUNC (loginAccount),(gpointer) "");
    gtk_widget_show (login_button);

    //Add Create Room Button
    GtkWidget *create_room_button = gtk_button_new_with_label ("Create Room");
    gtk_table_attach_defaults(GTK_TABLE (table), create_room_button, 2, 3, 7, 8);
    gtk_signal_connect(GTK_OBJECT (create_room_button), "clicked", GTK_SIGNAL_FUNC (createRoom),(gpointer)"");
    gtk_widget_show (create_room_button);

    //Add Enter Room Button
    GtkWidget *enter_room_button = gtk_button_new_with_label ("Enter Room");
    gtk_table_attach_defaults(GTK_TABLE (table), enter_room_button, 3, 4, 7, 8);
    gtk_signal_connect(GTK_OBJECT (enter_room_button), "clicked", GTK_SIGNAL_FUNC (enterRoom),(gpointer)"");
    gtk_widget_show (enter_room_button);

    //Add Leave Room Button
    GtkWidget *leave_room_button = gtk_button_new_with_label ("Leave Room");
    gtk_table_attach_defaults(GTK_TABLE (table), leave_room_button, 3, 4, 8, 9);
    gtk_signal_connect(GTK_OBJECT (leave_room_button), "clicked", GTK_SIGNAL_FUNC (leaveRoom),(gpointer)"");
    gtk_widget_show (leave_room_button);

    gtk_widget_show (table);
    gtk_widget_show (window);

    gtk_window_set_position(GTK_WINDOW(window) ,GTK_WIN_POS_CENTER);
    gtk_window_set_title(GTK_WINDOW(window), "Farhan's IRCClient");

   // g_timeout_add(5000, (GSourceFunc) time_handler, (gpointer) window);

    gtk_main ();

    return 0;
}
