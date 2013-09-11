;;;*****************
;;;* INITIAL STATE *
;;;*****************
   (Player (person-name Allen))
   (Player (person-name Andy))
   (Player (person-name Bill))
   (Player (person-name Ed))
   (Player (person-name Harry))
   (Player (person-name Jerry))
   (Player (person-name Mike))
   (Player (person-name Paul))
   (Player (person-name Sam))

   (Position (pos-name P))    ; Pitcher
   (Position (pos-name C))    ; Catcher
   (Position (pos-name 1st))  ; First Base
   (Position (pos-name 2nd))  ; Second Base
   (Position (pos-name 3rd))  ; Third Base
   (Position (pos-name SS))   ; Short Stop
   (Position (pos-name RF))   ; Right Field
   (Position (pos-name LF))   ; Left Field
   (Position (pos-name CF))   ; Center Field

   (Not-found-pos (pos P))    ; Pitcher
   (Not-found-pos (pos C))    ; Catcher
   (Not-found-pos (pos 1st))  ; First Base
   (Not-found-pos (pos 2nd))  ; Second Base
   (Not-found-pos (pos 3rd))  ; Third Base
   (Not-found-pos (pos SS))   ; Short Stop
   (Not-found-pos (pos RF))   ; Right Field
   (Not-found-pos (pos LF))   ; Left Field
   (Not-found-pos (pos CF))   ; Center Field

   (Not-found-person (person-name Allen))
   (Not-found-person (person-name Andy))
   (Not-found-person (person-name Bill))
   (Not-found-person (person-name Ed))
   (Not-found-person (person-name Harry))
   (Not-found-person (person-name Jerry))
   (Not-found-person (person-name Mike))
   (Not-found-person (person-name Paul))
   (Not-found-person (person-name Sam))
   
;;;
;;; Initial Facts about Allen
;;;
   (Not-Playing (player-name Allen) (position P))     ; Fact 5
   (Not-Playing (player-name Allen) (position LF))    ; Fact 26
   (Not-Playing (player-name Allen) (position CF))    ; Fact 26
   (Not-Playing (player-name Allen) (position RF))    ; Fact 26

;;;
;;; Initial facts about Andy
;;;
   (Not-Playing (player-name Andy) (position C))      ; Fact 1
   (Not-Playing (player-name Andy) (position SS))     ; Fact 9
   (Not-Playing (player-name Andy) (position LF))     ; Fact 26
   (Not-Playing (player-name Andy) (position RF))     ; Fact 26
   (Not-Playing (player-name Andy) (position CF))     ; Fact 26

;;;
;;; Initial facts about Bill
;;;
   (Not-Playing (player-name Bill) (position 2nd))    ; Fact 10
   (Not-Playing (player-name Bill) (position C))      ; Fact 10
   (Not-Playing (player-name Bill) (position 3rd))    ; Fact 14
   (Not-Playing (player-name Bill) (position SS))     ; Fact 14

;;;
;;; Initial Facts about Ed
;;;
   (Not-Playing (player-name Ed) (position 2nd))      ; Fact 2
   (Not-Playing (player-name Ed) (position RF))       ; Fact 6
   (Not-Playing (player-name Ed) (position CF))       ; Fact 6
   (Not-Playing (player-name Ed) (position LF))       ; Fact 6
   (Not-Playing (player-name Ed) (position SS))       ; Fact 20
   (Not-Playing (player-name Ed) (position 3rd))      ; Fact 21
   (Not-Playing (player-name Ed) (position C))        ; Fact 22
   (Not-Playing (player-name Ed) (position P))        ; Fact 25

;;;
;;; Initial facts about Harry
;;;
   (Not-Playing (player-name Harry) (position 3rd))   ; Fact 4
   (Not-Playing (player-name Harry) (position 2nd))   ; Fact 10
   (Not-Playing (player-name Harry) (position C))     ; Fact 10
   (Not-Playing (player-name Harry) (position LF))    ; Fact 26
   (Not-Playing (player-name Harry) (position RF))    ; Fact 26
   (Not-Playing (player-name Harry) (position CF))    ; Fact 26

;;;
;;; Initial facts about Jerry
;;;
   (Not-Playing (player-name Jerry) (position RF))    ; Fact 13
   (Not-Playing (player-name Jerry) (position CF))    ; Fact 13
   (Not-Playing (player-name Jerry) (position 3rd))   ; Fact 18
   (Not-Playing (player-name Jerry) (position C))     ; Fact 22
   (Not-Playing (player-name Jerry) (position P))     ; Fact 25
   (Not-Playing (player-name Jerry) (position LF))    ; Fact 26
   (Not-Playing (player-name Jerry) (position SS))    ; Fact 26
   (Not-Playing (player-name Jerry) (position 1st))   ; Fact 26

;;;
;;; Initial facts about Mike
;;;
   (Not-Playing (player-name Mike) (position 3rd))    ; Fact 18
   (Not-Playing (player-name Mike) (position 1st))    ; Fact 24
   (Not-Playing (player-name Mike) (position 2nd))    ; Fact 24
   (Not-Playing (player-name Mike) (position SS))     ; Fact 24
   (Not-Playing (player-name Mike) (position P))      ; Fact 24
   (Not-Playing (player-name Mike) (position C))      ; Fact 24

;;;
;;; Initial facts about Paul
;;;
   (Not-Playing (player-name Paul) (position P))      ; Fact 5
   (Not-Playing (player-name Paul) (position SS))     ; Fact 9
   (Not-Playing (player-name Paul) (position C))      ; Fact 10
   (Not-Playing (player-name Paul) (position 2nd))    ; Fact 10
   (Not-Playing (player-name Paul) (position CF))     ; Fact 13
   (Not-Playing (player-name Paul) (position RF))     ; Fact 13
   (Not-Playing (player-name Paul) (position 3rd))    ; Fact 22

;;;
;;; Initial facts about Sam
;;;
   (Not-Playing (player-name Sam) (position P))       ; Fact 19
   (Not-Playing (player-name Sam) (position C))       ; Fact 19
   (Not-Playing (player-name Sam) (position 3rd))     ; Fact 19
   (Not-Playing (player-name Sam) (position SS))      ; Fact 20