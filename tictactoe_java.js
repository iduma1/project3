/*NOTES: 
 *TEST
 *Each javascript is its own instance
 *Figure out a turn counter. Could use some sort of global var
 *CGI program will have to tell each instance of page who is connected
 *Tells you whose turn it is on screen?
 *
 *
 *
 */
//Variable that depends on the browser

//When the page loads, prompts user to enter name

var XMLHttp;
var cgiBusy = false;//Stores state of CGI (busy or not busy)
var pname; //Stores the player's name

if(navigator.appName == "Microsoft Internet Explorer") {
    XMLHttp = new ActiveXObject("Microsoft.XMLHTTP");
} else {
    XMLHttp = new XMLHttpRequest();
}

//Function that will send and receive messages from the ajax program
function callCGI(pos) {
	
	if (cgiBusy === true) {//If the CGI is busy, this function quits
	
		return;
		
	}else {//If CGI is not busy, 
	
		cgiBusy = true;//Change CGI state to busy
		
		XMLHttp.open("GET", "/cgi-bin/solorioc_tictactoe_ajax.cgi?"
						 +"&player=" + pname
						 +"&pos=" + pos
						 ,true);
			 
		XMLHttp.onreadystatechange = function() {//Reads in the board state
		
			if (XMLHttp.readyState == 4) {			
				displayBoard(XMLHttp.responseText);
			}
		}
	
	}
	
	XMLHttp.send(null);
	
	cgiBusy = false;//Reset the CGI state to not busy
					 
}

/*This function displays the mark in each box based on the message received from CGI program
  the function displays the appropriate image, depending on if the spot is occupied by an X or an O
*/

function displayMark(box,sign){
	if(sign=="X") {	
		document.getElementById(box).innerHTML='<img hspace="30px" src="https://s3.amazonaws.com/piktochartv2-dev/v2/uploads/8a5899ab-d13e-4cc7-8a12-36279b4e20c1/67fe65d4bb6da9c4303037822efc43a48e7cba44_original.png" height = 140px width = 100px/>';
	}else if (sign=="O")
		document.getElementById(box).innerHTML='<img hspace="10px" src="http://i45.tinypic.com/23l1eo.jpg" height = 140px width = 140px />';
	}

/*This function is what updates the board*/
function displayBoard(board) {
	/*Board will be a string like ""O,X,Z,Z,Z,Z,O,X,O" in the case that the game is not over
	 *Board will be a string like "O,X,Z,Z,Z,Z,O,X,O,WIN,JOHN" in the case that there is a winner
	 *Board will be a string like "O,X,Z,Z,Z,Z,O,X,O,TIE"in the case that there is a tie
	 */
	
	var splits = board.split(",");//Turns the string into an array separated by the commas
	
	/*Updates each box in the board*/
	displayMark('UL',splits[0]);
	displayMark('UM',splits[1]);
	displayMark('UR',splits[2]);
	displayMark('ML',splits[3]);
	displayMark('MM',splits[4]);
	displayMark('MR',splits[5]);
	displayMark('LL',splits[6]);
	displayMark('LM',splits[7]);
	displayMark('LR',splits[8]);
	
	/*Checks the condition of the game*/
	
	if (splits.length >= 9) {
		if(splits[9]=="TIE") {//If the TIE string is in the array,
			alert("Tie Game!");//Alerts user
			location.reload();//Refreshes page to start new game
		}
		
		if(splits[9]=="WIN") {//If the WIN string is in the array, 
			alert(splits[10]+" won!");//Tells each person who won
			location.reload();
		}
	}
	
}

window.onload = function getName() {
		pname = prompt("Enter Your Name", "player");//On entering the website, a player enters their name	
		p1.innerText = pname;//Displays the players name on the screen
		callCGI(0);//Sends a call out to the CGI program that a player has connected
}
