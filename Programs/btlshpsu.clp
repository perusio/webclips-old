;;;
;;; Battleship Set up
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
                  (slot Size (type INTEGER))
                  (slot Limit (type INTEGER))
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
(deftemplate PlayersShips (slot ShipType) (multislot Coords))
(deftemplate Grid (slot Player) (multislot XY))

(deffacts ShipState
(Ship (Name Carrier)    (Size 5) (Limit 6) (Alignment X) (Status UnPlaced)
      (Player Human) (Abbrev C))
(Ship (Name Battleship) (Size 4) (Limit 7) (Alignment X) (Status UnPlaced)
      (Player Human) (Abbrev B))
(Ship (Name Submarine)  (Size 3) (Limit 8) (Alignment X) (Status UnPlaced)
      (Player Human) (Abbrev S))
(Ship (Name Cruiser)    (Size 2) (Limit 9) (Alignment X) (Status UnPlaced)
      (Player Human) (Abbrev R))

(Ship (Name Carrier)    (Size 5) (Limit 6) (Alignment X) (Status UnPlaced)
      (Player Computer) (Abbrev C))
(Ship (Name Battleship) (Size 4) (Limit 7) (Alignment X) (Status UnPlaced)
      (Player Computer) (Abbrev B))
(Ship (Name Submarine)  (Size 3) (Limit 8) (Alignment X) (Status UnPlaced)
      (Player Computer) (Abbrev S))
(Ship (Name Cruiser)    (Size 2) (Limit 9) (Alignment X) (Status UnPlaced)
      (Player Computer) (Abbrev R))
)

(deffacts Coordinates
(CoordsTable (Axis X) (coord A B C D E F G H I J))
(CoordsTable (Axis Y) (coord 1 2 3 4 5 6 7 8 9 10))
(Grid (Player Human) (XY . . . . . . . . . .
                         . . . . . . . . . .
                         . . . . . . . . . .
                         . . . . . . . . . .
                         . . . . . . . . . .
                         . . . . . . . . . .
                         . . . . . . . . . .
                         . . . . . . . . . .
                         . . . . . . . . . .
                         . . . . . . . . . .))

(Grid (Player Computer) (XY . . . . . . . . . .
                            . . . . . . . . . .
                            . . . . . . . . . .
                            . . . . . . . . . .
                            . . . . . . . . . .
                            . . . . . . . . . .
                            . . . . . . . . . .
                            . . . . . . . . . .
                            . . . . . . . . . .
                            . . . . . . . . . .))
(HitSquares (Player Human))
(HitSquares (Player Computer))
)

;;;
;;; Returns a random number from 1 to ?Upperlimit
;;;
(deffunction GetRandom (?UpperLimit)
   (+ 1 (mod (random) ?UpperLimit)))

;;;
;;; Rules
;;;
(defrule InitSetup
=>
(bind ?ti (time))
;;;(printout t "time : " ?ti)
(seed (integer ?ti))
(assert (Phase DetermineAligment)))

;;;
;;; Sets the Alignment for a ship to Horizontal or Vertical
;;;
(defrule SetAlign
(Phase DetermineAligment)
?rn <- (Ship (Name ?ShipType) (Alignment X))
=>
(bind ?Align (GetRandom 2))
(if (= ?Align 1)
   then
      (modify ?rn (Alignment H))
   else
      (modify ?rn (Alignment V))))

;;;
;;; Done setting Alignment, now set the Coordinates
;;;
(defrule SetCoordsPhase
?rnPhase <- (Phase DetermineAligment)
=>
(retract ?rnPhase)
(assert (Phase DetermineCoords)))

;;;
;;; Set the coordinates if the ship is Horizontal
;;;
(defrule SetCoordsHorz
(Phase DetermineCoords)
(CoordsTable (Axis X) (coord $?XCoord))
(CoordsTable (Axis Y) (coord $?YCoord))
?rnShip <- (Ship (Name ?ShipType) (Size ?len) (Alignment H) (Limit ?lim)
                 (Status UnPlaced) (Player ?plyr))
?rnPhase <- (Phase DetermineCoords)
=>
(bind ?Row    (GetRandom ?lim))
(bind ?XStart (GetRandom ?lim))
(bind ?Y      (nth ?Row ?YCoord))
;;;(printout t "For player : " ?plyr " ship type : " ?ShipType " Alignment  : Horizontal the coordinates are : " crlf)
;;;(printout t "Row : " ?Y " starting col : " ?XStart crlf)
(loop-for-count (?Indx 0 (- ?len 1)) do
   (bind ?Col (+ ?Indx ?XStart))
   (bind ?PotXY (sym-cat (nth ?Col ?XCoord) ?Y))
;;;   (printout t ?PotXY " -- Col: " ?Col " Row: " ?Row)
   (assert (PotentiallyOccupiedSquare (XY ?PotXY) (Player ?plyr) (GridNum (+ (* (- ?Row 1) (length ?YCoord)) ?Col))))
)
(retract ?rnPhase)
(assert (Phase CheckCoords))
(modify ?rnShip (Status Pending)))
;;;(printout t crlf))

;;;
;;; Set the coordinates if the ship is Vertical
;;;
(defrule SetCoordsVert
(Phase DetermineCoords)
(CoordsTable (Axis X) (coord $?XCoord))
(CoordsTable (Axis Y) (coord $?YCoord))
?rnShip <- (Ship (Name ?ShipType) (Size ?len) (Alignment V) (Limit ?lim)
                 (Status UnPlaced) (Player ?plyr))
?rnPhase <- (Phase DetermineCoords)
=>
(bind ?Col    (GetRandom ?lim))
(bind ?YStart (GetRandom ?lim))
(bind ?X      (nth ?Col ?XCoord))
;;;(printout t "For player : " ?plyr " ship type : " ?ShipType " Alignment  : Vertical the coordinates are : " crlf)
;;;(printout t "Column : " ?X " starting row : " ?YStart crlf)
(loop-for-count (?Indx 0 (- ?len 1)) do
   (bind ?Row (+ ?Indx ?YStart))
   (bind ?PotXY (sym-cat ?X (nth ?Row ?YCoord)))
;;;   (printout t ?PotXY " -- Col: " ?Col " Row: " ?Row)
   (assert (PotentiallyOccupiedSquare (XY ?PotXY) (Player ?plyr) (GridNum (+ (* (- ?Row 1) (length ?YCoord)) ?Col))))
)
(retract ?rnPhase)
(assert (Phase CheckCoords))
(modify ?rnShip (Status Pending)))
;;;(printout t crlf))

;;;
;;; Coordinate is occupied, start process to eliminate the candidate XYs
;;;
(defrule CoordOccupied
(OccupiedSquare (XY ?CoordXY) (ShipType ?IncumbentST) (Player ?plyr))
(PotentiallyOccupiedSquare (XY ?CoordXY) (Player ?plyr))
?rnShip <- (Ship (Name ?ST) (Status Pending))
?rnPhase <- (Phase CheckCoords)
=>
;;;(printout t "Coordinate : " ?CoordXY " for player " ?plyr " was attempted to be occupied by " ?ST
;;;            " but it is occupied by the " ?IncumbentST crlf)
(retract ?rnPhase)
(modify ?rnShip (Status InError))
(assert (Phase DrainPotSquares)))

;;;
;;; A preoccupied coordinate was found, eliminate all coords for this ship 
;;; and set is status back to UnPlaced
;;;
(defrule RemovePotCoords
?rnXY <- (PotentiallyOccupiedSquare (XY ?CoordXY))
?rnShip <- (Ship (Name ?ST) (Status InError))
?rnPhase <- (Phase DrainPotSquares)
=>
;;;(printout t "Removing Coordinate : " ?CoordXY crlf)
(retract ?rnXY))

;;;
;;; All coords on ship replaced, set ship status back to UnPlaced
;;;
(defrule ResetShip
?rnShip <- (Ship (Name ?ST) (Status InError))
?rnPhase <- (Phase DrainPotSquares)
=>
(retract ?rnPhase)
(modify ?rnShip (Status UnPlaced))
(assert (Phase DetermineCoords)))

;;;
;;; If all Potentially occupied squares are clear, make them occupied
;;;
(defrule SetCoordOccupied
?rnXY <- (PotentiallyOccupiedSquare (XY ?CoordXY) (Player ?plyr) (GridNum ?sq))
?rnShip <- (Ship (Status Pending) (Abbrev ?abv))
?rnPhase <- (Phase CheckCoords)
=>
;;;(printout t "Occupying Coordinate : " ?CoordXY " with the " ?ST crlf)
(assert (OccupiedSquare (XY ?CoordXY) (ShipType ?abv) (Player ?plyr) (GridNum ?sq)
        (PaintStatus UnPainted)))
(retract ?rnXY))

;;;
;;; If all coords for the current ship have been occupied, do the next ship
;;;
(defrule NextShip
?rnShip <- (Ship (Name ?ST) (Status Pending))
?rnPhase <- (Phase CheckCoords)
=>
(modify ?rnShip (Status Placed))
(retract ?rnPhase)
(assert (Phase DetermineCoords)))

;;;
;;; If coords are calc'd, then update the human grid.
;;;
(defrule SetGridUpdate
?rnPhase <- (Phase DetermineCoords)
=>
(retract ?rnPhase)
(assert (Phase GridUpdate)))

;;;
;;; Update the human's grid. This is done by replacing the '.' at a given
;;; with <font color=white>"Ship Abbrev"</font>
;;;
(defrule UpdateGrid
?rnPhase <- (Phase GridUpdate)
?rnSquare <- (OccupiedSquare (ShipType ?abv) (Player Human) (GridNum ?offset)
             (PaintStatus UnPainted))
?rnGrid <- (Grid (Player Human) (XY $?PlayingGrid))
=>
;;;(printout t "Offset : " ?offset " abbrev : " ?abv " Ship Print : " ?PrntShip crlf)
(bind ?NewGrid (replace$ ?PlayingGrid ?offset ?offset ?abv))
;;;(printout t "Grid : " ?NewGrid crlf)
(modify ?rnSquare (PaintStatus Painted))
(modify ?rnGrid (XY ?NewGrid)))

;;;
;;; If all grid updates are done, then Paint the screen. WebCLIPS will paint the next
;;; screen from a NextScreen entry in WebCLIPS.INI
;;;
(defrule PaintScreen
?rnPhase <- (Phase GridUpdate)
=>
(retract ?rnPhase)
(assert (Phase PaintScreen)))

;;;
;;; Paint the Initial screen
;;;
(defrule PainttheScreen
?rnPaint <- (Phase PaintScreen)
(CoordsTable (Axis X) (coord $?XCoord))
(CoordsTable (Axis Y) (coord $?YCoord))
(Grid (Player Human) (XY $?PlayingGrid))
=>
(retract ?rnPaint)
(HTMLHeader "Battleship Setup Screen")
(BodyTag bgcolor=#C0C0C0)
(TableStart cellpadding=10)
(printout t (TableRowStartTag) (TableDataStartTag align=center) "Your Forces"
                               (TableDataEndTag)
            (TableDataStartTag align=center) "Your Opponents Forces"
                               (TableDataEndTag)
            (TableRowEndTag) crlf)
;;;
;;; Section that prints the Human Player's Grid
;;;
(printout t (TableRowStartTag) (TableDataStartTag) (TableStartTag) crlf)
(printout t (TableRowStartTag) (TableDataStartTag) (PreStartTag) "   ")
(loop-for-count (?Indx 1 (length$ ?XCoord)) do
   (printout t (nth ?Indx ?XCoord) " "))
(printout t (TableDataEndTag) (TableRowEndTag) crlf)
(loop-for-count (?Row 1 (length ?YCoord)) do
   (printout t (TableRowStartTag) (TableDataStartTag) (PreStartTag))
   (format t "%2d " ?Row)
   (loop-for-count (?Col 1 (length$ ?YCoord)) do
      (bind ?SQ (nth (+ (* (- ?Row 1) (length ?YCoord)) ?Col) ?PlayingGrid))
      (if (eq ?SQ .)
         then
            (printout t ?SQ " ")
         else
            (printout t (FontStartTag color=white) ?SQ (FontEndTag) " "))
   )
   (printout t (TableDataEndTag) (TableRowEndTag) crlf)
)
(printout t (TableEndTag) (TableDataEndTag) crlf)
;;;
;;; Section that prints the Computer Player's Grid
;;;
(printout t (TableDataStartTag) (TableStartTag) crlf)
(printout t (TableRowStartTag) (TableDataStartTag) (PreStartTag) "   ")
(loop-for-count (?Indx 1 (length$ ?XCoord)) do
   (printout t (nth ?Indx ?XCoord) " "))
(loop-for-count (?Row 1 (length ?YCoord)) do
   (printout t (TableRowStartTag) (TableDataStartTag) (PreStartTag))
   (format t "%2d " ?Row)
   (loop-for-count (?Col 1 (length$ ?YCoord)) do
      (printout t ". ")
   )
   (printout t (TableDataEndTag) (TableRowEndTag) crlf)
)
(printout t crlf (TableEndTag) (TableDataEndTag) (TableRowStartTag)
                 (TableDataStartTag) " " (TableDataEndTag) (TableDataStartTag) crlf
                 (FormStartTag Action=/cgi-bin/WebCLIPS.exe method=post)
                 (CenterStartTag) "Please Enter your next Guess : "
                 (ParagraphStartTag))
(DropDownStart fact)
(loop-for-count (?Indx 1 (length$ ?XCoord)) do
   (bind ?XG (nth ?Indx ?XCoord))
   (Option ?XG "Value=(XGuess" ?XG ")")
)
(DropDownEnd)
(DropDownStart fact)
(loop-for-count (?Indx 1 (length$ ?YCoord)) do
   (bind ?YG (nth ?Indx ?YCoord))
   (Option ?YG "Value=(YGuess" ?YG ")")
)
(DropDownEnd)
(SubmitButton Bombs Away!)
(printout t (TableDataEndTag) (TableRowEndTag))
(TableEnd)
(assert (Phase CleanUp)))

;;;
;;; Eliminate any unnecessary facts, set all squares to Intact status
;;;

;;;
;;; Set all occupied sqaures for Human player to UnPainted
;;;
(defrule SquareReset
?rnPhase <- (Phase CleanUp)
?rnSquare <- (OccupiedSquare (PaintStatus Painted))
=>
(modify ?rnSquare (PaintStatus Intact)))

;;;
;;; Eliminate Coordinate Axes
;;;
(defrule EliminateCoords
?rnPhase <- (Phase CleanUp)
?rnCoords <- (CoordsTable (Axis ?ax))
=>
(retract ?rnCoords))

;;;
;;; Eliminate Grid
;;;
(defrule EliminateGrid
?rnPhase <- (Phase Cleanup)
?rnGrid <- (Grid (Player ?plyr))
=>
(retract ?rnGrid))

;;;
;;; Eliminate ScreenName fact
;;;
(defrule EliminateScreenName
?rnPhase <- (Phase CleanUp)
?rnScreen <- (ScreenName (ScrnName $?SN))
=>
(retract ?rnScreen))

;;;
;;; Finally set the Phase fact to Look for a hit
;;;
(defrule UpdateFinalPhase
?rnPhase <- (Phase CleanUp)
=>
(retract ?rnPhase)
(assert (Phase SetupGuess)))
