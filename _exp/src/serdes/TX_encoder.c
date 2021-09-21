// MSDG reference code that has been ported in serdes_plat.c

// TX_encoder.c : main project file.


#include<stdio.h>
#include<stdlib.h>


int dec2pseudobin(int val) {
    int retval =0;
    
    // bit[5] == +6;  bit[4] == +4;  bit[3] == +2;     
    // bit[2] == +2;  bit[1] == +1;  bit[0] == +1;     
    //   E.g.: 21 (dec) == 01 0101 (bin)     
    //            --> Weight of TX bank = +4 +2 +1 = +7. 
    //                Hence for a weight of +7, a PXX_DXEN[5:0]=21 is needed.
    
    // Case statement was easier to build for 16 values than
    // a mathematical equation that does the same thing.
    switch (val) 
    {
        case 0 : retval=0 ; break;   //000000
        case 1 : retval=1 ; break;   //000001
        case 2 : retval=3 ; break;   //000011
        case 3 : retval=5 ; break;   //000101
    
        case 4 : retval=7 ;  break;  //000111
        case 5 : retval=13 ; break;  //001101
        case 6 : retval=15 ; break;  //001111
        case 7 : retval=21 ; break;  //010101

        case 8 : retval=23 ; break;  //010111
        case 9 : retval=42 ; break;  //101010 
        case 10: retval=48 ; break;  //110000
        case 11: retval=50 ; break;  //110010

        case 12: retval=56 ; break;  //111000
        case 13: retval=58 ; break;  //111010
        case 14: retval=60 ; break;  //111100
        case 15: retval=62 ; break;  //111110

        case 16: retval=63 ; break;  //111111 
        default: retval=0 ;
    }
    
    return retval;
}


void setPX(int val, int *PXA_D1EN, int *PXA_D2EN,  int *PXB_D1EN, int *PXB_D2EN) {
    if(val>16) {
        *PXA_D2EN =63;   *PXA_D1EN =0;
        *PXB_D2EN = dec2pseudobin(val-16);   *PXB_D1EN =63-(*PXB_D2EN);
    } else {
        *PXA_D2EN = dec2pseudobin(val);      *PXA_D1EN =63-(*PXA_D2EN); 
        *PXB_D2EN = 0;   *PXB_D1EN =63; 
    }
}


/************************************************************************/
/* Function Description                                                 */
/* TXctrls_maxswing :  Sets the 13 6-bit words that control the         */
/*                     TX amplitude.                                    */ 
/*                     The encoding scheme is set to have maxswing on   */
/*                     the TX output.                                   */
/************************************************************************/
   
int TXctrls_maxswing(int PRE, int POST, int CAL, 
                          int *P1A_D1EN, int *P1A_D2EN,    int *P1B_D1EN, int *P1B_D2EN,          
                          int *P2A_D1EN, int *P2A_D2EN,    int *P2B_D1EN, int *P2B_D2EN, 
                          int *P1A_PTEN, int *P1B_PTEN, int *P2A_PTEN, int *P2B_PTEN,
                          int *P3A_D1EN ) 
{

    //Check if inputs are within valid ranges.
    if( !((PRE>=0)&&(PRE<=32) && (POST>=0)&&(POST<=32) && (CAL>=64)&&(CAL<=80)) ) {
        printf("*[ERROR]*: Invalid range. One of the arguments has an invalid value.\n\n");
        printf("PRE  valid range=[0:32] \nPOST valid range=[0:32] \nCAL  valid range=[64:80]\n  ");
        return -1; //-1 == found error. Aborting function.
    }   

    setPX(PRE,  P2A_D1EN, P2A_D2EN, P2B_D1EN, P2B_D2EN);
    setPX(POST, P1A_D1EN, P1A_D2EN, P1B_D1EN, P1B_D2EN);
    *P3A_D1EN = dec2pseudobin(CAL-64);

    /* max swing implies that PT=0 (i.e.: disable TX parrallel termination */
    *P1A_PTEN=0;  *P1B_PTEN=0;  *P2A_PTEN=0;  *P2B_PTEN=0;  

    return 0; //0==no error
}



int main(int argc, char *argv[])
{

    // Inputs
    int PRE=0, POST=0, CAL=0;
    // Outputs
    int TX_P1A_D1EN, TX_P1A_D2EN,    TX_P1B_D1EN, TX_P1B_D2EN;          
    int TX_P2A_D1EN, TX_P2A_D2EN,    TX_P2B_D1EN, TX_P2B_D2EN; 
    int TX_P1A_PTEN, TX_P1B_PTEN,    TX_P2A_PTEN, TX_P2B_PTEN;
    int TX_P3A_D1EN;

    char c;

    if( argc > 4 ) printf("\n*[WARNING]*: Only the first 3 arguments will be taken. Remainder are ignored.\n");
    if( argc < 4 ) { printf("\n*[ERROR]*: This command needs 4 argument.\n E.g.: <command_name> PRE POST CAL\n");  return 0;}

    if(argc>1) PRE = atoi(argv[1]);  
    if(argc>2) POST= atoi(argv[2]);
    if(argc>3) CAL = atoi(argv[3]);
    

    //TX encoding function of the TX control signals
    //(this function does all the heavy lifting)
    if(TXctrls_maxswing(PRE, POST, CAL, 
        &TX_P1A_D1EN, &TX_P1A_D2EN,    &TX_P1B_D1EN, &TX_P1B_D2EN,          
        &TX_P2A_D1EN, &TX_P2A_D2EN,    &TX_P2B_D1EN, &TX_P2B_D2EN, 
        &TX_P1A_PTEN, &TX_P1B_PTEN, &TX_P2A_PTEN, &TX_P2B_PTEN,
        &TX_P3A_D1EN)==0) {

        //print outputs
        printf("PRE=%d ; POST=%d ; CAL=%d ;\n\n", PRE, POST, CAL );

        printf("P1 Bank (POST=%d, unit:TX segments) \n", POST);
        printf("P1 Bank control signals :\n");
        printf("TX_P1A_D1EN[5:0]=%d ; TX_P1A_D2EN[5:0]=%d ;    TX_P1B_D1EN[5:0]=%d ; TX_P1B_D2EN[5:0]=%d ;\n", TX_P1A_D1EN, TX_P1A_D2EN, TX_P1B_D1EN, TX_P1B_D2EN );

        printf("\nP2 Bank (PRE=%d, unit:TX segments) \n", PRE);
        printf("P2 Bank control signals :\n");
        printf("TX_P2A_D1EN[5:0]=%d ; TX_P2A_D2EN[5:0]=%d ;    TX_P2B_D1EN[5:0]=%d ; TX_P2B_D2EN[5:0]=%d ;\n", TX_P2A_D1EN, TX_P2A_D2EN, TX_P2B_D1EN, TX_P2B_D2EN );

        printf("\nP3 Bank (CAL=%d, unit:TX segments) \n", CAL);
        printf("P3 Bank control signals :\n");
        printf("TX_P3A_D1EN[5:0]=%d ;\n", TX_P3A_D1EN);

        printf("\nParrallel termination control signals (disabled for full swing):\n");
        printf("TX_P1A_PTEN[5:0]=%d ; TX_P1B_PTEN[5:0]=%d ;    TX_P2A_PTEN[5:0]=%d ; TX_P2B_PTEN[5:0]=%d ;\n", TX_P1A_PTEN, TX_P1B_PTEN, TX_P2A_PTEN, TX_P2B_PTEN);
    }

    return 0;
}
