;;;
;;; Battleship Version 1.1
;;;
;;; Mod log
;;;
;;; Who              |    Date    | Description
;;;------------------+------------+-------------------------------------------
;;; M.Giordano       | 07-Dec-99  | Added restart rule

;;;
;;; Randon Guessing, Guessed squares as individual facts
;;;

;;;
;;; Import WebCLIPS templates and functions
;;;
(defmodule MAIN
    (import WebCLIPSHTML deffunction ?ALL)
    (import WebCLIPSTemplates deftemplate ?ALL))

;;;
;;; Templates and Initial Facts
;;;
(deftemplate Ship (slot Name)
                  (slot Size    (type INTEGER))
                  (slot NumHits (type INTEGER))
                  (slot Limit   (type INTEGER))
                  (slot Alignment)
                  (slot Status)
                  (slot Player)
                  (slot Abbrev))
(deftemplate HitSquares (slot Player) (multislot XY))
(deftemplate CoordsTable (slot Axis) (multislot coord))
(deftemplate OccupiedSquare (slot XY) (slot ShipType) (slot Player)
                            (slot GridNum (type INTEGER))
                            (slot PaintStatus))
(deftemplate PotentiallyOccupiedSquare (slot XY) (slot Player)
                            (slot GridNum (type INTEGER)))
(deftemplate Grid (slot Player) (multislot XY))
(deftemplate AlreadyGuessed (slot Player) (slot XY))

;;;
;;; globals
;;;
(defglobal ?*CompGuess* = " ")
(defglobal ?*CompResult* = " ")
(defglobal ?*HumanGuess* = " ")
(defglobal ?*HumanResult* = " ")
(defglobal ?*XGuessHuman* = 0)
(defglobal ?*YGuessHuman* = 0)
(defglobal ?*XGuessComp* = 0)
(defglobal ?*YGuessComp* = 0)

(deffacts Coordinates
(CoordsTable (Axis X) (coord A B C D E F G H I J))
(CoordsTable (Axis Y) (coord 1 2 3 4 5 6 7 8 9 10))
)

;;;
;;; Functions
;;;

;;;
;;; Returns a random number from 1 to ?Upperlimit
;;;
(deffunction GetRandom (?UpperLimit)
   (+ 1 (mod (random) ?UpperLimit)))

;;;
;;; Update the display grid
;;;
(deffunction UpdateGrid (?Grid ?Offset ?rnGrid ?abv)
(if (eq ?abv X)
   then
      (bind ?SQ (sym-cat X ))
   else
      (bind ?SQ (sym-cat ?abv)))
(bind ?NewGrid (replace$ ?Grid ?Offset ?Offset ?SQ))
(modify ?rnGrid (XY ?NewGrid)))

;;;
;;; Calculate the offset into the display grid based on the symbol of the
;;; human's guess
;;;
(deffunction CalcGN (?XCoord ?YCoord ?Plyr)
(if (eq ?Plyr Human)
   then
      (return (+ (* (- ?*YGuessComp* 1) (length$ ?YCoord)) (member$ ?*XGuessComp* ?XCoord)))
   else
      (return (+ (* (- ?*YGuessHuman* 1) (length$ ?YCoord)) (member$ ?*XGuessHuman* ?XCoord)))))

;;;
;;; Rules
;;;

;;;
;;; Initial processing
;;;
;;; Seed the random number generator
;;; Form the Humans guess from Web Server Input
;;;
(defrule SetupTurn
?rnPhase <- (Phase SetupGuess)
?rnXGuess <- (XGuess ?GuessForX)
?rnYGuess <- (YGuess ?GuessForY)
=>
(bind ?ti (time))
(seed (integer ?ti))
(bind ?*XGuessHuman* ?GuessForX)
(bind ?*YGuessHuman* ?GuessForY)
(bind ?GuessedSquare (sym-cat ?GuessForX ?GuessForY))
(bind ?*HumanGuess* (str-cat "Your Guess was : " ?GuessedSquare (BreakTag)))
(retract ?rnPhase ?rnXGuess ?rnYGuess)
(assert (Guess ?GuessedSquare))
(assert (Phase LookForSquare))
(watch facts)
(watch rules)
(printout clpout "Starting Battle Ship -- version 1.1" crlf)
)

;;;
;;; Restart processing -- No guess just repaint the screen
;;;
(defrule RestartGame
?rnPhase <- (Phase SetupGuess)
?rnRestart <- (Restart True)
=>
(watch facts)
(watch rules)
(retract ?rnPhase ?rnRestart)
(assert (Phase PaintScreenHeader)))


;;;
;;; Square has been guessed by the human already
;;;
(defrule HumanGuessYes
?rnPhase <- (Phase LookForSquare)
?rnHumanGuess <- (Guess ?HG)
(AlreadyGuessed (Player Human) (XY ?HG))
=>
(retract ?rnHumanGuess ?rnPhase)
(bind ?*HumanGuess* (str-cat "Your Guess was : " ?HG (BreakTag)))
(bind ?*HumanResult* (str-cat "Already Guessed" (BreakTag) "Please try again"))
(assert (Phase PaintScreenHeader)))

;;;
;;; Square has NOT been guessed by the human already
;;;
(defrule HumanGuessNo
?rnPhase <- (Phase LookForSquare)
?rnHumanGuess <- (Guess ?HG)
=>
(retract ?rnPhase)
(assert (Phase LookForHit))
(assert (AlreadyGuessed (Player Human) (XY ?HG)))
(assert (CheckPlayer Computer)))

;;;
;;; The Guess hit a ship
;;;
(defrule HitShip
?rnPhase <- (Phase LookForHit)
?rnPlyr <- (CheckPlayer ?Plyr)
?rnGuess <- (Guess ?Square)
?rnHitSquare <- (HitSquares (Player ?Plyr) (XY $?HitSQ))
(OccupiedSquare (Player ?Plyr) (ShipType ?abv) (XY ?Square) (GridNum ?GN))
(CoordsTable (Axis X) (coord $?xcoord))
(CoordsTable (Axis Y) (coord $?ycoord))
?rnShip <- (Ship (Abbrev ?abv) (Name ?shpName) (Status ?stat) (NumHits ?num)
                 (Player ?Plyr))
?rnGrid <- (Grid (Player ?Plyr) (XY $?PlayingGrid))
=>
(bind ?NewHits (insert$ ?HitSQ 1 ?Square))
(modify ?rnShip (Status Hit) (NumHits (+ ?num 1)))
(modify ?rnHitSquare (XY ?NewHits))
(UpdateGrid ?PlayingGrid (CalcGN ?xcoord ?ycoord ?Plyr) ?rnGrid ?abv)
(retract ?rnPhase ?rnGuess)
(if (eq ?Plyr Computer)
   then
      (bind ?*HumanResult* (str-cat ?shpName " hit at : " ?Square (BreakTag)))
   else
      (bind ?*CompResult* (str-cat ?shpName " hit at : " ?Square (BreakTag)))
      (assert (Phase CheckSunk)))
(assert (Phase CheckSunk)))

;;;
;;; The guess missed a ship
;;;
(defrule MissShip
?rnPhase <- (Phase LookForHit)
?rnPlyr <- (CheckPlayer ?Plyr)
?rnGuess <- (Guess ?Square)
?rnGrid <- (Grid (Player ?Plyr) (XY $?PlayingGrid))
(not (OccupiedSquare (Player ?Plyr) (XY ?Square)))
(CoordsTable (Axis X) (coord $?xcoord))
(CoordsTable (Axis Y) (coord $?ycoord))
=>
(UpdateGrid ?PlayingGrid (CalcGN ?xcoord ?ycoord ?Plyr) ?rnGrid X)
(retract ?rnPlyr ?rnPhase ?rnGuess)
(if (eq ?Plyr Computer)
   then
      (bind ?*HumanResult* (str-cat "Missed!" (BreakTag)))
      (assert (Phase MakeGuess))
      (assert (CheckPlayer Human))
   else
      (bind ?*CompResult* (str-cat "Missed!" (BreakTag)))
      (assert (Phase PaintScreenHeader))))

;;;
;;; We hit a ship, and we sunk it!
;;;
(defrule CheckSunkYes
?rnPhase <- (Phase CheckSunk)
(CheckPlayer ?Plyr)
?rnShip <- (Ship (Size ?sze) (NumHits ?num) (Player ?Plyr) (Name ?shpName)
                 (Status ~Sunk))
(test (= ?sze ?num))
=>
(modify ?rnShip (Status Sunk))
(if (eq ?Plyr Computer)
   then
      (bind ?*HumanResult* (str-cat "You sunk my " ?shpName (BreakTag)))
   else
      (bind ?*CompResult* (str-cat "I sunk your " ?shpName (BreakTag))))
(retract ?rnPhase)
(assert (Phase CheckWin)))

;;;
;;; A ship was hit, but not sunk
;;;
(defrule CheckSunkNo
?rnPhase <- (Phase CheckSunk)
?rnPlyr <- (CheckPlayer ?Plyr)
?rnShip <- (Ship (Size ?sze) (NumHits ?num) (Player ?Plyr))
(test (not (= ?sze ?num)))
=>
(retract ?rnPhase ?rnPlyr)
(if (eq ?Plyr Computer)
   then
      (assert (Phase MakeGuess))
      (assert (CheckPlayer Human))
   else
      (assert (Phase PaintScreenHeader))))

;;;
;;; A ship just got sunk, and someone won
;;;
(defrule CheckWin
?rnPhase <- (Phase CheckWin)
(CheckPlayer ?Plyr)
(Ship (Abbrev C) (Player ?Plyr) (Status Sunk))
(Ship (Abbrev B) (Player ?Plyr) (Status Sunk))
(Ship (Abbrev S) (Player ?Plyr) (Status Sunk))
(Ship (Abbrev R) (Player ?Plyr) (Status Sunk))
=>
(if (eq ?Plyr Computer)
   then
      (printout t (h1 "You won!" align=center))
      (printout t (str-cat ?*HumanGuess* (BreakTag) ?*HumanResult*))
   else
      (printout t (h1 "I won!" align=center))
      (printout t (str-cat ?*CompGuess* (BreakTag) ?*CompResult*)))
(halt))

;;;
;;; A ship just got sunk, no win
;;;
(defrule NoWin
?rnPhase <- (Phase CheckWin)
?rnPlyr <- (CheckPlayer ?Plyr)
(not (and (Ship (Abbrev C) (Player ?Plyr) (Status Sunk))
          (Ship (Abbrev B) (Player ?Plyr) (Status Sunk))
          (Ship (Abbrev S) (Player ?Plyr) (Status Sunk))
          (Ship (Abbrev R) (Player ?Plyr) (Status Sunk))))
=>
(retract ?rnPhase ?rnPlyr)
(if (eq ?Plyr Computer)
   then
      (assert (Phase MakeGuess))
      (assert (CheckPlayer Human))
   else
      (assert (Phase PaintScreenHeader))))
;;;
;;; Find a coordinate that has NOT been guessed yet
;;;
;;; First, make a guess
;;;
(defrule GenerateGuess
?rnPhase <- (Phase MakeGuess)
(CoordsTable (Axis X) (coord $?xcoord))
(CoordsTable (Axis Y) (coord $?ycoord))
=>
(bind ?*XGuessComp* (nth (GetRandom (length$ ?xcoord)) ?xcoord))
(bind ?*YGuessComp* (nth (GetRandom (length$ ?ycoord)) ?ycoord))
(bind ?SQ (sym-cat ?*XGuessComp* ?*YGuessComp*))
(assert (Guess ?SQ))
(bind ?*CompGuess* (str-cat "My Guess is : " ?SQ (BreakTag)))
(retract ?rnPhase)
(assert (Phase CheckGuess)))

;;;
;;; Next Check to see if the coordinate was already guessed. If coordinate
;;; already guessed, try again.
;;;
(defrule CheckGuess
?rnPhase <- (Phase CheckGuess)
(AlreadyGuessed (Player Computer) (XY ?SQ))
?rnGuess <- (Guess ?SQ)
=>
(retract ?rnPhase ?rnGuess)
(assert (Phase MakeGuess)))

;;;
;;; If the coordinate was unguessed, look to see if we hit a ship or not and
;;; assert fact that square has already been guessd.
;;;
(defrule LookForShip
?rnPhase <- (Phase CheckGuess)
?rnGuess <- (Guess ?SQ)
=>
(retract ?rnPhase)
(assert (Phase LookForHit))
(assert (AlreadyGuessed (Player Computer) (XY ?SQ))))

;;;=======================================================================;;;
;;;=======================================================================;;;
;;;=======================================================================;;;
;;;=======================================================================;;;

;;;
;;; Paint the Screen
;;;
(defrule PainttheScreen
?rnPhase <- (Phase PaintScreenHeader)
(CoordsTable (Axis X) (coord $?XCoord))
(CoordsTable (Axis Y) (coord $?YCoord))
=>
(HTMLHeader "WebCLIPS version of BattleShip 1.1")
(BodyTag bgcolor=#C0C0C0)
(TableStart cellpadding=10)
(printout t (TableRowStartTag) (TableDataStartTag align=center) "Your Forces"
                               (TableDataEndTag)
            (TableDataStartTag align=center) "Your Opponents Forces"
                               (TableDataEndTag)
            (TableRowEndTag) crlf)
(retract ?rnPhase)
(assert (Phase PaintHumanGrid)))

;;;
;;; Section that prints the Human Player's Grid
;;;
(defrule PaintHumanBoard
?rnPhase <- (Phase PaintHumanGrid)
(CoordsTable (Axis X) (coord $?XCoord))
(CoordsTable (Axis Y) (coord $?YCoord))
(Grid (Player Human) (XY $?PlayingGrid))
(HitSquares (Player Human) (XY $?HitSquares))
=>
(printout t (TableRowStartTag) (TableDataStartTag) (TableStartTag) crlf)
(printout t (TableRowStartTag) (TableDataStartTag) (PreStartTag) "   ")
(loop-for-count (?Indx 1 (length$ ?XCoord)) do
   (printout t (nth ?Indx ?XCoord) " "))
(printout t (TableDataEndTag) (TableRowEndTag) crlf)
(loop-for-count (?Row 1 (length ?YCoord)) do
   (printout t (TableRowStartTag) (TableDataStartTag) (PreStartTag))
   (format t "%2d " ?Row)
   (loop-for-count (?Col 1 (length$ ?YCoord)) do
      (bind ?CurrSQ (nth (+ (* (- ?Row 1) (length$ ?YCoord)) ?Col) ?PlayingGrid))
      (if (or (eq ?CurrSQ .) (eq ?CurrSQ X))
         then
            (printout t ?CurrSQ " ")
         else
            (bind ?CurrCoord (sym-cat (nth ?Col ?XCoord) (nth ?Row ?YCoord)))
            (if (eq (member$ ?CurrCoord ?HitSquares) FALSE)
               then
                  (bind ?Color color=white)
               else
                  (bind ?Color color=red)
            )
            (printout t (FontStartTag ?Color) ?CurrSQ (FontEndTag) " ")
      )
   )
   (printout t (TableDataEndTag) (TableRowEndTag) crlf)
)
(printout t (TableEndTag) (TableDataEndTag) crlf)
(retract ?rnPhase)
(assert (Phase PaintCompGrid)))

;;;
;;; Section that prints the Computer Player's Grid
;;;
(defrule PaintCompBoard
?rnPhase <- (Phase PaintCompGrid)
(CoordsTable (Axis X) (coord $?XCoord))
(CoordsTable (Axis Y) (coord $?YCoord))
(Grid (Player Computer) (XY $?PlayingGrid))
(HitSquares (Player Computer) (XY $?HitSquares))
=>
(printout t (TableDataStartTag) (TableStartTag) crlf)
(printout t (TableRowStartTag) (TableDataStartTag) (PreStartTag) "   ")
(loop-for-count (?Indx 1 (length$ ?XCoord)) do
   (printout t (nth ?Indx ?XCoord) " "))
(loop-for-count (?Row 1 (length ?YCoord)) do
   (printout t (TableRowStartTag) (TableDataStartTag) (PreStartTag))
   (format t "%2d " ?Row)
   (loop-for-count (?Col 1 (length$ ?YCoord)) do
      (bind ?CurrSQ (nth (+ (* (- ?Row 1) (length$ ?YCoord)) ?Col) ?PlayingGrid))
      (if (or (eq ?CurrSQ .) (eq ?CurrSQ X))
         then
            (printout t ?CurrSQ " ")
         else
            (printout t (FontStartTag color=red) ?CurrSQ (FontEndTag) " ")
      )
   )
   (printout t (TableDataEndTag) (TableRowEndTag) crlf)
)
(printout t crlf (TableEndTag) (TableDataEndTag) (TableRowStartTag))
(retract ?rnPhase)
(assert (Phase PaintStatus)))

;;;
;;; Section that prints the status area
;;;
(defrule PaintStatusArea
?rnPhase <- (Phase PaintStatus)
(CoordsTable (Axis X) (coord $?XCoord))
(CoordsTable (Axis Y) (coord $?YCoord))
=>
(printout t      (TableDataStartTag)
                 ?*CompGuess*
                 ?*CompResult*
                 ?*HumanGuess*
                 ?*HumanResult*
                 (TableDataEndTag))

;;;
;;; Section that prints the humans status area
;;;
(printout t      (TableDataStartTag) crlf
                 (FormStartTag Action=/WebCLIPS/cgi-bin/webclips.exe method=post)
                 (CenterStartTag) "Please Enter your next Guess : "
                 (ParagraphStartTag))
(DropDownStart fact)
(loop-for-count (?Indx 1 (length$ ?XCoord)) do
   (bind ?XG (nth ?Indx ?XCoord))
   (Option ?XG "Value=(XGuess" ?XG ")" )
)
(DropDownEnd)
(DropDownStart fact)
(loop-for-count (?Indx 1 (length$ ?YCoord)) do
   (bind ?YG (nth ?Indx ?YCoord))
   (Option ?YG "Value=(YGuess" ?YG ")" )
)
(DropDownEnd)
(SubmitButton Bombs Away!)
(printout t (TableDataEndTag) (TableRowEndTag))
(TableEnd)
(Anchor /WebCLIPS/batlship.htm "View Source for batlship.clp and WebCLIPS.ini")
(retract ?rnPhase)
(assert (Phase CleanUp)))

;;;
;;; Eliminate any unnecessary facts.
;;;
(defrule EliminateCoords
?rnPhase <- (Phase CleanUp)
?rnCoords <- (CoordsTable (Axis ?axs))
=>
(retract ?rnCoords))

;;;
;;; Finally set the Phase fact to Look for a hit
;;;
(defrule UpdateFinalPhase
?rnPhase <- (Phase CleanUp)
=>
(retract ?rnPhase)
(assert (Phase SetupGuess)))
