/*NOTES: 
 *The javascript receives a board 
 *
 *
 *
 */
//Variable that depends on the browser

//When the page loads, prompts user to enter name

var XMLHttp;//Browser-specific var
var isCgiBusy = false;//Stores state of CGI (busy or not busy)
var playerName; //Stores the player's name
var opponentName;//Name of the person the player is opposing
var boardState="Z,Z,Z,Z,Z,Z,Z,Z,Z";//Stores the current state of the board. Starts off empty. 

/*Default theme for the baord.*/
var xImg = "<img hspace='30px' src='https://s3.amazonaws.com/piktochartv2-dev/v2/uploads/8a5899ab-d13e-4cc7-8a12-36279b4e20c1/67fe65d4bb6da9c4303037822efc43a48e7cba44_original.png' height = 140px width = 100px/>";
var oImg = "<img hspace='10px' src='http://i45.tinypic.com/23l1eo.jpg' height = 140px width = 140px />";

/*XMLHttp object definition based on the web browser*/
if(navigator.appName == "Microsoft Internet Explorer") {
    XMLHttp = new ActiveXObject("Microsoft.XMLHTTP");
} else {
    XMLHttp = new XMLHttpRequest();
}

window.onload = function getName() {//When the page loads, it requests a user's name
		playerName = prompt("Enter Your Name", "player");//On entering the website, a player enters their name	
		p1.innerText = playerName;//Displays the players name on the screen
		//callCGI(10);//Sends a call out to the CGI program that a player has connected
}

function themeSelect(themeChoice) {
	if (themeChoice == "joeBarry") {
		xImg = "<img hspace='30px' src='https://s3.amazonaws.com/piktochartv2-dev/v2/uploads/8a5899ab-d13e-4cc7-8a12-36279b4e20c1/67fe65d4bb6da9c4303037822efc43a48e7cba44_original.png' height = 140px width = 100px/>";
		oImg = "<img hspace='10px' src='http://i45.tinypic.com/23l1eo.jpg' height = 140px width = 140px/>";
	} else if (themeChoice == "puppiesKittens") {
		xImg = "<img hspace='10px' vspace='10px' src='http://static.wixstatic.com/media/e51cae_14d203aa93df4438a60cbdc2edb40468.png_srz_449_423_85_22_0.50_1.20_0.00_png_srz' height = 130px width = 138px/>";
		oImg = "<img hspace='10px' src='http://www.downesvets.co.uk/wp-content/uploads/2015/07/kitten-package1.png' height = 140px width = 140px />";
	} else {
		xImg = "<img hspace='10px' src='https://s-media-cache-ak0.pinimg.com/originals/92/c8/5b/92c85b38a1453f8657c2d6cc137d17a6.png' height = 140px width = 140px />";
		oImg = "<img hspace='10px' src='https://ih0.redbubble.net/image.283066282.2098/sticker,375x360.u1.png' height = 140px width = 140px />";
	}

}



//Uses the callCGI function to update the board every 3 seconds
/*setInterval(function(){ 
	callCGI(reqUpdate);//
},3000);*/



/*Function that will send and receive messages from the cgi program. 
 *The functions expects two parameters, a command and an position (position is optional)
 *List of Commands: playerRegister: sends a message that a new player has signed on
 					reqUpdate: request update from server
 *					sendMove: send a player's move to the server
 *					restartServer: if a player disconnects, the server is restarted for another game
 *The CGI program will send back a string with the game's state, including the signs in each position
 *and contains information about if the game is a tie, or if there is a winner. 
 */
 
function callCGI(command, pos) {
	
	pos = pos || 10;//If pos is not passed, defaults to 10 - an undefined place on the board
	 
	if (isCgiBusy == true) {
		return; //Quits if CGI is busy (prevents deadlocks)
	}else {
	
		isCgiBusy = true;//Change CGI state to busy if not busy
		
		/*If the command was reqUpdate or restartServer, there is no position to send*/
		if(command != "sendMove") {
			XMLHttp.open("GET", "/cgi-bin/solorioc_tictactoe_ajax.cgi?"
						 +"&command=" + command
						 +"&player=" + player
						 ,true);
		}else {//If the command was sendMove, sends player's name and position clicked
		
		XMLHttp.open("GET", "/cgi-bin/solorioc_tictactoe_ajax.cgi?"
						 +"&command=" + command
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
	
	isCgiBusy = false;//Reset the CGI state to not busy
					 
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
	
	if(opponentName == null) {
		if (gameState[9] != playerName && gameState[9] != "onePlayer") {
				opponentName = gameState[9];
				p2.innerText = opponentName;
		}

	}
	turnDisplay(gameState);
	
	isGameOver(gameState);//Checks if the game met a terminal condition	
}

/*This function displays the mark in each box based on the message received from CGI program
 *the function displays the appropriate image, depending on if the spot is occupied by an X or an O
 *if the sign is anything other than X or O, no image is displayed
*/

function displayMark(box,sign){//Checks the sign and determines which image to display
	if(sign=="X") {	
		document.getElementById(box).innerHTML= xImg;
	}else if (sign=="O")
		document.getElementById(box).innerHTML= oImg;
	}

/*This function updates the web page to display if it is the players turn or not*/
function turnDisplay(boardState) {
	document.getElementById("turnDisplay").style.visibility="visible";
	
	if (boardState[9] == playerName) {
		turnStatus.innerText = "Your turn!";
	} else if (boardState[9] == "onePlayer") {
		turnStatus.innerText = "Waiting for a second player to connect...";
	} else {
		turnStatus.innerText = boardState[9] + "'s Turn";
	}
}

/*This function determines if the game has met a terminal condition. */
function isGameOver(boardState) {//Function checks if the boardState has a terminal condition, such as a winner or a tie

	if (boardState.length > 10) {//This string
	
		document.getElementById("turnDisplay").style.visibility="hidden";//Hides the turn display div
		
		if(boardState[10]=="TIE") {//If the TIE string is in the array,
			document.getElementById("gameOver").style.visibility="visible";//Reveals the div in the HTML
			gameStatus.innerText = "Tie Game!";//Pushes text into gameStatus div
		}
	
		if(boardState[10]=="WIN") {//If the WIN string is in the array, 
			document.getElementById("gameOver").style.visibility="visible";//Reveals the div in HTML
			gameStatus.innerText = boardState[11]+ " won!";//Pushes text into gameStatus div	
		}
	}else {return;}
}