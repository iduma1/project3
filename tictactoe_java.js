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

var XMLHttp;//Browser-specific var
var cgiBusy = false;//Stores state of CGI (busy or not busy)
var playerName; //Stores the player's name
var o
var boardState="Z,Z,Z,Z,Z,Z,Z,Z,Z";//Stores the current state of the board

/*XMLHttp object definition based on the web browser*/
if(navigator.appName == "Microsoft Internet Explorer") {
    XMLHttp = new ActiveXObject("Microsoft.XMLHTTP");
} else {
    XMLHttp = new XMLHttpRequest();
}

window.onload = function getName() {//When the page loads, it requests a user's name
		playerName = prompt("Enter Your Name", "player");//On entering the website, a player enters their name	
		p1.innerText = playerName;//Displays the players name on the screen
		callCGI(10);//Sends a call out to the CGI program that a player has connected
}

/*Uses the callCGI function to update the board every 3 seconds
setInterval(function(){ 
	callCGI(10);//pos=10 means update the board
},3000);
*/

//Function that will send and receive messages from the cgi program. When pos=10, program requests board update
function callCGI(pos) {
	
	if (cgiBusy == true) {//If the CGI is busy, this function quits
	
		return;
		
	}else {//If CGI is not busy, 
	
		cgiBusy = true;//Change CGI state to busy
		
		if(pos==10) {//When pos is 10, we simply are updating the board. We don't want a player sending a move.
			XMLHttp.open("GET", "/cgi-bin/solorioc_tictactoe_ajax.cgi?"
						 +"&player=" + "update"
						 +"&pos=" + pos
						 ,true);
		}else {//When the user clicks a box on the board
		
		XMLHttp.open("GET", "/cgi-bin/solorioc_tictactoe_ajax.cgi?"
						 +"&player=" + playerName
						 +"&pos=" + pos
						 ,true);
		}
		
		XMLHttp.onreadystatechange = function() {//When the state changes, 
		
			if (XMLHttp.readyState === 4) {//On the 4th state, 
				boardState = XMLHttp.responseText;//The string that the tictactoe_ajax.cpp c-outs is stored
				updateBoard(boardState);//Use the displayBoard function to update the board on player's screen
			}
		}
	
	}
	
	XMLHttp.send(null);
	
	cgiBusy = false;//Reset the CGI state to not busy
					 
}

/*This function upates the images displayed on the board*/
function updateBoard(board) {
	/*Board will be a string like ""O,X,Z,Z,Z,Z,O,X,O" in the case that the game is not over
	 *Board will be a string like "O,X,Z,Z,Z,Z,O,X,O,WIN,JOHN" in the case that there is a winner
	 *Board will be a string like "O,X,Z,Z,Z,Z,O,X,O,TIE"in the case that there is a tie
	 */
	
	var gameState = board.split(",");//Turns the string into an array separated by the commas
	
	/*Displays the apropriate mark in each box as given by the boxSign array*/
	displayMark('UL',gameState[0]);//box 0
	displayMark('UM',gameState[1]);//box 1
	displayMark('UR',gameState[2]);//box 2
	displayMark('ML',gameState[3]);//box 3
	displayMark('MM',gameState[4]);//box 4
	displayMark('MR',gameState[5]);//box 5
	displayMark('LL',gameState[6]);//box 6
	displayMark('LM',gameState[7]);//box 7
	displayMark('LR',gameState[8]);//box 8
	
	turnDisplay(gameState);
	
	isGameOver(gameState);//Checks if the game met a terminal condition
}

/*This function displays the mark in each box based on the message received from CGI program
 *the function displays the appropriate image, depending on if the spot is occupied by an X or an O
 *if the sign is anything other than X or O, no image is displayed
*/

function displayMark(box,sign){//Checks the sign and determines which image to display
	if(sign=="X") {	
		document.getElementById(box).innerHTML='<img hspace="30px" src="https://s3.amazonaws.com/piktochartv2-dev/v2/uploads/8a5899ab-d13e-4cc7-8a12-36279b4e20c1/67fe65d4bb6da9c4303037822efc43a48e7cba44_original.png" height = 140px width = 100px/>';
	}else if (sign=="O")
		document.getElementById(box).innerHTML='<img hspace="10px" src="http://i45.tinypic.com/23l1eo.jpg" height = 140px width = 140px />';
	}
	
function isGameOver(boardState) {//Function checks if the boardState has a terminal condition, such a s a winner or a tie

	if (boardState.length > 11) {//This string
	
		document.getElementById("turnDisplay").style.visibility="hidden";//Hides the turn display div
		
		if(boardState[12]=="TIE") {//If the TIE string is in the array,
			gameStatus.innerText = "Tie Game!";//Pushes text into gameStatus div
			document.getElementById("gameOver").style.visibility="visible";//Reveals the div in the HTML
		}
	
		if(boardState[12]=="WIN") {//If the WIN string is in the array, 
			gameStatus.innerText = boardState[13]+ " won!";//Pushes text into gameStatus div
			document.getElementById("gameOver").style.visibility="visible";//Reveals the div in HTML
		}
	}else {return;}
}

function turnDisplay(boardState) {
	if (boardState[9] == playerName) {
		turnDisplay.innerText = "Your turn!"
	} else {
		turnDisplay.innerText = boardState[10]+"'s Turn"
	}
}

