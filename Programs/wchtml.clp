;;;------------------------------------------------------------------
;;;- Module : WebCLIPSHTML                                          -
;;;-                                                                -
;;;- The purpose of this module is to provide CLIPS functions that  -
;;;- generate HTML (V3.2) to a web server. This module must be      -
;;;- imported by other CLIPS programs.                              -
;;;-                                                                -
;;;- Source File : WCHTML.CLP                                       -
;;;-                                                                -
;;;- Who            |   Date    | Description                       -
;;;- ---------------+-----------+---------------------------------- -
;;;- M.Giordano     | 31-Jul-97 | Create                            -
;;;- M.Giordano     | 19-Dec-97 | Changed str-cat to sym-cat        -
;;;-                                                                -
;;;-                                                                -
;;;- Note :                                                         -
;;;-                                                                -
;;;- For performance critical programs the FormatArgs function call -
;;;- can be replaced with (sym-cat " " (implode$ ?args)). The side  -
;;;- effect is an extra space in the first tag. For example :       -
;;;-                                                                -
;;;-    <hr> would become <hr >                                     -
;;;-                                                                -
;;;- Some browsers may complain about this format, so to be able to -
;;;- use the widest possible set of browsers, this functionality    -
;;;- is provided.                                                   -
;;;-                                                                -
;;;------------------------------------------------------------------

(defmodule WebCLIPSHTML 
   (export deffunction ?ALL))

;
;;- General function to format arguments passed to a function. 
;
(deffunction FormatArgs ($?args)
   (if (> (length$ ?args) 0)
      then
         (return (sym-cat " " (implode$ ?args)))
      else
         (return "")))

;
;;- General function to URL encode arguments passed to a function. 
;
(deffunction URLEncode ($?args)
   (loop-for-count (?count 1 (length$ ?args)) do
                   (printout t (nth ?count ?args) "+" ))
   TRUE)

;;;------------------------------------------------------------------
;;;-                                                                -
;;;-                           Header tags                          -
;;;-                                                                -
;;;------------------------------------------------------------------

(deffunction HTMLHeader (?title)
   (printout t "<!DOCTYPE HTML PUBLIC -//W3C//DTD HTML 3.2//EN>" crlf
               "<html><title>" ?title "</title>"))

(deffunction BodyTag ($?bodyargs)
   (printout t "<body" (FormatArgs ?bodyargs) ">" crlf))

;
;;- <HEAD> tag support (stand alone)
;
(deffunction HeadStart ($?dummy)
   (printout t <HEAD>))

;
;;- </HEAD> tag support (stand alone)
;
(deffunction HeadEnd ($?dummy)
   (printout t </HEAD>))

;
;;- <HEAD> tag support
;
(deffunction HeadStartTag ($?dummy)
   <HEAD>)

;
;;- </HEAD> tag support
;
(deffunction HeadEndTag ($?dummy)
   </HEAD>)

;
;;- <SCRIPT> tag support (stand alone)
;
(deffunction ScriptStart (?lang)
   (printout t "<SCRIPT LANGUAGE=" ?lang > ))

;
;;- </SCRIPT> tag support (stand alone)
;
(deffunction ScriptEnd ($?dummy)
   (printout t </SCRIPT>))

;
;;- <SCRIPT> tag support
;
(deffunction ScriptStartTag (?lang)
   (return (sym-cat "<SCRIPT LANGUAGE=" ?lang > )))

;
;;- </SCRIPT> tag support
;
(deffunction ScriptEndTag ($?dummy)
   </SCRIPT>)

;
;;- <SCRIPT> tag support JavaScript specific (stand alone)
;
(deffunction JavaScriptStart ($?dummy)
   (printout t "<SCRIPT LANGUAGE=JavaScript>" ))

;
;;- </SCRIPT> tag support (stand alone) JavaScript specific
;
(deffunction JavaScriptEnd ($?dummy)
   (printout t </SCRIPT>))

;
;;- <SCRIPT> tag support JavaScript specific
;
(deffunction JavaScriptStartTag ($?dummy)
   (return "<SCRIPT LANGUAGE=JavaScript>"))

;
;;- </SCRIPT> tag support JavaScript specific
;
(deffunction JavaScriptEndTag ($?dummy)
   </SCRIPT>)

;
;;- <SCRIPT> tag support VBScript specific (stand alone)
;
(deffunction VBScriptStart ($?dummy)
   (printout t "<SCRIPT LANGUAGE=VBScript>" ))

;
;;- </SCRIPT> tag support (stand alone) VBScript specific
;
(deffunction VBScriptEnd ($?dummy)
   (printout t </SCRIPT>))

;
;;- <SCRIPT> tag support VBScript specific
;
(deffunction VBScriptStartTag ($?dummy)
   (return "<SCRIPT LANGUAGE=VBScript>"))

;
;;- </SCRIPT> tag support VBScript specific
;
(deffunction VBScriptEndTag ($?dummy)
   </SCRIPT>)

;
;;- <BASE> tag support (stand alone)
;
(deffunction Base (?URL)
   (printout t "<BASE HREF=" ?URL > ))

;
;;- <BASE> tag support
;
(deffunction BaseTag (?URL)
   (return (sym-cat "<BASE HREF=" ?URL > )))

;
;;- <LINK> tag support (stand alone)
;
(deffunction Link (?URL)
   (printout t "<LINK HREF=" ?URL > ))

;
;;- <LINK> tag support
;
(deffunction LinkTag (?URL)
   (return (sym-cat "<LINK HREF=" ?URL > )))

;;;------------------------------------------------------------------
;;;-                                                                -
;;;-                       Miscellaneous tags                       -
;;;-                                                                -
;;;------------------------------------------------------------------

;
;;- <br> tag support (stand alone)
;
(deffunction Break ($?arg)
   (printout t <br>))

;
;;- <br> tag support
;
(deffunction BreakTag ($?dummy)
   <br>)

;
;;- <p> tag support (stand alone)
;
(deffunction Paragraph ($?align)
   (printout t <p (FormatArgs ?align) > ))

;
;;- <p> tag support
;
(deffunction ParagraphStartTag ($?align)
   (return (sym-cat <p (FormatArgs ?align) > )))

;
;;- </p> tag support
;
(deffunction ParagraphEndTag ($?dummy)
   </p>)

;
;;- <isindex> tag support (stand alone)
;
(deffunction IsIndex ($?dummy)
   (printout t <isindex>))

;
;;- <isindex> tag support
;
(deffunction IsIndexTag ($?dummy)
   <isindex>)

;
;;- <applet> tag support
;
(deffunction Applet (?prog ?width ?height $?args)
   (printout t "<applet code=" ?prog " width=" ?width 
               " height=" ?height
               (FormatArgs ?args) > ))

;
;;- <param> tag support
;
(deffunction AppletParm ($?args)               
   (printout t "<param ")
   (bind ?count 1)
   (while (< ?count (length$ ?args))
      (printout t "name=" (nth ?count ?args))
      (bind ?count (+ 1 ?count))
      (printout t " value=" (nth ?count ?args))
      (bind ?count (+ 1 ?count)))
   (printout t ">" ))

;
;;- <!--# ...> tag support SSI Include support
;
(deffunction SSI (?SSIType ?URL $?args)
   (printout t "<!--#" ?SSIType "=" ?URL (FormatArgs ?args) > ))

;
;;- <IMG> tag support (stand alone)
;
(deffunction Image (?URL $?args)
   (printout t "<img src=" ?URL (FormatArgs ?args) > ))

;
;;- <IMG> tag support
;
(deffunction ImageTag (?URL $?args)
   (return (sym-cat "<img src=" ?URL (FormatArgs ?args) > )))

;;;------------------------------------------------------------------
;;;-                                                                -
;;;-                            Form tags                           -
;;;-                                                                -
;;;------------------------------------------------------------------

;
;;- <FORM> tag support. Parameter1 is the executable
;;-                     Parameter2 is the method (optional)
;;-                     If parameter2 is omitted the default method
;;-                     is GET.
;
(deffunction FormStart (?exec $?method)
   (printout t "<FORM ACTION=/cgi-bin/" ?exec)
   (if (eq 0 (length$ ?method))
      then
        (printout t " METHOD=GET>" crlf)
      else
        (printout t " METHOD=" (implode$ ?method) ">" crlf)))

;
;;- </FORM> tag support (stand alone)
;
(deffunction FormEnd ($?dummy)
   </FORM>)

;
;;- <FORM> tag support
;
(deffunction FormStartTag ($?args)
   return (sym-cat "<FORM" (FormatArgs ?args) > ))

;
;;- </FORM> tag support
;
(deffunction FormEndTag ($?dummy)
   </FORM>)

;
;;- <SELECT> tag support. (stand alone)
;
(deffunction SelectStart (?varname $?args)
   (printout t "<SELECT NAME=" ?varname " " (FormatArgs ?args) > ))

;
;;- </SELECT> tag support. (stand alone)
;
(deffunction SelectEnd ($?dummy)
   (printout t </SELECT>))

;
;;- <SELECT> tag support.
;
(deffunction SelectStartTag (?varname $?args)
   (return (sym-cat "<SELECT NAME=" ?varname (FormatArgs ?args) > )))

;
;;- </SELECT> tag support. (stand alone)
;
(deffunction SelectEndTag ($?dummy)
   </SELECT>)

;
;;- <SELECT> tag support. Dropdown box specific (stand alone)
;
(deffunction DropDownStart (?varname)
   (printout t "<SELECT NAME=" ?varname " SIZE=1>" ))

;
;;- <SELECT> tag support. Dropdown box specific
;
(deffunction DropDownStartTag (?varname)
   (return (sym-cat "<SELECT NAME=" ?varname " SIZE=1>" )))

;
;;- </SELECT> tag support. Dropdown box specific (stand alone)
;
(deffunction DropDownEnd ($?dummy)
   (printout t </SELECT>))

;
;;- </SELECT> tag support. Dropdown box specific
;
(deffunction DropDownEndTag ($?dummy)
   </SELECT>)

;
;;- <SELECT> tag support. List box specific (stand alone)
;
(deffunction ListBoxStart (?varname ?NumElems)
   (printout t "<SELECT NAME=" ?varname " SIZE=" ?NumElems > ))

;
;;- <SELECT> tag support. List box specific
;
(deffunction ListBoxStartTag (?varname ?NumElems)
   (return (sym-cat "<SELECT NAME=" ?varname " SIZE=" ?NumElems > )))

;
;;- </SELECT> tag support. List box specific (stand alone)
;
(deffunction ListBoxEnd ($?dummy)
   (printout t </SELECT>))

;
;;- </SELECT> tag support. List box specific
;
(deffunction ListBoxEndTag ($?dummy)
   </SELECT>)

;
;;- <OPTION> tag support
;
(deffunction Option (?value $?args)
   (printout t "<OPTION ")
   (URLEncode ?args)
   (printout t  ">" ?value crlf))

;
;;- <INPUT TYPE="submit" VALUE=XXX> support
;
(deffunction SubmitButton ($?caption)
   (printout t "<INPUT TYPE=submit")
   (if (> (length$ ?caption) 0)
      then
         (printout t " VALUE=" "\"" (implode$ ?caption) "\"" ))
   (printout t ">"))

;
;;- <INPUT TYPE="reset"> support
;
(deffunction ResetButton ($?caption)
   (printout t "<INPUT TYPE=reset>"))

;
;;- <INPUT TYPE=hidden NAME=fact Value="New Screen Name"> support
;
(deffunction HiddenData ($?factvalue)
   (printout t "<INPUT TYPE=hidden Name=fact Value=\"")
   (printout t (implode$ ?factvalue) "\"" > crlf))

;
;;- <textarea> & </textarea> tag support (stand alone)
;
(deffunction TextArea (?rows ?cols $?text)
   (printout t "<textarea ROWS=" ?rows " COLS=" ?cols ">"
               (implode$ ?text) "</textarea>"))

;
;;- <textarea> tag support
;
(deffunction TextAreaStartTag ($?args)
   (return (sym-cat <textarea (FormatArgs ?args) > )))

;
;;- </textarea> tag support
;
(deffunction TextAreaEndTag ($?args)
   </textarea>)

;
;;- <INPUT TYPE=text ...> support Text box specific (stand alone)
;
(deffunction TextBox (?name ?size $?maxlength)
   (printout t "<INPUT TYPE=text NAME=" ?name " SIZE=" ?size)
   (if (eq 0 (length$ ?maxlength))
      then
        (printout t > )
      else
        (printout t " MAXLENGTH=" (implode$ ?maxlength) > )))

;
;;- <INPUT TYPE=text ...> support Text box specific
;
(deffunction TextBoxTag (?name ?size $?maxlength)
   (if (eq 0 (length$ ?maxlength))
      then
         (return (sym-cat "<INPUT TYPE=text NAME=" ?name 
                          " SIZE=" ?size > ))
      else
         (return (sym-cat "<INPUT TYPE=text NAME=" ?name 
                          " SIZE=" ?size
                          " MAXLENGTH=" (implode$ ?maxlength) > ))))

;
;;- <INPUT TYPE=text ...> support Password box specific (stand alone)
;
(deffunction PasswordBox (?name ?size $?maxlength)
   (printout t "<INPUT TYPE=password NAME=" ?name " SIZE=" ?size)
   (if (eq 0 (length$ ?maxlength))
      then
        (printout t > )
      else
        (printout t " MAXLENGTH=" (implode$ ?maxlength) > )))

;
;;- <INPUT TYPE=text ...> support Password box specific
;
(deffunction PasswordBoxTag (?name ?size $?maxlength)
   (if (eq 0 (length$ ?maxlength))
      then
         (return (sym-cat "<INPUT TYPE=password NAME=" ?name 
                          " SIZE=" ?size > ))
      else
         (return (sym-cat "<INPUT TYPE=password NAME=" ?name 
                          " SIZE=" ?size
                          " MAXLENGTH=" (implode$ ?maxlength) > ))))

;
;;- <INPUT TYPE=text ...> support TextArea box specific (stand alone)
;
(deffunction TextAreaBox (?name ?size ?maxlength)
   (printout t "<INPUT TYPE=textarea NAME=" ?name " SIZE=" ?size
               " MAXLENGTH=" ?maxlength > ))

;
;;- <INPUT TYPE=text ...> support TextArea box specific
;
(deffunction TextAreaBoxTag (?name ?size ?maxlength)
   (return (sym-cat "<INPUT TYPE=textarea NAME=" ?name " SIZE=" ?size
                    " MAXLENGTH=" ?maxlength > )))

;
;;- <INPUT TYPE=text ...> support Radio button specific (stand alone)
;
(deffunction RadioButton (?name ?text $?args)
   (printout t "<INPUT TYPE=radio NAME=" ?name (FormatArgs ?args) 
               > ?text))

;
;;- <INPUT TYPE=text ...> support Radio button specific
;
(deffunction RadioButtonTag (?name ?text $?args)
   (return (sym-cat "<INPUT TYPE=radio NAME=" ?name (FormatArgs ?args)
            > ?text)))

;
;;- <INPUT TYPE=text ...> support Check Box specific (stand alone)
;
(deffunction CheckBox (?name ?text $?args)
   (printout t "<INPUT TYPE=checkbox NAME=" ?name (FormatArgs ?args)
               > ?text))

;
;;- <INPUT TYPE=text ...> support Check Box specific
;
(deffunction CheckBoxTag (?name ?text $?args)
   (return (sym-cat "<INPUT TYPE=checkbox NAME=" ?name 
                    (FormatArgs ?args) > ?text)))

(deffunction ImageButton (?name ?URL $?args)
   (printout t "<INPUT TYPE=image NAME=" ?name " SRC=" ?URL 
               (FormatArgs ?args) >))

(deffunction ImageButtonTag (?name ?URL $?args)
   (return (sym-cat "<INPUT TYPE=image NAME=" ?name " SRC=" ?URL 
                    (FormatArgs ?args) > )))

;;;------------------------------------------------------------------
;;;-                                                                -
;;;-                           Anchor tags                          -
;;;-                                                                -
;;;------------------------------------------------------------------

;
;;- <a href...> support
;
(deffunction Anchor (?URL ?LinkText)
   (printout t "<a href=" ?URL ">" ?LinkText "</a>"))

(deffunction AnchorInternal (?IRL ?LinkText)
   (printout t "<a href=#" ?IRL ">" ?LinkText "</a>"))

(deffunction AnchorReference (?IRL ?LinkText)
   (printout t "<a NAME=" ?IRL ">" ?LinkText "</a>"))

;;;------------------------------------------------------------------
;;;-                                                                -
;;;-                           List tags                            -
;;;-                                                                -
;;;------------------------------------------------------------------

;
;;- <ul> tag support. Simple table support (stand alone)
;
(deffunction ul ($?args)
   (printout t <ul>)
   (loop-for-count (?count 1 (length$ ?args)) do
                   (printout t <li> (nth ?count ?args)))
   (printout t </ul>))

;
;;- <ul> tag support
;
(deffunction ULStartTag ($?args)
   <ul>)

;
;;- </ul> tag support
;
(deffunction ULEndTag ($?args)
   </ul>)

;
;;- <ol> tag support. Simple table support (stand alone)
;
(deffunction ol ($?args)
   (printout t <ol>)
   (loop-for-count (?count 1 (length$ ?args)) do
                   (printout t <li> (nth ?count ?args)))
   (printout t </ol>))

;
;;- <ol> tag support
;
(deffunction OLStartTag ($?args)
   <ol>)

;
;;- </ol> tag support
;
(deffunction OLEndTag ($?args)
   </ol>)

;
;;- <dir> tag support. Simple table support (stand alone)
;
(deffunction dir ($?args)
   (printout t <dir>)
   (loop-for-count (?count 1 (length$ ?args)) do
                   (printout t <li> (nth ?count ?args)))
   (printout t </dir>))

;
;;- <dir> tag support
;
(deffunction DirectoryStartTag ($?args)
   <dir>)

;
;;- </dir> tag support
;
(deffunction DirectoryEndTag ($?args)
   </dir>)

;
;;- <menu> tag support. Simple table support (stand alone)
;
(deffunction Menu ($?args)
   (printout t <menu>)
   (loop-for-count (?count 1 (length$ ?args)) do
                   (printout t <li> (nth ?count ?args)))
   (printout t </menu>))

;
;;- <menu> tag support
;
(deffunction MenuStartTag ($?args)
   <menu>)

;
;;- </menu> tag support
;
(deffunction MenuEndTag ($?args)
   </menu>)

;
;;- <li> tag support
;
(deffunction li ($?args)
   (printout t <li>)
   (loop-for-count (?count 1 (length$ ?args)) do
                   (printout t (nth ?count ?args))))

;;;------------------------------------------------------------------
;;;-                                                                -
;;;-                       <hr> tag parameters                      -
;;;-                                                                -
;;;- Valid values for ?args                                         -
;;;-                                                                -
;;;- SIZE=number                                                    -
;;;- The SIZE attributes lets the author give an indication of how  -
;;;- thick they wish the horizontal rule to be.  A pixel value      -
;;;- should be given.                                               -
;;;-                                                                -
;;;- WIDTH=number|percent                                           -
;;;- The default horizontal rule is always as wide as the page.     -
;;;- With the WIDTH attribute, the author can specify an exact      -
;;;- width in pixels, or a relative width measured in percent of    -
;;;- document width.                                                -
;;;-                                                                -
;;;- ALIGN=left|right|center                                        -
;;;- Now that horizontal rules do not have to be the width of the   -
;;;- page it possible to specify the horizontal alignment of the    -
;;;- rule.                                                          -
;;;-                                                                -
;;;- NOSHADE                                                        -
;;;- For those times when a solid bar is required, the NOSHADE      -
;;;- attribute specifies that the horizontal rule should not be     -
;;;- shaded at all.                                                 -
;;;-                                                                -
;;;------------------------------------------------------------------
;
;;- <hr> tag support
;
(deffunction hr ($?args)
   (printout t <hr)
   (printout t (FormatArgs ?args))
   (printout t > ))

;;;------------------------------------------------------------------
;;;-                                                                -
;;;-                       <hx> tag parameters                      -
;;;-                                                                -
;;;- Valid values for ?args                                         -
;;;-                                                                -
;;;- ALIGN=left|right|center                                        -
;;;- This parameter justifies the text in the browser window.       -
;;;-                                                                -
;;;------------------------------------------------------------------

;
;;- <h1> tag support
;
(deffunction h1 (?title $?args)
   (printout t "<h1")
   (printout t (FormatArgs ?args))
   (printout t ">" ?title "</h1>" crlf))

;
;;- <h2> tag support
;
(deffunction h2 (?title $?args)
   (printout t "<h2")
   (printout t (FormatArgs ?args))
   (printout t ">" ?title "</h3>" crlf))

;
;;- <h3> tag support
;
(deffunction h3 (?title $?args)
   (printout t "<h3")
   (printout t (FormatArgs ?args))
   (printout t ">" ?title "</h3>" crlf))

;
;;- <h4> tag support
;
(deffunction h4 (?title $?args)
   (printout t "<h4")
   (printout t (FormatArgs ?args))
   (printout t ">" ?title "</h4>" crlf))

;
;;- <h5> tag support
;
(deffunction h5 (?title $?args)
   (printout t "<h5")
   (printout t (FormatArgs ?args))
   (printout t ">" ?title "</h5>" crlf))

;
;;- <h6> tag support
;
(deffunction h6 (?title $?args)
   (printout t "<h6")
   (printout t (FormatArgs ?args))
   (printout t ">" ?title "</h6>" crlf))

;;;------------------------------------------------------------------
;;;-                                                                -
;;;-                       Tables tags Section                      -
;;;-                                                                -
;;;------------------------------------------------------------------

;
;;- <table> tag support (stand alone)
;
(deffunction TableStart ($?args)
   (printout t "<table" (FormatArgs ?args) ">" crlf))

;
;;- <table> tag support
;
(deffunction TableStartTag ($?args)
   (return (sym-cat <table (FormatArgs ?args) > )))

;
;;- </table> tag support (stand alone)
;
(deffunction TableEnd ($?dummy)
   (printout t "</table>" crlf))

;
;;- </table> tag support
;
(deffunction TableEndTag ($?dummy)
   </table>)

;
;;- <caption> & </caption> tag (stand alone)
;
(deffunction Caption (?text $?args)
   (printout t "<caption")
   (printout t (FormatArgs ?args))
   (printout t ">" ?text "</caption>" crlf))

;
;;- <caption> tag support
;
(deffunction CaptionStartTag ($?args)
   (return (sym-cat <caption (FormatArgs ?args) > )))

;
;;- </caption> tag support
;
(deffunction CaptionEndTag ($?args)
   </caption>)

;
;;- <th> tag support (stand alone)
;
(deffunction TableHeader ($?args)
   (printout t "<th")
   (printout t (FormatArgs ?args))
   (printout t ">" crlf))

;
;;- <th> tag support
;
(deffunction TableHeaderStartTag ($?args)
   (return (sym-cat <th (FormatArgs ?args) > )))

;
;;- </th> tag support
;
(deffunction TableHeaderEndTag ($?args)
   </th>)

;
;;- Print out one row in a table (stand alone)
;
(deffunction TableRow ($?args)
   (printout t "<tr>")
   (loop-for-count (?count 1 (length$ ?args)) do
                   (printout t "<td>" (nth ?count ?args) "</td>"))
   (printout t "</tr>" crlf))

;
;;- <tr> tag support
;
(deffunction TableRowStartTag ($?args)
   (return (sym-cat <tr (FormatArgs ?args) > )))

;
;;- </tr> tag support
;
(deffunction TableRowEndTag ($?args)
   </tr>)

;
;;- <td> tag support
;
(deffunction TableDataStartTag ($?args)
   (return (sym-cat <td (FormatArgs ?args) > )))

;
;;- </td> tag support
;
(deffunction TableDataEndTag ($?args)
   </td>)

;;;------------------------------------------------------------------
;;;-                                                                -
;;;-                       Text Format Section                      -
;;;-                                                                -
;;;------------------------------------------------------------------

;;;-                             ADDRESS
;
;;- <address> & </address> tag support for a contiguous string
;
(deffunction Address (?text)
   (printout t <address> (implode$ ?text) </address>)) 

;
;;- <address> tag support
;
(deffunction AddressStartTag ($?dummy)
   <address>)

;
;;- </address> tag support
;
(deffunction AddressEndTag ($?dummy)
   </Address>)

;;;-                               BIG
;
;;- <big> & </big> tag support for a contiguous string
;
(deffunction Big (?text)
   (printout t <big> (implode$ ?text) </big>)) 

;
;;- <big> tag support
;
(deffunction BigStartTag ($?dummy)
   <big>)

;
;;- </big> tag support
;
(deffunction BigEndTag ($?dummy)
   </big>)

;;;-                          BLOCKQUOTE
;
;;- <blockquote> & </blockquote> tag support for a contiguous string
;
(deffunction BlockQuote ($?text)
   (printout t <blockquote> (implode$ ?text) </blockquote>))

;
;;- <blockquote> tag support
;
(deffunction BlockQuoteStartTag ($?dummy)
   <blockquote>)

;
;;- </blockquote> tag support
;
(deffunction BlockQuoteEndTag ($?dummy)
   </block>)

;;;-                            BOLD
;
;;- <b> & </b> tag support for a contiguous string
;
(deffunction Bold ($?text)
   (sym-cat <b> (implode$ ?text) </b>))

;
;;- <b> tag support
;
(deffunction BoldStartTag ($?dummy)
   <b>)

;
;;- </b> tag support
;
(deffunction BoldEndTag ($?dummy)
   </b>)

;;;-                           CENTER
;
;;- <center> & </center> tag support for a contiguous string
;
(deffunction Center ($?text)
   (printout t <center> (implode$ ?text) </center>))

;
;;- <center> tag support
;
(deffunction CenterStartTag ($?dummy)
   <center>)

;
;;- </center> tag support
;
(deffunction CenterEndTag ($?dummy)
   </center>)

;;;-                            CITE
;
;;- <cite> & </cite> tag support for a contiguous string
;
(deffunction Cite ($?text)
   (printout t <cite> (implode$ ?text) </cite>))

;
;;- <cite> tag support
;
(deffunction CiteStartTag ($?dummy)
   <cite>)

;
;;- </cite> tag support
;
(deffunction CiteEndTag ($?dummy)
   </cite>)

;;;-                          CODE
;
;;- <code> & </code> tag support for a contiguous string
;
(deffunction Code ($?text)
   (printout t <code> (implode$ ?text) </code>))

;
;;- <code> tag support
;
(deffunction CodeStartTag ($?dummy)
   <code>)

;
;;- </code> tag support
;
(deffunction CodeEndTag ($?dummy)
   </code>)

;;;-                            DEFINITION
;
;;- <dd> tag support
;
(deffunction DefStartTag ($?args)
   <dd>)

;;;-                         DEFINITION LIST
;
;;- <dl> tag support (stand alone)
;
(deffunction DefList ($?args)
   (printout t <dl (FormatArgs ?args) > ))

;
;;- <dl> tag support
;
(deffunction DefListStartTag ($?args)
   (return (sym-cat <dl (FormatArgs ?args) > )))

;
;;- </dl> tag support
;
(deffunction DefListEndTag ($?args)
   </dl>)

;;;-                         DEFINITION TERM
;
;;- <dt> tag support
;
(deffunction DefTermTag ($?args)
   </dt>)

;;;-                          EMPHASIZE
;
;;- <em> & </em> tag support for a contiguous string
;
(deffunction Emphasize (?text)
   (printout t <em> (implode$ ?text) </em>)) 

;
;;- <em> tag support
;
(deffunction EmphasizeStartTag ($?dummy)
   <em>)

;
;;- </em> tag support
;
(deffunction EmphasizeEndTag ($?dummy)
   </em>)

;;;-                                FONT
;
;;- <font> & </font> tag support for a contiguous string
;
(deffunction Font (?text $?args)
   (printout t "<font")
   (printout t (FormatArgs ?args))
   (printout t ">" ?text "</font>" crlf))

;
;;- <font> tag support
;
(deffunction FontStartTag ($?args)
   (return (sym-cat <font (FormatArgs ?args) > )))

;
;;- </font> tag support
;
(deffunction FontEndTag ($?args)
   </font>)

;;;-                           ITALICS
;
;;- <i> & </i> tag support for a contiguous string
;
(deffunction Italics ($?text)
   (sym-cat <i> (implode$ ?text) </i>)) 

;
;;- <i> tag support
;
(deffunction ItalicsStartTag ($?dummy)
   <i>)

;
;;- </i> tag support
;
(deffunction ItalicsEndTag ($?dummy)
   </i>)

;;;-                             KEYBOARD
;
;;- <kbd> & </kbd> tag support for a contiguous string
;
(deffunction Keyboard (?text $?args)
   (printout t <kbd> (implode$ ?text) </kbd>)) 

;
;;- <kbd> tag support
;
(deffunction KeyboardStartTag ($?dummy)
   <kbd>)

;
;;- </kbd> tag support
;
(deffunction KeyboardEndTag ($?dummy)
   </kbd>)

;;;-                            PREFORMATTED
;
;;- <pre> & </pre> tag support for a contiguous string
;
(deffunction Pre (?text $?args)
   (printout t "<pre")
   (printout t (FormatArgs ?args))
   (printout t ">" ?text "</pre>" crlf))

;
;;- <pre> tag support
;
(deffunction PreStartTag ($?args)
   (return (sym-cat <pre (FormatArgs ?args) > )))

;
;;- </pre> tag support
;
(deffunction PreEndTag ($?args)
   </pre>)

;;;-                            SAMPLE
;
;;- <samp> & </samp> tag support for a contiguous string
;
(deffunction Sample (?text)
   (printout t <samp> (implode$ ?text) </samp>)) 

;
;;- <samp> tag support
;
(deffunction SampleStartTag ($?dummy)
   <samp>)

;
;;- </samp> tag support
;
(deffunction SampleEndTag ($?dummy)
   </samp>)

;;;-                            SMALL
;
;;- <small> & </small> tag support for a contiguous string
;
(deffunction Small (?text)
   (printout t <small> (implode$ ?text) </small>)) 

;
;;- <small> tag support
;
(deffunction SmallStartTag ($?dummy)
   <small>)

;
;;- </small> tag support
;
(deffunction SmallEndTag ($?dummy)
   </small>)

;;;-                              SPAN
;
;;- <span> & </span> tag support for a contiguous string
;
(deffunction Span (?text $?args)
   (printout t "<span")
   (printout t (FormatArgs ?args))
   (printout t ">" ?text "</span>" crlf))

;
;;- <span> tag support
;
(deffunction SpanStartTag ($?args)
   (return (sym-cat <span (FormatArgs ?args) > )))

;
;;- </span> tag support
;
(deffunction SpanEndTag ($?args)
   </span>)

;;;-                          STRIKE THRU
;
;;- <s> & </s> tag support for a contiguous string
;
(deffunction StrikeThru (?text)
   (printout t <s> (implode$ ?text) </s>)) 

;
;;- <s> tag support
;
(deffunction StrikeThruStartTag ($?dummy)
   <s>)

;
;;- </s> tag support
;
(deffunction StrikeThruEndTag ($?dummy)
   </s>)

;;;-                            STRONG
;
;;- <strong> & </strong> tag support for a contiguous string
;
(deffunction Strong (?text)
   (printout t <strong> (implode$ ?text) </strong>)) 

;
;;- <strong> tag support
;
(deffunction StrongStartTag ($?dummy)
   <strong>)

;
;;- </strong> tag support
;
(deffunction StrongEndTag ($?dummy)
   </strong>)

;;;-                          SUBSCRIPT
;
;;- <sub> & </sub> tag support for a contiguous string
;
(deffunction Subscript (?text)
   (printout t <sub> (implode$ ?text) </sub>)) 

;
;;- <sub> tag support
;
(deffunction SubscriptStartTag ($?dummy)
   <sub>)

;
;;- </sub> tag support
;
(deffunction SubscriptEndTag ($?dummy)
   </sub>)

;;;-                          SUPERSCRIPT
;
;;- <sup> & </sup> tag support for a contiguous string
;
(deffunction Superscript (?text)
   (printout t <sup> (implode$ ?text) </sup>)) 

;
;;- <sup> tag support
;
(deffunction SuperscriptStartTag ($?dummy)
   <sup>)

;
;;- </sup> tag support
;
(deffunction SuperscriptEndTag ($?dummy)
   </sup>)

;;;-                           TELETYPE
;
;;- <tt> & </tt> tag support for a contiguous string
;
(deffunction Teletype (?text)
   (printout t <tt> (implode$ ?text) </tt>)) 

;
;;- <tt> tag support
;
(deffunction TeletypeStartTag ($?dummy)
   <tt>)

;
;;- </tt> tag support
;
(deffunction TeletypeEndTag ($?dummy)
   </tt>)

;;;-                          UNDERLINE
;
;;- <u> & </u> tag support for a contiguous string
;
(deffunction Underline ($?text)
   (sym-cat <u> (implode$ ?text) </u>)) 

;
;;- <u> tag support
;
(deffunction UnderlineStartTag ($?dummy)
   <u>)

;
;;- </u> tag support
;
(deffunction UnderlineEndTag ($?dummy)
   </u>)

;;;-                       VARIABLE DEFINITION
;
;;- <var> & </var> tag support for a contiguous string
;
(deffunction Var (?text)
   (printout t <var> (implode$ ?text) </var>)) 

;
;;- <var> tag support
;
(deffunction VarStartTag ($?dummy)
   <var>)

;
;;- </var> tag support
;
(deffunction VarEndTag ($?dummy)
   </var>)

