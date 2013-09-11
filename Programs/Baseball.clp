;;;======================================================
;;;
;;; Name       : Michael Giordano
;;; Instructor : Dr.Kurfess
;;; Date       : 11/18/96
;;; Assignment : Home Work #2 -- Baseball
;;;
;;;======================================================

;;;======================================================
;;;   Baseball
;;;     
;;;   Matches the person with a position given relationships.
;;;   Also provides an interactive query to show positions or
;;;   relationships.
;;;
;;;   Facts obtainded from [Giarratano, Riley 1994], p. 439
;;;    
;;;
;;;  CLIPS Version 6.0
;;;
;;;  To execute, merely load, reset and run.
;;;======================================================
(defmodule MAIN 
  (import WebCLIPSHTML deffunction ?ALL)
  (import WebCLIPSTemplates deftemplate ?ALL))

;;;*************
;;;* TEMPLATES *
;;;*************
(deftemplate Position (slot pos-name))
(deftemplate Player (slot person-name))
(deftemplate Not-Playing (slot player-name) (slot position))
(deftemplate Playing (slot player-name) (slot position))
(deftemplate Not-found-pos (slot pos))
(deftemplate Not-found-person (slot person-name))
;;;(deftemplate ScreenName (slot Name))
;;;*****************
;;;* INITIAL STATE *
;;;*****************
;;;*********
;;;* RULES *
;;;*********
;
;;-Please note that this CLIPS program does NOT need to print an HTML header as it is
;;-already present in the document hwbb.htm
;
(defrule InitSetup "Setup HTML for output"
(declare (salience 99))
=>
(printout t (CenterStartTag))
(TableStart border=3 cellpadding=10)
(printout t (TableRowStartTag) (TableDataStartTag align=center) (BoldStartTag) "Position" (BoldEndTag) (TableDataEndTag)
                               (TableDataStartTag align=center) (BoldStartTag) "Person" (BoldEndTag) (TableDataEndTag)
            (TableRowEndTag)))

(defrule FinalCleanUp "Clean Up HTML for output"
(declare (salience -99))
=>
(printout t (TableEnd) (CenterEndTag)))


(defrule Check-LF "Only slot left for LF"
   (Not-found-pos (pos LF))
   (Not-found-person (person-name ?pn))
   (Not-Playing (player-name ?pn) (position P))
   (Not-Playing (player-name ?pn) (position C))
   (Not-Playing (player-name ?pn) (position 1st))
   (Not-Playing (player-name ?pn) (position 2nd))
   (Not-Playing (player-name ?pn) (position 3rd))
   (Not-Playing (player-name ?pn) (position SS))
   (Not-Playing (player-name ?pn) (position CF))
   (Not-Playing (player-name ?pn) (position RF))
   ?rnflf <- (Not-found-pos (pos LF))
   ?rnfpn <- (Not-found-person (person-name ?pn))
=>
(TableRow "Left Field" ?pn)
(retract ?rnflf ?rnfpn)
(assert (Playing (player-name ?pn) (position LF))))

(defrule Check-CF "Only slot left for CF"
   (Not-found-pos (pos CF))
   (Not-found-person (person-name ?pn))
   (Not-Playing (player-name ?pn) (position P))
   (Not-Playing (player-name ?pn) (position C))
   (Not-Playing (player-name ?pn) (position 1st))
   (Not-Playing (player-name ?pn) (position 2nd))
   (Not-Playing (player-name ?pn) (position 3rd))
   (Not-Playing (player-name ?pn) (position SS))
   (Not-Playing (player-name ?pn) (position LF))
   (Not-Playing (player-name ?pn) (position RF))
   ?rnfcf <- (Not-found-pos (pos CF))
   ?rnfpn <- (Not-found-person (person-name ?pn))
=>
(TableRow "Center Field" ?pn)
(retract ?rnfcf ?rnfpn)
(assert (Playing (player-name ?pn) (position CF))))

(defrule Check-RF "Only slot left for RF"
   (Not-found-pos (pos RF))
   (Not-found-person (person-name ?pn))
   (Not-Playing (player-name ?pn) (position P))
   (Not-Playing (player-name ?pn) (position C))
   (Not-Playing (player-name ?pn) (position 1st))
   (Not-Playing (player-name ?pn) (position 2nd))
   (Not-Playing (player-name ?pn) (position 3rd))
   (Not-Playing (player-name ?pn) (position SS))
   (Not-Playing (player-name ?pn) (position CF))
   (Not-Playing (player-name ?pn) (position LF))
   ?rnfrf <- (Not-found-pos (pos RF))
   ?rnfpn <- (Not-found-person (person-name ?pn))
=>
(TableRow "Right Field" ?pn)
(retract ?rnfrf ?rnfpn)
(assert (Playing (player-name ?pn) (position RF))))

(defrule Check-SS "Only slot left for SS"
   (Not-found-pos (pos SS))
   (Not-found-person (person-name ?pn))
   (Not-Playing (player-name ?pn) (position P))
   (Not-Playing (player-name ?pn) (position C))
   (Not-Playing (player-name ?pn) (position 1st))
   (Not-Playing (player-name ?pn) (position 2nd))
   (Not-Playing (player-name ?pn) (position 3rd))
   (Not-Playing (player-name ?pn) (position RF))
   (Not-Playing (player-name ?pn) (position CF))
   (Not-Playing (player-name ?pn) (position LF))
   ?rnfss <- (Not-found-pos (pos SS))
   ?rnfpn <- (Not-found-person (person-name ?pn))
=>
(TableRow "Short Stop" ?pn)
(retract ?rnfss ?rnfpn)
(assert (Playing (player-name ?pn) (position SS))))

(defrule Check-3rd "Only slot left for 3rd"
   (Not-found-pos (pos 3rd))
   (Not-found-person (person-name ?pn))
   (Not-Playing (player-name ?pn) (position P))
   (Not-Playing (player-name ?pn) (position C))
   (Not-Playing (player-name ?pn) (position 1st))
   (Not-Playing (player-name ?pn) (position 2nd))
   (Not-Playing (player-name ?pn) (position SS))
   (Not-Playing (player-name ?pn) (position RF))
   (Not-Playing (player-name ?pn) (position CF))
   (Not-Playing (player-name ?pn) (position LF))
   ?rnf3rd <- (Not-found-pos (pos 3rd))
   ?rnfpn <- (Not-found-person (person-name ?pn))
=>
(TableRow "3rd Base" ?pn)
(retract ?rnf3rd ?rnfpn)
(assert (Playing (player-name ?pn) (position 3rd))))

(defrule Check-2nd "Only slot left for 2nd"
   (Not-found-pos (pos 2nd))
   (Not-found-person (person-name ?pn))
   (Not-Playing (player-name ?pn) (position P))
   (Not-Playing (player-name ?pn) (position C))
   (Not-Playing (player-name ?pn) (position 1st))
   (Not-Playing (player-name ?pn) (position 3rd))
   (Not-Playing (player-name ?pn) (position SS))
   (Not-Playing (player-name ?pn) (position RF))
   (Not-Playing (player-name ?pn) (position CF))
   (Not-Playing (player-name ?pn) (position LF))
   ?rnf2nd <- (Not-found-pos (pos 2nd))
   ?rnfpn <- (Not-found-person (person-name ?pn))
=>
(TableRow "2nd Base" ?pn)
(retract ?rnf2nd ?rnfpn)
(assert (Playing (player-name ?pn) (position 2nd))))

(defrule Check-1st "Only slot left for 1st"
   (Not-found-pos (pos 1st))
   (Not-found-person (person-name ?pn))
   (Not-Playing (player-name ?pn) (position P))
   (Not-Playing (player-name ?pn) (position C))
   (Not-Playing (player-name ?pn) (position 2nd))
   (Not-Playing (player-name ?pn) (position 3rd))
   (Not-Playing (player-name ?pn) (position SS))
   (Not-Playing (player-name ?pn) (position CF))
   (Not-Playing (player-name ?pn) (position RF))
   (Not-Playing (player-name ?pn) (position LF))
   ?rnf1st <- (Not-found-pos (pos 1st))
   ?rnfpn <- (Not-found-person (person-name ?pn))
=>
(TableRow "1st Base" ?pn)
(retract ?rnf1st ?rnfpn)
(assert (Playing (player-name ?pn) (position 1st))))

(defrule Check-C "Only slot left for C"
   (Not-found-pos (pos C))
   (Not-found-person (person-name ?pn))
   (Player (person-name ?pn))
   (Not-Playing (player-name ?pn) (position P))
   (Not-Playing (player-name ?pn) (position 1st))
   (Not-Playing (player-name ?pn) (position 2nd))
   (Not-Playing (player-name ?pn) (position 3rd))
   (Not-Playing (player-name ?pn) (position SS))
   (Not-Playing (player-name ?pn) (position CF))
   (Not-Playing (player-name ?pn) (position RF))
   (Not-Playing (player-name ?pn) (position LF))
   ?rnfc  <- (Not-found-pos (pos C))
   ?rnfpn <- (Not-found-person (person-name ?pn))
=>
(TableRow "Catcher" ?pn)
(retract ?rnfc ?rnfpn)
(assert (Playing (player-name ?pn) (position C))))

(defrule Check-P "Only slot left for P"
   (Not-found-pos (pos P))
   (Not-found-person (person-name ?pn))
   (Not-Playing (player-name ?pn) (position C))
   (Not-Playing (player-name ?pn) (position 1st))
   (Not-Playing (player-name ?pn) (position 2nd))
   (Not-Playing (player-name ?pn) (position 3rd))
   (Not-Playing (player-name ?pn) (position SS))
   (Not-Playing (player-name ?pn) (position CF))
   (Not-Playing (player-name ?pn) (position RF))
   (Not-Playing (player-name ?pn) (position LF))
   ?rnfp  <- (Not-found-pos (pos P))
   ?rnfpn <- (Not-found-person (person-name ?pn))
=>
(TableRow "Pitcher" ?pn)
(retract ?rnfp ?rnfpn)
(assert (Playing (player-name ?pn) (position P))))

(defrule Check-Allen "Only slot (for a position) left is Allen"
   (Not-found-pos (pos ?ps))
   (Not-found-person (person-name Allen))
   (Not-Playing (player-name Andy)  (position ?ps))
   (Not-Playing (player-name Bill)  (position ?ps))
   (Not-Playing (player-name Ed)    (position ?ps))
   (Not-Playing (player-name Harry) (position ?ps))
   (Not-Playing (player-name Jerry) (position ?ps))
   (Not-Playing (player-name Mike)  (position ?ps))
   (Not-Playing (player-name Paul)  (position ?ps))
   (Not-Playing (player-name Sam)   (position ?ps))
   ?rnfp  <- (Not-found-person (person-name Allen))
   ?rnfps <- (Not-found-pos (pos ?ps))
=>
(TableRow ?ps "Allen")
(retract ?rnfp ?rnfps)
(assert (Playing (player-name Allen) (position ?ps))))

(defrule Check-Andy "Only slot (for a position) left is Andy"
   (Not-found-person (person-name Andy))
   (Not-found-pos (pos ?ps))
   (Not-Playing (player-name Allen) (position ?ps))
   (Not-Playing (player-name Bill)  (position ?ps))
   (Not-Playing (player-name Ed)    (position ?ps))
   (Not-Playing (player-name Harry) (position ?ps))
   (Not-Playing (player-name Jerry) (position ?ps))
   (Not-Playing (player-name Mike)  (position ?ps))
   (Not-Playing (player-name Paul)  (position ?ps))
   (Not-Playing (player-name Sam)   (position ?ps))
   ?rnfp  <- (Not-found-person (person-name Andy))
   ?rnfps <- (Not-found-pos (pos ?ps))
=>
(printout t  "Andy is playing " ?ps "<br>" crlf)
(TableRow ?ps "Andy")
(retract ?rnfp ?rnfps)
(assert (Playing (player-name Andy) (position ?ps))))

(defrule Check-Bill "Only slot (for a position) left is Bill"
   (Not-found-person (person-name Bill))
   (Not-found-pos (pos ?ps))
   (Not-Playing (player-name Allen) (position ?ps))
   (Not-Playing (player-name Andy)  (position ?ps))
   (Not-Playing (player-name Ed)    (position ?ps))
   (Not-Playing (player-name Harry) (position ?ps))
   (Not-Playing (player-name Jerry) (position ?ps))
   (Not-Playing (player-name Mike)  (position ?ps))
   (Not-Playing (player-name Paul)  (position ?ps))
   (Not-Playing (player-name Sam)   (position ?ps))
   ?rnfp  <- (Not-found-person (person-name Bill))
   ?rnfps <- (Not-found-pos (pos ?ps))
=>
(TableRow ?ps "Bill")
(retract ?rnfp ?rnfps)
(assert (Playing (player-name Bill) (position ?ps))))

(defrule Check-Ed "Only slot (for a position) left is Ed"
   (Not-found-person (person-name Ed))
   (Not-found-pos (pos ?ps))
   (Not-Playing (player-name Allen) (position ?ps))
   (Not-Playing (player-name Andy)  (position ?ps))
   (Not-Playing (player-name Bill)  (position ?ps))
   (Not-Playing (player-name Harry) (position ?ps))
   (Not-Playing (player-name Jerry) (position ?ps))
   (Not-Playing (player-name Mike)  (position ?ps))
   (Not-Playing (player-name Paul)  (position ?ps))
   (Not-Playing (player-name Sam)   (position ?ps))
   ?rnfp  <- (Not-found-person (person-name Ed))
   ?rnfps <- (Not-found-pos (pos ?ps))
=>
(TableRow ?ps "Ed")
(retract ?rnfp ?rnfps)
(assert (Playing (player-name Ed) (position ?ps))))

(defrule Check-Harry "Only slot (for a position) left is Harry"
   (Not-found-person (person-name Harry))
   (Not-found-pos (pos ?ps))
   (Not-Playing (player-name Allen) (position ?ps))
   (Not-Playing (player-name Andy)  (position ?ps))
   (Not-Playing (player-name Bill)  (position ?ps))
   (Not-Playing (player-name Ed)    (position ?ps))
   (Not-Playing (player-name Jerry) (position ?ps))
   (Not-Playing (player-name Mike)  (position ?ps))
   (Not-Playing (player-name Paul)  (position ?ps))
   (Not-Playing (player-name Sam)   (position ?ps))
   ?rnfp  <- (Not-found-person (person-name Harry))
   ?rnfps <- (Not-found-pos (pos ?ps))
=>
(TableRow ?ps "Harry")
(retract ?rnfp ?rnfps)
(assert (Playing (player-name Harry) (position ?ps))))

(defrule Check-Jerry "Only slot (for a position) left is Jerry"
   (Not-found-person (person-name Jerry))
   (Not-found-pos (pos ?ps))
   (Not-Playing (player-name Allen) (position ?ps))
   (Not-Playing (player-name Andy)  (position ?ps))
   (Not-Playing (player-name Bill)  (position ?ps))
   (Not-Playing (player-name Ed)    (position ?ps))
   (Not-Playing (player-name Harry) (position ?ps))
   (Not-Playing (player-name Mike)  (position ?ps))
   (Not-Playing (player-name Paul)  (position ?ps))
   (Not-Playing (player-name Sam)   (position ?ps))
   ?rnfp  <- (Not-found-person (person-name Jerry))
   ?rnfps <- (Not-found-pos (pos ?ps))
=>
(TableRow ?ps "Jerry")
(retract ?rnfp ?rnfps)
(assert (Playing (player-name Jerry) (position ?ps))))

(defrule Check-Mike "Only slot (for a position) left is Mike"
   (Not-found-person (person-name Mike))
   (Not-found-pos (pos ?ps))
   (Not-Playing (player-name Allen) (position ?ps))
   (Not-Playing (player-name Andy)  (position ?ps))
   (Not-Playing (player-name Bill)  (position ?ps))
   (Not-Playing (player-name Ed)    (position ?ps))
   (Not-Playing (player-name Harry) (position ?ps))
   (Not-Playing (player-name Jerry) (position ?ps))
   (Not-Playing (player-name Paul)  (position ?ps))
   (Not-Playing (player-name Sam)   (position ?ps))
   ?rnfp  <- (Not-found-person (person-name Mike))
   ?rnfps <- (Not-found-pos (pos ?ps))
=>
(TableRow ?ps "Mike")
(retract ?rnfp ?rnfps)
(assert (Playing (player-name Mike) (position ?ps))))

(defrule Check-Paul "Only slot (for a position) left is Paul"
   (Not-found-person (person-name Paul))
   (Not-found-pos (pos ?ps))
   (Not-Playing (player-name Allen) (position ?ps))
   (Not-Playing (player-name Andy)  (position ?ps))
   (Not-Playing (player-name Bill)  (position ?ps))
   (Not-Playing (player-name Ed)    (position ?ps))
   (Not-Playing (player-name Harry) (position ?ps))
   (Not-Playing (player-name Jerry) (position ?ps))
   (Not-Playing (player-name Mike)  (position ?ps))
   (Not-Playing (player-name Sam)   (position ?ps))
   ?rnfp  <- (Not-found-person (person-name Paul))
   ?rnfps <- (Not-found-pos (pos ?ps))
=>
(TableRow ?ps "Paul")
(retract ?rnfp ?rnfps)
(assert (Playing (player-name Paul) (position ?ps))))

(defrule Check-Sam "Only slot (for a position) left is Sam"
   (Not-found-person (person-name Sam))
   (Not-found-pos (pos ?ps))
   (Not-Playing (player-name Allen) (position ?ps))
   (Not-Playing (player-name Andy)  (position ?ps))
   (Not-Playing (player-name Bill)  (position ?ps))
   (Not-Playing (player-name Ed)    (position ?ps))
   (Not-Playing (player-name Harry) (position ?ps))
   (Not-Playing (player-name Jerry) (position ?ps))
   (Not-Playing (player-name Mike)  (position ?ps))
   (Not-Playing (player-name Paul)  (position ?ps))
   ?rnfp  <- (Not-found-person (person-name Sam))
   ?rnfps <- (Not-found-pos (pos ?ps))
=>
(TableRow ?ps "Sam")
(retract ?rnfp ?rnfps)
(assert (Playing (player-name Sam) (position ?ps))))

;
;- 1st found, assert facts that no one else can be 1st
;
(defrule rest-no-1st ""
  (Playing (player-name ?p-name) (position 1st))
  (Player (person-name ?pn))
=>
(assert (Not-Playing (player-name ?pn) (position 1st))))

;
;- 2nd found, assert facts that no one else can be 2nd
;
(defrule rest-no-2nd ""
  (Playing (player-name ?p-name) (position 2nd))
  (Player (person-name ?pn))
=>
(assert (Not-Playing (player-name ?pn) (position 2nd))))

;
;- 3rd found, assert facts that no one else can be 3rd
;
(defrule rest-no-3rd ""
  (Playing (player-name ?p-name) (position 3rd))
  (Player (person-name ?pn))
=>
(assert (Not-Playing (player-name ?pn) (position 3rd))))

;
;- P found, assert facts that no one else can be P
;
(defrule rest-no-P ""
  (Playing (player-name ?p-name) (position P))
  (Player (person-name ?pn))
  (Position (pos-name ?ps))
=>
(assert (Not-Playing (player-name ?pn) (position P)))
(assert (Not-Playing (player-name ?pn) (position ?ps))))

;
;- C found, assert facts that no one else can be C
;
(defrule rest-no-C ""
  (Playing (player-name ?p-name) (position C))
  (Player (person-name ?pn))
  (Position (pos-name ?ps))
=>
(assert (Not-Playing (player-name ?pn) (position C)))
(assert (Not-Playing (player-name ?pn) (position ?ps))))

;
;- LF found, assert facts that no one else can be LF
;
(defrule rest-no-LF ""
  (Playing (player-name ?p-name) (position LF))
  (Player (person-name ?pn))
=>
(assert (Not-Playing (player-name ?pn) (position LF))))

;
;- RF found, assert facts that no one else can be RF
;
(defrule rest-no-RF ""
  (Playing (player-name ?p-name) (position RF))
  (Player (person-name ?pn))
=>
(assert (Not-Playing (player-name ?pn) (position RF))))

;
;- Allen found, assert facts that no one else can be another pos
;
(defrule rest-no-Allen ""
  (Playing (player-name Allen) (position ?ps))
=>
(assert (Not-Playing (player-name Allen) (position ?ps))))
