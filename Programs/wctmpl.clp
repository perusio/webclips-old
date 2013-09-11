;;;------------------------------------------------------------------
;;;- Module : WebCLIPSTemplates                                     -
;;;-                                                                -
;;;- The purpose of this module is to provide CLIPS deftemplates    -
;;;- for functions in WebCLIPS. This module must be imported by     -
;;;- other CLIPS programs.                                          -
;;;-                                                                -
;;;- Source File : WCTMPL.CLP                                       -
;;;-                                                                -
;;;- Who            |   Date    | Description                       -
;;;- ---------------+-----------+---------------------------------- -
;;;- M.Giordano     | 07-Aug-97 | Create                            -
;;;-                                                                -
;;;------------------------------------------------------------------

(defmodule WebCLIPSTemplates 
   (export deftemplate ?ALL))

(deftemplate ScreenName (multislot ScrnName))

(deftemplate odbcquery     (multislot query) (slot connect)
                            (slot deftemplatename) (slot implodedata))

(deftemplate odbcqueryfile (slot file)       (slot connect)
                            (slot deftemplatename) (slot implodedata))
