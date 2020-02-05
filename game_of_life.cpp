// Name: Cihat Kapusuz
// Student ID: 2016400126
// Compilation Status: Compiling
// Working Status: Working
// Periodic Bonus: Done
// Checkered Bonus: Done
#include "mpi.h"
#include <iostream>
#include <fstream>
#include <math.h>
#include <vector>
#include <algorithm>
#define MATRIX_SIZE 360
using namespace std;
void send_left_right_top_bot(int rank, int tag, int len_row, int size, int jump, int sleft[], int sright[], int stop[], int sbottom[]){
		// sends right edge to the right worker		
		if((rank%jump)==0){ // if the processor on the right edge it sends the processor which has (jump-1) rank lower
			MPI_Send(sright, len_row, MPI_INT, (rank-jump+1), tag, MPI_COMM_WORLD);
		}
		else{
			MPI_Send(sright, len_row, MPI_INT, (rank+1), tag, MPI_COMM_WORLD);
		}		
		// sends left edge to the left worker
		if((rank%jump)==1){ // if the processor on the left edge it sends the processor which has (jump-1) rank higher
			MPI_Send(sleft, len_row, MPI_INT, (rank+jump-1), tag, MPI_COMM_WORLD);
		}
		else{
			MPI_Send(sleft, len_row, MPI_INT, (rank-1), tag, MPI_COMM_WORLD);
		}
		// sends top edge to the top worker
		if(rank==jump){ // if the processor is at the right top corner it sends to highest ranked processor
			MPI_Send(stop, len_row, MPI_INT, (size-1), tag, MPI_COMM_WORLD);	
		}
		else{
			MPI_Send(stop, len_row, MPI_INT, (rank+size-1-jump)%(size-1), tag, MPI_COMM_WORLD);
		}	
		// sends bottom edge to the lower worker
		if(rank==(size-1-jump)){ // if the processor is at the 1 row upper to the right bottom corner it sends to highest ranked processor
			MPI_Send(sbottom, len_row, MPI_INT, (rank+jump), tag, MPI_COMM_WORLD);
		}
		else{
			MPI_Send(sbottom, len_row, MPI_INT, ((rank+jump)%(size-1)), tag, MPI_COMM_WORLD);
		}
}
void receive_right_left_bot_top(int rank, int tag, int len_row, int size, int jump, int rright[], int rleft[], int rbottom[], int rtop[]){
		// receives right edge of the right worker as additional left column
		if((rank%jump)==1){ // if the processor is at the left edge it will receive it's left column from (jump-1) rank higher
			MPI_Recv(rleft, len_row, MPI_INT, (rank+jump-1), tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
		else{
			MPI_Recv(rleft, len_row, MPI_INT, (rank-1), tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
		// receives left edge of the right worker as additional right column
		if((rank%jump)==0){ // if the processor is at the right edge it will receive it's left column from (jump-1) rank lower
			MPI_Recv(rright, len_row, MPI_INT, (rank-jump+1), tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
		else{
			MPI_Recv(rright, len_row, MPI_INT, (rank+1), tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
		// receives top edge of the bottom worker as additional bottom row
		if(rank==(size-1-jump)){ // if the processor is at the 1 row upper to the right bottom corner it receives from the highest ranked processor
			MPI_Recv(rbottom, len_row, MPI_INT, (rank+jump), tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
		else{
			MPI_Recv(rbottom, len_row, MPI_INT, (rank+jump)%(size-1), tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
		// receives bottom edge of the top worker as additional top row
		if(rank==jump){ // if the processor is at the right top corner it recieves from the highest ranked processor
			MPI_Recv(rtop, len_row, MPI_INT, size-1, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
		else{
			MPI_Recv(rtop, len_row, MPI_INT, (rank+size-1-jump)%(size-1), tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
}
void send_all_corners(int rank, int tag, int size, int jump, int stopleftcorner[], int stoprightcorner[], int sbottomleftcorner[], int sbottomrightcorner[]){
//sendtopleft
		if(rank==1){ // if rank is 1, send top left corner to the highest ranked processor
			MPI_Send(stopleftcorner, 1, MPI_INT, (size-1), tag, MPI_COMM_WORLD);
		}
		else if(rank%jump==1){ // else if processor is at left edge, send top left corner to previous processor
			MPI_Send(stopleftcorner, 1, MPI_INT, (rank-1), tag, MPI_COMM_WORLD);
		}
		else if(rank<=jump){ // else if processor is at top edge, send top left corner to (size-1-jump-1) process higher
			MPI_Send(stopleftcorner, 1, MPI_INT, (rank-jump-1+size-1), tag, MPI_COMM_WORLD);
		}
		else{
			MPI_Send(stopleftcorner, 1, MPI_INT, (rank-jump-1), tag, MPI_COMM_WORLD);
		}
		//sendtopright
		if(rank==jump){ // if rank equals jump, send top right corner to the processor which is at bottom left
			MPI_Send(stoprightcorner, 1, MPI_INT, (size-1-jump+1), tag, MPI_COMM_WORLD);
		}
		else if(rank%jump==0){ // else if processor is at right edge, send top right corner to the processor which is ((2*jump)-1) rank lower
			MPI_Send(stoprightcorner, 1, MPI_INT, (rank-(2*jump)+1), tag, MPI_COMM_WORLD);
		}
		else if(rank<=jump){ // else if processor is at top edge, send top right corner to (size-1-jump+1) processor higher
			MPI_Send(stoprightcorner, 1, MPI_INT, (rank-jump+1+size-1), tag, MPI_COMM_WORLD);
		}
		else{
			MPI_Send(stoprightcorner, 1, MPI_INT, (rank-jump+1), tag, MPI_COMM_WORLD);
		}
		//sendbottomleft
		if(rank==(size-1-jump+1)){ // if the processor is at the bottom left corner, send bottom left corner to the processor which is at top right corner 
			MPI_Send(sbottomleftcorner, 1, MPI_INT, (jump), tag, MPI_COMM_WORLD);
		}
		else if(rank%jump==1){ // else if the processor is at the left edge, send bottom left corner to the processor which is ((2*jump)-1) rank higher
			MPI_Send(sbottomleftcorner, 1, MPI_INT, (rank+(2*jump)-1), tag, MPI_COMM_WORLD);
		}
		else if(rank>=(size-1-jump+1)){ // else if processor is at bottom edge, send bottom left corner to the processor which is (size-1-jump+1) rank lower
			MPI_Send(sbottomleftcorner, 1, MPI_INT, (jump+rank-size), tag, MPI_COMM_WORLD);			
		}
		else{
			MPI_Send(sbottomleftcorner, 1, MPI_INT, (rank+jump-1), tag, MPI_COMM_WORLD);
		}
		//sendbottomright
		if(rank==(size-1)){ // if the processor is at bottom right corner, send bottom right corner to the first processor
			MPI_Send(sbottomrightcorner, 1, MPI_INT, (1), tag, MPI_COMM_WORLD);
		}
		else if(rank%jump==0){ // else if the processor is at the right edge, send bottom right corner to the next processor
			MPI_Send(sbottomrightcorner, 1, MPI_INT, (rank+1), tag, MPI_COMM_WORLD);
		}
		else if(rank>=(size-1-jump+1)){ // else if the processor is at the bottom edge, send bottom right corner to the processor which is (size-1-jump-1) rank lower 
			MPI_Send(sbottomrightcorner, 1, MPI_INT, (rank+jump+1-(size-1)), tag, MPI_COMM_WORLD);
		}
		else{
			MPI_Send(sbottomrightcorner, 1, MPI_INT, (rank+jump+1), tag, MPI_COMM_WORLD);
		}
}
void receive_all_corners(int rank, int tag, int size, int jump, int rbottomrightcorner[], int rbottomleftcorner[], int rtoprightcorner[], int rtopleftcorner[]){
		//recievebottomright
		if(rank==(size-1)){ // if processor is at right bottom corner, receive bottom right corner from processor which has rank 1
			MPI_Recv(rbottomrightcorner, 1, MPI_INT, (1), tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
		else if(rank%jump==0){ // else if processor is at right edge, receive bottom right corner from next processor
			MPI_Recv(rbottomrightcorner, 1, MPI_INT, (rank+1), tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
		else if(rank>=(size-1-jump+1)){ // else if processor is at bottom edge, receive bottom right corner from the processor which is (size-1-jump-1) rank lower
			MPI_Recv(rbottomrightcorner, 1, MPI_INT, (jump-size+1+rank+1), tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
		else{
			MPI_Recv(rbottomrightcorner, 1, MPI_INT, (rank+jump+1), tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
		//receivebottomleft
		if(rank==(size-1-jump+1)){ // if processor is at left bottom corner, receive bottom left corner from processor which has rank jump
			MPI_Recv(rbottomleftcorner, 1, MPI_INT, (jump), tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
		else if(rank%jump==1){ // else if processor is at left edge, receive bottom left corner from the processor which is ((2*jump)-1) rank higher
			MPI_Recv(rbottomleftcorner, 1, MPI_INT, (rank+(2*jump)-1), tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
		else if(rank>=(size-1-jump+1)){ // else if processor is at bottom edge, receive bottom right corner from the processor which is ((size-1)-jump+1) rank lower
			MPI_Recv(rbottomleftcorner, 1, MPI_INT, (rank-(size-1)+jump-1), tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
		else{
			MPI_Recv(rbottomleftcorner, 1, MPI_INT, (rank+jump-1), tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
		//receivetopright
		if(rank==jump){ // if processor is at right top corner, receive top right corner from processor which is at left bottom corner
			MPI_Recv(rtoprightcorner, 1, MPI_INT, (size-1-jump+1), tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
		else if(rank%jump==0){ // if processor is at right edge, receive top right corner from processor which is ((2*jump)-1) rank lower
			MPI_Recv(rtoprightcorner, 1, MPI_INT, (rank-(2*jump)+1), tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
		else if(rank<=jump){ // if process is at top edge, receive top right corner from processor which is (size-1-jump+1) rank higher
			MPI_Recv(rtoprightcorner, 1, MPI_INT, (size-1+rank-jump+1), tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
		else{
			MPI_Recv(rtoprightcorner, 1, MPI_INT, (rank-jump+1), tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}		
		//receivetopleft
		if(rank==1){ // if processor is at left top corner, receive top left corner from processor which is at right bottom corner
			MPI_Recv(rtopleftcorner, 1, MPI_INT, (size-1), tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
		else if(rank%jump==1){ // else if processor is at left edge, receive top left corner from previous processor
			MPI_Recv(rtopleftcorner, 1, MPI_INT, (rank-1), tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
		else if(rank<=jump){ // else if processor is at top edge, receive top left corner from processor which is (size-1-jump-1) rank higher
			MPI_Recv(rtopleftcorner, 1, MPI_INT, (rank+(size-1)-jump-1), tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
		else{
			MPI_Recv(rtopleftcorner, 1, MPI_INT, (rank-jump-1), tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
}

int main(int argc, char *argv[])
{
    int rank, size, matrix[MATRIX_SIZE][MATRIX_SIZE], tag = 201; // defines rank, size main matrix and tag
    int stoprightcorner[1], rtoprightcorner[1];			 // defines send and recieve top right corner
    int stopleftcorner[1], rtopleftcorner[1];			 // defines send and recieve top left corner
    int sbottomrightcorner[1], rbottomrightcorner[1];		 // defines send and recieve bottom right corner
    int sbottomleftcorner[1], rbottomleftcorner[1];		 // defines send and recieve bottom left corner
    
    MPI_Init(&argc, &argv); //Starts the MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // gets rank of the processor
    MPI_Comm_size(MPI_COMM_WORLD, &size); // gets how many processor are in the program
	int no_alive_neighbour=0;         // number of alive neighbour to compare with game restrictions
	int jump= (int)sqrt((size-1));    // sqrt of the worker processes
	int len_row = MATRIX_SIZE/jump;   // number of rows and columns in 1 worker processor
	int message[len_row][len_row];    // part of array which will be stored in the processor
	int temp[len_row][len_row];	  // temporary array to update each node with respect to 'Game of Life'
	int sright[len_row];              // an array that will send right edge of the belonging processor
	int sleft[len_row];		  // an array that will send left edge of the belonging processor
	int stop[len_row];		  // an array that will send top edge of the belonging processor
	int sbottom[len_row];             // an array that will send bottom edge of the belonging processor
	int rright[len_row];		  // an array that will hold received additional right edge from the left processor
	int rleft[len_row];		  // an array that will hold received additional left edge from the right processor
	int rtop[len_row];		  // an array that will hold received additional top edge from the top processor
	int rbottom[len_row];		  // an array that will hold received additional bottom edge from the bottom processor
	int no_turn=atoi(argv[3]);        // number of turns that game will be played
	int turn=0;                       // initialize current turn
	vector<int> ranker1;		  // to decide which processors will send its top, bottom, left and right edge first then recieve
	vector<int> ranker2;		  // to decide which processors will send its top right-left and bottom right-left corner first then recieve
	for(int i=0; i<jump; i+=2){
		for(int j=1; j<jump; j+=2){
			ranker1.push_back(i*jump+j); // adds odd ranked processors in even rows
		}
	}
	for(int i=0; i<jump; i+=2){
		for(int j=2; j<=jump; j+=2){
			ranker1.push_back((i+1)*jump+j);  // adds even ranked processors in odd rows
		}
	}
	for(int i=0; i<jump; i+=2){
		for(int j=1; j<=jump; j++){
			ranker2.push_back(i*jump+j);  // adds even numbered rows' processors
		}
	}
		
    if (0 == rank) {
	//take input into the manager process
	ifstream myfile;
	myfile.open(argv[1]);
		if (myfile.is_open()) {
			for(int i=0; i<MATRIX_SIZE; i++){
				for(int j=0; j<MATRIX_SIZE; j++){
					myfile >> matrix[i][j];			
				}
			}
		}
	myfile.close();
	//take input completed

	//split messages with corresponding ranks and send them
	for(int i=0; i<MATRIX_SIZE; i+=len_row){ // i represents the corresponding row's first index
		for( int j=0; j<MATRIX_SIZE; j+=len_row){ // j represents the corresponding column's first index
			for(int k=i; k<i+len_row; k++){ 
				for(int l=j; l<j+len_row; l++){
					message[k-i][l-j]=matrix[k][l];
				}
			}
		MPI_Send(message, MATRIX_SIZE*MATRIX_SIZE/(jump*jump), MPI_INT, (((i*jump*jump)/MATRIX_SIZE)+(j/(len_row)))+1, tag, MPI_COMM_WORLD);
		}
	}
	//message send from rank 0 is done

    }

	if (rank!=0) {
	        MPI_Recv(message, MATRIX_SIZE*MATRIX_SIZE/(jump*jump), MPI_INT, 0, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // receive all messages that have sent from rank 0
		//start turn loop
		while(turn<no_turn){			
		turn++;	
		// initialize temp array to be used for 'Game of Life' restrictions
		for(int i=0; i<len_row; i++){
			for(int j=0; j<len_row; j++){
				temp[i][j]=message[i][j];
			}
		}
		// initialization of temp array finished
		// initialize necessary parts that needs to be sent to other processes
	        for(int i=0; i<len_row; i++){
			sright[i]=message[i][(len_row)-1]; 	          // gets right part of the its grid before send
			sleft[i]=message[i][0];			          // gets left part of the its grid before send
			stop[i]=message[0][i];			          // gets top part of the its grid before send
			sbottom[i]=message[(len_row)-1][i]; 	          // gets bottom part of the its grid before send
		}
		stopleftcorner[0]=message[0][0];                          // gets top left corner of the message before sending it
		stoprightcorner[0]=message[0][(len_row)-1];		  // gets top right corner of the message before sending it
		sbottomleftcorner[0]=message[(len_row)-1][0];		  // gets bottom left corner of the message before sending it
		sbottomrightcorner[0]=message[(len_row)-1][(len_row)-1];  // gets bottom right corner of the message before sending it
	// sends and receives top, bottom, left and right edges
	if (find(ranker1.begin(), ranker1.end(), rank) != ranker1.end()){
		
		send_left_right_top_bot(rank, tag, len_row, size, jump, sleft, sright, stop, sbottom);
		receive_right_left_bot_top(rank, tag, len_row, size, jump, rright, rleft, rbottom, rtop);
	
	} else {
		receive_right_left_bot_top(rank, tag, len_row, size, jump, rright, rleft, rbottom, rtop);
		send_left_right_top_bot(rank, tag, len_row, size, jump, sleft, sright, stop, sbottom);
	}
	// sends and receives top, bottom, left and right edges are done

	// send and receives top left-right corners, bottom left-right corners	
	if (find(ranker2.begin(), ranker2.end(), rank) != ranker2.end()){
		send_all_corners(rank, tag, size, jump, stopleftcorner, stoprightcorner, sbottomleftcorner, sbottomrightcorner);
		receive_all_corners(rank, tag, size, jump, rbottomrightcorner, rbottomleftcorner, rtoprightcorner, rtopleftcorner);
	}
	else{
		receive_all_corners(rank, tag, size, jump, rbottomrightcorner, rbottomleftcorner, rtoprightcorner, rtopleftcorner);
		send_all_corners(rank, tag, size, jump, stopleftcorner, stoprightcorner, sbottomleftcorner, sbottomrightcorner);
	}
	// all corner are sent and received
	
	// message passing done, implement the 'Game of Life'
	for(int i=0; i<len_row; i++){
		for(int j=0; j<len_row; j++){
			//implementcorners
			if((i==0) && (j==0)){ // calculate top left corner
				no_alive_neighbour=rtopleftcorner[0]+rleft[0]+rleft[1]+rtop[0]+rtop[1]+message[0][1]+message[1][0]+message[1][1];
			}else if((i==0) && (j==(len_row-1))){ // calculate top right corner
				no_alive_neighbour=rtoprightcorner[0]+rright[0]+rright[1]+rtop[(len_row-2)]+rtop[(len_row-1)]+message[0][(len_row-2)]+message[1][(len_row-1)]+message[1][(len_row-2)];
			}else if((i==(len_row-1)) && (j==(len_row-1))){ // calculate bottom right corner
				no_alive_neighbour=rbottomrightcorner[0]+rright[(len_row-1)]+rright[(len_row-2)]+rbottom[(len_row-2)]+rbottom[(len_row-1)]+message[(len_row-1)][(len_row-2)]+message[(len_row-2)][(len_row-1)]+message[(len_row-2)][(len_row-2)];
			}else if((i==(len_row-1)) && (j==0)){ // calculate bottom left corner
				no_alive_neighbour=rbottomleftcorner[0]+rleft[(len_row-1)]+rleft[(len_row-2)]+rbottom[0]+rbottom[1]+message[(len_row-1)][1]+message[(len_row-2)][0]+message[(len_row-2)][1];
			}//corners are done
			//implement the edges
			else if(i==0){ // calculate top edge without corners
				no_alive_neighbour=rtop[j-1]+rtop[j]+rtop[j+1]+message[0][j-1]+message[0][j+1]+message[1][j-1]+message[1][j]+message[1][j+1];
			}else if(j==0){ // calculate left edge without corners
				no_alive_neighbour=rleft[i-1]+rleft[i]+rleft[i+1]+message[i-1][0]+message[i+1][0]+message[i-1][1]+message[i][1]+message[i+1][1];
			}else if(j==(len_row-1)){ // calculate right edge without corners
				no_alive_neighbour=rright[i-1]+rright[i]+rright[i+1]+message[i-1][(len_row-1)]+message[i+1][(len_row-1)]+message[i-1][(len_row-2)]+message[i][(len_row-2)]+message[i+1][(len_row-2)];
			}else if(i==(len_row-1)){ // calculate bottom edge without corners
				no_alive_neighbour=rbottom[j-1]+rbottom[j]+rbottom[j+1]+message[(len_row-1)][j-1]+message[(len_row-1)][j+1]+message[(len_row-2)][j-1]+message[(len_row-2)][j]+message[(len_row-2)][j+1];
			}
			//edges are done
			else{	
				no_alive_neighbour=message[i-1][j-1]+message[i-1][j]+message[i-1][j+1]+message[i][j-1]+message[i][j+1]+message[i+1][j-1]+message[i+1][j]+message[i+1][j+1];
			}
			// 'Game of Life' rules are applied
			if((message[i][j]==0) && (no_alive_neighbour==3)){
				temp[i][j]=1;
				}
			if((message[i][j]==1) && ((no_alive_neighbour<2) || (no_alive_neighbour>3))){
				temp[i][j]=0;
			}
			// life or death has decided and stored in temp array
		}
	}
	// update the changes from the round
	for(int i=0; i<len_row; i++){
		for(int j=0; j<len_row; j++){
			message[i][j]=temp[i][j];
			}
	}
	// update finished
}
//turn loop finished
	MPI_Send(message, MATRIX_SIZE*MATRIX_SIZE/(jump*jump), MPI_INT, 0, tag, MPI_COMM_WORLD); // send back grid back to the rank 0 process
	}
	if (0 == rank) {
	// manager recieves the matrix back from its workers and puts messages to appropriate location
	for(int i=0; i<MATRIX_SIZE; i+=len_row){
		for( int j=0; j<MATRIX_SIZE; j+=len_row){
		MPI_Recv(message, MATRIX_SIZE*MATRIX_SIZE/(jump*jump), MPI_INT, (((i*jump*jump)/MATRIX_SIZE)+(j/(len_row)))+1,tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE); //recieve message
			for(int k=i; k<i+len_row; k++){
				for(int l=j; l<j+len_row; l++){
					matrix[k][l]=message[k-i][l-j]; // update first matrix
				}
			}
		}
	}

	//write to output file
	ofstream outputfile;
	outputfile.open(argv[2]);	
	for(int i=0; i<MATRIX_SIZE; i++){
		for(int j=0; j<MATRIX_SIZE; j++){
		outputfile << matrix[i][j] << " ";
		}
	outputfile << endl;
	}
    	outputfile.close();
 	}
	//output operation is done

    MPI_Finalize(); // finalize the mpi
    return 0;
}
