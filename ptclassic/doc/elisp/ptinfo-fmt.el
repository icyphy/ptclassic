; Version $Id$
; Author:	T. M. Parks
; Created:	23 November 1993
;
; Support for pt-info.sty

(put 'Author 'latexinfo-format 'latexinfo-format-author)
(put 'Contrib 'latexinfo-format 'latexinfo-format-author)

;
; A list environment with automatic index entries.
;
(put 'indexlist 'latexinfo-format 'ptinfo-index-description)
(put 'indexlist 'latexinfo-end 'latexinfo-end-description)
(defvar ptinfo-index-key nil)
(defun ptinfo-index-description ()
  (setq ptinfo-index-key (latexinfo-parse-line-arg))
  (latexinfo-discard-command)
  (save-excursion (insert "\\begin{description}\n")))

; A list item for states.
(put 'state 'latexinfo-format 'ptinfo-state)
(defun ptinfo-state ()
  (let ((type (latexinfo-parse-line-arg))
        (name (latexinfo-parse-line-arg))
        (default (latexinfo-parse-line-arg)))
    (latexinfo-discard-command)
    (save-excursion
      (insert "\\item[" name "]")
      (insert "(\\code{" type "}) Default = \\samp{" default "}")
      (insert "\\vindex{" name " " ptinfo-index-key "}\n"))))

; A list item for ports.
(put 'port 'latexinfo-format 'ptinfo-port)
(defun ptinfo-port ()
  (let ((type (latexinfo-parse-line-arg))
        (name (latexinfo-parse-line-arg)))
    (latexinfo-discard-command)
    (save-excursion
      (insert "\\item[" name "]")
      (insert "(\\code{" type "})")
      (insert "\\vindex{" name " " ptinfo-index-key "}\n"))))

; A list item for blocks.
(put 'block 'latexinfo-format 'ptinfo-block)
(defun ptinfo-block ()
  (let ((name (latexinfo-parse-line-arg)))
    (latexinfo-discard-command)
    (save-excursion
      (insert "\\item[" name "]")
      (insert "\\tindex{" name " " ptinfo-index-key "}"))))

; A list item for functions.
(put 'function 'latexinfo-format 'ptinfo-function)
(defun ptinfo-function ()
  (let ((type (latexinfo-parse-line-arg))
        (name (latexinfo-parse-line-arg))
        (args (latexinfo-parse-line-arg)))
    (latexinfo-discard-command)
    (save-excursion
      (insert "\\item[" type " " name " " args "]")
      (insert "\\findex{" name ptinfo-index-key "}\n"))))

; A list item for variables.
(put 'variable 'latexinfo-format 'ptinfo-variable)
(defun ptinfo-variable ()
  (let ((type (latexinfo-parse-line-arg))
        (name (latexinfo-parse-line-arg)))
    (latexinfo-discard-command)
    (save-excursion
      (insert "\\item[" type " " name "]")
      (insert "\\vindex{" name ptinfo-index-key "}\n"))))

; A list item for C++ class member functions (methods).
(put 'method 'latexinfo-format 'ptinfo-method)
(defun ptinfo-method ()
  (let ((type (latexinfo-parse-line-arg))
        (name (latexinfo-parse-line-arg))
        (args (latexinfo-parse-line-arg)))
    (latexinfo-discard-command)
    (save-excursion
      (insert "\\item[" type " " name " " args "]")
      (insert "\\findex{" name ", " ptinfo-index-key " method}")
      (insert "\\findex{" ptinfo-index-key " class, " name " method}\n"))))

; A list item for C++ class data members.
(put 'member 'latexinfo-format 'ptinfo-member)
(defun ptinfo-member ()
  (let ((type (latexinfo-parse-line-arg))
        (name (latexinfo-parse-line-arg)))
    (latexinfo-discard-command)
    (save-excursion
      (insert "\\item[" type " " name "]")
      (insert "\\vindex{" name ", " ptinfo-index-key " member}\n")
      (insert "\\vindex{" ptinfo-index-key " class, " name " member}\n"))))

